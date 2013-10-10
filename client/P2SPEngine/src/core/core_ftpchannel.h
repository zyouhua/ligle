////////////////////////////////////////////////////////////////////////////////
// P2P Client Eegineering (P2P_CE)
// core_ftpchannel.h
//最后修改时间：9-11-2006
////////////////////////////////////////////////////////////////////////////////
#ifndef _CORE_CFTPCHANNEL_H
#define _CORE_CFTPCHANNEL_H

#include "core_channel.h"
#include "icl_socket.h"
#include <string>

using namespace icl;

const int CMDLEN=2048;

//FTPchannel的状态
enum FtpState
{
	FTPINIT,
	FTPCONNECT,
  FTPSHAKE,
	FTPUSER,
	FTPCWD,
	FTPSIZE,
	FTPPASV,
	FTPREST,
	FTPDATACONNECT,
	FTPRECEIVE,
	FTPRETR,
  FTPTYPE
};

////////////////////////////////////////////////////////////////////////////////
//class CFtpCtrlConnection
//用来FTP的控制连接，继承自CTcpClient

class CFtpCtrlConnection : public CTcpClient
{
//成员变量
private:
	char m_Buffer[CMDLEN];
  string GetResponseCmd();
public:
	CFtpCtrlConnection(){Clear();}
	~CFtpCtrlConnection(){Clear();};
  void Clear();
	int GetResponseState();
	int GetPasvPort();
	int64 GetFileLength();
	bool SendCommand(char* Cmd);
};

////////////////////////////////////////////////////////////////////////////////
//class CFtpChannel
//FTP类型的通道，用来FTP下载，内有两个连接，控制连接和数据连接,继承自CChannel

class CFtpChannel : public CChannel
{
private:
	CFtpCtrlConnection m_CtrlConnection;
	CTcpClient m_DataConnection;
	int64 m_FileLen;
  int m_DataPort;
protected:
  void OnInit();
  void OnConnect();
  void OnShake();
  void OnUser();
  void OnCwd();
  void OnType();
  void OnSize();
  void OnPasv();
  void OnDataConnect();
  void OnRest();
  void OnRetr();
  void OnReceive(); 

public:
	virtual void Handle();
	CFtpChannel(CResource* Resource,CTask* Task);
	~CFtpChannel(){}
};

#endif
