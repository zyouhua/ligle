////////////////////////////////////////////////////////////////////////////////
// P2P Client Eegineering (P2P_CE)
// core_ftpchannel.cpp
//最后修改时间：9-11-2006
////////////////////////////////////////////////////////////////////////////////

#include "core_ftpchannel.h"
#include "icl_classes.h"
#include "icl_sysutils.h"


////////////////////////////////////////////////////////////////////////////////
//   class CFtpCtrlConnection

//------------------------------------------------------------------------------
//构造函数
//------------------------------------------------------------------------------
void CFtpCtrlConnection::Clear()
{
  memset(m_Buffer,0,CMDLEN);
  Disconnect();
}

//------------------------------------------------------------------------------
//   发送命令
//------------------------------------------------------------------------------
bool CFtpCtrlConnection::SendCommand(char* Cmd)
{
  memset(m_Buffer,0,CMDLEN);
  strcpy(m_Buffer,Cmd);
  strcat(m_Buffer,"\r\n");
	int Len=strlen(m_Buffer);
	if(WriteBuffer(m_Buffer,Len)==Len)
  {
    memset(m_Buffer,0,CMDLEN);
    return  true;
  }
  return  false;
}

//------------------------------------------------------------------------------
//   取得服务器返回的状态
//------------------------------------------------------------------------------
int CFtpCtrlConnection::GetResponseState()
{
  string strTemp="";
  int nPos=0;
  string strCmdLine=GetResponseCmd();
  if(!(strCmdLine.length()>0)) return 0;
  while((nPos=strCmdLine.find("\r\n",0))>0)
  {
    strTemp=strCmdLine.substr(0,nPos+2);
    if(strTemp.at(3)!='-')
      return atoi(strTemp.c_str());
    strCmdLine=strCmdLine.substr(nPos+2,strCmdLine.length());
  }
  return 0;
}
string CFtpCtrlConnection::GetResponseCmd()
{
  string strTemp="";
  memset(m_Buffer,0,CMDLEN);
  if(ReadBuffer(m_Buffer,CMDLEN))
    strTemp=m_Buffer;
  return strTemp;
  /*
  memset(m_Buffer,0,CMDLEN);
  if(ReadBuffer(m_Buffer,CMDLEN)) return atoi(m_Buffer);
  return 0;
  */
}
//-----------------------------------------------------------------------------
//  取得数据连接的端口
//-----------------------------------------------------------------------------
int CFtpCtrlConnection::GetPasvPort()
{
	int pos=((string)m_Buffer).find(')',0);
	int pos1=((string)m_Buffer).find_last_of(',',pos);
	int pos2=((string)m_Buffer).find_last_of(',',pos1-1);
	int port1=atoi(&m_Buffer[pos2+1]);
  int port2=atoi(&m_Buffer[pos1+1]);
	return (port1*256+port2);
}

//------------------------------------------------------------------------------
//   取得文件长度
//------------------------------------------------------------------------------
int64 CFtpCtrlConnection::GetFileLength()
{
	int pos=((string)m_Buffer).find("213",0);
  return atof(&m_Buffer[pos+3]);
}

////////////////////////////////////////////////////////////////////////////////
//  class CFtpChannel

//------------------------------------------------------------------------------
//  构造函数
//------------------------------------------------------------------------------
CFtpChannel::CFtpChannel(CResource* Resource,CTask* Task):CChannel(Resource,Task)
{
	m_State=FTPINIT;
}

//------------------------------------------------------------------------------
//  处理函数,由状态决定作什么，还需要修改
//------------------------------------------------------------------------------
void CFtpChannel::Handle()
{
  if(!m_Task->Check()) throw CException();
  switch(m_State)
  {
	  case FTPINIT:    OnInit(); break;
    case FTPCONNECT: OnConnect(); break;
    case FTPSHAKE:   OnShake(); break;
    case FTPUSER:    OnUser(); break;
    case FTPCWD:     OnCwd();  break;
    case FTPTYPE:    OnType(); break;
    case FTPSIZE:    OnSize(); break;
	  case FTPPASV:    OnPasv(); break;
    case FTPDATACONNECT: OnDataConnect(); break;
    case FTPREST:    OnRest(); break;
    case FTPRETR:    OnRetr(); break;
    case FTPRECEIVE: OnReceive(); break;
    default: break;
  }
}

void CFtpChannel::OnInit()
{
  m_CtrlConnection.ConnectNonBlock(m_Resource->m_strIp,m_Resource->m_nPort);
  SetState(FTPCONNECT);
}

void CFtpChannel::OnConnect()
{
  if(m_CtrlConnection.IsConnect())
  {
    SetState(FTPSHAKE);
    return;
  }
  CheckTimeOut();
}

void CFtpChannel::OnShake()
{
  int State=m_CtrlConnection.GetResponseState();
  if(State==0) { CheckTimeOut(); return; }
  if(State==220)
  {
    m_CtrlConnection.SendCommand("USER anonymous");
    SetState(FTPUSER);
    return;
  }
  DoOnError(State);
}

void CFtpChannel::OnUser()
{
  int State=m_CtrlConnection.GetResponseState();
  if(State==0) { CheckTimeOut(); return; }
  if(State==230||State==220)
  {
    int pos=m_Resource->m_strPath.find_last_of('/',m_Resource->m_strPath.length());
    string path=m_Resource->m_strPath.substr(1,pos);
    char cmd[1024];
    memset(cmd,0,1024);
    strcpy(cmd,"CWD ");
    strcat(cmd,path.c_str());
    m_CtrlConnection.SendCommand(cmd);
    SetState(FTPCWD);
    return;
  }
  else if(State==331)
  {
    char cmd[1024];
    memset(cmd,0,1024);
    strcpy(cmd,"PASS ");
    strcat(cmd,"ligle");
    m_CtrlConnection.SendCommand(cmd);
    return;
  }
  DoOnError(State);
}

void CFtpChannel::OnCwd()
{
  int State=m_CtrlConnection.GetResponseState();
  if(State==0) { CheckTimeOut(); return; }
  if(State==250)
  {
    m_CtrlConnection.SendCommand("TYPE I");
    SetState(FTPTYPE);
    return;
  }
  DoOnError(State);
}

void CFtpChannel::OnType()
{
  int State=m_CtrlConnection.GetResponseState();
  if(State==0) { CheckTimeOut(); return; }
  if(State==200)
  {
    int pos=m_Resource->m_strPath.find_last_of('/',m_Resource->m_strPath.length());
    string file=m_Resource->m_strPath.substr(pos+1,m_Resource->m_strPath.length());
    char cmd[1024];
    memset(cmd,0,1024);
    strcat(cmd,"SIZE ");
    strcat(cmd,file.c_str());
    m_CtrlConnection.SendCommand(cmd);
    SetState(FTPSIZE);
    return;
  }
  DoOnError(State);
}

void CFtpChannel::OnSize()
{
  int State=m_CtrlConnection.GetResponseState();
  if(State==0) { CheckTimeOut(); return; }
  if(State==213)
  {
    m_FileLen=m_CtrlConnection.GetFileLength();
    m_CtrlConnection.SendCommand("PASV ");
    SetState(FTPPASV);
    return;
  }
  DoOnError(State);
}

void CFtpChannel::OnPasv()
{
  int State=m_CtrlConnection.GetResponseState();
  if(State==0) { CheckTimeOut(); return; }
  if(State==227)
  {
    m_DataPort=m_CtrlConnection.GetPasvPort();
    m_DataConnection.ConnectNonBlock(m_Resource->m_strIp,m_DataPort);
    SetState(FTPDATACONNECT);
    return;
  }
  DoOnError(State);
}

void CFtpChannel::OnDataConnect()
{
  if(m_DataConnection.IsConnect())
  {
    if(!GetBlock())
    {
      if(!SetLength(m_FileLen)) return;
      GetBlock();
    }
    char size[30];
    _i64toa(m_Block->m_nStart,size,10);
    char cmd[1024];
    memset(cmd,0,1024);
    strcat(cmd,"REST ");
    strcat(cmd,size);
    m_CtrlConnection.SendCommand(cmd);
    SetState(FTPREST);
  }
  CheckTimeOut();
}

void CFtpChannel::OnRest()
{
  int State=m_CtrlConnection.GetResponseState();
  if(State==0) { CheckTimeOut(); return; }
  if(State==350)
  {
    int pos=m_Resource->m_strPath.find_last_of('/',m_Resource->m_strPath.length());
    string file=m_Resource->m_strPath.substr(pos+1,m_Resource->m_strPath.length());
    char cmd[1024];
    memset(cmd,0,1024);
    strcat(cmd,"RETR ");
    strcat(cmd,file.c_str());
    m_CtrlConnection.SendCommand(cmd);
    SetState(FTPRETR);
    return;
  }
  DoOnError(State);
}

void CFtpChannel::OnRetr()
{
  int State=m_CtrlConnection.GetResponseState();
  if(State==0) { CheckTimeOut(); return; }
  if(State==150)
  {
    SetState(FTPRECEIVE);
    return;
  }
  DoOnError(State);
}

void CFtpChannel::OnReceive()
{
    int Readed = m_DataConnection.ReadBuffer(&m_Buffer[m_nPieceIndex],m_nPieceRemain);
    if(Readed)
    {
       m_nLastTicks=GetCurTicks();
       m_nPieceIndex+=Readed;
       m_nPieceRemain-=Readed;
       if(m_nPieceRemain==0)
       {
          Save();
          if(m_Block->IsComplete())
          {
            m_CtrlConnection.SendCommand("QUIT ");
            m_DataConnection.Disconnect();
            SetState(FTPPASV);
            return;
          }
          SetPiece();
       }
    }
    CheckTimeOut();
}

