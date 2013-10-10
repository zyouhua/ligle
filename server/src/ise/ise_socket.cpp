///////////////////////////////////////////////////////////////////////////////
// Iris Server Engine (ISE)
//
// 文件名称: ise_socket.cpp
// 功能描述: 网络基础类库
// 最后修改: 2005-07-14
///////////////////////////////////////////////////////////////////////////////

#include "ise_socket.h"
#include "ise_sysutils.h"

namespace ise
{

///////////////////////////////////////////////////////////////////////////////
// 杂项函数

//-----------------------------------------------------------------------------
// 描述: 整形IP(主机字节顺序) -> 串型IP
//-----------------------------------------------------------------------------
string IpToString(int nIp)
{
#pragma pack(1)
    union CIpUnion
    {
        int nValue;
        struct
        {
            unsigned char ch1;  // nValue的最低字节
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
// 描述: 串型IP -> 整形IP(主机字节顺序)
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
// 描述: 获取本机IP
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
// 描述: 填充 SockAddr 结构
//-----------------------------------------------------------------------------
void GetSocketAddr(SockAddr& SockAddr, const string& strHost, int nPort)
{
    bzero(&SockAddr, sizeof(SockAddr));
    SockAddr.sin_family = AF_INET;
    SockAddr.sin_addr = CInetAddress(strHost.c_str()).GetInAddr();
    SockAddr.sin_port = htons(nPort);
}

//-----------------------------------------------------------------------------
// 描述: 填充 SockAddr 结构
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
// 描述: 返回字符串型IP (如 "127.0.0.1")
// 备注: 此处不能使用inet_ntoa转换函数，因为它是不可重入的。
//----------------------------------------------------------------------------
string CInetAddress::GetString() const
{
    return IpToString(m_nAddr);
}

//----------------------------------------------------------------------------
// 描述: 将字符串型IP/主机地址 (如 "127.0.0.1"、"www.google.com") 赋给自己
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
// 描述: 将整型IP (主机字节顺序) 赋给自己
//----------------------------------------------------------------------------
CInetAddress& CInetAddress::operator = (uint nIpHostValue)
{
    m_nAddr = nIpHostValue;
    return *this;
}

//----------------------------------------------------------------------------
// 描述: 将 in_addr 型 IP 赋给自己
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
// 描述: 绑定套接字
//-----------------------------------------------------------------------------
void CSocket::Bind(int nPort)
{
    SockAddr addr;
    int nValue = 1;

    GetSocketAddr(addr, INADDR_ANY, nPort);

    // 强制重新绑定，而不受其它因素的影响
    setsockopt(m_nHandle, SOL_SOCKET, SO_REUSEADDR, &nValue, sizeof(int));
    // 绑定套接字
    if (bind(m_nHandle, (struct sockaddr*)&addr, sizeof(addr)) < 0)
        throw CSocketException(strerror(errno), __FILE__, __LINE__);
}

//-----------------------------------------------------------------------------
// 描述: 打开套接字
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
// 描述: 关闭套接字
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
// 描述: 接收数据
//-----------------------------------------------------------------------------
int CUdpSocket::RecvBuffer(void *pBuffer, int nSize)
{
    PeerAddress RemoteAddr;
    return RecvBuffer(pBuffer, nSize, RemoteAddr);
}

//-----------------------------------------------------------------------------
// 描述: 接收数据
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
// 描述: 发送数据
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
// 描述: 启动监听线程
//-----------------------------------------------------------------------------
void CUdpServer::StartListenerThreads()
{
    m_pListenerThreadPool->CreateThreads();
}

//-----------------------------------------------------------------------------
// 描述: 停止监听线程
//-----------------------------------------------------------------------------
void CUdpServer::StopListenerThreads()
{
    m_pListenerThreadPool->StopThreads();
}

//-----------------------------------------------------------------------------
// 描述: 设置监听端口
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
// 描述: 开启 UDP 服务器
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
// 描述: 关闭 UDP 服务器
//-----------------------------------------------------------------------------
void CUdpServer::Close()
{
    StopListenerThreads();
    CUdpSocket::Close();
}

//-----------------------------------------------------------------------------
// 描述: 取得监听线程的数量
//-----------------------------------------------------------------------------
int CUdpServer::GetListenerThreadCount()
{ 
    return m_pListenerThreadPool->GetMaxThreadCount(); 
}

//-----------------------------------------------------------------------------
// 描述: 设置监听线程的数量
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
// 描述: 发送数据
//   nTimeoutSecs - 指定超时时间(秒)，若超过指定时间仍未接收完全部数据则抛出异常。
//                  若 nTimeoutSecs 为 0，则表示不进行超时检测。
// 备注: 
//   1. 此处采用非阻塞模式，以便能及时退出。
//   2. 若接收数据失败，则抛出异常。
//-----------------------------------------------------------------------------
void CTcpConnection::SendBuffer(void *pBuffer, int nSize, int nTimeoutSecs)
{
    const int SELECT_WAIT_MSEC = 250;    // 每次等待时间 (毫秒)

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
        // 设定每次等待时间
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

        // 如果需要超时检测
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
// 描述: 接收数据
// 参数:
//   nTimeoutSecs - 指定超时时间(秒)，若超过指定时间仍未接收完全部数据则抛出异常。
//                  若 nTimeoutSecs 为 0，则表示不进行超时检测。
// 备注: 
//   1. 此处采用非阻塞模式，以便能及时退出。
//   2. 若接收数据失败，则抛出异常。
//-----------------------------------------------------------------------------
void CTcpConnection::RecvBuffer(void *pBuffer, int nSize, int nTimeoutSecs)
{
    const int SELECT_WAIT_MSEC = 250;    // 每次等待时间 (毫秒)

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
        // 设定每次等待时间
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

        // 如果需要超时检测
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
// 描述: 发送数据 (非阻塞)
// 返回: 实际发送出去的数据字节数
// 备注: 若发生错误，则抛出异常。
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
// 描述: 接收数据 (非阻塞)
// 返回: 实际接收到的数据字节数
// 备注: 若发生错误，则抛出异常。
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
// 描述: 接收字符串，直到收到指定的结束符
// 参数:
//   chTerminalChar - 结束符字符
//   nTimeoutSecs   - 指定超时时间(秒)，若超过指定时间仍未接收完全部数据则抛出异常。
//                    若 nTimeoutSecs 为 0，则表示不进行超时检测。
// 备注: 若接收数据失败，则抛出异常。
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
// 描述: 断开连接
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
// 描述: 返回当前是否为连接状态
//-----------------------------------------------------------------------------
bool CTcpConnection::GetConnected()
{
    CheckDisconnect(false);
    return m_Socket.GetActive();
}

///////////////////////////////////////////////////////////////////////////////
// class CTcpClient

//-----------------------------------------------------------------------------
// 描述: 发起TCP连接请求
// 备注: 若连接失败，则抛出异常。
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
// 描述: 启动监听线程
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
// 描述: 停止监听线程
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
// 描述: 开启TCP服务器
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
// 描述: 关闭TCP服务器
//-----------------------------------------------------------------------------
void CTcpServer::Close()
{
    if (GetActive())
    {
        // 先停止线程，以免socket失效后，select函数失灵造成100%CPU。
        StopListenerThread();
        m_Socket.Close();
    }
}

//-----------------------------------------------------------------------------
// 描述: 开启/关闭TCP服务器
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
// 描述: 设置TCP服务器监听端口
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
// 描述: UDP服务器监听工作
//-----------------------------------------------------------------------------
void CUdpListenerThread::Execute()
{
    const int MAX_UDP_BUFFER_SIZE = 8192;   // UDP数据包最大字节数
    const int SELECT_WAIT_MSEC    = 250;    // 每次等待时间 (毫秒)

    fd_set fds;
    struct timeval tv;
    int nSocketHandle = m_pUdpServer->GetHandle();
    CBuffer PacketBuffer(MAX_UDP_BUFFER_SIZE);
    PeerAddress RemoteAddr;
    int r, n;

    while (!GetTerminated() && m_pUdpServer->GetActive())
    try
    {
        // 设定每次等待时间
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
// 描述: 注册线程
//-----------------------------------------------------------------------------
void CUdpListenerThreadPool::RegisterThread(CUdpListenerThread *pThread)
{
    CAutoSynchronizer Syncher(m_Lock);

    if (m_ThreadList.IndexOf(pThread) == -1)
        m_ThreadList.Add(pThread);
}

//-----------------------------------------------------------------------------
// 描述: 注销线程
//-----------------------------------------------------------------------------
void CUdpListenerThreadPool::UnregisterThread(CUdpListenerThread *pThread)
{
    CAutoSynchronizer Syncher(m_Lock);

    m_ThreadList.Remove(pThread);
}

//-----------------------------------------------------------------------------
// 描述: 创建线程
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
// 描述: 通知并等待所有线程退出
//-----------------------------------------------------------------------------
void CUdpListenerThreadPool::StopThreads()
{
    const int MAX_THREAD_WAIT_FOR_SECS = 5; // 线程池清空时最多等待时间(秒)
    const double SLEEP_INTERVAL = 0.5;      // 每次等待的时间间隔(秒)
    double nWaitSecs = 0;

    // 通知线程退出
    {
        CAutoSynchronizer Syncher(m_Lock);

        for (int i = 0; i < m_ThreadList.Count(); i++)
        {
            CUdpListenerThread *pThread;
            pThread = (CUdpListenerThread*)m_ThreadList[i];
            pThread->Terminate();
        }
    }

    // 等待线程退出
    while (nWaitSecs < MAX_THREAD_WAIT_FOR_SECS)
    {
        if (m_ThreadList.Count() == 0) break;
        NanoSleep(SLEEP_INTERVAL, true);
        nWaitSecs += SLEEP_INTERVAL;
    }

    // 若等待超时，则强行杀死各线程
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
// 描述: TCP服务器监听工作
//-----------------------------------------------------------------------------
void CTcpListenerThread::Execute()
{
    const int SELECT_WAIT_MSEC = 250;    // 每次等待时间 (毫秒)

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
        // 设定每次等待时间
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
