////////////////////////////////////////////////////////////////////////////////
// P2P Client Eegineering (P2P_CE)
// 最后修改：
////////////////////////////////////////////////////////////////////////////////

#ifndef _SERVER_PEERSERVER_H_
#define _SERVER_PEERSERVER_H_

#include "icl_socket.h"
#include "global.h"

using namespace icl;
//////////////////////////////////////////////////////////////////////////////
//CTcpService -- 服务类
class CTcpService
{
public:
  virtual void Handle(CTcpConnection* TcpConnection){}
  CTcpService(){}
  virtual ~CTcpService(){}
};

////////////////////////////////////////////////////////////////////////////////
//CTcpServer的子类

class CPeerTcpServer : public CTcpServer
{
private:
  CTcpService *m_TcpService;
public:

public:
  virtual void OnConnect(CTcpConnection* TcpConnection);
  void SetService(CTcpService* TcpService);
  CPeerTcpServer();
  virtual ~CPeerTcpServer();

};

////////////////////////////////////////////////////////////////////////////////
//CPeerTcpServer全局变量

extern CPeerTcpServer  PeerTcpServer;

#endif
