#include "trackerserver.h"

////////////////////////////////////////////////////////////////////////////////
//全局变量
CTrackerServer TrackerServer;

////////////////////////////////////////////////////////////////////////////////
//  CTrackerServer

CTrackerServer::CTrackerServer()
{
  m_MarkList.clear();
  SetLocalPort( GetFreePort(NPT_UDP, CLIENT_UDP_PORT_START, CHECK_PORT_TIMES));
}

CTrackerServer::~CTrackerServer()
{
  Close();
}

void CTrackerServer::AddPacketMark(PACKETMARK nPacketMark)
{
	if(m_MarkList.size()>50) m_MarkList.pop_back();
    m_MarkList.push_front(nPacketMark);
}

bool CTrackerServer::FindPacketMark(PACKETMARK nPacketMark)
{
	for(CMarkListIter iter = m_MarkList.begin(); iter != m_MarkList.end(); ++iter)
	{
		if((*iter) == nPacketMark)
			return true;
	}
  return false;
}

void CTrackerServer::Register(CTrackerService* TrackerService)
{
    m_ServiceList.push_back(TrackerService);
}

void CTrackerServer::OnRecvData(void* pPacketBuffer, int nPacketSize, const CPeerAddress& RemoteAddr)
{

  CUdpPacketHeader *pPacket = (CUdpPacketHeader *)pPacketBuffer;

  //从包中取得动作代码
  int ActionCode = pPacket->nActionCode;
  uint SeqNumber = pPacket->nSeqNumber;
  uint Order = pPacket->nOrder;

  PACKETMARK nPacketMark = SeqNumber;
  nPacketMark <<= 32;
  nPacketMark = nPacketMark + Order;

  if(FindPacketMark(nPacketMark))   return ;

  AddPacketMark(nPacketMark);
  for(CTSListIter iter = m_ServiceList.begin(); iter != m_ServiceList.end(); ++iter)
  {
    bool bState = (*iter)->Check(ActionCode);
    if(bState)
    {
      (*iter)->Handle(pPacketBuffer, nPacketSize);
      return;
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
// TrackerService 基类

CTrackerService::CTrackerService()
{
    m_nActionCode = 0;
}

CTrackerService::~CTrackerService()
{
//nothing
}
