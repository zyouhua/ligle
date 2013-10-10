////////////////////////////////////////////////////////////////////////////////
// P2P Client Engineering (P2P_CE)
//
// 文件名称: core_httpchannel.cpp
// 功能描述: 提供p2p内核功能调用
// 最后修改:
////////////////////////////////////////////////////////////////////////////////

#include "core_httpchannel.h"
#include "icl_sysutils.h"

#include <string.h>

////////////////////////////////////////////////////////////////////////////////
//class CHttpChannel

CHttpChannel::CHttpChannel(CResource *Resource,CTask *Task) : CChannel(Resource,Task)
{
  m_State = HTTPINIT;
  m_Retry=false;
  m_Delay=0;
  m_Chunk=false;
  Clear();
}

void CHttpChannel::Clear()
{
  m_ChunkTail=false;
  m_FileLen=0;
  m_Code=0;
  memset(m_Head,0,HEADLEN);
  memset(m_Location,0,250);
  memset(m_ServerResetCookie,0,2048);
  m_Connection.Disconnect();
}


//-----------------------------------------------------------------------------
//描述：重载父类的 Handle()，这是CHttpChannel功能的执行函数
//      是下载任务整个状态变化状态
//-----------------------------------------------------------------------------
void CHttpChannel::Handle()
{
    if(!m_Task->Check()) throw CException();
    switch(m_State)
    {
      case HTTPINIT:
           OnInit();
           break;
      case HTTPCONNECT:
           OnConnect();
           break;
      case HTTPGETLENGTH:
           OnGetLength();
           break;
      case HTTPGETHEAD:
           OnGetHead();
           break;
      case HTTPWAIT:
           OnWait();
           break;
      case HTTPRECEIVE:
           OnReceive();
           break;
    }
}

void CHttpChannel::OnInit()
{
  m_Connection.ConnectNonBlock(m_Resource->m_strIp,m_Resource->m_nPort);      //调用非阻塞连接函数
  SetState(HTTPCONNECT);
}


void CHttpChannel::OnConnect()
{
   if(m_Connection.IsConnect())
   {
       if(GetBlock())
       {
          SendHead(m_Block->m_nStart); //其它请求增援得格式化请求头
          SetState(HTTPGETHEAD);
          return;
        }
        SendHead();      //首次请求增援得格式化请求头
        SetState(HTTPGETLENGTH);
        return;
    }
    CheckTimeOut();
}

void CHttpChannel::OnGetLength()
{
  if(!GetHead()) { CheckTimeOut(); return;}
  if(HandleHead())
  {
    if(!SetLength(m_FileLen)) return;
    GetBlock();
    SetState(HTTPRECEIVE);
  }
}

void CHttpChannel::OnGetHead()
{
  if(!GetHead()) { CheckTimeOut(); return;}
  if(HandleHead())
    SetState(HTTPRECEIVE);
}

void CHttpChannel::OnWait()
{
  if (GetTickDiff(m_nLastTicks, GetCurTicks()) > m_Delay)
   SetState(HTTPINIT);
}

void CHttpChannel::OnReceive()
{
    if(m_Chunk) { GetChunkHead(); return; }

    int Readed = m_Connection.ReadBuffer(&m_Buffer[m_nPieceIndex],m_nPieceRemain);
    if(Readed)
    {
       m_nLastTicks=GetCurTicks();
       m_nPieceIndex+=Readed;
       m_nPieceRemain-=Readed;
       if(m_nPieceRemain==0)
       {
          Saves();
          if(m_Block->IsComplete())
          {
            Clear();
            SetState(HTTPWAIT);
            return;
          }
          SetPiece();
       }
    }
    CheckTimeOut();
}



bool CHttpChannel::GetHead()
{
  int nIndex = 0;
  while(m_Connection.ReadBuffer(&m_Head[nIndex++],1))
  {
    if(nIndex >= 4 &&
       m_Head[nIndex-4]=='\r' &&
       m_Head[nIndex-3]=='\n' &&
       m_Head[nIndex-2]=='\r' &&
       m_Head[nIndex-1]=='\n') return true;
  }
  return false;
}

void CHttpChannel::GetChunkHead()
{
  int readed;
  if(m_ChunkTail)
  {
    readed=m_Connection.ReadBuffer(m_Head,2);
    if(readed)
    {
      m_nLastTicks=GetCurTicks();
      m_ChunkTail=false;
    }
    else
    {
      CheckTimeOut();
      return;
    }
  }

  readed=m_Connection.ReadBuffer(m_Head,6);
  if(readed)
  {
    m_nLastTicks=GetCurTicks();
    int length = strtol(m_Head,NULL,16);
    if(length<(m_Block->m_nSize))
    {
      m_nPieceRemain=length;
      m_ChunkTail=true;
    }
    m_Chunk=false;
  }
  CheckTimeOut();
}

void CHttpChannel::Saves()
{
  if(m_ChunkTail) m_Chunk=true;
  Save();
}

bool CHttpChannel::HandleHead()
{
  string head=LowerCase(m_Head);

  m_Code=atoi(&m_Head[9]);
  if(m_Code==200)
  {
    char *Key = "content-length: ";
    int nPos = head.find(Key,0);
    nPos += strlen(Key);
    m_FileLen =atof(&m_Head[nPos]);
    m_Retry=true;
    m_Resource->SetUsable();
    return true;
  }

  if(m_Code==206)
  {
    m_Retry=true;
    m_Resource->SetUsable();
    int pos=head.find("chunked",0);
    if(pos>=0) m_Chunk=true;
    return true;
  }

  if(m_Code==302 || m_Code==301)
  {
    char *Key="location";
    char *CookieKey = "Set-Cookie";
    int nStart = head.find(Key,0);
    nStart += strlen(Key)+2;
    int nEnd = head.find("\r\n",nStart);
    strncpy(m_Location,&m_Head[nStart],nEnd-nStart);
    m_Resource->ReSetURL(m_Location);
    m_Task->Rename(m_Location);

    int CookieValueStart = ((string)m_Head).find(CookieKey,0);
    if(CookieValueStart >= 0)
    {
        CookieValueStart += strlen(CookieKey)+2;
        int  CookieValueEnd = ((string)m_Head).find_first_of(";",CookieValueStart);
        strncpy(m_ServerResetCookie,&m_Head[CookieValueStart],CookieValueEnd - CookieValueStart);
        m_Resource->ReSetRequestCookie(m_ServerResetCookie);
    }
    throw CException();
  }

  if(m_Retry)
  {
    if(m_Delay>1000*60) throw CException();
    m_Delay+=1000*5;
    Clear();
    SetState(HTTPWAIT);
    return false;
  }
  DoOnError(m_Code);
}

//-----------------------------------------------------------------------------
//描述：SendHead先格式化请求头，再发送请求头，最后获得请求头
//-----------------------------------------------------------------------------
bool CHttpChannel::SendHead(int64 Start)
{
    //第一行：方法、请求路径、版本
    strcat(m_Head,"GET ");
    strcat(m_Head,(m_Resource->m_strPath).c_str());
    strcat(m_Head," HTTP/1.1");
    strcat(m_Head,"\r\n");

    //第二行：获得主机名
    strcat(m_Head,"Host: ");
    strcat(m_Head,(m_Resource->m_strHostName).c_str());
    strcat(m_Head,"\r\n");

    //第三行： 接收文件类型
    strcat(m_Head,"Accept: */*");
    strcat(m_Head,"\r\n");

    //第四行：浏览器类型
    strcat(m_Head,"User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.0; .NET CLR 1.1.4322)");
    strcat(m_Head,"\r\n");

    //第五行：判断资源来处，防止盗链
    strcat(m_Head,"Referer: ");
    strcat(m_Head,(m_Resource->m_strRefe).c_str());
    strcat(m_Head,"\r\n");
    
     //第六行：服务端的Cookie
    if((m_Resource->m_ServerResetCookie)!="")
    {
      strcat(m_Head,"Cookie: ");
      strcat(m_Head,(m_Resource->m_ServerResetCookie).c_str());
      strcat(m_Head,"\r\n");
    }
   //第七行：本地Cookie
    if(m_Resource->m_LocationCookie!="")
    {
      strcat(m_Head,"Cookie: ");
      strcat(m_Head,(m_Resource->m_LocationCookie).c_str());
      strcat(m_Head,"\r\n");
    }

    //第八行：缓存控制
    strcat(m_Head,"Cache-Control: no-cache");
    strcat(m_Head,"\r\n");

    //第九行：缓存
    strcat(m_Head,"Pragma: no-cache");
    strcat(m_Head,"\r\n");

    //第十行：连接设置
    strcat(m_Head,"Connection: close");
    strcat(m_Head,"\r\n");

    //第一十一行:请求的数据起始字节位置，断点续传
    if(Start>0)
    {
      char Temp[25];
      strcat(m_Head,"Range: bytes=");
      _i64toa(Start,Temp,10);
      strcat(m_Head,Temp);
      strcat(m_Head,"-");
    }
    strcat(m_Head,"\r\n");
    //第一十二行:  空行
    strcat(m_Head,"\r\n");

    int HeadLen = strlen(m_Head);
    if(m_Connection.WriteBuffer(m_Head,HeadLen)==HeadLen)
    {
      memset(m_Head,0,HEADLEN);
      return  true;
    }
    return false;
}

