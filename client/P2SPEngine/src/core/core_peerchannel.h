////////////////////////////////////////////////////////////////////////////////
// P2P Client Eegineering (P2P_CE)
// core_peerchannel.h
// 最后修改：2006.11.27
////////////////////////////////////////////////////////////////////////////////

#ifndef _CORE_PEERCHANNEL_H_
#define _CORE_PEERCHANNEL_H_

#include "core_channel.h"
#include "icl_socket.h"

using namespace icl;

enum CPeerState
{
  PEERINIT,                          //初始状态
  PEERCONNENCT,                      //连接状态
  PEERGETSTATE,                      //分析状态
  PEERRECEIVE                       //接受数据状态
};

#pragma pack(1)

struct CPeerRequest
{
  char FileCode[METAFILE_CODE];
  int64 FileLength;
  int64 Start;
  int64 End;
  int Reserved;

};

struct CPeerBack
{
  int State;
  int Reserved;

};

#pragma pack()

class CPeerChannel : public CChannel
{
private:
  CTcpClient   m_Connection;
  CPeerRequest m_Request;
  CPeerBack    m_Back;
  void OnInit();
  void OnConnect();
  void OnGetState();
  void OnReceive();
  void SendHead();
public:
  virtual void Handle();
  CPeerChannel(CResource *Resource,CTask *Task);
  ~CPeerChannel(){m_Connection.Disconnect();}
};

#endif

