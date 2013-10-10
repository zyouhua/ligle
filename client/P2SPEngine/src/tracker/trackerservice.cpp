////////////////////////////////////////////////////////////////////////////////
//CTrackerService.cpp
////////////////////////////////////////////////////////////////////////////////
#include "trackerservice.h"

////////////////////////////////////////////////////////////////////////////////
// 收到服务器传来的用户IP
CTSGetIp::CTSGetIp()
{
  m_nActionCode = SC_FINDER_GET_IP;
}

void CTSGetIp::Handle(void* Buffer, int Size)
{
   //解包
  CReGetUserIpPacket opPacket;
  opPacket.Unpack(Buffer, Size);

  int count = opPacket.UserList.size();

  int TaskId = opPacket.nTaskId;
  int SeqNumber = opPacket.Header.nSeqNumber;
  Tracker.Back(SeqNumber);    //消包

  CTask *Task;
  for(int i = 0; i < count; i++)
  {
    if(! TaskPool.GetTask(TaskId,Task))  break;

    int UserIp = opPacket.UserList.at(i).nUserIp;
    int TcpPort = opPacket.UserList.at(i).wTcpPort;
    string FileCode = opPacket.strHashValue;

    Task->SetCode(FileCode);
    CResource *Resource = new CResource(IpToString(UserIp), TcpPort);
    CResourcePool* ResourcePool = Task->GetResourcePool();
    ResourcePool->AddResource(Resource);
  }
}

////////////////////////////////////////////////////////////////////////////////
// 收到服务器传来的URL
CTSGetUrl::CTSGetUrl()
{
  m_nActionCode = SC_FINDER_GET_URL;
}

void CTSGetUrl::Handle(void* Buffer, int Size)
{
  //解包
  CReGetUrlPacket opPacket;
  opPacket.Unpack(Buffer, Size);

  //得到包中的资源
  string strUrl = opPacket.strUrl;
  string strRef = opPacket.strReference;
  string FileCode = opPacket.strHashValue;
  int TaskId = opPacket.nTaskId;

  int SeqNumber = opPacket.Header.nSeqNumber;
  Tracker.Back(SeqNumber);

  CTask *Task;
  if(TaskPool.GetTask(TaskId,Task))
  {
    Task->SetCode(FileCode);
    CResource *Resource = new CResource(strUrl, strRef, true);
    CResourcePool* ResourcePool = Task->GetResourcePool();
    ResourcePool->AddResource(Resource);
  }
}

////////////////////////////////////////////////////////////////////////////////
// 收到服务器的请求: 上传资源列表
CTSUpload::CTSUpload()
{
  m_nActionCode = SC_FINDER_REQUEST_FILE;
}

void CTSUpload::Handle(void* Buffer, int Size)
{
  //解包
  CRequestFilePacket  opPacket;
  opPacket.Unpack(Buffer, Size);

  CMetaList MetaList;
  int nCount= MetaFile.Load(MetaList,1);
  if( nCount ) Tracker.UploadFile( nCount, MetaList);
}

////////////////////////////////////////////////////////////////////////////////
//收到通用应答包
CTSAck::CTSAck()
{
  m_nActionCode = SC_FINDER_ACK;
}

void CTSAck::Handle(void* Buffer, int Size)
{
  //解包
  CReOnPacket  opPacket;
  opPacket.Unpack(Buffer, Size);
  int SeqNumber = opPacket.Header.nSeqNumber;
  Tracker.Back(SeqNumber);
}

////////////////////////////////////////////////////////////////////////////////
// 收到服务器传来的空包(无资源)
CTSNoop::CTSNoop()
{
  m_nActionCode = SC_FINDER_NONE_Resource;
}

void CTSNoop::Handle(void* Buffer, int Size)
{
  //解包
  CReNoopPacket opPacket;
  opPacket.Unpack(Buffer, Size);
  int SeqNumber = opPacket.Header.nSeqNumber;
  Tracker.Back(SeqNumber);
}

////////////////////////////////////////////////////////////////////////////////
//  收到服务器回来的积分包。

CTSIntegral::CTSIntegral()
{
    m_nActionCode = SC_FINDER_INTEGRAL_Resource;
}

void CTSIntegral::Handle(void * Buffer,int size)
{
  CReIntegralPacket  opPacket;
  opPacket.Unpack(Buffer, size);
  int nIntegral = opPacket.nIntegral;  //得到积分后如何传出去。
  int SeqNumber = opPacket.Header.nSeqNumber;
  Tracker.Back(SeqNumber);
}

////////////////////////////////////////////////////////////////////////////////