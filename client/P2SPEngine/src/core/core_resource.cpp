////////////////////////////////////////////////////////////////////////////////
// P2P Client Eegineering (P2P_CE)
// core_resource.cpp
//最后修改时间：9-11-2006
////////////////////////////////////////////////////////////////////////////////

#include "core_resource.h"
#include "core_task.h"
#include "core_channel.h"
#include "core_httpchannel.h"
#include "core_ftpchannel.h"
#include "core_peerchannel.h"
#include <iostream.h>
#include <wininet.h>

#pragma comment(lib,"wininet.lib")

extern    CChannelPool ChannelPool;
////////////////////////////////////////////////////////////////////////////
//class  CResource

CResource::CResource(string strUrl,string strRef,bool bIsGetLength)
{
	m_strPath="";
	m_strIp="";
	m_strHostName="";
  m_nChannels=0;
	m_nErrorCode=0;
	m_strUrl=strUrl;
	m_strRefe=strRef;
    m_LocationCookie = "";
    m_ServerResetCookie = "";
	m_bIsGetLength=bIsGetLength;
	m_bReady=false;
  m_bUsable=false;
}

CResource::CResource(string strIP,int Port)
{
	m_strPath="";
	m_strUrl="";
	m_strRefe="";
	m_strHostName="";
  m_nChannels=0;
	m_nErrorCode=0;
	m_bReady=true;
	m_bIsGetLength=true;
  m_bUsable=true;
  m_Type=P2P;
	m_strIp=strIP;
  m_nPort=Port;
  m_nMaxChannels=1;
}

//----------------------------------------------------------------

//   处理函数
//----------------------------------------------------------------

void CResource::Handle(CTask* pTask)
{
  if(!m_bReady) Init(pTask);
  if(!m_bIsGetLength) { DoCheckLength(pTask); return; }
  if(m_bReady && !GetError() && m_nChannels<m_nMaxChannels)
    CreateChannel(pTask);
  if(!m_nChannels) m_nErrorCode=0;
}

void CResource::Init(CTask* pTask)
{
  if(!AnalyseUrl())
  {
    pTask->Stop(RS_URLERROR);
    return;
  }
  if(!GetHostIp())
  {
    pTask->Stop(RS_NOIP);
    return;
  }
  GetCookie();
}

void CResource::DoCheckLength(CTask* pTask)
{
  if(GetError())
  {
    pTask->Stop(RS_ACCESSERROR);
    return;
  }
  if(m_nChannels) return;
  CreateChannel(pTask);
}
//------------------------------------------------------------------
//   分析URL取得有用的信息
//------------------------------------------------------------------
bool CResource::AnalyseUrl()
{
  if(m_strUrl.length()<10) return false;

  int start=0;
  int end=m_strUrl.find_first_of(':',start);
  if(end<0) return false;

	string content=m_strUrl.substr(start,end);

	if(content=="http")
	{
		m_Type=HTTP;
		m_nPort=80;
    m_nMaxChannels=5;
	}
	if(content=="ftp")
	{
		m_Type=FTP;
		m_nPort=21;
    m_nMaxChannels=5;
	}

  start=end+3;
  end=m_strUrl.find_first_of('/',start);
  if(end<0) return false;

  m_strHostName=m_strUrl.substr(start,end-start);

  start=m_strHostName.find_first_of(':',0);
  if(start>0)
  {
	  content=m_strHostName.substr(start+1,5);
	  m_nPort=atoi(content.c_str());
    m_strHostName=m_strHostName.substr(0,start);
  }

  m_strPath=m_strUrl.substr(end,m_strUrl.length()-end);
  return true;
}
//-----------------------------------------------------------------
//根据URL取得ip地址
//-----------------------------------------------------------------
bool CResource::GetHostIp()
{
  string temp;
  struct hostent* pHost;
  pHost=gethostbyname(m_strHostName.c_str());
  if(!pHost) return false;

  struct in_addr ip_addr;
  memcpy(&ip_addr,pHost->h_addr_list[0],4);
  temp=inet_ntoa(ip_addr);
  m_strIp=temp;
  m_bReady=true;
  return true;
}

void CResource::ReSetURL(string strURL)
{
  if(m_bUsable) return;
  m_strUrl=strURL;
  m_bReady=false;
}

//-----------------------------------------------------------------
//获取Url的Cookies值
//----------------------------------------------------------------
void CResource::GetCookie()
{
    LPSTR szCookieValue = char(0);
    DWORD szCookieBufferSize = 0;

    Retry:
    if(!InternetGetCookie(m_strUrl.c_str(),NULL,szCookieValue,&szCookieBufferSize))
    {
        if(GetLastError() == ERROR_INSUFFICIENT_BUFFER)
        {
            delete[] szCookieValue;
            szCookieValue = new char[szCookieBufferSize+1];
            goto Retry;
        }

    }
    if(szCookieValue != NULL)
    {
        m_LocationCookie = szCookieValue;
        delete[] szCookieValue;
    }
}
//-----------------------------------------------------------------
//  创建连接
//  参数: Res  和channel相关联的资源指针
//-----------------------------------------------------------------
void CResource::CreateChannel(CTask* pTask)
{
  switch(m_Type)
  {
    case HTTP:
    {
      CChannel* pChannel=new CHttpChannel(this,pTask);
      ChannelPool.AddChannel(pChannel);
    }
      break;
    case FTP:
    {
      CChannel* pChannel=new CFtpChannel(this,pTask);
      ChannelPool.AddChannel(pChannel);
    }
      break;
    case P2P:
    {
      CChannel* pChannel=new CPeerChannel(this,pTask);
      ChannelPool.AddChannel(pChannel);
    }
      break;
    default:break;
  }
}

CResource::~CResource()
{
}

//////////////////////////////////////////////////////////////////////////////////////
//class CResourcePool


//-----------------------------------------------------------------
//添加资源
//res:资源对象指针的引用
//-----------------------------------------------------------------
void CResourcePool::AddResource(CResource* resource)
{
	m_ResourcePool.push_back(resource);
	m_nSize+=1;
}

CResourcePool::CResourcePool()
{
	m_nSize=0;
}
//-----------------------------------------------------------------
//删除资源
//-----------------------------------------------------------------
void CResourcePool::DeleteResource(CResource* &res)
{
	if(res)
	{
		delete res;
		m_ResourcePool.remove(res);
    m_nSize--;
	}
}

void CResourcePool::Clear()
{
	list<CResource*>::iterator iter;
    for(iter=m_ResourcePool.begin();iter!=m_ResourcePool.end();iter++)
	{
		if(*iter)
			delete (*iter);
  }
  m_ResourcePool.clear();

}
//-----------------------------------------------------------------
//处理资源
//-----------------------------------------------------------------
void CResourcePool::HandleResources(CTask* pTask)
{
  list<CResource*>::iterator iter;
  for(iter=m_ResourcePool.begin();iter!=m_ResourcePool.end();iter++)
    (*iter)->Handle(pTask);
}
