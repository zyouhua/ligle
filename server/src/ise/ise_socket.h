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
// ��ǰ����

class CTcpServer;
class CListenerThread;
class CUdpListenerThread;
class CUdpListenerThreadPool;
class CTcpListenerThread;

///////////////////////////////////////////////////////////////////////////////
// ���Ͷ���

typedef struct sockaddr_in SockAddr;

// IP:Port
struct PeerAddress
{
    uint nIp;       // IP (�����ֽ�˳��)
    int nPort;      // �˿�

    PeerAddress() : nIp(0), nPort(0) {}
    PeerAddress(uint _nIp, int _nPort) 
    { nIp = _nIp;  nPort = _nPort; }
};

///////////////////////////////////////////////////////////////////////////////
// �����

string IpToString(int nIp);
int StringToIp(const string& strString);
string GetLocalIp();
void GetSocketAddr(SockAddr& SockAddr, const string& strHost, int nPort);
void GetSocketAddr(SockAddr& SockAddr, uint nIpHostValue, int nPort);

///////////////////////////////////////////////////////////////////////////////
// class CSocketException - �����쳣��
//
// ע: ���е���������쳣��ʹ�ø�������������ࡣ

class CSocketException : public CException
{
public:
    CSocketException(int nErrorNo, const char *sErrorMsg, const char *sFileName = NULL, int nLineNo = EMPTY_LINE_NO) :
		CException(nErrorNo, sErrorMsg, sFileName, nLineNo) {}
    CSocketException(const char *sErrorMsg, const char *sFileName = NULL, int nLineNo = EMPTY_LINE_NO) :
	    CException(sErrorMsg, sFileName, nLineNo) {}
};

///////////////////////////////////////////////////////////////////////////////
// class CInetAddress - �����ַ��

class CInetAddress
{
protected:
    uint m_nAddr;   // ���ֽ����ͣ������ֽ�˳��

    /* in_addr �ṹ���� (netinet/in.h):
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
// class CSocket - �׽�����

class CSocket
{
friend class CTcpServer;

protected:
    bool m_bActive;     // �׽����Ƿ�׼������
    int m_nHandle;      // �׽��־��
    int m_nDomain;      // �׽��ֵ�Э����� (PF_UNIX, PF_INET, PF_INET6, PF_IPX, ...)
    int m_nType;        // �׽������ͣ�����ָ�� (SOCK_STREAM, SOCK_DGRAM, SOCK_RAW, SOCK_RDM, SOCK_SEQPACKET)
    int m_nProtocol;    // �׽�������Э�飬��Ϊ0 (IPPROTO_IP, IPPROTO_UDP, IPPROTO_TCP, ...)
    bool m_bBlockMode;  // �Ƿ�Ϊ����ģʽ (ȱʡΪ����ģʽ)

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
// class CUdpSocket - UDP �׽�����

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
// class CUdpClient - UDP Client ��

class CUdpClient : public CUdpSocket
{
public:
    CUdpClient() { Open(); }
};

///////////////////////////////////////////////////////////////////////////////
// class CUdpServer - UDP Server ��

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

    // �¼�����: �յ����ݰ�
    virtual void OnRecvData(void *pPacketBuffer, int nPacketSize, const PeerAddress& RemoteAddr) {}

    int GetLocalPort() { return m_nLocalPort; }
    void SetLocalPort(int nValue);

    int GetListenerThreadCount();
    void SetListenerThreadCount(int nValue);
};

///////////////////////////////////////////////////////////////////////////////
// class CTcpSocket - TCP �׽�����

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
// class CTcpConnection - TCP Connection ��

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
// class CTcpClient - TCP Client ��

class CTcpClient : public CTcpConnection
{
public:
    void Connect(const string& strHost, int nPort);
};

///////////////////////////////////////////////////////////////////////////////
// class CTcpServer - TCP Server ��

class CTcpServer
{
public:
    enum { LISTEN_QUEUE_SIZE = 15 };   // TCP�������г���

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

    // �¼�����: �յ����� (ע: pConnection �ǶѶ�����ʹ�����ͷ�)
    virtual void OnConnect(CTcpConnection *pConnection) {}
};

///////////////////////////////////////////////////////////////////////////////
// class CListenerThread - �����߳���

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
// class CUdpListenerThread - UDP�����������߳���

class CUdpListenerThread : public CListenerThread
{
private:
    CUdpListenerThreadPool *m_pThreadPool;  // �����̳߳�
    CUdpServer *m_pUdpServer;               // ����UDP������
    int m_nIndex;                           // �߳��ڳ��е�������(0-based)
protected:
    virtual void Execute();
public:
    explicit CUdpListenerThread(CUdpListenerThreadPool *pThreadPool, int nIndex);
    virtual ~CUdpListenerThread();
};

///////////////////////////////////////////////////////////////////////////////
// class CUdpListenerThreadPool - UDP�����������̳߳���

class CUdpListenerThreadPool
{
private:
    CUdpServer *m_pUdpServer;               // ����UDP������
    CList m_ThreadList;                     // �߳��б�
    int m_nMaxThreadCount;                  // ��������߳�����
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

    // ��������UDP������
    CUdpServer& GetUdpServer() { return *m_pUdpServer; }
};

///////////////////////////////////////////////////////////////////////////////
// class CTcpListenerThread - TCP�����������߳���

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
// class CTcpConnectionWarp - ��װTCP���ӣ������д����

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
    // ���ò������������Ҫ��ȡ����д��ֻ��Ҫ����Ӧ�ĳ�������Ϊ0��
    void SetParam(CTcpConnection* pTcpConnection,
        char* pReadBuffer, int ReadLength,
        char* pWriteBuffer, int WriteLength);

    ///////////////////////////////////////////////////////////////////////////
    // �������ݣ�������յ�Ԥ�ڵ������򷵻�true
    bool IsDoneReadBuffer();

    ///////////////////////////////////////////////////////////////////////////
    // �������ݣ�������ݷ�����Ϸ���true
    bool IsDoneWriteBuffer();

    ///////////////////////////////////////////////////////////////////////////
    // ��������Ҫ��ȡ���ֽ����������Ѿ���ȡ��һЩ���ݣ�
    void AdjustReadLength(int ReadLength);

    ///////////////////////////////////////////////////////////////////////////
    // ��������Ҫд�����ֽ����������Ѿ�д����һЩ���ݣ�
    void AdjustWriteLength(int WriteLength);

    CTcpConnection* GetTcpConnection() { return m_pTcpConnection; };
};

///////////////////////////////////////////////////////////////////////////////

} // namespace ise

#endif // _ISE_SOCKET_H_ 
