#include "tracker.h"

////////////////////////////////////////////////////////////////////////////////
//全局变量

CTracker Tracker;

////////////////////////////////////////////////////////////////////////////////
//class CTracker
CTracker::CTracker()
{
  m_bThreadNeedSleep = true;      // 线程是否要休息
  m_fThreadSleepSeconds = 0.2;    // 线程每次休息的时间，秒
  m_State = true;
  m_Times = 0;

  m_TrackerAddr.nIp = StringToIp("192.168.0.108");
  m_TrackerAddr.nPort = 9100;
}

CTracker::~CTracker()
{
  Stop();
}

void CTracker::Start()
{
  StartWorkerThread();            // 启动工作者线程
}

void CTracker::Stop()
{
  StopWorkerThread();            //停止工作者线程
}

void CTracker::HandleRequest()
{
  if(!m_State) return;

  CAutoSynchronizer Syncher(m_Lock);
  CRequestIterator iter;
  CTrackerRequest* Request;

  for(iter = m_RequestList.begin(); iter != m_RequestList.end(); ++iter)
  {
    Request=(*iter);
    if(!(Request->State) ){ SendPacket(Request); continue; }

    int DiffTime = GetCurTicks() - Request->LastSend;
    if(DiffTime >= TK_TIMEOUT )
    {
      if(Request->Times == 3) { m_State = false; break; }  //重发三次,视用户下线
      SendPacket(Request);
    }
  }
}

void CTracker::SendPacket(CTrackerRequest* Request)
{
  TrackerServer.SendBuffer(Request->pPacket->GetBuffer(), Request->Size, m_TrackerAddr, 1);
  Request->LastSend = GetCurTicks();
  Request->Times++;
  Request->State = true;
}


//工作者线程执行函数
void CTracker::Handle()
{
  HandleRequest();
  if(++m_Times >= 60*5)    //每隔一分钟产生一个保活包
  {
    KeepAlive();
    m_Times = 0;
  }
}

void CTracker::AddRequest(CUdpBizPacket* Packet)
{
    CAutoSynchronizer Syncher(m_Lock);
    CTrackerRequest* Request = new CTrackerRequest();
    Request->pPacket = Packet;
    Request->State = false;
    Request->Size = Packet->GetSize();
    Request->Times = 0;
    Request->LastSend = 0;
    Request->SeqNumber = Packet->Header.nSeqNumber;
    m_RequestList.push_back(Request);
}


void CTracker::KeepAlive()
{
  CKeepAlivePacket* pKeepAlivePacket = new CKeepAlivePacket();
  pKeepAlivePacket->InitHeader();
  pKeepAlivePacket->InitData();

  TrackerServer.SendBuffer(pKeepAlivePacket->GetBuffer(), pKeepAlivePacket->GetSize(), m_TrackerAddr, 1);
}

void CTracker::Integral()
{
  CIntegralPacket* pIntegralPacket = new CIntegralPacket();
  pIntegralPacket->InitHeader();
  pIntegralPacket->InitData();

  TrackerServer.SendBuffer(pIntegralPacket->GetBuffer(), pIntegralPacket->GetSize(), m_TrackerAddr, 1);
}

void CTracker::Logout()
{
  CLogoutPacket* LogoutPacket = new CLogoutPacket();
  LogoutPacket->InitHeader();
  LogoutPacket->InitData();

  TrackerServer.SendBuffer(LogoutPacket->GetBuffer(), LogoutPacket->GetSize(), m_TrackerAddr, 1);
}

void CTracker::UploadPack(int Size, CUploadList& UpLoadList)
{
  CUploadFileListPacket* pUploadFileListPacket = new CUploadFileListPacket();
  pUploadFileListPacket->InitHeader();
  pUploadFileListPacket->InitData(Size, UpLoadList);
  AddRequest(pUploadFileListPacket);
}

void CTracker::UploadFile(int nFileCount, CMetaList& MetaList)
{
  const uint MAX_UPLOAD_FILES = 15;
  list<CMetaData*>::iterator iter;

  CUploadData UploadData;
  CUploadList UploadList;
  for(iter = MetaList.begin(); iter != MetaList.end(); iter++)
  {
    UploadData.nFileLength = (*iter)->FileSize;
    UploadData.strHashValue = (*iter)->FileCode;
    UploadList.push_back(UploadData);
    delete (*iter);

    if(UploadList.size() == MAX_UPLOAD_FILES)
    {
      UploadPack(MAX_UPLOAD_FILES, UploadList);
      UploadList.clear();
    }
  }
  if( UploadList.size()) UploadPack(UploadList.size(), UploadList);
}

void CTracker::Downloaded(string strUrl, int64 FileLength, string strHashValue)
{
    CDownloadFileCompletePacket* DownloadCompletePacket = new CDownloadFileCompletePacket();
    DownloadCompletePacket->InitHeader();
    DownloadCompletePacket->InitData(strUrl, FileLength, strHashValue);
    AddRequest(DownloadCompletePacket);
}

void CTracker::AskResource(string strUrl, string strRef, int64 FileLength, int TaskId)
{
    CRequestResourcePacket* RequestResourcePacket= new CRequestResourcePacket();
    RequestResourcePacket->InitHeader();
    RequestResourcePacket->InitData(strUrl, strRef, FileLength, TaskId);
    AddRequest(RequestResourcePacket);
}

void CTracker::Back(int SeqNumber)
{
  CAutoSynchronizer Syncher(m_Lock);
  for(CRequestIterator iter = m_RequestList.begin(); iter != m_RequestList.end(); iter++)
  {
    if(SeqNumber == (*iter)->SeqNumber)
    {
      delete (*iter)->pPacket;
      m_RequestList.erase(iter);
      break;
    }
  }
  if(!m_State) ResetList();
}

void CTracker::ResetList()
{
  for(CRequestIterator iter = m_RequestList.begin(); iter != m_RequestList.end(); iter++)
  {
    (*iter)->State=false;
    (*iter)->Times=0;
  }
  m_State=true;
}

