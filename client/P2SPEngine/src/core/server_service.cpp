
#include "server_service.h"
#include "server_serverchannel.h"

void CPeerService::Handle(CTcpConnection *TcpConnection)
{
  CServerChannel *ServerChannel = new CServerChannel(TcpConnection);
  ServerChannelPool.AddChannel(ServerChannel);
}

