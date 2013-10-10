/////////////////////////////////////////////////////////////////////////////
//tracker.h

#ifndef _TRACKER_H_
#define _TRACKER_H_

#include "trackerserver.h"
#include "trackerpacket.h"
#include "core_metafile.h"
#include "icl_sysutils.h"
#include "icl_handler.h"

const int TK_TIMEOUT = 30000;          //���ݰ���ʱʱ��,(����)

    //��װ�˿ͻ�����������������ݰ������
struct CTrackerRequest
{
	CUdpBizPacket *pPacket;
	bool State;
	int Size;
	int Times;
	int LastSend;
	int SeqNumber;
};

typedef list<CTrackerRequest*> CRequestList;
typedef CRequestList::iterator CRequestIterator;

class CTracker : public CThreadHandler
{
private:
  CRequestList m_RequestList;       //���ݰ��������
  CPeerAddress m_TrackerAddr;
  CSeqAllocator m_SeqAllocator;     //���кŷ�����
  CCriticalSection m_Lock;
  bool m_State;                     //�û�����״̬
  int m_Times;                      //�߳�ִ�д���
  char * strUserName ;
  char * strUserPassword;
private:
  void AddRequest(CUdpBizPacket* Packet);
  void HandleRequest();
  void SendPacket(CTrackerRequest* Request);
  void ResetList();
  void UploadPack(int Size, CUploadList& UpLoadList);
public:
  CTracker();
  virtual ~CTracker();

  void Start();
  void Stop();

  virtual void Handle();     //�������߳�ִ�к������ڹ涨ʱ���ﵽ�����﷢��������һ��ʱ������������

  //���ܿͻ��˵Ĳ������,���ҽ���õİ�����������
  void KeepAlive();
  void Integral();
  void Logout();
  void UploadFile(int nFiles, CMetaList& MetaList);
  void Downloaded(string Url, int64 FileLength, string FileCode);
  void AskResource(string Url, string Ref, int64 FileLength, int TaskID);

  void Back(int SeqNumber);   //�����ݰ�����������
  void SetstrUserName(const char strUserName[10]);
  char * GetstrUserName() {return  strUserName; };
  void SetstrUserPassword(const char strUserPassword[10]);
  char * GetstrUserPassword() {return strUserPassword; };
  CSeqAllocator& GetAllocator(){ return m_SeqAllocator; }     //ȡ�����кŷ�����
};

extern CTracker Tracker;

#endif
