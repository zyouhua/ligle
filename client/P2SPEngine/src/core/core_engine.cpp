///////////////////////////////////////////////////////////////////////////////
// P2P Client Engineering (P2P_CE)
//
// 文件名称: core_engine.cpp
// 功能描述: 提供p2p内核功能调用
// 最后修改:
///////////////////////////////////////////////////////////////////////////////
#include "core_engine.h"
#include "core_channel.h"
#include "core_task.h"
#include "server_serverchannel.h"
#include "server_service.h"
#include "trackerservice.h"

///////////////////////////////////////////////////////////////////////////////
//class CEngine

CEngine Engine;


void CEngine::Initialize()
{
  if(!NetworkInitialize()) throw CException();

  TaskHandler.StartWorkerThread();
  ChannelHandler.StartWorkerThread();
  ServerChannelHandler.StartWorkerThread();
  Tracker.Start();
  StartTrackerServer();
  StartPeerServer();
}

void CEngine::Finalize()
{
  TaskPool.StopTasks();
  PeerTcpServer.Close();
  TrackerServer.Close();
  Tracker.Stop();
  ChannelHandler.StopWorkerThread();
  ServerChannelHandler.StopWorkerThread();
  TaskHandler.StopWorkerThread();
  MetaFile.Pack();
}

void CEngine::StartTrackerServer()
{
  CTrackerService* Service;

  Service=new CTSGetIp;
  TrackerServer.Register(Service);
  Service=new CTSGetUrl;
  TrackerServer.Register(Service);
  Service=new CTSUpload;
  TrackerServer.Register(Service);
  Service=new CTSAck;
  TrackerServer.Register(Service);
  Service=new CTSNoop;
  TrackerServer.Register(Service);

  TrackerServer.Open();
}

void CEngine::StartPeerServer()
{
  CTcpService *PeerService = new CPeerService();
  PeerTcpServer.SetService(PeerService);
  PeerTcpServer.Open();
}


