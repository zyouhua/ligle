///////////////////////////////////////////////////////////////////////////////
// Iris Server Engine (ISE)
//
// �ļ�����: ise_socket.cpp
// ��������: ����������
// ����޸�: 2005-07-14
///////////////////////////////////////////////////////////////////////////////

#include "ise_socket.h"
#include "ise_sysutils.h"

namespace ise
{

///////////////////////////////////////////////////////////////////////////////
// �����

//-----------------------------------------------------------------------------
// ����: ����IP(�����ֽ�˳��) -> ����IP
//-----------------------------------------------------------------------------
string IpToString(int nIp)
{
#pragma pack(1)
    union CIpUnion
    {
        int nValue;
        struct
        {
            unsigned char ch1;  // nValue������ֽ�
            unsigned char ch2;
            unsigned char ch3;
            unsigned char ch4;
        } Bytes;
    } IpUnion;
#pragma pack()
    char strString[64];

    IpUnion.nValue = nIp;
    sprintf(strString, "%u.%u.%u.%u", IpUnion.Bytes.ch4, IpUnion.Bytes.ch3, 
        IpUnion.Bytes.ch2, IpUnion.Bytes.ch1);
    return strString;
}

//-----------------------------------------------------------------------------
// ����: ����IP -> ����IP(�����ֽ�˳��)
//-----------------------------------------------------------------------------
int StringToIp(const string& strString)
{
#pragma pack(1)
    union CIpUnion
    {
        int nValue;
        struct
        {
            unsigned char ch1;
            unsigned char ch2;
            unsigned char ch3;
            unsigned char ch4;
        } Bytes;
    } IpUnion;
#pragma pack()
    IntegerArray IntList;

    SplitStringToInt(strString, '.', IntList);
    if (IntList.size() == 4)
    {
        IpUnion.Bytes.ch1 = IntList[3];
        IpUnion.Bytes.ch2 = IntList[2];
        IpUnion.Bytes.ch3 = IntList[1];
        IpUnion.Bytes.ch4 = IntList[0];
        return IpUnion.nValue;
    }
    else
        return 0;
}

//-----------------------------------------------------------------------------
// ����: ��ȡ����IP
//-----------------------------------------------------------------------------
string GetLocalIp()
{
    const int MAX_INTERFACES = 16;
    string strResult;
    int nFd, nIntfCount, nIp; 
    struct ifreq buf[MAX_INTERFACES]; 
    struct ifconf ifc; 

    if ((nFd = socket(AF_INET, SOCK_DGRAM, 0)) >= 0) 
    { 
        ifc.ifc_len = sizeof(buf); 
        ifc.ifc_buf = (caddr_t) buf; 
        if (!ioctl(nFd, SIOCGIFCONF, (char*)&ifc)) 
        { 
            nIntfCount = ifc.ifc_len / sizeof(struct ifreq); 
            if (nIntfCount > 0)
            {
                ioctl(nFd, SIOCGIFADDR, (char*)&buf[nIntfCount-1]);
                nIp = ((struct sockaddr_in*)(&buf[nIntfCount-1].ifr_addr))->sin_addr.s_addr;
                strResult = IpToString(ntohl(nIp)); 
            }
        }
        close(nFd);
    }

    return strResult;
}

//-----------------------------------------------------------------------------
// ����: ��� SockAddr �ṹ
//-----------------------------------------------------------------------------
void GetSocketAddr(SockAddr& SockAddr, const string& strHost, int nPort)
{
    bzero(&SockAddr, sizeof(SockAddr));
    SockAddr.sin_family = AF_INET;
    SockAddr.sin_addr = CInetAddress(strHost.c_str()).GetInAddr();
    SockAddr.sin_port = htons(nPort);
}

//-----------------------------------------------------------------------------
// ����: ��� SockAddr �ṹ
//-----------------------------------------------------------------------------
void GetSocketAddr(SockAddr& SockAddr, uint nIpHostValue, int nPort)
{
    bzero(&SockAddr, sizeof(SockAddr));
    SockAddr.sin_family = AF_INET;
    SockAddr.sin_addr.s_addr = htonl(nIpHostValue);
    SockAddr.sin_port = htons(nPort);
}

///////////////////////////////////////////////////////////////////////////////
// class CInetAddress

CInetAddress::CInetAddress()
{
    m_nAddr = 0;
}

CInetAddress::CInetAddress(const CInetAddress& src)
{
    m_nAddr = src.m_nAddr;
}

CInetAddress::CInetAddress(const char* sHost)
{
    *this = string(sHost);
}

CInetAddress::CInetAddress(uint nIpHostValue)
{
    m_nAddr = nIpHostValue;
}

struct in_addr CInetAddress::GetInAddr() const
{
    struct in_addr addr;
    addr.s_addr = htonl(m_nAddr);
    return addr;
}

//----------------------------------------------------------------------------
// ����: �����ַ�����IP (�� "127.0.0.1")
// ��ע: �˴�����ʹ��inet_ntoaת����������Ϊ���ǲ�������ġ�
//----------------------------------------------------------------------------
string CInetAddress::GetString() const
{
    return IpToString(m_nAddr);
}

//----------------------------------------------------------------------------
// ����: ���ַ�����IP/������ַ (�� "127.0.0.1"��"www.google.com") �����Լ�
//----------------------------------------------------------------------------
CInetAddress& CInetAddress::operator = (const string& strHost)
{
    struct in_addr addr;

    if (strHost == "*")
    {
        m_nAddr = INADDR_ANY;
        return *this;
    }

    if (inet_aton(strHost.c_str(), &addr))
    {
        m_nAddr = ntohl(addr.s_addr);
        return *this;
    }

    struct hostent stHostBuf, *pstHost;
    char *pchBuffer;
    size_t nBufLen;
    int nHErr;
    
    nBufLen = 1024;
    pchBuffer = new char[nBufLen];
    while (gethostbyname_r(strHost.c_str(), &stHostBuf, pchBuffer, nBufLen, &pstHost, &nHErr) != 0 || pstHost == NULL)
    {
        if (nHErr != NETDB_INTERNAL || errno != ERANGE)
        {
            delete[] pchBuffer;
            throw CSocketException("gethostbyname_r fail", __FILE__, __LINE__);
        }
        else
        {
            delete[] pchBuffer;
            nBufLen *= 2;
            pchBuffer = new char[nBufLen];
        }
    }

    m_nAddr = ntohl((*(struct in_addr*)pstHost->h_addr_list[0]).s_addr);
    delete[] pchBuffer;

    return *this;
}

//----------------------------------------------------------------------------
// ����: ������IP (�����ֽ�˳��) �����Լ�
//----------------------------------------------------------------------------
CInetAddress& CInetAddress::operator = (uint nIpHostValue)
{
    m_nAddr = nIpHostValue;
    return *this;
}

//----------------------------------------------------------------------------
// ����: �� in_addr �� IP �����Լ�
//----------------------------------------------------------------------------
CInetAddress& CInetAddress::operator = (const struct in_addr& addr)
{
    m_nAddr = ntohl(addr.s_addr);
    return *this;
}

///////////////////////////////////////////////////////////////////////////////
// class CSocket

CSocket::CSocket() :
    m_bActive(false),
    m_nHandle(-1),
    m_nDomain(PF_INET),
    m_nType(SOCK_STREAM),
    m_nProtocol(IPPROTO_IP),
    m_bBlockMode(true)
{
}

CSocket::~CSocket()
{
    Close();
}

void CSocket::DoSetBlockMode(int nHandle, bool bValue)
{
    int nFlag = fcntl(nHandle, F_GETFL);

    if (bValue)
        nFlag &= ~O_NONBLOCK;
    else
        nFlag |= O_NONBLOCK;
        
    if (fcntl(nHandle, F_SETFL, nFlag) < 0)
        throw CSocketException(strerror(errno), __FILE__, __LINE__);
}

void CSocket::SetActive(bool bValue)
{
    if (bValue != m_bActive)
    {
        if (bValue) Open();
        else Close();
    }
}

void CSocket::SetDomain(int nValue)
{
    if (nValue != m_nDomain)
    {
        if (GetActive()) Close();
        m_nDomain = nValue;
    }
}

void CSocket::SetType(int nValue)
{
    if (nValue != m_nType)
    {
        if (GetActive()) Close();
        m_nType = nValue;
    }
}

void CSocket::SetProtocol(int nValue)
{
    if (nValue != m_nProtocol)
    {
        if (GetActive()) Close();
        m_nProtocol = nValue;
    }
}

void CSocket::SetBlockMode(bool bValue)
{
    if (m_bActive)
        DoSetBlockMode(m_nHandle, bValue);
    m_bBlockMode = bValue;
}

void CSocket::SetHandle(int nValue)
{
    if (nValue != m_nHandle)
    {
        if (GetActive()) Close();
        m_nHandle = nValue;
        if (m_nHandle != -1) 
            m_bActive = true;
    }
}

//-----------------------------------------------------------------------------
// ����: ���׽���
//-----------------------------------------------------------------------------
void CSocket::Bind(int nPort)
{
    SockAddr addr;
    int nValue = 1;

    GetSocketAddr(addr, INADDR_ANY, nPort);

    // ǿ�����°󶨣��������������ص�Ӱ��
    setsockopt(m_nHandle, SOL_SOCKET, SO_REUSEADDR, &nValue, sizeof(int));
    // ���׽���
    if (bind(m_nHandle, (struct sockaddr*)&addr, sizeof(addr)) < 0)
        throw CSocketException(strerror(errno), __FILE__, __LINE__);
}

//-----------------------------------------------------------------------------
// ����: ���׽���
//-----------------------------------------------------------------------------
void CSocket::Open()
{
    if (!m_bActive)
    {
        try
        {
            int nHandle;
            nHandle = socket(m_nDomain, m_nType, m_nProtocol);
            if (nHandle == -1)
                throw CSocketException(strerror(errno), __FILE__, __LINE__);
            m_bActive = (nHandle != -1);
            if (m_bActive)
            {
                m_nHandle = nHandle;
                SetBlockMode(m_bBlockMode);
            }
        }
        catch (CSocketException& e)
        {
            close(m_nHandle);
            m_nHandle = -1;
            m_bActive = false;
            throw e;
        }
    }
}

//-----------------------------------------------------------------------------
// ����: �ر��׽���
//-----------------------------------------------------------------------------
void CSocket::Close()
{
    if (m_bActive)
    {
        close(m_nHandle);
        m_nHandle = -1;
        m_bActive = false;
    }
}

///////////////////////////////////////////////////////////////////////////////
// class CUdpSocket

//-----------------------------------------------------------------------------
// ����: ��������
//-----------------------------------------------------------------------------
int CUdpSocket::RecvBuffer(void *pBuffer, int nSize)
{
    PeerAddress RemoteAddr;
    return RecvBuffer(pBuffer, nSize, RemoteAddr);
}

//-----------------------------------------------------------------------------
// ����: ��������
//-----------------------------------------------------------------------------
int CUdpSocket::RecvBuffer(void *pBuffer, int nSize, PeerAddress& RemoteAddr)
{
    SockAddr addr;
    socklen_t nSockLen = sizeof(addr);
    int nBytes;

    bzero(&addr, sizeof(addr));
    nBytes = recvfrom(m_nHandle, pBuffer, nSize, 0, (struct sockaddr*)&addr, &nSockLen);
    
    if (nBytes > 0)
    {
        RemoteAddr.nIp = ntohl(addr.sin_addr.s_addr);
        RemoteAddr.nPort = ntohs(addr.sin_port);
    }

    return nBytes;
}

//-----------------------------------------------------------------------------
// ����: ��������
//-----------------------------------------------------------------------------
int CUdpSocket::SendBuffer(void *pBuffer, int nSize, const CInetAddress& ToAddr, 
    int nToPort, int nSendTimes)
{
    int nResult = 0;
    SockAddr addr;
    socklen_t nSockLen = sizeof(addr);
    int nBytes;

    GetSocketAddr(addr, ToAddr.GetHostValue(), nToPort);

    for (int i = 0; i < nSendTimes; i++)
        nResult = sendto(m_nHandle, pBuffer, nSize, 0, (struct sockaddr*)&addr, nSockLen);

    return nResult;
}

///////////////////////////////////////////////////////////////////////////////
// class CUdpServer

CUdpServer::CUdpServer() :
    m_nLocalPort(0),
    m_pListenerThreadPool(NULL)
{
    m_pListenerThreadPool = new CUdpListenerThreadPool(this);
    SetListenerThreadCount(1);
}

CUdpServer::~CUdpServer()
{
    delete m_pListenerThreadPool;
}

//-----------------------------------------------------------------------------
// ����: ���������߳�
//-----------------------------------------------------------------------------
void CUdpServer::StartListenerThreads()
{
    m_pListenerThreadPool->CreateThreads();
}

//-----------------------------------------------------------------------------
// ����: ֹͣ�����߳�
//-----------------------------------------------------------------------------
void CUdpServer::StopListenerThreads()
{
    m_pListenerThreadPool->StopThreads();
}

//-----------------------------------------------------------------------------
// ����: ���ü����˿�
//-----------------------------------------------------------------------------
void CUdpServer::SetLocalPort(int nValue)
{
    if (nValue != m_nLocalPort)
    {
        if (GetActive()) Close();
        m_nLocalPort = nValue;
    }
}

//-----------------------------------------------------------------------------
// ����: ���� UDP ������
//-----------------------------------------------------------------------------
void CUdpServer::Open()
{
    try
    {
        if (!m_bActive)
        {
            CUdpSocket::Open();
            if (m_bActive)
            {
                Bind(m_nLocalPort);
                StartListenerThreads();
            }
        }
    }
    catch (CSocketException& e)
    {
        Close();
        throw e;
    }
}

//-----------------------------------------------------------------------------
// ����: �ر� UDP ������
//-----------------------------------------------------------------------------
void CUdpServer::Close()
{
    StopListenerThreads();
    CUdpSocket::Close();
}

//-----------------------------------------------------------------------------
// ����: ȡ�ü����̵߳�����
//-----------------------------------------------------------------------------
int CUdpServer::GetListenerThreadCount()
{ 
    return m_pListenerThreadPool->GetMaxThreadCount(); 
}

//-----------------------------------------------------------------------------
// ����: ���ü����̵߳�����
//-----------------------------------------------------------------------------
void CUdpServer::SetListenerThreadCount(int nValue)
{ 
    if (nValue < 1) nValue = 1;
    m_pListenerThreadPool->SetMaxThreadCount(nValue); 
}

///////////////////////////////////////////////////////////////////////////////
// class CTcpConnection

CTcpConnection::CTcpConnection()
{
    m_bClosedGracefully = false;
    m_Socket.SetBlockMode(false);
}


CTcpConnection::CTcpConnection(int nSocketHandle, const PeerAddress& RemoteAddr)
{
    m_bClosedGracefully = false;

    m_Socket.SetHandle(nSocketHandle);
    m_Socket.SetBlockMode(false);
    m_nRemoteAddr = RemoteAddr;
}

void CTcpConnection::CheckDisconnect(bool bThrowException)
{
    bool bDisconnected;

    if (m_bClosedGracefully)
    {
        Disconnect();
        bDisconnected = true;
    }
    else
        bDisconnected = !m_Socket.GetActive();

    if (bDisconnected && bThrowException)
        throw CSocketException("socket error", __FILE__, __LINE__);
}

void CTcpConnection::ResetConnection()
{
    m_bClosedGracefully = false;
}

//-----------------------------------------------------------------------------
// ����: ��������
//   nTimeoutSecs - ָ����ʱʱ��(��)��������ָ��ʱ����δ������ȫ���������׳��쳣��
//                  �� nTimeoutSecs Ϊ 0�����ʾ�����г�ʱ��⡣
// ��ע: 
//   1. �˴����÷�����ģʽ���Ա��ܼ�ʱ�˳���
//   2. ����������ʧ�ܣ����׳��쳣��
//-----------------------------------------------------------------------------
void CTcpConnection::SendBuffer(void *pBuffer, int nSize, int nTimeoutSecs)
{
    const int SELECT_WAIT_MSEC = 250;    // ÿ�εȴ�ʱ�� (����)

    fd_set fds;
    struct timeval tv;
    int nSocketHandle = m_Socket.GetHandle();
    int n, r, nRemain, nIndex;
    uint nStartTime, nElapsedSecs;

    if (nSize <= 0) return;
    if (!m_Socket.GetActive())
        throw CSocketException("socket error", __FILE__, __LINE__);

    nRemain = nSize;
    nIndex = 0;
    nStartTime = time(NULL);
    nElapsedSecs = 0;

    while(m_Socket.GetActive() && nRemain > 0)
    {
        // �趨ÿ�εȴ�ʱ��
        tv.tv_sec = 0;
        tv.tv_usec = SELECT_WAIT_MSEC * 1000;

        FD_ZERO(&fds);
        FD_SET(nSocketHandle, &fds);
        
        r = select(nSocketHandle + 1, NULL, &fds, NULL, &tv);
        if (r < 0)
        {
            if(errno == EINTR) continue;
            else throw CSocketException(strerror(errno), __FILE__, __LINE__);
        }

        if (r > 0 && m_Socket.GetActive() && FD_ISSET(nSocketHandle, &fds))
        {
            n = send(nSocketHandle, &((char*)pBuffer)[nIndex], nRemain, 0);
            if (n <= 0) m_bClosedGracefully = true;  // error
            CheckDisconnect(true);

            nIndex += n;
            nRemain -= n;
        }

        // �����Ҫ��ʱ���
        if (nTimeoutSecs > 0 && nRemain > 0)
        {
            nElapsedSecs = time(NULL) - nStartTime;
            if (nElapsedSecs >= nTimeoutSecs)
                throw CSocketException("tcp send timeout", __FILE__, __LINE__);
        }
    }

    if (nRemain > 0)
        throw CSocketException("socket error", __FILE__, __LINE__);
}

//-----------------------------------------------------------------------------
// ����: ��������
// ����:
//   nTimeoutSecs - ָ����ʱʱ��(��)��������ָ��ʱ����δ������ȫ���������׳��쳣��
//                  �� nTimeoutSecs Ϊ 0�����ʾ�����г�ʱ��⡣
// ��ע: 
//   1. �˴����÷�����ģʽ���Ա��ܼ�ʱ�˳���
//   2. ����������ʧ�ܣ����׳��쳣��
//-----------------------------------------------------------------------------
void CTcpConnection::RecvBuffer(void *pBuffer, int nSize, int nTimeoutSecs)
{
    const int SELECT_WAIT_MSEC = 250;    // ÿ�εȴ�ʱ�� (����)

    fd_set fds;
    struct timeval tv;
    int nSocketHandle = m_Socket.GetHandle();
    int n, r, nRemain, nIndex;
    uint nStartTime, nElapsedSecs;

    if (nSize <= 0) return;
    if (!m_Socket.GetActive())
        throw CSocketException("socket error", __FILE__, __LINE__);

    nRemain = nSize;
    nIndex = 0;
    nStartTime = time(NULL);
    nElapsedSecs = 0;

    while(m_Socket.GetActive() && nRemain > 0)
    {
        // �趨ÿ�εȴ�ʱ��
        tv.tv_sec = 0;
        tv.tv_usec = SELECT_WAIT_MSEC * 1000;

        FD_ZERO(&fds);
        FD_SET(nSocketHandle, &fds);
        
        r = select(nSocketHandle + 1, &fds, NULL, NULL, &tv);
        if (r < 0)
        {
            if(errno == EINTR) continue;
            else throw CSocketException(strerror(errno), __FILE__, __LINE__);
        }

        if (r > 0 && m_Socket.GetActive() && FD_ISSET(nSocketHandle, &fds))
        {
            n = recv(nSocketHandle, &((char*)pBuffer)[nIndex], nRemain, 0);
            if (n <= 0) m_bClosedGracefully = true;  // error
            CheckDisconnect(true);

            nIndex += n;
            nRemain -= n;
        }

        // �����Ҫ��ʱ���
        if (nTimeoutSecs > 0 && nRemain > 0)
        {
            nElapsedSecs = time(NULL) - nStartTime;
            if (nElapsedSecs >= nTimeoutSecs)
                throw CSocketException("tcp recv timeout", __FILE__, __LINE__);
        }
    }

    if (nRemain > 0)
        throw CSocketException("socket error", __FILE__, __LINE__);
}

//-----------------------------------------------------------------------------
// ����: �������� (������)
// ����: ʵ�ʷ��ͳ�ȥ�������ֽ���
// ��ע: �������������׳��쳣��
//-----------------------------------------------------------------------------
int CTcpConnection::WriteBuffer(void *pBuffer, int nSize)
{
    int nResult = send(m_Socket.GetHandle(), (char*)pBuffer, nSize, 0);
    if (nResult < 0)
    {
        if (errno != EWOULDBLOCK)
            m_bClosedGracefully = true;  // error
        else
            nResult = 0;
    }

    CheckDisconnect(true);
    return nResult;
}

//-----------------------------------------------------------------------------
// ����: �������� (������)
// ����: ʵ�ʽ��յ��������ֽ���
// ��ע: �������������׳��쳣��
//-----------------------------------------------------------------------------
int CTcpConnection::ReadBuffer(void *pBuffer, int nSize)
{
    int nResult = recv(m_Socket.GetHandle(), (char*)pBuffer, nSize, 0);
    if (nResult < 0)
    {
        if (errno != EWOULDBLOCK)
            m_bClosedGracefully = true;  // error
        else
            nResult = 0;
    }

    CheckDisconnect(true);
    return nResult;
}

//-----------------------------------------------------------------------------
// ����: �����ַ�����ֱ���յ�ָ���Ľ�����
// ����:
//   chTerminalChar - �������ַ�
//   nTimeoutSecs   - ָ����ʱʱ��(��)��������ָ��ʱ����δ������ȫ���������׳��쳣��
//                    �� nTimeoutSecs Ϊ 0�����ʾ�����г�ʱ��⡣
// ��ע: ����������ʧ�ܣ����׳��쳣��
//-----------------------------------------------------------------------------
string CTcpConnection::RecvString(char chTerminalChar, int nTimeoutSecs)
{
    string strResult;
    char ch;

    while (true)
    {
        RecvBuffer(&ch, 1, nTimeoutSecs);
        if (ch == chTerminalChar) break;
        strResult += ch;
    }

    return strResult;
}

//-----------------------------------------------------------------------------
// ����: �Ͽ�����
//-----------------------------------------------------------------------------
void CTcpConnection::Disconnect()
{
    if (m_Socket.GetActive())
    {
        shutdown(m_Socket.GetHandle(), SHUT_RDWR);
        m_Socket.Close();
        m_bClosedGracefully = true;
    }
}

//-----------------------------------------------------------------------------
// ����: ���ص�ǰ�Ƿ�Ϊ����״̬
//-----------------------------------------------------------------------------
bool CTcpConnection::GetConnected()
{
    CheckDisconnect(false);
    return m_Socket.GetActive();
}

///////////////////////////////////////////////////////////////////////////////
// class CTcpClient

//-----------------------------------------------------------------------------
// ����: ����TCP��������
// ��ע: ������ʧ�ܣ����׳��쳣��
//-----------------------------------------------------------------------------
void CTcpClient::Connect(const string& strHost, int nPort)
{
    if (GetConnected()) Disconnect();

    try
    {
        m_Socket.Open();
        if (m_Socket.GetActive())
        {
            SockAddr addr;

            ResetConnection();
            GetSocketAddr(addr, strHost, nPort);

            bool bOldBlockMode = m_Socket.GetBlockMode();
            m_Socket.SetBlockMode(true);

            if (connect(m_Socket.GetHandle(), (struct sockaddr*)&addr, sizeof(addr)) < 0)
                throw CSocketException("tcp connect fail", __FILE__, __LINE__);
                
            m_Socket.SetBlockMode(bOldBlockMode);
            m_nRemoteAddr = PeerAddress(ntohl(addr.sin_addr.s_addr), nPort);
        }
    }
    catch (CSocketException& e)
    {
        m_Socket.Close();
        throw e;
    }
}

///////////////////////////////////////////////////////////////////////////////
// class CTcpServer

CTcpServer::CTcpServer() :
    m_nLocalPort(0),
    m_pListenerThread(NULL)
{
}

CTcpServer::~CTcpServer()
{
    Close();
}

//-----------------------------------------------------------------------------
// ����: ���������߳�
//-----------------------------------------------------------------------------
void CTcpServer::StartListenerThread()
{
    if (!m_pListenerThread)
    {
        m_pListenerThread = new CTcpListenerThread(this);
        m_pListenerThread->Run();
    }
}

//-----------------------------------------------------------------------------
// ����: ֹͣ�����߳�
//-----------------------------------------------------------------------------
void CTcpServer::StopListenerThread()
{
    if (m_pListenerThread)
    {
        m_pListenerThread->Terminate();
        m_pListenerThread->WaitFor();
        delete m_pListenerThread;
        m_pListenerThread = NULL;
    }
}

//-----------------------------------------------------------------------------
// ����: ����TCP������
//-----------------------------------------------------------------------------
void CTcpServer::Open()
{
    try
    {
        if (!GetActive())
        {
            m_Socket.Open();
            m_Socket.Bind(m_nLocalPort);
            if (listen(m_Socket.GetHandle(), LISTEN_QUEUE_SIZE) < 0)
                throw CSocketException("tcp server listen fail", __FILE__, __LINE__);
            StartListenerThread();
        }
    }
    catch (CSocketException& e)
    {
        Close();
        throw e;
    }
}

//-----------------------------------------------------------------------------
// ����: �ر�TCP������
//-----------------------------------------------------------------------------
void CTcpServer::Close()
{
    if (GetActive())
    {
        // ��ֹͣ�̣߳�����socketʧЧ��select����ʧ�����100%CPU��
        StopListenerThread();
        m_Socket.Close();
    }
}

//-----------------------------------------------------------------------------
// ����: ����/�ر�TCP������
//-----------------------------------------------------------------------------
void CTcpServer::SetActive(bool bValue)
{
    if (GetActive() != bValue)
    {
        if (bValue) Open();
        else Close();
    }
}

//-----------------------------------------------------------------------------
// ����: ����TCP�����������˿�
//-----------------------------------------------------------------------------
void CTcpServer::SetLocalPort(int nValue)
{
    if (nValue != m_nLocalPort)
    {
        if (GetActive()) Close();
        m_nLocalPort = nValue;
    }
}

///////////////////////////////////////////////////////////////////////////////
// class CUdpListenerThread

CUdpListenerThread::CUdpListenerThread(CUdpListenerThreadPool *pThreadPool, int nIndex) :
    m_pThreadPool(pThreadPool), 
    m_nIndex(nIndex)
{
    SetFreeOnTerminate(true);
    m_pUdpServer = &(pThreadPool->GetUdpServer());
    m_pThreadPool->RegisterThread(this);
}

CUdpListenerThread::~CUdpListenerThread()
{
    m_pThreadPool->UnregisterThread(this);
}

//-----------------------------------------------------------------------------
// ����: UDP��������������
//-----------------------------------------------------------------------------
void CUdpListenerThread::Execute()
{
    const int MAX_UDP_BUFFER_SIZE = 8192;   // UDP���ݰ�����ֽ���
    const int SELECT_WAIT_MSEC    = 250;    // ÿ�εȴ�ʱ�� (����)

    fd_set fds;
    struct timeval tv;
    int nSocketHandle = m_pUdpServer->GetHandle();
    CBuffer PacketBuffer(MAX_UDP_BUFFER_SIZE);
    PeerAddress RemoteAddr;
    int r, n;

    while (!GetTerminated() && m_pUdpServer->GetActive())
    try
    {
        // �趨ÿ�εȴ�ʱ��
        tv.tv_sec = 0;
        tv.tv_usec = SELECT_WAIT_MSEC * 1000;

        FD_ZERO(&fds);
        FD_SET(nSocketHandle, &fds);

        r = select(nSocketHandle + 1, &fds, NULL, NULL, &tv);

        if (r > 0 && m_pUdpServer->GetActive() && FD_ISSET(nSocketHandle, &fds))
        {
            n = m_pUdpServer->RecvBuffer(PacketBuffer.Data(), MAX_UDP_BUFFER_SIZE, RemoteAddr);
            if (n > 0)
            {
                m_pUdpServer->OnRecvData(PacketBuffer.Data(), n, RemoteAddr);
            }
        }
    }
    catch (CSocketException& e)
    { ; }
}

///////////////////////////////////////////////////////////////////////////////
// class CUdpListenerThreadPool

CUdpListenerThreadPool::CUdpListenerThreadPool(CUdpServer *pUdpServer) :
    m_pUdpServer(pUdpServer),
    m_nMaxThreadCount(0)
{
}

CUdpListenerThreadPool::~CUdpListenerThreadPool()
{
}

//-----------------------------------------------------------------------------
// ����: ע���߳�
//-----------------------------------------------------------------------------
void CUdpListenerThreadPool::RegisterThread(CUdpListenerThread *pThread)
{
    CAutoSynchronizer Syncher(m_Lock);

    if (m_ThreadList.IndexOf(pThread) == -1)
        m_ThreadList.Add(pThread);
}

//-----------------------------------------------------------------------------
// ����: ע���߳�
//-----------------------------------------------------------------------------
void CUdpListenerThreadPool::UnregisterThread(CUdpListenerThread *pThread)
{
    CAutoSynchronizer Syncher(m_Lock);

    m_ThreadList.Remove(pThread);
}

//-----------------------------------------------------------------------------
// ����: �����߳�
//-----------------------------------------------------------------------------
void CUdpListenerThreadPool::CreateThreads()
{
    for (int i = 0; i < m_nMaxThreadCount; i++)
    {
        CUdpListenerThread *pThread;
        pThread = new CUdpListenerThread(this, i);
        pThread->Run();
    }
}

//-----------------------------------------------------------------------------
// ����: ֪ͨ���ȴ������߳��˳�
//-----------------------------------------------------------------------------
void CUdpListenerThreadPool::StopThreads()
{
    const int MAX_THREAD_WAIT_FOR_SECS = 5; // �̳߳����ʱ���ȴ�ʱ��(��)
    const double SLEEP_INTERVAL = 0.5;      // ÿ�εȴ���ʱ����(��)
    double nWaitSecs = 0;

    // ֪ͨ�߳��˳�
    {
        CAutoSynchronizer Syncher(m_Lock);

        for (int i = 0; i < m_ThreadList.Count(); i++)
        {
            CUdpListenerThread *pThread;
            pThread = (CUdpListenerThread*)m_ThreadList[i];
            pThread->Terminate();
        }
    }

    // �ȴ��߳��˳�
    while (nWaitSecs < MAX_THREAD_WAIT_FOR_SECS)
    {
        if (m_ThreadList.Count() == 0) break;
        NanoSleep(SLEEP_INTERVAL, true);
        nWaitSecs += SLEEP_INTERVAL;
    }

    // ���ȴ���ʱ����ǿ��ɱ�����߳�
    if (m_ThreadList.Count() > 0)
    {
        CAutoSynchronizer Syncher(m_Lock);
    
        for (int i = 0; i < m_ThreadList.Count(); i++)
        {
            CUdpListenerThread *pThread;
            pThread = (CUdpListenerThread*)m_ThreadList[i];
            pThread->Kill();
        }

        m_ThreadList.Clear(); 
    }
}

///////////////////////////////////////////////////////////////////////////////
// class CTcpListenerThread

CTcpListenerThread::CTcpListenerThread(CTcpServer *pTcpServer) :
    m_pTcpServer(pTcpServer)
{
    SetFreeOnTerminate(false);
}

//-----------------------------------------------------------------------------
// ����: TCP��������������
//-----------------------------------------------------------------------------
void CTcpListenerThread::Execute()
{
    const int SELECT_WAIT_MSEC = 250;    // ÿ�εȴ�ʱ�� (����)

    fd_set fds;
    struct timeval tv;
    SockAddr addr;
    socklen_t nSockLen = sizeof(addr);
    int nSocketHandle = m_pTcpServer->GetSocket().GetHandle();
    PeerAddress RemoteAddr;
    int r, nAcceptHandle;

    while (!GetTerminated() && m_pTcpServer->GetActive())
    try
    {
        // �趨ÿ�εȴ�ʱ��
        tv.tv_sec = 0;
        tv.tv_usec = SELECT_WAIT_MSEC * 1000;

        FD_ZERO(&fds);
        FD_SET(nSocketHandle, &fds);

        r = select(nSocketHandle + 1, &fds, NULL, NULL, &tv);

        if (r > 0 && m_pTcpServer->GetActive() && FD_ISSET(nSocketHandle, &fds))
        {
            nAcceptHandle = accept(nSocketHandle, (struct sockaddr*)&addr, &nSockLen);
            if (nAcceptHandle != -1)
            {
                RemoteAddr = PeerAddress(ntohl(addr.sin_addr.s_addr), ntohs(addr.sin_port));
                m_pTcpServer->OnConnect(new CTcpConnection(nAcceptHandle, RemoteAddr));
            }
        }
    }
    catch (CSocketException& e)
    { ; }
}

///////////////////////////////////////////////////////////////////////////////
// class CTcpConnectionWarp

void CTcpConnectionWarp::SetParam(CTcpConnection* pTcpConnection,
    char* pReadBuffer, int ReadLength,
    char* pWriteBuffer, int WriteLength)
{
    m_pTcpConnection = pTcpConnection;
    m_pReadBuffer = pReadBuffer;
    m_ReadLength = ReadLength;
    m_pWriteBuffer = pWriteBuffer;
    m_WriteLength = WriteLength;

    m_DoneReadLength = 0;
    m_DoneWriteLength = 0;
}

bool CTcpConnectionWarp::IsDoneReadBuffer()
{
    if (m_ReadLength && (m_pReadBuffer) && (m_DoneReadLength < m_ReadLength))
    {
        m_DoneReadLength += m_pTcpConnection->ReadBuffer(
            m_pReadBuffer + m_DoneReadLength,
            m_ReadLength - m_DoneReadLength);
    } else return true;
    
    if (m_ReadLength && (m_pReadBuffer) && (m_DoneReadLength < m_ReadLength))
        return false;
    else
        return true;
}

bool CTcpConnectionWarp::IsDoneWriteBuffer()
{
    if (m_WriteLength && (m_pWriteBuffer) && (m_DoneWriteLength < m_WriteLength))
    {
        m_DoneWriteLength += m_pTcpConnection->WriteBuffer(
            m_pWriteBuffer + m_DoneWriteLength,
            m_WriteLength - m_DoneWriteLength);
    } else return true;

    if (m_WriteLength && (m_pWriteBuffer) && (m_DoneWriteLength < m_WriteLength))
        return false;
    else
        return true;
}

void CTcpConnectionWarp::AdjustReadLength(int ReadLength)
{
    m_ReadLength = ReadLength;
}

void CTcpConnectionWarp::AdjustWriteLength(int WriteLength)
{
    m_WriteLength = WriteLength;
}

///////////////////////////////////////////////////////////////////////////////

} // namespace ise
