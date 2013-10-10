////////////////////////////////////////////////////////////////////////////////
// P2P Client Eegineering (P2P_CE)
// core_peerchannel.cpp
// ×îºóÐÞ¸Ä£º2006.11.27
////////////////////////////////////////////////////////////////////////////////

#include "core_peerchannel.h"
#include "icl_sysutils.h"

CPeerChannel::CPeerChannel(CResource *Resource,CTask *Task):CChannel(Resource,Task)
{
  m_State = PEERINIT;
}

void CPeerChannel::Handle()
{
  if(!m_Task->Check()) throw CException();
  switch(m_State)
  {
    case PEERINIT:
         OnInit();
         break;
    case PEERCONNENCT:
         OnConnect();
         break;
    case PEERGETSTATE:
         OnGetState();
         break;
    case PEERRECEIVE:
         OnReceive();
         break;
  }
}

void CPeerChannel::OnInit()
{
  m_Connection.ConnectNonBlock(m_Resource->m_strIp,m_Resource->m_nPort);
  SetState(PEERCONNENCT);
}

void CPeerChannel::OnConnect()
{
  if(m_Connection.IsConnect())
  {
    GetBlock();
    SendHead();
    SetState(PEERGETSTATE);
    return;
  }
  CheckTimeOut();
}

void CPeerChannel::SendHead()
{
  strcpy(m_Request.FileCode,(m_Task->GetCode()).c_str());
  m_Request.FileLength=m_Task->GetLength();
  m_Request.Start=m_Block->m_nStart;
  m_Request.End=m_Block->m_nEnd;
  m_Connection.WriteBuffer(&m_Request,sizeof(m_Request));
}

void CPeerChannel::OnGetState()
{
  if(!m_Connection.ReadBuffer(&m_Back,sizeof(m_Back)))
  {
    CheckTimeOut();
    return;
  }
  if(m_Back.State==200)
  {
    SetState(PEERRECEIVE);
    return;
  }
  DoOnError(m_Back.State);
}

void CPeerChannel::OnReceive()
{
  int Readed = m_Connection.ReadBuffer(&m_Buffer[m_nPieceIndex],m_nPieceRemain);
  if(!Readed)
  {
    m_NeedSleep=true;
    CheckTimeOut();
    return;
  }

  m_NeedSleep=false;
  m_nLastTicks = GetCurTicks();
  m_nPieceIndex += Readed;
  m_nPieceRemain -= Readed;
  if(m_nPieceRemain == 0)
  {
    Save();
    if(m_Block->IsComplete())
    {
      m_Connection.Disconnect();
      m_NeedSleep=true;
      SetState(PEERINIT);
      return;
    }
    SetPiece();
  }
}


