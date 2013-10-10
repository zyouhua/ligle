///////////////////////////////////////////////////////////////////////////////
// P2P Client Engineering (P2P_CE)
//
// �ļ�����: core_channel.cpp
// ��������: �ṩp2p�ں˹��ܵ���
// ����޸�:
///////////////////////////////////////////////////////////////////////////////

#include "core_channel.h"
#include "icl_sysutils.h"

////////////////////////////////////////////////////////////////////////////////
//CChannelPool��ȫ�ֶ���

CChannelPool ChannelPool;

CChannelHandler ChannelHandler;
////////////////////////////////////////////////////////////////////////////////
//class CChannel

CChannel::CChannel(CResource *Resource,CTask *Task)
{
    m_Resource = Resource;
    m_Resource->Attach();
    m_Task = Task;
    m_Task->Attach();
    m_Block=NULL;
    memset(m_Buffer,0,PIECELEN);
    m_nBlockStart=0;
    m_nPieceIndex=0;
    m_nPieceRemain=PIECELEN;
    m_NeedSleep=true;
}

void CChannel::SetState(int nState)
{
    m_State = nState;
    m_nLastTicks = GetCurTicks();
}

void CChannel::CheckTimeOut()
{
  if (GetTickDiff(m_nLastTicks, GetCurTicks()) > STATE_TIMEOUT)
  {
    m_Resource->SetError(RS_TIMEOUT);
    throw CException();
  }
}

void CChannel::DoOnError(int Error)
{
  m_Resource->SetError(Error);
  throw CException();
}

void CChannel::Save()
{
  if(m_nBlockStart!=(m_Block->m_nStart)) throw CException();

  m_Task->Save(m_nBlockStart,m_nPieceIndex,m_Buffer);
  m_Block->UpDate(m_nPieceIndex);
}

bool CChannel::SetLength(int64 FileLen)
{
  if(!(m_Task->SetLength(FileLen))) return false;
  m_Resource-> LenghtGeted();
  return true;
}

void CChannel::SetPiece()
{
  m_nBlockStart = m_Block->m_nStart;
  m_nPieceIndex=0;
  m_nPieceRemain = (m_Block->m_nSize>PIECELEN ? PIECELEN: m_Block->m_nSize);
}


bool CChannel::GetBlock()
{
    if(!m_Task->GetBlock(m_Block)) throw CException();   //completed

    if(m_Block)
    {
       SetPiece();
       return true;
    }
    return false;
}

CChannel::~CChannel()
{
  if(m_Block) m_Block->Detach();
  m_Resource->Detach();
  m_Task->Detach();
  ChannelPool.RemoveChannel(this);
}



////////////////////////////////////////////////////////////////////////////////
//class CChannelPool

//-----------------------------------------------------------------------------
//������HandleAllChannel�ǵ���������channel������ִ��Handle
//-----------------------------------------------------------------------------
bool CChannelPool::HandleAllChannels()
{
  CAutoSynchronizer AutoSyncher(m_Lock);
  list<CChannel*>::iterator iter;    //����һ��list�����ĵ�����
  CChannel *pChannel=NULL;
  bool NeedSleep=true;
  for(iter = m_Channels.begin();iter!=m_Channels.end();)
  {
    pChannel=(*iter++);
    try
    {
      pChannel->Handle();
      NeedSleep=NeedSleep && (pChannel->NeedSleep());
    }
    catch (CException& e)
    {
      delete pChannel; // �����κ��쳣������������
    }
  }
  return NeedSleep;
}

//-----------------------------------------------------------------------------
//������AddChannel����������һ�� CChannel
//-----------------------------------------------------------------------------
void CChannelPool::AddChannel(CChannel* pChannel)
{
  CAutoSynchronizer AutoSyncher(m_Lock);
  m_Channels.push_back(pChannel);
}

void CChannelPool::RemoveChannel(CChannel* pChannel)
{
  m_Channels.remove(pChannel);
}

/////////////////////////////////////////////////////
// class CChannelHandle�߳�����ͨ��������

void CChannelHandler::Handle()
{
  m_bThreadNeedSleep=ChannelPool.HandleAllChannels();
}

