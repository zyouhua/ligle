/////////////////////////////////////////////////////////////////////////////
//tracker.h

#ifndef _TRACKER_H_
#define _TRACKER_H_

#include "trackerserver.h"
#include "trackerpacket.h"
#include "core_metafile.h"
#include "icl_sysutils.h"
#include "icl_handler.h"

const int TK_TIMEOUT = 30000;          //数据包超时时间,(豪秒)

    //封装了客户端向服务器发送数据包的情况
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
  CRequestList m_RequestList;       //数据包存放容器
  CPeerAddress m_TrackerAddr;
  CSeqAllocator m_SeqAllocator;     //序列号分配器
  CCriticalSection m_Lock;
  bool m_State;                     //用户在线状态
  int m_Times;                      //线程执行次数
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

  virtual void Handle();     //工作者线程执行函数，在规定时间里到队列里发包，并且一定时间里产生保活包

  //接受客户端的参数打包,并且将打好的包放入容器中
  void KeepAlive();
  void Integral();
  void Logout();
  void UploadFile(int nFiles, CMetaList& MetaList);
  void Downloaded(string Url, int64 FileLength, string FileCode);
  void AskResource(string Url, string Ref, int64 FileLength, int TaskID);

  void Back(int SeqNumber);   //从数据包队列中消包
  void SetstrUserName(const char strUserName[10]);
  char * GetstrUserName() {return  strUserName; };
  void SetstrUserPassword(const char strUserPassword[10]);
  char * GetstrUserPassword() {return strUserPassword; };
  CSeqAllocator& GetAllocator(){ return m_SeqAllocator; }     //取得序列号分配器
};

extern CTracker Tracker;

#endif
