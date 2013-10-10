///////////////////////////////////////////////////////////////////////////////
// Iris Server Engine (ISE)
// ise_socket.h
// Classes:
//   > CSocketException
//   > CInetAddress
//   > CSocket
//   > CUdpSocket
//   > CUdpClient
//   > CUdpServer
//   > CTcpSocket
//   > CTcpConnection
//   > CTcpClient
//   > CTcpServer
//   > CListenerThread
//   > CUdpListenerThread
//   > CUdpListenerThreadPool
//   > CTcpListenerThread
//   > CTcpConnectionWarp
///////////////////////////////////////////////////////////////////////////////

#ifndef _ISE_SOCKET_H_
#define _ISE_SOCKET_H_

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/param.h> 
#include <sys/ioctl.h> 
#include <net/if_arp.h> 
#include <net/if.h> 
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>

#include "ise_classes.h"

using namespace ise;

namespace ise
{

///////////////////////////////////////////////////////////////////////////////
// 提前声明

class CTcpServer;
class CListenerThread;
class CUdpListenerThread;
class CUdpListenerThreadPool;
class CTcpListenerThread;

///////////////////////////////////////////////////////////////////////////////
// 类型定义

typedef struct sockaddr_in SockAddr;

// IP:Port
struct PeerAddress
{
    uint nIp;       // IP (主机字节顺序)
    int nPort;      // 端口

    PeerAddress() : nIp(0), nPort(0) {}
    PeerAddress(uint _nIp, int _nPort) 
    { nIp = _nIp;  nPort = _nPort; }
};

///////////////////////////////////////////////////////////////////////////////
// 杂项函数

string IpToString(int nIp);
int StringToIp(const string& strString);
string GetLocalIp();
void GetSocketAddr(SockAddr& SockAddr, const string& strHost, int nPort);
void GetSocketAddr(SockAddr& SockAddr, uint nIpHostValue, int nPort);

///////////////////////////////////////////////////////////////////////////////
// class CSocketException - 网络异常类
//
// 注: 所有的网络操作异常都使用该类或它的派生类。

class CSocketException : public CException
{
public:
    CSocketException(int nErrorNo, const char *sErrorMsg, const char *sFileName = NULL, int nLineNo = EMPTY_LINE_NO) :
		CException(nErrorNo, sErrorMsg, sFileName, nLineNo) {}
    CSocketException(const char *sErrorMsg, const char *sFileName = NULL, int nLineNo = EMPTY_LINE_NO) :
	    CException(sErrorMsg, sFileName, nLineNo) {}
};

///////////////////////////////////////////////////////////////////////////////
// class CInetAddress - 网络地址类

class CInetAddress
{
protected:
    uint m_nAddr;   // 四字节整型，主机字节顺序

    /* in_addr 结构定义 (netinet/in.h):
    typedef uint32_t in_addr_t;
    struct in_addr
    { in_addr_t s_addr; };
    */

public:
    CInetAddress();
    CInetAddress(const CInetAddress& src);
    CInetAddress(const char* sHost);
    CInetAddress(uint nIpHostValue);

    uint GetHostValue() const { return m_nAddr; }
    uint GetNetValue() const { return htonl(m_nAddr); }
    struct in_addr GetInAddr() const;
    string GetString() const;

    CInetAddress& operator = (const string& strHost);
    CInetAddress& operator = (uint nIpHostValue);
    CInetAddress& operator = (const struct in_addr& addr);
};

///////////////////////////////////////////////////////////////////////////////
// class CSocket - 套接字类

class CSocket
{
friend class CTcpServer;

protected:
    bool m_bActive;     // 套接字是否准备就绪
    int m_nHandle;      // 套接字句柄
    int m_nDomain;      // 套接字的协议家族 (PF_UNIX, PF_INET, PF_INET6, PF_IPX, ...)
    int m_nType;        // 套接字类型，必须指定 (SOCK_STREAM, SOCK_DGRAM, SOCK_RAW, SOCK_RDM, SOCK_SEQPACKET)
    int m_nProtocol;    // 套接字所用协议，可为0 (IPPROTO_IP, IPPROTO_UDP, IPPROTO_TCP, ...)
    bool m_bBlockMode;  // 是否为阻塞模式 (缺省为阻塞模式)

private:
    void DoSetBlockMode(int nHandle, bool bValue);

protected:
    void SetActive(bool bValue);
    void SetDomain(int nValue);
    void SetType(int nValue);
    void SetProtocol(int nValue);

    void Bind(int nPort);
public:
    CSocket();
    virtual ~CSocket();

    virtual void Open();
    virtual void Close();

    bool GetActive() { return m_bActive; }
    int GetHandle() { return m_nHandle; }
    bool GetBlockMode() { return m_bBlockMode; }
    void SetBlockMode(bool bValue);
    void SetHandle(int nValue);
};

///////////////////////////////////////////////////////////////////////////////
// class CUdpSocket - UDP 套接字类

class CUdpSocket : public CSocket
{
public:
    CUdpSocket()
    {
        m_nType = SOCK_DGRAM;
        m_nProtocol = IPPROTO_UDP;
        m_bBlockMode = true;
    }

    int RecvBuffer(void *pBuffer, int nSize);
    int RecvBuffer(void *pBuffer, int nSize, PeerAddress& RemoteAddr);
    int SendBuffer(void *pBuffer, int nSize, const CInetAddress& ToAddr, int nToPort, int nSendTimes = 1);
};

///////////////////////////////////////////////////////////////////////////////
// class CUdpClient - UDP Client 类

class CUdpClient : public CUdpSocket
{
public:
    CUdpClient() { Open(); }
};

///////////////////////////////////////////////////////////////////////////////
// class CUdpServer - UDP Server 类

class CUdpServer : public CUdpSocket
{
private:
    int m_nLocalPort;
    CUdpListenerThreadPool *m_pListenerThreadPool;

protected:
    virtual void StartListenerThreads();
    virtual void StopListenerThreads();

public:
    CUdpServer();
    virtual ~CUdpServer();

    virtual void Open();
    virtual void Close();

    // 事件函数: 收到数据包
    virtual void OnRecvData(void *pPacketBuffer, int nPacketSize, const PeerAddress& RemoteAddr) {}

    int GetLocalPort() { return m_nLocalPort; }
    void SetLocalPort(int nValue);

    int GetListenerThreadCount();
    void SetListenerThreadCount(int nValue);
};

///////////////////////////////////////////////////////////////////////////////
// class CTcpSocket - TCP 套接字类

class CTcpSocket : public CSocket
{
public:
    CTcpSocket() 
    {
        m_nType = SOCK_STREAM;  
        m_nProtocol = IPPROTO_TCP; 
        m_bBlockMode = false;
    }
};

///////////////////////////////////////////////////////////////////////////////
// class CTcpConnection - TCP Connection 类

class CTcpConnection
{
protected:
    CTcpSocket m_Socket;
    PeerAddress m_nRemoteAddr;
    bool m_bClosedGracefully;

    void CheckDisconnect(bool bThrowException);
    void ResetConnection();
public:
    CTcpConnection();
    CTcpConnection(int nSocketHandle, const PeerAddress& RemoteAddr);
    virtual ~CTcpConnection() {}

    virtual void SendBuffer(void *pBuffer, int nSize, int nTimeoutSecs = 0);
    virtual void RecvBuffer(void *pBuffer, int nSize, int nTimeoutSecs = 0);

    virtual int WriteBuffer(void *pBuffer, int nSize);
    virtual int ReadBuffer(void *pBuffer, int nSize);

    virtual string RecvString(char chTerminalChar = '\0', int nTimeoutSecs = 0);

    virtual void Disconnect();
    virtual bool GetConnected();

    PeerAddress GetRemoteAddr() { return m_nRemoteAddr; }
};

///////////////////////////////////////////////////////////////////////////////
// class CTcpClient - TCP Client 类

class CTcpClient : public CTcpConnection
{
public:
    void Connect(const string& strHost, int nPort);
};

///////////////////////////////////////////////////////////////////////////////
// class CTcpServer - TCP Server 类

class CTcpServer
{
public:
    enum { LISTEN_QUEUE_SIZE = 15 };   // TCP监听队列长度

private:
    CTcpSocket m_Socket;
    int m_nLocalPort;
    CTcpListenerThread *m_pListenerThread;

protected:
    virtual void StartListenerThread();
    virtual void StopListenerThread();

public:
    CTcpServer();
    virtual ~CTcpServer();

    virtual void Open();
    virtual void Close();

    bool GetActive() { return m_Socket.GetActive(); }
    int GetLocalPort() { return m_nLocalPort; }
    CTcpSocket& GetSocket() { return m_Socket; }
    void SetActive(bool bValue);
    void SetLocalPort(int nValue);

    // 事件函数: 收到连接 (注: pConnection 是堆对象，需使用者释放)
    virtual void OnConnect(CTcpConnection *pConnection) {}
};

///////////////////////////////////////////////////////////////////////////////
// class CListenerThread - 监听线程类

class CListenerThread : public CThread
{
protected:
    virtual void Execute() {}
public:
    CListenerThread()
    {
        SetPolicy(THREAD_POLICY_RR);
        SetPriority(THREAD_PRIORITY_HIGH);
    }
    virtual ~CListenerThread() {}
};

///////////////////////////////////////////////////////////////////////////////
// class CUdpListenerThread - UDP服务器监听线程类

class CUdpListenerThread : public CListenerThread
{
private:
    CUdpListenerThreadPool *m_pThreadPool;  // 所属线程池
    CUdpServer *m_pUdpServer;               // 所属UDP服务器
    int m_nIndex;                           // 线程在池中的索引号(0-based)
protected:
    virtual void Execute();
public:
    explicit CUdpListenerThread(CUdpListenerThreadPool *pThreadPool, int nIndex);
    virtual ~CUdpListenerThread();
};

///////////////////////////////////////////////////////////////////////////////
// class CUdpListenerThreadPool - UDP服务器监听线程池类

class CUdpListenerThreadPool
{
private:
    CUdpServer *m_pUdpServer;               // 所属UDP服务器
    CList m_ThreadList;                     // 线程列表
    int m_nMaxThreadCount;                  // 允许最大线程数量
    CCriticalSection m_Lock;
public:
    explicit CUdpListenerThreadPool(CUdpServer *pUdpServer);
    virtual ~CUdpListenerThreadPool();

    void RegisterThread(CUdpListenerThread *pThread);
    void UnregisterThread(CUdpListenerThread *pThread);

    void CreateThreads();
    void StopThreads();

    int GetMaxThreadCount() { return m_nMaxThreadCount; }
    void SetMaxThreadCount(int nValue) { m_nMaxThreadCount = nValue; }

    // 返回所属UDP服务器
    CUdpServer& GetUdpServer() { return *m_pUdpServer; }
};

///////////////////////////////////////////////////////////////////////////////
// class CTcpListenerThread - TCP服务器监听线程类

class CTcpListenerThread : public CListenerThread
{
private:
    CTcpServer *m_pTcpServer;
protected:
    virtual void Execute();
public:
    explicit CTcpListenerThread(CTcpServer *pTcpServer);
};

///////////////////////////////////////////////////////////////////////////////
// class CTcpConnectionWarp - 封装TCP连接，方便读写数据

class CTcpConnectionWarp
{
private:
    CTcpConnection* m_pTcpConnection;
    char* m_pReadBuffer;
    int m_ReadLength;
    char* m_pWriteBuffer;
    int m_WriteLength;

    int m_DoneReadLength;
    int m_DoneWriteLength;
public:
    CTcpConnectionWarp() {} ;
    virtual ~CTcpConnectionWarp() {};

    ///////////////////////////////////////////////////////////////////////////
    // 设置参数。如果不需要读取或者写出只需要将相应的长度设置为0。
    void SetParam(CTcpConnection* pTcpConnection,
        char* pReadBuffer, int ReadLength,
        char* pWriteBuffer, int WriteLength);

    ///////////////////////////////////////////////////////////////////////////
    // 接受数据，如果接收到预期的数据则返回true
    bool IsDoneReadBuffer();

    ///////////////////////////////////////////////////////////////////////////
    // 发送数据，如果数据发送完毕返回true
    bool IsDoneWriteBuffer();

    ///////////////////////////////////////////////////////////////////////////
    // 重新设置要读取的字节数（可能已经读取了一些数据）
    void AdjustReadLength(int ReadLength);

    ///////////////////////////////////////////////////////////////////////////
    // 重新设置要写出的字节数（可能已经写出了一些数据）
    void AdjustWriteLength(int WriteLength);

    CTcpConnection* GetTcpConnection() { return m_pTcpConnection; };
};

///////////////////////////////////////////////////////////////////////////////

} // namespace ise

#endif // _ISE_SOCKET_H_ 
