////////////////////////////////////////////////////////////////////////////////
//CTrackerService.cpp
////////////////////////////////////////////////////////////////////////////////
#include "trackerservice.h"

////////////////////////////////////////////////////////////////////////////////
// �յ��������������û�IP
CTSGetIp::CTSGetIp()
{
  m_nActionCode = SC_FINDER_GET_IP;
}

void CTSGetIp::Handle(void* Buffer, int Size)
{
   //���
  CReGetUserIpPacket opPacket;
  opPacket.Unpack(Buffer, Size);

  int count = opPacket.UserList.size();

  int TaskId = opPacket.nTaskId;
  int SeqNumber = opPacket.Header.nSeqNumber;
  Tracker.Back(SeqNumber);    //����

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
// �յ�������������URL
CTSGetUrl::CTSGetUrl()
{
  m_nActionCode = SC_FINDER_GET_URL;
}

void CTSGetUrl::Handle(void* Buffer, int Size)
{
  //���
  CReGetUrlPacket opPacket;
  opPacket.Unpack(Buffer, Size);

  //�õ����е���Դ
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
// �յ�������������: �ϴ���Դ�б�
CTSUpload::CTSUpload()
{
  m_nActionCode = SC_FINDER_REQUEST_FILE;
}

void CTSUpload::Handle(void* Buffer, int Size)
{
  //���
  CRequestFilePacket  opPacket;
  opPacket.Unpack(Buffer, Size);

  CMetaList MetaList;
  int nCount= MetaFile.Load(MetaList,1);
  if( nCount ) Tracker.UploadFile( nCount, MetaList);
}

////////////////////////////////////////////////////////////////////////////////
//�յ�ͨ��Ӧ���
CTSAck::CTSAck()
{
  m_nActionCode = SC_FINDER_ACK;
}

void CTSAck::Handle(void* Buffer, int Size)
{
  //���
  CReOnPacket  opPacket;
  opPacket.Unpack(Buffer, Size);
  int SeqNumber = opPacket.Header.nSeqNumber;
  Tracker.Back(SeqNumber);
}

////////////////////////////////////////////////////////////////////////////////
// �յ������������Ŀհ�(����Դ)
CTSNoop::CTSNoop()
{
  m_nActionCode = SC_FINDER_NONE_Resource;
}

void CTSNoop::Handle(void* Buffer, int Size)
{
  //���
  CReNoopPacket opPacket;
  opPacket.Unpack(Buffer, Size);
  int SeqNumber = opPacket.Header.nSeqNumber;
  Tracker.Back(SeqNumber);
}

////////////////////////////////////////////////////////////////////////////////
//  �յ������������Ļ��ְ���

CTSIntegral::CTSIntegral()
{
    m_nActionCode = SC_FINDER_INTEGRAL_Resource;
}

void CTSIntegral::Handle(void * Buffer,int size)
{
  CReIntegralPacket  opPacket;
  opPacket.Unpack(Buffer, size);
  int nIntegral = opPacket.nIntegral;  //�õ����ֺ���δ���ȥ��
  int SeqNumber = opPacket.Header.nSeqNumber;
  Tracker.Back(SeqNumber);
}

////////////////////////////////////////////////////////////////////////////////