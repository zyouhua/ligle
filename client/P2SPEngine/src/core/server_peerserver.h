////////////////////////////////////////////////////////////////////////////////
// P2P Client Eegineering (P2P_CE)
// ����޸ģ�
////////////////////////////////////////////////////////////////////////////////

#ifndef _SERVER_PEERSERVER_H_
#define _SERVER_PEERSERVER_H_

#include "icl_socket.h"
#include "global.h"

using namespace icl;
//////////////////////////////////////////////////////////////////////////////
//CTcpService -- ������
class CTcpService
{
public:
  virtual void Handle(CTcpConnection* TcpConnection){}
  CTcpService(){}
  virtual ~CTcpService(){}
};

////////////////////////////////////////////////////////////////////////////////
//CTcpServer������

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
//CPeerTcpServerȫ�ֱ���

extern CPeerTcpServer  PeerTcpServer;

#endif
