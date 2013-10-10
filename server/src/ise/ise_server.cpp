///////////////////////////////////////////////////////////////////////////////
// Iris Server Engine (ISE)
//
// 文件名称: ise_server.cpp
// 功能描述: 服务器框架
// 最后修改: 2005-07-14
///////////////////////////////////////////////////////////////////////////////

#include "ise_server.h"
#include "ise_system.h"
#include "ise_sysutils.h"

namespace ise
{

///////////////////////////////////////////////////////////////////////////////
// class CServerOptions

CServerOptions::CServerOptions()
{
    SetServerType(DEF_SERVER_TYPE);
    SetAdjustThreadInterval(DEF_ADJUST_THREAD_INTERVAL);
    SetHelperThreadCount(DEF_HELPER_THREAD_COUNT);

    SetUdpServerPort(DEF_UDP_SERVER_PORT);
    SetUdpListenerThreadCount(DEF_UDP_LISTENER_THD_COUNT);
    SetUdpRequestGroupCount(DEF_UDP_REQ_GROUP_COUNT);
    for (int i = 0; i < DEF_UDP_REQ_GROUP_COUNT; i++)
    {
        SetUdpRequestQueueCapacity(i, DEF_UDP_REQ_QUEUE_CAPACITY);
        SetUdpWorkerThreadCount(i, DEF_UDP_WORKER_THREADS_MIN, DEF_UDP_WORKER_THREADS_MAX);
    }
    SetUdpRequestEffWaitTime(DEF_UDP_REQ_EFF_WAIT_TIME);
    SetUdpWorkerThreadTimeout(DEF_UDP_WORKER_THD_TIMEOUT);
    SetUdpRequestQueueAlertLine(DEF_UDP_QUEUE_ALERT_LINE);

    SetTcpRequestGroupCount(DEF_TCP_REQ_GROUP_COUNT);
    for (int i = 0; i < DEF_TCP_REQ_GROUP_COUNT; i++)
    {
        SetTcpServerPort(i, DEF_TCP_SERVER_PORT);
        SetTcpRequestQueueCapacity(i, DEF_TCP_REQ_QUEUE_CAPACITY);
        SetTcpWorkerThreadCount(i, DEF_TCP_WORKER_THREADS_MIN, DEF_TCP_WORKER_THREADS_MAX);
    }
    SetTcpRequestQueueAlertLine(DEF_TCP_QUEUE_ALERT_LINE);
}

//-----------------------------------------------------------------------------
// 描述: 设置服务器类型(UDP|TCP)
// 参数:
//   nSvrType - 服务器器类型(可多选或不选)
// 示例:
//   SetServerType(ST_UDP | ST_TCP);
//-----------------------------------------------------------------------------
void CServerOptions::SetServerType(uint nSvrType)
{
    m_nServerType = nSvrType;
}

//-----------------------------------------------------------------------------
// 描述: 设置后台维护工作者线程数量的时间间隔(秒)
//-----------------------------------------------------------------------------
void CServerOptions::SetAdjustThreadInterval(int nSecs)
{
    if (nSecs <= 0) nSecs = DEF_ADJUST_THREAD_INTERVAL;
    m_nAdjustThreadInterval = nSecs;
}

//-----------------------------------------------------------------------------
// 描述: 设置辅助线程的数量
//-----------------------------------------------------------------------------
void CServerOptions::SetHelperThreadCount(int nCount)
{
    if (nCount < 0) nCount = 0;
    m_nHelperThreadCount = nCount;
}

//-----------------------------------------------------------------------------
// 描述: 设置UDP服务端口号
//-----------------------------------------------------------------------------
void CServerOptions::SetUdpServerPort(int nPort)
{
    m_nUdpServerPort = nPort;
}

//-----------------------------------------------------------------------------
// 描述: 设置UDP监听线程的数量
//-----------------------------------------------------------------------------
void CServerOptions::SetUdpListenerThreadCount(int nCount)
{
    if (nCount < 1) nCount = 1;

    m_nUdpListenerThreadCount = nCount;
}

//-----------------------------------------------------------------------------
// 描述: 设置UDP数据包的组别总数
//-----------------------------------------------------------------------------
void CServerOptions::SetUdpRequestGroupCount(int nCount)
{
    if (nCount <= 0) nCount = DEF_UDP_REQ_GROUP_COUNT;

    m_nUdpRequestGroupCount = nCount;
    m_UdpRequestGroupOpts.resize(nCount);
}

//-----------------------------------------------------------------------------
// 描述: 设置UDP请求队列的最大容量 (即可容纳多少个数据包)
// 参数:
//   nGroupIndex - 组别号 (0-based)
//   nCapacity   - 容量
//-----------------------------------------------------------------------------
void CServerOptions::SetUdpRequestQueueCapacity(int nGroupIndex, int nCapacity)
{
    if (nGroupIndex < 0 || nGroupIndex >= m_nUdpRequestGroupCount) return;

    if (nCapacity <= 0) nCapacity = DEF_UDP_REQ_QUEUE_CAPACITY;

    m_UdpRequestGroupOpts[nGroupIndex].nRequestQueueCapacity = nCapacity;
}

//-----------------------------------------------------------------------------
// 描述: 设置UDP工作者线程个数的上下限
// 参数:
//   nGroupIndex - 组别号 (0-based)
//   nMinThreads - 线程个数的下限
//   nMaxThreads - 线程个数的上限
//-----------------------------------------------------------------------------
void CServerOptions::SetUdpWorkerThreadCount(int nGroupIndex, int nMinThreads, int nMaxThreads)
{
    if (nGroupIndex < 0 || nGroupIndex >= m_nUdpRequestGroupCount) return;

    if (nMinThreads < 1) nMinThreads = 1;
    if (nMaxThreads < nMinThreads) nMaxThreads = nMinThreads;

    m_UdpRequestGroupOpts[nGroupIndex].nMinWorkerThreads = nMinThreads;
    m_UdpRequestGroupOpts[nGroupIndex].nMaxWorkerThreads = nMaxThreads;
}

//-----------------------------------------------------------------------------
// 描述: 设置UDP请求在队列中的有效等待时间，超时则不予处理
// 参数:
//   nMSecs - 等待秒数
//-----------------------------------------------------------------------------
void CServerOptions::SetUdpRequestEffWaitTime(int nSecs)
{
    if (nSecs <= 0) nSecs = DEF_UDP_REQ_EFF_WAIT_TIME;
    m_nUdpRequestEffWaitTime = nSecs;
}

//-----------------------------------------------------------------------------
// 描述: 设置UDP工作者线程的工作超时时间(秒)，若为0表示不进行超时检测
//-----------------------------------------------------------------------------
void CServerOptions::SetUdpWorkerThreadTimeout(int nSecs)
{
    if (nSecs < 0) nSecs = 0;
    m_nUdpWorkerThreadTimeout = nSecs;
}

//-----------------------------------------------------------------------------
// 描述: 设置请求队列中数据包数量警戒线
//-----------------------------------------------------------------------------
void CServerOptions::SetUdpRequestQueueAlertLine(int nCount)
{
    if (nCount < 1) nCount = 1;
    m_nUdpRequestQueueAlertLine = nCount;
}

//-----------------------------------------------------------------------------
// 描述: 设置TCP数据包的组别总数
//-----------------------------------------------------------------------------
void CServerOptions::SetTcpRequestGroupCount(int nCount)
{
    if (nCount < 0) nCount = 0;

    m_nTcpRequestGroupCount = nCount;
    m_TcpRequestGroupOpts.resize(nCount);
}

//-----------------------------------------------------------------------------
// 描述: 设置TCP服务端口号
// 参数:
//   nGroupIndex - 组别号 (0-based)
//   nPort       - 端口号
//-----------------------------------------------------------------------------
void CServerOptions::SetTcpServerPort(int nGroupIndex, int nPort)
{
    if (nGroupIndex < 0 || nGroupIndex >= m_nTcpRequestGroupCount) return;

    m_TcpRequestGroupOpts[nGroupIndex].nTcpServerPort = nPort;
}

//-----------------------------------------------------------------------------
// 描述: 设置TCP请求队列的最大容量 (即可容纳多少个连接)
// 参数:
//   nGroupIndex - 组别号 (0-based)
//   nCapacity   - 容量
//-----------------------------------------------------------------------------
void CServerOptions::SetTcpRequestQueueCapacity(int nGroupIndex, int nCapacity)
{
    if (nGroupIndex < 0 || nGroupIndex >= m_nTcpRequestGroupCount) return;

    if (nCapacity <= 0) nCapacity = DEF_TCP_REQ_QUEUE_CAPACITY;

    m_TcpRequestGroupOpts[nGroupIndex].nRequestQueueCapacity = nCapacity;
}

//-----------------------------------------------------------------------------
// 描述: 设置TCP工作者线程个数的上下限
// 参数:
//   nGroupIndex - 组别号 (0-based)
//   nMinThreads - 线程个数的下限
//   nMaxThreads - 线程个数的上限
//-----------------------------------------------------------------------------
void CServerOptions::SetTcpWorkerThreadCount(int nGroupIndex, int nMinThreads, int nMaxThreads)
{
    if (nGroupIndex < 0 || nGroupIndex >= m_nTcpRequestGroupCount) return;

    if (nMinThreads < 1) nMinThreads = 1;
    if (nMaxThreads < nMinThreads) nMaxThreads = nMinThreads;

    m_TcpRequestGroupOpts[nGroupIndex].nMinWorkerThreads = nMinThreads;
    m_TcpRequestGroupOpts[nGroupIndex].nMaxWorkerThreads = nMaxThreads;
}

//-----------------------------------------------------------------------------
// 描述: 设置TCP请求队列中连接数量警戒线
//-----------------------------------------------------------------------------
void CServerOptions::SetTcpRequestQueueAlertLine(int nCount)
{
    if (nCount < 1) nCount = 1;
    m_nTcpRequestQueueAlertLine = nCount;
}

//-----------------------------------------------------------------------------
// 描述: 取得UDP请求队列的最大容量 (即可容纳多少个数据包)
// 参数:
//   nGroupIndex - 组别号 (0-based)
//-----------------------------------------------------------------------------
int CServerOptions::GetUdpRequestQueueCapacity(int nGroupIndex)
{
    if (nGroupIndex < 0 || nGroupIndex >= m_nUdpRequestGroupCount) return -1;

    return m_UdpRequestGroupOpts[nGroupIndex].nRequestQueueCapacity;
}

//-----------------------------------------------------------------------------
// 描述: 取得UDP工作者线程个数的上下限
// 参数:
//   nGroupIndex - 组别号 (0-based)
//   nMinThreads - 存放线程个数的下限
//   nMaxThreads - 存放线程个数的上限
//-----------------------------------------------------------------------------
void CServerOptions::GetUdpWorkerThreadCount(int nGroupIndex, int& nMinThreads, int& nMaxThreads)
{
    if (nGroupIndex < 0 || nGroupIndex >= m_nUdpRequestGroupCount) return;

    nMinThreads = m_UdpRequestGroupOpts[nGroupIndex].nMinWorkerThreads;
    nMaxThreads = m_UdpRequestGroupOpts[nGroupIndex].nMaxWorkerThreads;
}

//-----------------------------------------------------------------------------
// 描述: 取得TCP服务端口号
// 参数:
//   nGroupIndex - 组别号 (0-based)
//-----------------------------------------------------------------------------
int CServerOptions::GetTcpServerPort(int nGroupIndex)
{
    if (nGroupIndex < 0 || nGroupIndex >= m_nTcpRequestGroupCount) return -1;

    return m_TcpRequestGroupOpts[nGroupIndex].nTcpServerPort;
}

//-----------------------------------------------------------------------------
// 描述: 取得TCP请求队列的最大容量 (即可容纳多少个连接)
// 参数:
//   nGroupIndex - 组别号 (0-based)
//-----------------------------------------------------------------------------
int CServerOptions::GetTcpRequestQueueCapacity(int nGroupIndex)
{
    if (nGroupIndex < 0 || nGroupIndex >= m_nTcpRequestGroupCount) return -1;

    return m_TcpRequestGroupOpts[nGroupIndex].nRequestQueueCapacity;
}

//-----------------------------------------------------------------------------
// 描述: 取得TCP工作者线程个数的上下限
// 参数:
//   nGroupIndex - 组别号 (0-based)
//   nMinThreads - 存放线程个数的下限
//   nMaxThreads - 存放线程个数的上限
//-----------------------------------------------------------------------------
void CServerOptions::GetTcpWorkerThreadCount(int nGroupIndex, int& nMinThreads, int& nMaxThreads)
{
    if (nGroupIndex < 0 || nGroupIndex >= m_nTcpRequestGroupCount) return;

    nMinThreads = m_TcpRequestGroupOpts[nGroupIndex].nMinWorkerThreads;
    nMaxThreads = m_TcpRequestGroupOpts[nGroupIndex].nMaxWorkerThreads;
}

///////////////////////////////////////////////////////////////////////////////
// class CThreadTimeoutChecker

CThreadTimeoutChecker::CThreadTimeoutChecker(CThread *pThread) :
    m_pThread(pThread),
    m_tStartTime(0),
    m_bStarted(false),
    m_nTimeoutSecs(0)
{
}

//-----------------------------------------------------------------------------
// 描述: 开始计时
//-----------------------------------------------------------------------------
void CThreadTimeoutChecker::Start()
{
    CAutoSynchronizer Syncher(m_Lock);

    m_tStartTime = time(NULL);
    m_bStarted = true;
}

//-----------------------------------------------------------------------------
// 描述: 停止计时
//-----------------------------------------------------------------------------
void CThreadTimeoutChecker::Stop()
{
    CAutoSynchronizer Syncher(m_Lock);

    m_bStarted = false;
}

//-----------------------------------------------------------------------------
// 描述: 检测线程是否已超时，若超时则通知其退出
//-----------------------------------------------------------------------------
bool CThreadTimeoutChecker::Check()
{
    bool bResult = false;

    if (m_bStarted && m_nTimeoutSecs > 0)
    {
        if ((uint)time(NULL) - m_tStartTime >= m_nTimeoutSecs)
        {
            if (!m_pThread->GetTerminated()) m_pThread->Terminate();
            bResult = true;
        }
    }

    return bResult;
}

//-----------------------------------------------------------------------------
// 描述: 返回是否已开始计时
//-----------------------------------------------------------------------------
bool CThreadTimeoutChecker::GetStarted()
{
    CAutoSynchronizer Syncher(m_Lock);

    return m_bStarted;
}

///////////////////////////////////////////////////////////////////////////////
// class CUdpPacket

void CUdpPacket::SetPacketBuffer(void *pPakcetBuffer, int nPacketSize)
{
    if (m_pPacketBuffer) 
    {
        free(m_pPacketBuffer);
        m_pPacketBuffer = NULL;
    }

    m_pPacketBuffer = malloc(nPacketSize);
    if (!m_pPacketBuffer)
        throw CException("out of memory", __FILE__, __LINE__);

    memcpy(m_pPacketBuffer, pPakcetBuffer, nPacketSize);
}

///////////////////////////////////////////////////////////////////////////////
// class CUdpRequestQueue

//-----------------------------------------------------------------------------
// 描述: 构造函数
// 参数:
//   pOwnGroup - 指定所属组别
//-----------------------------------------------------------------------------
CUdpRequestQueue::CUdpRequestQueue(CUdpRequestGroup *pOwnGroup)
{
    int nGroupIndex;

    m_pOwnGroup = pOwnGroup;
    nGroupIndex = pOwnGroup->GetGroupIndex();
    m_nCapacity = Application.GetServerOptions().GetUdpRequestQueueCapacity(nGroupIndex);
    m_nEffWaitTime = Application.GetServerOptions().GetUdpRequestEffWaitTime();
    m_nPacketCount = 0;
}

//-----------------------------------------------------------------------------
// 描述: 向队列中添加数据包
//-----------------------------------------------------------------------------
void CUdpRequestQueue::AddPacket(CUdpPacket *pPacket)
{
    if (m_nCapacity <= 0) return;
    bool bRemoved = false;

    {
        CAutoSynchronizer Syncher(m_Lock);

        if (m_nPacketCount >= m_nCapacity)
        {
            CUdpPacket *p;
            p = m_PacketList.front();
            delete p;
            m_PacketList.pop_front();
            m_nPacketCount--;
            bRemoved = true;
        }

        m_PacketList.push_back(pPacket);
        m_nPacketCount++;
    }

    if (!bRemoved) m_ThreadSem.Increase();
}

//-----------------------------------------------------------------------------
// 描述: 从队列中取出数据包 (取出后应自行释放，若失败则返回 NULL)
// 备注: 若队列中尚没有数据包，则一直等待。
//-----------------------------------------------------------------------------
CUdpPacket* CUdpRequestQueue::ExtractPacket()
{
    m_ThreadSem.Wait();

    {
        CAutoSynchronizer Syncher(m_Lock);
        CUdpPacket *p, *pResult = NULL;

        while (m_nPacketCount > 0)
        {
            p = m_PacketList.front();
            m_PacketList.pop_front();
            m_nPacketCount--;

            if (time(NULL) - p->m_nRecvTimeStamp <= m_nEffWaitTime)
            {
                pResult = p;
                break;
            }
            else
            {
                delete p;
            }
        }
    
        return pResult;
    }
}

//-----------------------------------------------------------------------------
// 描述: 清空队列
//-----------------------------------------------------------------------------
void CUdpRequestQueue::Clear()
{
    CAutoSynchronizer Syncher(m_Lock);
    CUdpPacket *p;

    for (int i = 0; i < m_PacketList.size(); i++)
    {
        p = m_PacketList[i];
        delete p;
    }

    m_PacketList.clear();
    m_nPacketCount = 0;
    m_ThreadSem.Reset();
}

//-----------------------------------------------------------------------------
// 描述: 增加信号量的值，使等待数据的线程中断等待
//-----------------------------------------------------------------------------
void CUdpRequestQueue::BreakWaiting(int nSemCount)
{
    for (int i = 0; i < nSemCount; i++)
        m_ThreadSem.Increase();
}

///////////////////////////////////////////////////////////////////////////////
// class CUdpWorkerThread

CUdpWorkerThread::CUdpWorkerThread(CUdpWorkerThreadPool *pThreadPool) :
    m_pOwnPool(pThreadPool),
    m_TimeoutChecker(this)
{
    SetFreeOnTerminate(true);
    // 启用超时检测
    m_TimeoutChecker.SetTimeoutSecs(Application.GetServerOptions().GetUdpWorkerThreadTimeout());

    m_pOwnPool->RegisterThread(this);
}

CUdpWorkerThread::~CUdpWorkerThread()
{
    m_pOwnPool->UnregisterThread(this);
}

//-----------------------------------------------------------------------------
// 描述: 线程的执行函数
//-----------------------------------------------------------------------------
void CUdpWorkerThread::Execute()
{
    int nGroupIndex;
    CUdpRequestQueue *pRequestQueue;
    CCustomDispatcher *pDispatcher;
    CUdpPacket *pPacket;

    nGroupIndex = m_pOwnPool->GetRequestGroup().GetGroupIndex(); 
    pRequestQueue = &(m_pOwnPool->GetRequestGroup().GetRequestQueue());
    pDispatcher = &(m_pOwnPool->GetRequestGroup().GetMainUdpServer().GetDispatcher());

    while (!GetTerminated())
    try
    {
        pPacket = pRequestQueue->ExtractPacket();
        if (pPacket)
        {
            std::auto_ptr<CUdpPacket> AutoPtr(pPacket);
            CAutoInvoker AutoInvker(m_TimeoutChecker);

            // 分派数据包
            if (!GetTerminated())
                pDispatcher->DispatchUdpPacket(*this, nGroupIndex, *pPacket);
        }
    }
    catch (CException& e)
    { ; }
}

//-----------------------------------------------------------------------------
// 描述: 执行 Terminate() 前的附加操作
//-----------------------------------------------------------------------------
void CUdpWorkerThread::DoTerminate()
{
    //Logger.Write("CUdpWorkerThread be terminate");  // debug
}

//-----------------------------------------------------------------------------
// 描述: 执行 Kill() 前的附加操作
//-----------------------------------------------------------------------------
void CUdpWorkerThread::DoKill()
{
    //Logger.Write("CUdpWorkerThread be killed");  // debug
}

///////////////////////////////////////////////////////////////////////////////
// class CUdpWorkerThreadPool

CUdpWorkerThreadPool::CUdpWorkerThreadPool(CUdpRequestGroup *pOwnGroup) :
    m_pOwnGroup(pOwnGroup)
{
}

CUdpWorkerThreadPool::~CUdpWorkerThreadPool()
{
}

//-----------------------------------------------------------------------------
// 描述: 创建 nCount 个线程
//-----------------------------------------------------------------------------
void CUdpWorkerThreadPool::CreateThreads(int nCount)
{
    for (int i = 0; i < nCount; i++)
    {
        CUdpWorkerThread *pThread;
        pThread = new CUdpWorkerThread(this);
        pThread->Run();
    }
}

//-----------------------------------------------------------------------------
// 描述: 终止 nCount 个线程
//-----------------------------------------------------------------------------
void CUdpWorkerThreadPool::TerminateThreads(int nCount)
{
    CAutoSynchronizer Syncher(m_Lock);

    int nTermCount = 0;
    if (nCount > m_ThreadList.Count())
        nCount = m_ThreadList.Count();

    for (int i = m_ThreadList.Count() - 1; i >= 0; i--)
    {
        CUdpWorkerThread *pThread;
        pThread = (CUdpWorkerThread*)m_ThreadList[i];
        if (pThread->IsIdle())
        {
            pThread->Terminate();
            nTermCount++;
            if (nTermCount >= nCount) break;
        }
    }
}

//-----------------------------------------------------------------------------
// 描述: 检测线程是否工作超时 (超时线程: 因某一请求进入工作状态但长久未完成的线程)
//-----------------------------------------------------------------------------
void CUdpWorkerThreadPool::CheckThreadTimeout()
{
    CAutoSynchronizer Syncher(m_Lock);

    for (int i = 0; i < m_ThreadList.Count(); i++)
    {
        CUdpWorkerThread *pThread;
        pThread = (CUdpWorkerThread*)m_ThreadList[i];
        pThread->GetTimeoutChecker().Check();
    }
}

//-----------------------------------------------------------------------------
// 描述: 强行杀死僵死的线程 (僵死线程: 已被通知退出但长久不退出的线程)
//-----------------------------------------------------------------------------
void CUdpWorkerThreadPool::KillZombieThreads()
{
    CAutoSynchronizer Syncher(m_Lock);

    for (int i = m_ThreadList.Count() - 1; i >= 0; i--)
    {
        CUdpWorkerThread *pThread;
        pThread = (CUdpWorkerThread*)m_ThreadList[i];
        if (pThread->GetTermElapsedSecs() >= MAX_THREAD_TERM_SECS)
        {
            pThread->Kill();
            m_ThreadList.Remove(pThread);
        }
    }
}

//-----------------------------------------------------------------------------
// 描述: 注册线程
//-----------------------------------------------------------------------------
void CUdpWorkerThreadPool::RegisterThread(CUdpWorkerThread *pThread)
{
    CAutoSynchronizer Syncher(m_Lock);

    if (m_ThreadList.IndexOf(pThread) == -1)
        m_ThreadList.Add(pThread);
}

//-----------------------------------------------------------------------------
// 描述: 注销线程
//-----------------------------------------------------------------------------
void CUdpWorkerThreadPool::UnregisterThread(CUdpWorkerThread *pThread)
{
    CAutoSynchronizer Syncher(m_Lock);

    m_ThreadList.Remove(pThread);
}

//-----------------------------------------------------------------------------
// 描述: 根据负载情况动态调整线程数量
//-----------------------------------------------------------------------------
void CUdpWorkerThreadPool::AdjustThreadCount()
{
    int nPacketCount, nThreadCount;
    int nMinThreads, nMaxThreads, nDeltaThreads;
    int nPacketAlertLine;

    // 取得线程数量上下限
    Application.GetServerOptions().GetUdpWorkerThreadCount(
        m_pOwnGroup->GetGroupIndex(), nMinThreads, nMaxThreads);
    // 取得请求队列中数据包数量警戒线
    nPacketAlertLine = Application.GetServerOptions().GetUdpRequestQueueAlertLine();

    // 检测线程是否工作超时
    CheckThreadTimeout();
    // 强行杀死僵死的线程
    KillZombieThreads();

    // 取得数据包数量和线程数量
    nPacketCount = m_pOwnGroup->GetRequestQueue().GetCount();
    nThreadCount = m_ThreadList.Count();

    // 保证线程数量在上下限范围之内
    if (nThreadCount < nMinThreads)
    {
        CreateThreads(nMinThreads - nThreadCount);
        Logger.Write("CreateThreads:%d", nMinThreads - nThreadCount);
        nThreadCount = nMinThreads;
    }
    if (nThreadCount > nMaxThreads)
    {
        TerminateThreads(nThreadCount - nMaxThreads);
        Logger.Write("TerminateThreads:%d", nThreadCount - nMaxThreads);
        nThreadCount = nMaxThreads;
    }

    // 如果请求队列中的数量超过警戒线，则尝试增加线程数量
    if (nThreadCount < nMaxThreads && nPacketCount >= nPacketAlertLine)
    {
        nDeltaThreads = Min(nMaxThreads - nThreadCount, 3);
        CreateThreads(nDeltaThreads);
        Logger.Write("CreateThreads1:%d", nDeltaThreads);
    }

    // 如果请求队列为空，则尝试减少线程数量
    if (nThreadCount > nMinThreads && nPacketCount == 0)
    {
        nDeltaThreads = 1;
        TerminateThreads(nDeltaThreads);
        Logger.Write("TerminateThreads1:%d", nDeltaThreads);
    }
}

//-----------------------------------------------------------------------------
// 描述: 通知所有线程退出
//-----------------------------------------------------------------------------
void CUdpWorkerThreadPool::TerminateAllThreads()
{
    CAutoSynchronizer Syncher(m_Lock);

    for (int i = 0; i < m_ThreadList.Count(); i++)
    {
        CUdpWorkerThread *pThread;
        pThread = (CUdpWorkerThread*)m_ThreadList[i];
        pThread->Terminate();
    }

    // 使线程从等待中解脱，尽快退出
    GetRequestGroup().GetRequestQueue().BreakWaiting(m_ThreadList.Count());
}

//-----------------------------------------------------------------------------
// 描述: 等待所有线程退出
//-----------------------------------------------------------------------------
void CUdpWorkerThreadPool::WaitForAllThreads()
{
    const double SLEEP_INTERVAL = 0.5;  // (秒)
    double nWaitSecs = 0;
    
    TerminateAllThreads();

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
        int n = m_ThreadList.Count();
    
        for (int i = 0; i < m_ThreadList.Count(); i++)
        {
            CUdpWorkerThread *pThread;
            pThread = (CUdpWorkerThread*)m_ThreadList[i];
            pThread->Kill();
        }

        m_ThreadList.Clear(); 

        Logger.Write("Killed udp worker threads. (GroupIndex:%d, ThreadCount:%d)", 
            GetRequestGroup().GetGroupIndex(), n);
    }
}

///////////////////////////////////////////////////////////////////////////////
// class CUdpRequestGroup

CUdpRequestGroup::CUdpRequestGroup(CMainUdpServer *pOwnMainUdpSvr, int nGroupIndex) :
    m_pOwnMainUdpSvr(pOwnMainUdpSvr),
    m_nGroupIndex(nGroupIndex),
    m_RequestQueue(this),
    m_ThreadPool(this)
{
}

///////////////////////////////////////////////////////////////////////////////
// class CMainUdpServer

CMainUdpServer::CMainUdpServer(CCustomDispatcher *pDispatcher) :
    m_pDispatcher(pDispatcher)
{
    InitRequestGroupList();
}

CMainUdpServer::~CMainUdpServer()
{
    ClearRequestGroupList();
}

//-----------------------------------------------------------------------------
// 描述: 初始化请求组别列表
//-----------------------------------------------------------------------------
void CMainUdpServer::InitRequestGroupList()
{
    ClearRequestGroupList();

    m_nRequestGroupCount = Application.GetServerOptions().GetUdpRequestGroupCount();
    for (int nGroupIndex = 0; nGroupIndex < m_nRequestGroupCount; nGroupIndex++)
    {
        CUdpRequestGroup *p;
        p = new CUdpRequestGroup(this, nGroupIndex);
        m_RequestGroupList.push_back(p);
    }
}

//-----------------------------------------------------------------------------
// 描述: 清空请求组别列表
//-----------------------------------------------------------------------------
void CMainUdpServer::ClearRequestGroupList()
{
    for (int i = 0; i < m_RequestGroupList.size(); i++)
        delete m_RequestGroupList[i];
    m_RequestGroupList.clear();
}

//-----------------------------------------------------------------------------
// 描述: 关闭服务器
//-----------------------------------------------------------------------------
void CMainUdpServer::Close()
{
    TerminateAllWorkerThreads();
    WaitForAllWorkerThreads();
    CUdpServer::Close();
}

//-----------------------------------------------------------------------------
// 描述: 收到数据包
//-----------------------------------------------------------------------------
void CMainUdpServer::OnRecvData(void *pPacketBuffer, int nPacketSize, const PeerAddress& RemoteAddr)
{
    int nGroupIndex;
    CUdpPacket *p;

    // 先进行数据包分类，得到组别号
    m_pDispatcher->ClassifyUdpPacket(pPacketBuffer, nPacketSize, nGroupIndex);

    // 如果组别号合法
    if (nGroupIndex >= 0 && nGroupIndex < m_nRequestGroupCount)
    {
        p = new CUdpPacket();
        if (p)
        {
            p->m_nRecvTimeStamp = time(NULL);
            p->m_RemoteAddr = RemoteAddr;
            p->m_nPacketSize = nPacketSize;
            p->SetPacketBuffer(pPacketBuffer, nPacketSize);

            // 添加到请求队列中
            m_RequestGroupList[nGroupIndex]->GetRequestQueue().AddPacket(p);
        }
    }
}

//-----------------------------------------------------------------------------
// 描述: 根据负载情况动态调整工作者线程数量
//-----------------------------------------------------------------------------
void CMainUdpServer::AdjustWorkerThreadCount()
{
    for (int i = 0; i < m_RequestGroupList.size(); i++)
        m_RequestGroupList[i]->GetThreadPool().AdjustThreadCount();
}

//-----------------------------------------------------------------------------
// 描述: 通知所有工作者线程退出
//-----------------------------------------------------------------------------
void CMainUdpServer::TerminateAllWorkerThreads()
{
    for (int i = 0; i < m_RequestGroupList.size(); i++)
        m_RequestGroupList[i]->GetThreadPool().TerminateAllThreads();
}

//-----------------------------------------------------------------------------
// 描述: 等待所有工作者线程退出
//-----------------------------------------------------------------------------
void CMainUdpServer::WaitForAllWorkerThreads()
{
    for (int i = 0; i < m_RequestGroupList.size(); i++)
        m_RequestGroupList[i]->GetThreadPool().WaitForAllThreads();
}

///////////////////////////////////////////////////////////////////////////////
// class CTcpRequestQueue

CTcpRequestQueue::CTcpRequestQueue(CTcpRequestGroup *pOwnGroup)
{
    int nGroupIndex;

    m_pOwnGroup = pOwnGroup;
    nGroupIndex = pOwnGroup->GetGroupIndex();
    m_nCapacity = Application.GetServerOptions().GetTcpRequestQueueCapacity(nGroupIndex);
}

//-----------------------------------------------------------------------------
// 描述: 向队列中添加TCP连接
//-----------------------------------------------------------------------------
void CTcpRequestQueue::AddTcpConnect(CTcpConnection *pTcpConn)
{
    if (m_nCapacity <= 0) return;
    bool bRemoved = false;

    {
        CAutoSynchronizer Syncher(m_Lock);

        if (m_TcpConnList.size() >= m_nCapacity)
        {
            CTcpConnection *p;
            p = m_TcpConnList.front();
            delete p;
            m_TcpConnList.pop_front();
            bRemoved = true;
        }

        m_TcpConnList.push_back(pTcpConn);
    }

    if (!bRemoved) m_ThreadSem.Increase();
}

//-----------------------------------------------------------------------------
// 描述: 从队列中取出TCP连接 (取出后应自行释放，若失败则返回 NULL)
// 备注: 若队列中尚没有连接，则一直等待。
//-----------------------------------------------------------------------------
CTcpConnection* CTcpRequestQueue::ExtractTcpConnect()
{
    m_ThreadSem.Wait();

    {
        CAutoSynchronizer Syncher(m_Lock);
        CTcpConnection *pResult = NULL;

        if (m_TcpConnList.size() > 0)
        {
            pResult = m_TcpConnList.front();
            m_TcpConnList.pop_front();
        }

        return pResult;
    }
}

//-----------------------------------------------------------------------------
// 描述: 清空队列
//-----------------------------------------------------------------------------
void CTcpRequestQueue::Clear()
{
    CAutoSynchronizer Syncher(m_Lock);
    CTcpConnection *p;

    for (int i = 0; i < m_TcpConnList.size(); i++)
    {
        p = m_TcpConnList[i];
        delete p;
    }

    m_TcpConnList.clear();
    m_ThreadSem.Reset();
}

//-----------------------------------------------------------------------------
// 描述: 增加信号量的值，使等待数据的线程中断等待
//-----------------------------------------------------------------------------
void CTcpRequestQueue::BreakWaiting(int nSemCount)
{
    for (int i = 0; i < nSemCount; i++)
        m_ThreadSem.Increase();
}

///////////////////////////////////////////////////////////////////////////////
// class CTcpWorkerThread

CTcpWorkerThread::CTcpWorkerThread(CTcpWorkerThreadPool *pThreadPool) :
    m_pOwnPool(pThreadPool),
    m_TimeoutChecker(this)
{
    SetFreeOnTerminate(true);
    // 禁用超时检测
    m_TimeoutChecker.SetTimeoutSecs(0);

    m_pOwnPool->RegisterThread(this);
}

CTcpWorkerThread::~CTcpWorkerThread()
{
    m_pOwnPool->UnregisterThread(this);
}

//-----------------------------------------------------------------------------
// 描述: 线程的执行函数
//-----------------------------------------------------------------------------
void CTcpWorkerThread::Execute()
{
    int nGroupIndex;
    CTcpRequestQueue *pRequestQueue;
    CCustomDispatcher *pDispatcher;
    CTcpConnection *pTcpConn;

    nGroupIndex = m_pOwnPool->GetRequestGroup().GetGroupIndex(); 
    pRequestQueue = &(m_pOwnPool->GetRequestGroup().GetRequestQueue());
    pDispatcher = &(m_pOwnPool->GetRequestGroup().GetMainTcpServer().GetDispatcher());

    while (!GetTerminated())
    try
    {
        pTcpConn = pRequestQueue->ExtractTcpConnect();
        if (pTcpConn)
        {
            std::auto_ptr<CTcpConnection> AutoPtr(pTcpConn);
            CAutoInvoker AutoInvker(m_TimeoutChecker);

            if (!GetTerminated())
            {
                // TCP工作者线程一旦开始工作，便不可随便退出
                SetTerminated(false);
                // 分派TCP连接
                pDispatcher->DispatchTcpConnection(*this, nGroupIndex, *pTcpConn);
            }
        }
    }
    catch (CException& e)
    { ; }
}

//-----------------------------------------------------------------------------
// 描述: 执行 Terminate() 前的附加操作
//-----------------------------------------------------------------------------
void CTcpWorkerThread::DoTerminate()
{
    // nothing
}

//-----------------------------------------------------------------------------
// 描述: 执行 Kill() 前的附加操作
//-----------------------------------------------------------------------------
void CTcpWorkerThread::DoKill()
{
    //Logger.Write("CTcpWorkerThread be killed");  // debug
}

///////////////////////////////////////////////////////////////////////////////
// class CTcpWorkerThreadPool

CTcpWorkerThreadPool::CTcpWorkerThreadPool(CTcpRequestGroup *pOwnGroup) :
    m_pOwnGroup(pOwnGroup)
{
}

CTcpWorkerThreadPool::~CTcpWorkerThreadPool()
{
}

//-----------------------------------------------------------------------------
// 描述: 创建 nCount 个线程
//-----------------------------------------------------------------------------
void CTcpWorkerThreadPool::CreateThreads(int nCount)
{
    for (int i = 0; i < nCount; i++)
    {
        CTcpWorkerThread *pThread;
        pThread = new CTcpWorkerThread(this);
        pThread->Run();
    }
}

//-----------------------------------------------------------------------------
// 描述: 终止 nCount 个线程
//-----------------------------------------------------------------------------
void CTcpWorkerThreadPool::TerminateThreads(int nCount)
{
    CAutoSynchronizer Syncher(m_Lock);

    int nTermCount = 0;
    if (nCount > m_ThreadList.Count())
        nCount = m_ThreadList.Count();

    for (int i = m_ThreadList.Count() - 1; i >= 0; i--)
    {
        CTcpWorkerThread *pThread;
        pThread = (CTcpWorkerThread*)m_ThreadList[i];
        if (pThread->IsIdle())
        {
            pThread->Terminate();
            nTermCount++;
            if (nTermCount >= nCount) break;
        }
    }
}

//-----------------------------------------------------------------------------
// 描述: 检测线程是否工作超时 (超时线程: 因某一请求进入工作状态但长久未完成的线程)
//-----------------------------------------------------------------------------
void CTcpWorkerThreadPool::CheckThreadTimeout()
{
    CAutoSynchronizer Syncher(m_Lock);

    for (int i = 0; i < m_ThreadList.Count(); i++)
    {
        CTcpWorkerThread *pThread;
        pThread = (CTcpWorkerThread*)m_ThreadList[i];
        pThread->GetTimeoutChecker().Check();
    }
}

//-----------------------------------------------------------------------------
// 描述: 强行杀死僵死的线程 (僵死线程: 已被通知退出但长久不退出的线程)
//-----------------------------------------------------------------------------
void CTcpWorkerThreadPool::KillZombieThreads()
{
    CAutoSynchronizer Syncher(m_Lock);

    for (int i = m_ThreadList.Count() - 1; i >= 0; i--)
    {
        CTcpWorkerThread *pThread;
        pThread = (CTcpWorkerThread*)m_ThreadList[i];
        if (pThread->GetTermElapsedSecs() >= MAX_THREAD_TERM_SECS)
        {
            pThread->Kill();
            m_ThreadList.Remove(pThread);
        }
    }
}

//-----------------------------------------------------------------------------
// 描述: 注册线程
//-----------------------------------------------------------------------------
void CTcpWorkerThreadPool::RegisterThread(CTcpWorkerThread *pThread)
{
    CAutoSynchronizer Syncher(m_Lock);

    if (m_ThreadList.IndexOf(pThread) == -1)
        m_ThreadList.Add(pThread);
}

//-----------------------------------------------------------------------------
// 描述: 注销线程
//-----------------------------------------------------------------------------
void CTcpWorkerThreadPool::UnregisterThread(CTcpWorkerThread *pThread)
{
    CAutoSynchronizer Syncher(m_Lock);

    m_ThreadList.Remove(pThread);
}

//-----------------------------------------------------------------------------
// 描述: 根据负载情况动态调整线程数量
//-----------------------------------------------------------------------------
void CTcpWorkerThreadPool::AdjustThreadCount()
{
    int nTcpConnCount, nThreadCount;
    int nMinThreads, nMaxThreads, nDeltaThreads;
    int nConnAlertLine;

    // 取得线程数量上下限
    Application.GetServerOptions().GetTcpWorkerThreadCount(
        m_pOwnGroup->GetGroupIndex(), nMinThreads, nMaxThreads);
    // 取得TCP请求队列中连接数量警戒线
    nConnAlertLine = Application.GetServerOptions().GetTcpRequestQueueAlertLine();

    // 检测线程是否工作超时
    CheckThreadTimeout();
    // 强行杀死僵死的线程
    KillZombieThreads();

    // 取得数据包数量和线程数量
    nTcpConnCount = m_pOwnGroup->GetRequestQueue().GetCount(); 
    nThreadCount = m_ThreadList.Count();

    // 保证线程数量在上下限范围之内
    if (nThreadCount < nMinThreads)
    {
        CreateThreads(nMinThreads - nThreadCount);
        nThreadCount = nMinThreads;
    }
    if (nThreadCount > nMaxThreads)
    {
        TerminateThreads(nThreadCount - nMaxThreads);
        nThreadCount = nMaxThreads;
    }

    // 如果TCP请求队列中的数量超过警戒线，则尝试增加线程数量
    if (nThreadCount < nMaxThreads && nTcpConnCount >= nConnAlertLine)
    {
        nDeltaThreads = Min(nMaxThreads - nThreadCount, 3);
        CreateThreads(nDeltaThreads);
    }

    // 如果TCP请求队列为空，则尝试减少线程数量
    if (nThreadCount > nMinThreads && nTcpConnCount == 0)
    {
        nDeltaThreads = 1;
        TerminateThreads(nDeltaThreads);
    }
    
    //Logger.Write("tcp conn:%d, thd:%d", nTcpConnCount, nThreadCount);  // debug
}

//-----------------------------------------------------------------------------
// 描述: 通知所有线程退出
//-----------------------------------------------------------------------------
void CTcpWorkerThreadPool::TerminateAllThreads()
{
    CAutoSynchronizer Syncher(m_Lock);

    for (int i = 0; i < m_ThreadList.Count(); i++)
    {
        CTcpWorkerThread *pThread;
        pThread = (CTcpWorkerThread*)m_ThreadList[i];
        pThread->Terminate();
    }

    // 使线程从等待中解脱，尽快退出
    GetRequestGroup().GetRequestQueue().BreakWaiting(m_ThreadList.Count());
}

//-----------------------------------------------------------------------------
// 描述: 等待所有线程退出
//-----------------------------------------------------------------------------
void CTcpWorkerThreadPool::WaitForAllThreads()
{
    const double SLEEP_INTERVAL = 0.5;  // (秒)
    double nWaitSecs = 0;
    
    TerminateAllThreads();

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
        int n = m_ThreadList.Count();
    
        for (int i = 0; i < m_ThreadList.Count(); i++)
        {
            CTcpWorkerThread *pThread;
            pThread = (CTcpWorkerThread*)m_ThreadList[i];
            pThread->Kill();
        }

        m_ThreadList.Clear(); 

        Logger.Write("Killed tcp worker threads. (ThreadCount:%d)", n);
    }
}

///////////////////////////////////////////////////////////////////////////////
// class CTcpRequestGroup

CTcpRequestGroup::CTcpRequestGroup(CMainTcpServer *pOwnMainTcpSvr, 
    int nGroupIndex, int nTcpPort) :
        m_pOwnMainTcpSvr(pOwnMainTcpSvr),
        m_nGroupIndex(nGroupIndex),
        m_nTcpPort(nTcpPort),
        m_RequestQueue(this),
        m_ThreadPool(this)
{
}

//-----------------------------------------------------------------------------
// 描述: 关闭服务器
//-----------------------------------------------------------------------------
void CTcpRequestGroup::Close()
{
    CTcpServer::Close();
    m_RequestQueue.Clear();
}

//-----------------------------------------------------------------------------
// 描述: 收到连接 (注: pConnection 是堆对象，需使用者释放)
//-----------------------------------------------------------------------------
void CTcpRequestGroup::OnConnect(CTcpConnection *pConnection)
{
    // 添加到TCP请求队列中
    m_RequestQueue.AddTcpConnect(pConnection);
}

///////////////////////////////////////////////////////////////////////////////
// class CMainTcpServer

CMainTcpServer::CMainTcpServer(CCustomDispatcher *pDispatcher) :
    m_pDispatcher(pDispatcher)
{
    InitRequestGroupList();
}

CMainTcpServer::~CMainTcpServer()
{
    ClearRequestGroupList();
}

//-----------------------------------------------------------------------------
// 描述: 初始化请求组别列表
//-----------------------------------------------------------------------------
void CMainTcpServer::InitRequestGroupList()
{
    ClearRequestGroupList();

    m_nRequestGroupCount = Application.GetServerOptions().GetTcpRequestGroupCount();
    for (int nGroupIndex = 0; nGroupIndex < m_nRequestGroupCount; nGroupIndex++)
    {
        CTcpRequestGroup *p;
        int nTcpPort;

        nTcpPort = Application.GetServerOptions().GetTcpServerPort(nGroupIndex);
        p = new CTcpRequestGroup(this, nGroupIndex, nTcpPort);
        m_RequestGroupList.push_back(p);
    }
}

//-----------------------------------------------------------------------------
// 描述: 清空请求组别列表
//-----------------------------------------------------------------------------
void CMainTcpServer::ClearRequestGroupList()
{
    for (int i = 0; i < m_RequestGroupList.size(); i++)
        delete m_RequestGroupList[i];
    m_RequestGroupList.clear();
}

//-----------------------------------------------------------------------------
// 描述: 开启服务器
//-----------------------------------------------------------------------------
void CMainTcpServer::Open()
{
    for (int i = 0; i < m_RequestGroupList.size(); i++)
    {
        int nPort;
        nPort = Application.GetServerOptions().GetTcpServerPort(i);
        m_RequestGroupList[i]->SetLocalPort(nPort);
        m_RequestGroupList[i]->Open();
    }
}

//-----------------------------------------------------------------------------
// 描述: 关闭服务器
//-----------------------------------------------------------------------------
void CMainTcpServer::Close()
{
    TerminateAllWorkerThreads();
    WaitForAllWorkerThreads();
    for (int i = 0; i < m_RequestGroupList.size(); i++)
        m_RequestGroupList[i]->Close();
}

//-----------------------------------------------------------------------------
// 描述: 根据负载情况动态调整工作者线程数量
//-----------------------------------------------------------------------------
void CMainTcpServer::AdjustWorkerThreadCount()
{
    for (int i = 0; i < m_RequestGroupList.size(); i++)
        m_RequestGroupList[i]->GetThreadPool().AdjustThreadCount();
}

//-----------------------------------------------------------------------------
// 描述: 通知所有工作者线程退出
//-----------------------------------------------------------------------------
void CMainTcpServer::TerminateAllWorkerThreads()
{
    for (int i = 0; i < m_RequestGroupList.size(); i++)
        m_RequestGroupList[i]->GetThreadPool().TerminateAllThreads();
}

//-----------------------------------------------------------------------------
// 描述: 等待所有工作者线程退出
//-----------------------------------------------------------------------------
void CMainTcpServer::WaitForAllWorkerThreads()
{
    for (int i = 0; i < m_RequestGroupList.size(); i++)
        m_RequestGroupList[i]->GetThreadPool().WaitForAllThreads();
}

///////////////////////////////////////////////////////////////////////////////
// class CHelperThread

CHelperThread::CHelperThread(CHelperThreadPool *pThreadPool, int nHelperIndex) :
    m_pOwnPool(pThreadPool),
    m_nHelperIndex(nHelperIndex)
{
    SetFreeOnTerminate(true);
    m_pOwnPool->RegisterThread(this);
}

CHelperThread::~CHelperThread()
{
    m_pOwnPool->UnregisterThread(this);
}

//-----------------------------------------------------------------------------
// 描述: 线程执行函数
//-----------------------------------------------------------------------------
void CHelperThread::Execute()
{
    m_pOwnPool->GetHelperServer().OnHelperThreadExecute(*this, m_nHelperIndex);
}

//-----------------------------------------------------------------------------
// 描述: 执行 Kill() 前的附加操作
//-----------------------------------------------------------------------------
void CHelperThread::DoKill()
{
    //Logger.Write("CHelperThread be killed"); // debug
}

///////////////////////////////////////////////////////////////////////////////
// class CHelperThreadPool

CHelperThreadPool::CHelperThreadPool(CHelperServer *pOwnHelperServer) :
    m_pOwnHelperSvr(pOwnHelperServer)
{
}

CHelperThreadPool::~CHelperThreadPool()
{
}

//-----------------------------------------------------------------------------
// 描述: 注册线程
//-----------------------------------------------------------------------------
void CHelperThreadPool::RegisterThread(CHelperThread *pThread)
{
    CAutoSynchronizer Syncher(m_Lock);

    if (m_ThreadList.IndexOf(pThread) == -1)
        m_ThreadList.Add(pThread);
}

//-----------------------------------------------------------------------------
// 描述: 注销线程
//-----------------------------------------------------------------------------
void CHelperThreadPool::UnregisterThread(CHelperThread *pThread)
{
    CAutoSynchronizer Syncher(m_Lock);

    m_ThreadList.Remove(pThread);
}

//-----------------------------------------------------------------------------
// 描述: 通知所有线程退出
//-----------------------------------------------------------------------------
void CHelperThreadPool::TerminateAllThreads()
{
    CAutoSynchronizer Syncher(m_Lock);

    for (int i = 0; i < m_ThreadList.Count(); i++)
    {
        CHelperThread *pThread;
        pThread = (CHelperThread*)m_ThreadList[i];
        pThread->Terminate();
    }
}

//-----------------------------------------------------------------------------
// 描述: 等待所有线程退出
//-----------------------------------------------------------------------------
void CHelperThreadPool::WaitForAllThreads()
{
    const double SLEEP_INTERVAL = 0.5;  // (秒)
    double nWaitSecs = 0;
    
    TerminateAllThreads();

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
        int n = m_ThreadList.Count();
    
        for (int i = 0; i < m_ThreadList.Count(); i++)
        {
            CHelperThread *pThread;
            pThread = (CHelperThread*)m_ThreadList[i];
            pThread->Kill();
        }

        m_ThreadList.Clear(); 

        Logger.Write("Killed helper threads. (ThreadCount:%d)", n);
    }
}

///////////////////////////////////////////////////////////////////////////////
// class CHelperServer

CHelperServer::CHelperServer(CCustomDispatcher *pDispatcher) :
    m_bActive(false),
    m_pDispatcher(pDispatcher),
    m_ThreadPool(this)
{
}

CHelperServer::~CHelperServer()
{
}

//-----------------------------------------------------------------------------
// 描述: 启动服务器
//-----------------------------------------------------------------------------
void CHelperServer::Open()
{
    if (!m_bActive)
    {
        int nCount = Application.GetServerOptions().GetHelperThreadCount();

        for (int i = 0; i < nCount; i++)
        {
            CHelperThread *pThread;
            pThread = new CHelperThread(&m_ThreadPool, i);
            pThread->Run();
        }

        m_bActive = true;
    }
}

//-----------------------------------------------------------------------------
// 描述: 关闭服务器
//-----------------------------------------------------------------------------
void CHelperServer::Close()
{
    if (m_bActive)
    {
        WaitForAllHelperThreads();
        m_bActive = false;
    }
}

//-----------------------------------------------------------------------------
// 描述: 辅助服务线程执行函数
// 参数:
//   nHelperIndex - 辅助线程序号(0-based)
//-----------------------------------------------------------------------------
void CHelperServer::OnHelperThreadExecute(CHelperThread& HelperThread, int nHelperIndex)
{
    m_pDispatcher->HelperThreadExecute(HelperThread, nHelperIndex);
}

//-----------------------------------------------------------------------------
// 描述: 通知所有辅助线程退出
//-----------------------------------------------------------------------------
void CHelperServer::TerminateAllHelperThreads()
{
    m_ThreadPool.TerminateAllThreads();
}

//-----------------------------------------------------------------------------
// 描述: 等待所有辅助线程退出
//-----------------------------------------------------------------------------
void CHelperServer::WaitForAllHelperThreads()
{
    m_ThreadPool.WaitForAllThreads();
}

///////////////////////////////////////////////////////////////////////////////
// class CMainServer

CMainServer::CMainServer() :
    m_pUdpServer(NULL),
    m_pTcpServer(NULL),
    m_pDispatcher(NULL)
{
}

CMainServer::~CMainServer()
{
}

//-----------------------------------------------------------------------------
// 描述: 服务器开始运行后，主线程进行后台守护工作
//-----------------------------------------------------------------------------
void CMainServer::RunBackground()
{
    int nAdjustThreadInterval = Application.GetServerOptions().GetAdjustThreadInterval();
    int nSecondCount = 0;

    while (!Application.GetTerminated())
    try 
    {
        try
        {
            // 每隔 nAdjustThreadInterval 秒执行一次
            if ((nSecondCount % nAdjustThreadInterval) == 0)
            {
                // 暂时屏蔽退出信号
                CSignalMasker SigMasker(true);
                SigMasker.SetSignals(1, SIGTERM);
                SigMasker.Block();

                // 维护工作者线程的数量
                if (m_pUdpServer) m_pUdpServer->AdjustWorkerThreadCount(); 
                if (m_pTcpServer) m_pTcpServer->AdjustWorkerThreadCount();
            }
        }
        catch (...)
        { ; }

        nSecondCount++;
        NanoSleep(1, true);  // 1秒
    }
    catch (...)
    { ; }
}

//-----------------------------------------------------------------------------
// 描述: 服务器初始化 (若初始化失败则抛出异常)
// 备注: 由 Application.Initialize() 调用
//-----------------------------------------------------------------------------
void CMainServer::Initialize()
{
    // 创建程序数据分派器
    m_pDispatcher = pExtension->CreateDispatcher();
    
    if (m_pDispatcher)
        m_pDispatcher->Initialize();
    else
        throw CException("CCustomDispatcher object expected", __FILE__, __LINE__);
     
    // 初始化 UDP 服务器
    if (Application.GetServerOptions().GetServerType() & ST_UDP) 
    {
        m_pUdpServer = new CMainUdpServer(m_pDispatcher);
        m_pUdpServer->SetLocalPort(Application.GetServerOptions().GetUdpServerPort());
        m_pUdpServer->SetListenerThreadCount(Application.GetServerOptions().GetUdpListenerThreadCount());
        m_pUdpServer->Open();
    }
    
    // 初始化 TCP 服务器
    if (Application.GetServerOptions().GetServerType() & ST_TCP)
    {
        m_pTcpServer = new CMainTcpServer(m_pDispatcher);
        m_pTcpServer->Open();
    }

    // 初始化辅助服务器
    m_pHelperServer = new CHelperServer(m_pDispatcher);
    m_pHelperServer->Open();
}

//-----------------------------------------------------------------------------
// 描述: 服务器结束化 
// 备注: 由 Application.Finalize() 调用，在 CMainServer 的析构函数中不必调用
//-----------------------------------------------------------------------------
void CMainServer::Finalize()
{
    if (m_pUdpServer) 
    {
        m_pUdpServer->Close();
        delete m_pUdpServer;
        m_pUdpServer = NULL;
    }

    if (m_pHelperServer)
    {
        m_pHelperServer->Close();
        delete m_pHelperServer;
        m_pHelperServer = NULL;
    }

    if (m_pTcpServer) 
    {
        m_pTcpServer->Close();
        delete m_pTcpServer;
        m_pTcpServer = NULL;
    }

    if (m_pDispatcher)
    {
        m_pDispatcher->Finalize();
        delete m_pDispatcher;
        m_pDispatcher = NULL;
    }
}

//-----------------------------------------------------------------------------
// 描述: 开始运行服务器
// 备注: 由 Application.Run() 调用
//-----------------------------------------------------------------------------
void CMainServer::Run()
{
    RunBackground();
}

///////////////////////////////////////////////////////////////////////////////
// class CDispatcherUtils

//-----------------------------------------------------------------------------
// 描述: 发送UDP数据包
// 参数:
//   nSendTimes - 发送次数 (缺省1次)
//-----------------------------------------------------------------------------
void CDispatcherUtils::SendUdpPacket(void *pBuffer, int nSize, 
    const CInetAddress& ToAddr, int nToPort, int nSendTimes)
{
    Application.GetMainServer().GetMainUdpServer().SendBuffer(
        pBuffer, nSize, ToAddr, nToPort, nSendTimes);
}

///////////////////////////////////////////////////////////////////////////////

} // namespace ise
