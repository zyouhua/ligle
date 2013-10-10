#ifndef TRACKERSERVER_H
#define TRACKERSERVER_H

#include "icl_socket.h"
#include <vector>
#include <deque>
#include "packet_udp.h"
#include "global.h"
using  namespace icl;

// 提前声明
class CTrackerService;

typedef vector<CTrackerService*> CTSList;
typedef CTSList::iterator CTSListIter;

typedef uint64 PACKETMARK;
typedef deque<PACKETMARK> CMarkList;
typedef CMarkList::iterator CMarkListIter;

class CTrackerServer : public CUdpServer
{
private:
  CMarkList m_MarkList;
  CTSList m_ServiceList;

private:
	void AddPacketMark(PACKETMARK nPacketMark);
	bool FindPacketMark(PACKETMARK nPacketMark);

public:

  CTrackerServer();
  virtual ~CTrackerServer();
  virtual void OnRecvData(void *pPacketBuffer, int nPacketSize, const CPeerAddress& RemoteAddr); //接收丢上来的数据
  void Register(CTrackerService* TrackerService); //由TrackerService子类自己注册进容器
};

///////////////////////////////////////////////////////
// TrackerService 基类
class CTrackerService
{
protected:
  int m_nActionCode;

public:
  CTrackerService();
  virtual ~CTrackerService();
  virtual void Handle(void* Buffer, int Size){}   //子类业务执行函数
  virtual bool Check(int nActionCode) {return (m_nActionCode==nActionCode);}
};

extern CTrackerServer TrackerServer;

#endif