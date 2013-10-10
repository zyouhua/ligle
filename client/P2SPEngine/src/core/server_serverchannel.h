////////////////////////////////////////////////////////////////////////////////
//
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _SERVER_SERVERCHANNEL_H_
#define _SERVER_SERVERCHANNEL_H_


#include "icl_socket.h"
#include "icl_classes.h"
#include "core_peerchannel.h"

using namespace icl;

const int SERVER_STATE_TIMEOUT    = 1000*10*1;       //10秒
const double SERVER_SLEEP_TIME    = 0.001;       //1毫秒

const int SENDLEN=200*1024;

enum SERVERSTATE
{
  SERVER_LISTEN,
  SERVER_SEND
};
////////////////////////////////////////////////////////////////////////////////
//class CServerChannel

class CServerChannel
{
private:
  char m_Buffer[SENDLEN];
  char m_FileName[200];
  int m_State;
  int m_Index;
  int m_Remain;
  bool m_NeedSleep;
  CTcpConnection *m_Connection;
  CPeerRequest m_Request;
  CPeerBack  m_Back;
  uint m_nLastTicks;

protected:
  void OnListen();
  void OnSend();
  void SetState(int State);
  void CheckTimeOut();
  void DoError();
  bool Ready();
  bool GetBuffer();

public:
  void Handle();
  inline bool NeedSleep(){return m_NeedSleep;}
  CServerChannel(CTcpConnection *TcpConnection);
  ~CServerChannel();
};

////////////////////////////////////////////////////////////////////////////////
//class CServerChannelPool

class CServerChannelPool
{
private:
  CCriticalSection m_Lock;
  list<CServerChannel*> m_ChannelList;
public:
  bool HandleAllChannels();
  void AddChannel(CServerChannel*);
  void RemoveChannel(CServerChannel*);
  CServerChannelPool(){}
  ~CServerChannelPool(){}
};

////////////////////////////////////////////////////////////////////////////////
//class CPeerConnectionHandler

class CServerChannelHandler : public CThreadHandler
{
protected:
  virtual void Handle();
public:
  CServerChannelHandler(){m_fThreadSleepSeconds=SERVER_SLEEP_TIME;}
  ~CServerChannelHandler(){}
};



////////////////////////////////////////////////////////////////////////////////
//全局对象

extern CServerChannelPool ServerChannelPool;
extern CServerChannelHandler ServerChannelHandler;
#endif
