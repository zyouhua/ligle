////////////////////////////////////////////////////////////////////////////////
//
//
////////////////////////////////////////////////////////////////////////////////

#include "server_serverchannel.h"
#include "core_metafile.h"
#include "icl_sysutils.h"

////////////////////////////////////////////////////////////////////////////////
//全局对象

CServerChannelPool ServerChannelPool;
CServerChannelHandler ServerChannelHandler;
////////////////////////////////////////////////////////////////////////////////
//class CServerChannel

CServerChannel::CServerChannel(CTcpConnection *TcpConnection)
{
  m_Connection = TcpConnection;
  ZeroBuffer(m_FileName,200);
  m_NeedSleep=true;
  SetState(SERVER_LISTEN);
}

CServerChannel::~CServerChannel()
{
  m_Connection->Disconnect();
  delete m_Connection;
  ServerChannelPool.RemoveChannel(this);
}

void CServerChannel::Handle()
{
  switch(m_State)
  {
    case SERVER_LISTEN:
         OnListen();
         break;
    case SERVER_SEND:
         OnSend();
         break;
  }
}

void CServerChannel::OnListen()
{
  if(!(m_Connection->ReadBuffer(&m_Request,sizeof(m_Request))))
  { CheckTimeOut(); return; }

  if(Ready())
  {
    m_Back.State=200;
    m_Connection->WriteBuffer(&m_Back,sizeof(m_Back));
    SetState(SERVER_SEND);
    return;
  }
  DoError();
}

void CServerChannel::OnSend()
{
  int Sended=m_Connection->WriteBuffer(&m_Buffer[m_Index],m_Remain);
  if(!Sended)
  {
    m_NeedSleep=true;
    CheckTimeOut();
    return;
  }

  m_NeedSleep=false;
  m_nLastTicks = GetCurTicks();
  m_Index+=Sended;
  m_Remain-=Sended;
  if(m_Remain==0)
  {
    if(m_Request.Start>=m_Request.End) throw CException();
    if(!GetBuffer()) throw CException();
  }
}

bool CServerChannel::Ready()
{
  if(!MetaFile.GetFile(m_Request.FileCode,m_FileName)) return false;
  return GetBuffer();
}

bool CServerChannel::GetBuffer()
{
  int64 BlockSize=m_Request.End-m_Request.Start;
  m_Index=0;
  m_Remain=(BlockSize>SENDLEN ? SENDLEN : BlockSize);

  CFileStream FileStream(m_FileName, FM_OPEN_READ | FM_SHARE_DENY_NONE);
  FileStream.Seek(m_Request.Start,SO_BEGINNING);
  if(FileStream.Read(m_Buffer,m_Remain)!= m_Remain) return false;
  m_Request.Start+=m_Remain;
  return true;
}

void CServerChannel::DoError()
{
  m_Back.State=400;
  m_Connection->WriteBuffer(&m_Back,sizeof(m_Back));
  throw CException();
}

void CServerChannel::SetState(int State)
{
  m_State = State;
  m_nLastTicks = GetCurTicks();
}

void CServerChannel::CheckTimeOut()
{
  if (GetTickDiff(m_nLastTicks, GetCurTicks()) > SERVER_STATE_TIMEOUT)
    throw CException();
}

////////////////////////////////////////////////////////////////////////////////
//class CPeerChannelPool

bool CServerChannelPool::HandleAllChannels()
{
  CAutoSynchronizer AutoSyncher(m_Lock);
  list<CServerChannel*>::iterator iter;
  CServerChannel *ServerChannel = NULL;
  bool NeedSleep=true;
  try
  {
    for(iter = m_ChannelList.begin();iter!=m_ChannelList.end();iter++)
    {
      ServerChannel = (*iter);
      ServerChannel->Handle();
      NeedSleep=NeedSleep &&  (ServerChannel->NeedSleep());
    }
  }
  catch(CException& e)
  {
    delete ServerChannel;
  }
  return NeedSleep;
}

void CServerChannelPool::AddChannel(CServerChannel *ServerChannel)
{
  CAutoSynchronizer AutoSyncher(m_Lock);
  m_ChannelList.push_back(ServerChannel);
}

void CServerChannelPool::RemoveChannel(CServerChannel *ServerChannel)
{
  m_ChannelList.remove(ServerChannel);
}

////////////////////////////////////////////////////////////////////////////////
//class CPeerChannelHandler

void CServerChannelHandler::Handle()
{
   m_bThreadNeedSleep=ServerChannelPool.HandleAllChannels();
}

