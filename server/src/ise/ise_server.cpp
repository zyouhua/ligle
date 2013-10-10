///////////////////////////////////////////////////////////////////////////////
// Iris Server Engine (ISE)
//
// �ļ�����: ise_server.cpp
// ��������: ���������
// ����޸�: 2005-07-14
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
// ����: ���÷���������(UDP|TCP)
// ����:
//   nSvrType - ������������(�ɶ�ѡ��ѡ)
// ʾ��:
//   SetServerType(ST_UDP | ST_TCP);
//-----------------------------------------------------------------------------
void CServerOptions::SetServerType(uint nSvrType)
{
    m_nServerType = nSvrType;
}

//-----------------------------------------------------------------------------
// ����: ���ú�̨ά���������߳�������ʱ����(��)
//-----------------------------------------------------------------------------
void CServerOptions::SetAdjustThreadInterval(int nSecs)
{
    if (nSecs <= 0) nSecs = DEF_ADJUST_THREAD_INTERVAL;
    m_nAdjustThreadInterval = nSecs;
}

//-----------------------------------------------------------------------------
// ����: ���ø����̵߳�����
//-----------------------------------------------------------------------------
void CServerOptions::SetHelperThreadCount(int nCount)
{
    if (nCount < 0) nCount = 0;
    m_nHelperThreadCount = nCount;
}

//-----------------------------------------------------------------------------
// ����: ����UDP����˿ں�
//-----------------------------------------------------------------------------
void CServerOptions::SetUdpServerPort(int nPort)
{
    m_nUdpServerPort = nPort;
}

//-----------------------------------------------------------------------------
// ����: ����UDP�����̵߳�����
//-----------------------------------------------------------------------------
void CServerOptions::SetUdpListenerThreadCount(int nCount)
{
    if (nCount < 1) nCount = 1;

    m_nUdpListenerThreadCount = nCount;
}

//-----------------------------------------------------------------------------
// ����: ����UDP���ݰ����������
//-----------------------------------------------------------------------------
void CServerOptions::SetUdpRequestGroupCount(int nCount)
{
    if (nCount <= 0) nCount = DEF_UDP_REQ_GROUP_COUNT;

    m_nUdpRequestGroupCount = nCount;
    m_UdpRequestGroupOpts.resize(nCount);
}

//-----------------------------------------------------------------------------
// ����: ����UDP������е�������� (�������ɶ��ٸ����ݰ�)
// ����:
//   nGroupIndex - ���� (0-based)
//   nCapacity   - ����
//-----------------------------------------------------------------------------
void CServerOptions::SetUdpRequestQueueCapacity(int nGroupIndex, int nCapacity)
{
    if (nGroupIndex < 0 || nGroupIndex >= m_nUdpRequestGroupCount) return;

    if (nCapacity <= 0) nCapacity = DEF_UDP_REQ_QUEUE_CAPACITY;

    m_UdpRequestGroupOpts[nGroupIndex].nRequestQueueCapacity = nCapacity;
}

//-----------------------------------------------------------------------------
// ����: ����UDP�������̸߳�����������
// ����:
//   nGroupIndex - ���� (0-based)
//   nMinThreads - �̸߳���������
//   nMaxThreads - �̸߳���������
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
// ����: ����UDP�����ڶ����е���Ч�ȴ�ʱ�䣬��ʱ���账��
// ����:
//   nMSecs - �ȴ�����
//-----------------------------------------------------------------------------
void CServerOptions::SetUdpRequestEffWaitTime(int nSecs)
{
    if (nSecs <= 0) nSecs = DEF_UDP_REQ_EFF_WAIT_TIME;
    m_nUdpRequestEffWaitTime = nSecs;
}

//-----------------------------------------------------------------------------
// ����: ����UDP�������̵߳Ĺ�����ʱʱ��(��)����Ϊ0��ʾ�����г�ʱ���
//-----------------------------------------------------------------------------
void CServerOptions::SetUdpWorkerThreadTimeout(int nSecs)
{
    if (nSecs < 0) nSecs = 0;
    m_nUdpWorkerThreadTimeout = nSecs;
}

//-----------------------------------------------------------------------------
// ����: ����������������ݰ�����������
//-----------------------------------------------------------------------------
void CServerOptions::SetUdpRequestQueueAlertLine(int nCount)
{
    if (nCount < 1) nCount = 1;
    m_nUdpRequestQueueAlertLine = nCount;
}

//-----------------------------------------------------------------------------
// ����: ����TCP���ݰ����������
//-----------------------------------------------------------------------------
void CServerOptions::SetTcpRequestGroupCount(int nCount)
{
    if (nCount < 0) nCount = 0;

    m_nTcpRequestGroupCount = nCount;
    m_TcpRequestGroupOpts.resize(nCount);
}

//-----------------------------------------------------------------------------
// ����: ����TCP����˿ں�
// ����:
//   nGroupIndex - ���� (0-based)
//   nPort       - �˿ں�
//-----------------------------------------------------------------------------
void CServerOptions::SetTcpServerPort(int nGroupIndex, int nPort)
{
    if (nGroupIndex < 0 || nGroupIndex >= m_nTcpRequestGroupCount) return;

    m_TcpRequestGroupOpts[nGroupIndex].nTcpServerPort = nPort;
}

//-----------------------------------------------------------------------------
// ����: ����TCP������е�������� (�������ɶ��ٸ�����)
// ����:
//   nGroupIndex - ���� (0-based)
//   nCapacity   - ����
//-----------------------------------------------------------------------------
void CServerOptions::SetTcpRequestQueueCapacity(int nGroupIndex, int nCapacity)
{
    if (nGroupIndex < 0 || nGroupIndex >= m_nTcpRequestGroupCount) return;

    if (nCapacity <= 0) nCapacity = DEF_TCP_REQ_QUEUE_CAPACITY;

    m_TcpRequestGroupOpts[nGroupIndex].nRequestQueueCapacity = nCapacity;
}

//-----------------------------------------------------------------------------
// ����: ����TCP�������̸߳�����������
// ����:
//   nGroupIndex - ���� (0-based)
//   nMinThreads - �̸߳���������
//   nMaxThreads - �̸߳���������
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
// ����: ����TCP�����������������������
//-----------------------------------------------------------------------------
void CServerOptions::SetTcpRequestQueueAlertLine(int nCount)
{
    if (nCount < 1) nCount = 1;
    m_nTcpRequestQueueAlertLine = nCount;
}

//-----------------------------------------------------------------------------
// ����: ȡ��UDP������е�������� (�������ɶ��ٸ����ݰ�)
// ����:
//   nGroupIndex - ���� (0-based)
//-----------------------------------------------------------------------------
int CServerOptions::GetUdpRequestQueueCapacity(int nGroupIndex)
{
    if (nGroupIndex < 0 || nGroupIndex >= m_nUdpRequestGroupCount) return -1;

    return m_UdpRequestGroupOpts[nGroupIndex].nRequestQueueCapacity;
}

//-----------------------------------------------------------------------------
// ����: ȡ��UDP�������̸߳�����������
// ����:
//   nGroupIndex - ���� (0-based)
//   nMinThreads - ����̸߳���������
//   nMaxThreads - ����̸߳���������
//-----------------------------------------------------------------------------
void CServerOptions::GetUdpWorkerThreadCount(int nGroupIndex, int& nMinThreads, int& nMaxThreads)
{
    if (nGroupIndex < 0 || nGroupIndex >= m_nUdpRequestGroupCount) return;

    nMinThreads = m_UdpRequestGroupOpts[nGroupIndex].nMinWorkerThreads;
    nMaxThreads = m_UdpRequestGroupOpts[nGroupIndex].nMaxWorkerThreads;
}

//-----------------------------------------------------------------------------
// ����: ȡ��TCP����˿ں�
// ����:
//   nGroupIndex - ���� (0-based)
//-----------------------------------------------------------------------------
int CServerOptions::GetTcpServerPort(int nGroupIndex)
{
    if (nGroupIndex < 0 || nGroupIndex >= m_nTcpRequestGroupCount) return -1;

    return m_TcpRequestGroupOpts[nGroupIndex].nTcpServerPort;
}

//-----------------------------------------------------------------------------
// ����: ȡ��TCP������е�������� (�������ɶ��ٸ�����)
// ����:
//   nGroupIndex - ���� (0-based)
//-----------------------------------------------------------------------------
int CServerOptions::GetTcpRequestQueueCapacity(int nGroupIndex)
{
    if (nGroupIndex < 0 || nGroupIndex >= m_nTcpRequestGroupCount) return -1;

    return m_TcpRequestGroupOpts[nGroupIndex].nRequestQueueCapacity;
}

//-----------------------------------------------------------------------------
// ����: ȡ��TCP�������̸߳�����������
// ����:
//   nGroupIndex - ���� (0-based)
//   nMinThreads - ����̸߳���������
//   nMaxThreads - ����̸߳���������
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
// ����: ��ʼ��ʱ
//-----------------------------------------------------------------------------
void CThreadTimeoutChecker::Start()
{
    CAutoSynchronizer Syncher(m_Lock);

    m_tStartTime = time(NULL);
    m_bStarted = true;
}

//-----------------------------------------------------------------------------
// ����: ֹͣ��ʱ
//-----------------------------------------------------------------------------
void CThreadTimeoutChecker::Stop()
{
    CAutoSynchronizer Syncher(m_Lock);

    m_bStarted = false;
}

//-----------------------------------------------------------------------------
// ����: ����߳��Ƿ��ѳ�ʱ������ʱ��֪ͨ���˳�
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
// ����: �����Ƿ��ѿ�ʼ��ʱ
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
// ����: ���캯��
// ����:
//   pOwnGroup - ָ���������
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
// ����: �������������ݰ�
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
// ����: �Ӷ�����ȡ�����ݰ� (ȡ����Ӧ�����ͷţ���ʧ���򷵻� NULL)
// ��ע: ����������û�����ݰ�����һֱ�ȴ���
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
// ����: ��ն���
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
// ����: �����ź�����ֵ��ʹ�ȴ����ݵ��߳��жϵȴ�
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
    // ���ó�ʱ���
    m_TimeoutChecker.SetTimeoutSecs(Application.GetServerOptions().GetUdpWorkerThreadTimeout());

    m_pOwnPool->RegisterThread(this);
}

CUdpWorkerThread::~CUdpWorkerThread()
{
    m_pOwnPool->UnregisterThread(this);
}

//-----------------------------------------------------------------------------
// ����: �̵߳�ִ�к���
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

            // �������ݰ�
            if (!GetTerminated())
                pDispatcher->DispatchUdpPacket(*this, nGroupIndex, *pPacket);
        }
    }
    catch (CException& e)
    { ; }
}

//-----------------------------------------------------------------------------
// ����: ִ�� Terminate() ǰ�ĸ��Ӳ���
//-----------------------------------------------------------------------------
void CUdpWorkerThread::DoTerminate()
{
    //Logger.Write("CUdpWorkerThread be terminate");  // debug
}

//-----------------------------------------------------------------------------
// ����: ִ�� Kill() ǰ�ĸ��Ӳ���
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
// ����: ���� nCount ���߳�
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
// ����: ��ֹ nCount ���߳�
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
// ����: ����߳��Ƿ�����ʱ (��ʱ�߳�: ��ĳһ������빤��״̬������δ��ɵ��߳�)
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
// ����: ǿ��ɱ���������߳� (�����߳�: �ѱ�֪ͨ�˳������ò��˳����߳�)
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
// ����: ע���߳�
//-----------------------------------------------------------------------------
void CUdpWorkerThreadPool::RegisterThread(CUdpWorkerThread *pThread)
{
    CAutoSynchronizer Syncher(m_Lock);

    if (m_ThreadList.IndexOf(pThread) == -1)
        m_ThreadList.Add(pThread);
}

//-----------------------------------------------------------------------------
// ����: ע���߳�
//-----------------------------------------------------------------------------
void CUdpWorkerThreadPool::UnregisterThread(CUdpWorkerThread *pThread)
{
    CAutoSynchronizer Syncher(m_Lock);

    m_ThreadList.Remove(pThread);
}

//-----------------------------------------------------------------------------
// ����: ���ݸ��������̬�����߳�����
//-----------------------------------------------------------------------------
void CUdpWorkerThreadPool::AdjustThreadCount()
{
    int nPacketCount, nThreadCount;
    int nMinThreads, nMaxThreads, nDeltaThreads;
    int nPacketAlertLine;

    // ȡ���߳�����������
    Application.GetServerOptions().GetUdpWorkerThreadCount(
        m_pOwnGroup->GetGroupIndex(), nMinThreads, nMaxThreads);
    // ȡ��������������ݰ�����������
    nPacketAlertLine = Application.GetServerOptions().GetUdpRequestQueueAlertLine();

    // ����߳��Ƿ�����ʱ
    CheckThreadTimeout();
    // ǿ��ɱ���������߳�
    KillZombieThreads();

    // ȡ�����ݰ��������߳�����
    nPacketCount = m_pOwnGroup->GetRequestQueue().GetCount();
    nThreadCount = m_ThreadList.Count();

    // ��֤�߳������������޷�Χ֮��
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

    // �����������е��������������ߣ����������߳�����
    if (nThreadCount < nMaxThreads && nPacketCount >= nPacketAlertLine)
    {
        nDeltaThreads = Min(nMaxThreads - nThreadCount, 3);
        CreateThreads(nDeltaThreads);
        Logger.Write("CreateThreads1:%d", nDeltaThreads);
    }

    // ����������Ϊ�գ����Լ����߳�����
    if (nThreadCount > nMinThreads && nPacketCount == 0)
    {
        nDeltaThreads = 1;
        TerminateThreads(nDeltaThreads);
        Logger.Write("TerminateThreads1:%d", nDeltaThreads);
    }
}

//-----------------------------------------------------------------------------
// ����: ֪ͨ�����߳��˳�
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

    // ʹ�̴߳ӵȴ��н��ѣ������˳�
    GetRequestGroup().GetRequestQueue().BreakWaiting(m_ThreadList.Count());
}

//-----------------------------------------------------------------------------
// ����: �ȴ������߳��˳�
//-----------------------------------------------------------------------------
void CUdpWorkerThreadPool::WaitForAllThreads()
{
    const double SLEEP_INTERVAL = 0.5;  // (��)
    double nWaitSecs = 0;
    
    TerminateAllThreads();

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
// ����: ��ʼ����������б�
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
// ����: �����������б�
//-----------------------------------------------------------------------------
void CMainUdpServer::ClearRequestGroupList()
{
    for (int i = 0; i < m_RequestGroupList.size(); i++)
        delete m_RequestGroupList[i];
    m_RequestGroupList.clear();
}

//-----------------------------------------------------------------------------
// ����: �رշ�����
//-----------------------------------------------------------------------------
void CMainUdpServer::Close()
{
    TerminateAllWorkerThreads();
    WaitForAllWorkerThreads();
    CUdpServer::Close();
}

//-----------------------------------------------------------------------------
// ����: �յ����ݰ�
//-----------------------------------------------------------------------------
void CMainUdpServer::OnRecvData(void *pPacketBuffer, int nPacketSize, const PeerAddress& RemoteAddr)
{
    int nGroupIndex;
    CUdpPacket *p;

    // �Ƚ������ݰ����࣬�õ�����
    m_pDispatcher->ClassifyUdpPacket(pPacketBuffer, nPacketSize, nGroupIndex);

    // ������źϷ�
    if (nGroupIndex >= 0 && nGroupIndex < m_nRequestGroupCount)
    {
        p = new CUdpPacket();
        if (p)
        {
            p->m_nRecvTimeStamp = time(NULL);
            p->m_RemoteAddr = RemoteAddr;
            p->m_nPacketSize = nPacketSize;
            p->SetPacketBuffer(pPacketBuffer, nPacketSize);

            // ��ӵ����������
            m_RequestGroupList[nGroupIndex]->GetRequestQueue().AddPacket(p);
        }
    }
}

//-----------------------------------------------------------------------------
// ����: ���ݸ��������̬�����������߳�����
//-----------------------------------------------------------------------------
void CMainUdpServer::AdjustWorkerThreadCount()
{
    for (int i = 0; i < m_RequestGroupList.size(); i++)
        m_RequestGroupList[i]->GetThreadPool().AdjustThreadCount();
}

//-----------------------------------------------------------------------------
// ����: ֪ͨ���й������߳��˳�
//-----------------------------------------------------------------------------
void CMainUdpServer::TerminateAllWorkerThreads()
{
    for (int i = 0; i < m_RequestGroupList.size(); i++)
        m_RequestGroupList[i]->GetThreadPool().TerminateAllThreads();
}

//-----------------------------------------------------------------------------
// ����: �ȴ����й������߳��˳�
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
// ����: ����������TCP����
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
// ����: �Ӷ�����ȡ��TCP���� (ȡ����Ӧ�����ͷţ���ʧ���򷵻� NULL)
// ��ע: ����������û�����ӣ���һֱ�ȴ���
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
// ����: ��ն���
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
// ����: �����ź�����ֵ��ʹ�ȴ����ݵ��߳��жϵȴ�
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
    // ���ó�ʱ���
    m_TimeoutChecker.SetTimeoutSecs(0);

    m_pOwnPool->RegisterThread(this);
}

CTcpWorkerThread::~CTcpWorkerThread()
{
    m_pOwnPool->UnregisterThread(this);
}

//-----------------------------------------------------------------------------
// ����: �̵߳�ִ�к���
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
                // TCP�������߳�һ����ʼ�������㲻������˳�
                SetTerminated(false);
                // ����TCP����
                pDispatcher->DispatchTcpConnection(*this, nGroupIndex, *pTcpConn);
            }
        }
    }
    catch (CException& e)
    { ; }
}

//-----------------------------------------------------------------------------
// ����: ִ�� Terminate() ǰ�ĸ��Ӳ���
//-----------------------------------------------------------------------------
void CTcpWorkerThread::DoTerminate()
{
    // nothing
}

//-----------------------------------------------------------------------------
// ����: ִ�� Kill() ǰ�ĸ��Ӳ���
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
// ����: ���� nCount ���߳�
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
// ����: ��ֹ nCount ���߳�
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
// ����: ����߳��Ƿ�����ʱ (��ʱ�߳�: ��ĳһ������빤��״̬������δ��ɵ��߳�)
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
// ����: ǿ��ɱ���������߳� (�����߳�: �ѱ�֪ͨ�˳������ò��˳����߳�)
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
// ����: ע���߳�
//-----------------------------------------------------------------------------
void CTcpWorkerThreadPool::RegisterThread(CTcpWorkerThread *pThread)
{
    CAutoSynchronizer Syncher(m_Lock);

    if (m_ThreadList.IndexOf(pThread) == -1)
        m_ThreadList.Add(pThread);
}

//-----------------------------------------------------------------------------
// ����: ע���߳�
//-----------------------------------------------------------------------------
void CTcpWorkerThreadPool::UnregisterThread(CTcpWorkerThread *pThread)
{
    CAutoSynchronizer Syncher(m_Lock);

    m_ThreadList.Remove(pThread);
}

//-----------------------------------------------------------------------------
// ����: ���ݸ��������̬�����߳�����
//-----------------------------------------------------------------------------
void CTcpWorkerThreadPool::AdjustThreadCount()
{
    int nTcpConnCount, nThreadCount;
    int nMinThreads, nMaxThreads, nDeltaThreads;
    int nConnAlertLine;

    // ȡ���߳�����������
    Application.GetServerOptions().GetTcpWorkerThreadCount(
        m_pOwnGroup->GetGroupIndex(), nMinThreads, nMaxThreads);
    // ȡ��TCP�����������������������
    nConnAlertLine = Application.GetServerOptions().GetTcpRequestQueueAlertLine();

    // ����߳��Ƿ�����ʱ
    CheckThreadTimeout();
    // ǿ��ɱ���������߳�
    KillZombieThreads();

    // ȡ�����ݰ��������߳�����
    nTcpConnCount = m_pOwnGroup->GetRequestQueue().GetCount(); 
    nThreadCount = m_ThreadList.Count();

    // ��֤�߳������������޷�Χ֮��
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

    // ���TCP��������е��������������ߣ����������߳�����
    if (nThreadCount < nMaxThreads && nTcpConnCount >= nConnAlertLine)
    {
        nDeltaThreads = Min(nMaxThreads - nThreadCount, 3);
        CreateThreads(nDeltaThreads);
    }

    // ���TCP�������Ϊ�գ����Լ����߳�����
    if (nThreadCount > nMinThreads && nTcpConnCount == 0)
    {
        nDeltaThreads = 1;
        TerminateThreads(nDeltaThreads);
    }
    
    //Logger.Write("tcp conn:%d, thd:%d", nTcpConnCount, nThreadCount);  // debug
}

//-----------------------------------------------------------------------------
// ����: ֪ͨ�����߳��˳�
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

    // ʹ�̴߳ӵȴ��н��ѣ������˳�
    GetRequestGroup().GetRequestQueue().BreakWaiting(m_ThreadList.Count());
}

//-----------------------------------------------------------------------------
// ����: �ȴ������߳��˳�
//-----------------------------------------------------------------------------
void CTcpWorkerThreadPool::WaitForAllThreads()
{
    const double SLEEP_INTERVAL = 0.5;  // (��)
    double nWaitSecs = 0;
    
    TerminateAllThreads();

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
// ����: �رշ�����
//-----------------------------------------------------------------------------
void CTcpRequestGroup::Close()
{
    CTcpServer::Close();
    m_RequestQueue.Clear();
}

//-----------------------------------------------------------------------------
// ����: �յ����� (ע: pConnection �ǶѶ�����ʹ�����ͷ�)
//-----------------------------------------------------------------------------
void CTcpRequestGroup::OnConnect(CTcpConnection *pConnection)
{
    // ��ӵ�TCP���������
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
// ����: ��ʼ����������б�
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
// ����: �����������б�
//-----------------------------------------------------------------------------
void CMainTcpServer::ClearRequestGroupList()
{
    for (int i = 0; i < m_RequestGroupList.size(); i++)
        delete m_RequestGroupList[i];
    m_RequestGroupList.clear();
}

//-----------------------------------------------------------------------------
// ����: ����������
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
// ����: �رշ�����
//-----------------------------------------------------------------------------
void CMainTcpServer::Close()
{
    TerminateAllWorkerThreads();
    WaitForAllWorkerThreads();
    for (int i = 0; i < m_RequestGroupList.size(); i++)
        m_RequestGroupList[i]->Close();
}

//-----------------------------------------------------------------------------
// ����: ���ݸ��������̬�����������߳�����
//-----------------------------------------------------------------------------
void CMainTcpServer::AdjustWorkerThreadCount()
{
    for (int i = 0; i < m_RequestGroupList.size(); i++)
        m_RequestGroupList[i]->GetThreadPool().AdjustThreadCount();
}

//-----------------------------------------------------------------------------
// ����: ֪ͨ���й������߳��˳�
//-----------------------------------------------------------------------------
void CMainTcpServer::TerminateAllWorkerThreads()
{
    for (int i = 0; i < m_RequestGroupList.size(); i++)
        m_RequestGroupList[i]->GetThreadPool().TerminateAllThreads();
}

//-----------------------------------------------------------------------------
// ����: �ȴ����й������߳��˳�
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
// ����: �߳�ִ�к���
//-----------------------------------------------------------------------------
void CHelperThread::Execute()
{
    m_pOwnPool->GetHelperServer().OnHelperThreadExecute(*this, m_nHelperIndex);
}

//-----------------------------------------------------------------------------
// ����: ִ�� Kill() ǰ�ĸ��Ӳ���
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
// ����: ע���߳�
//-----------------------------------------------------------------------------
void CHelperThreadPool::RegisterThread(CHelperThread *pThread)
{
    CAutoSynchronizer Syncher(m_Lock);

    if (m_ThreadList.IndexOf(pThread) == -1)
        m_ThreadList.Add(pThread);
}

//-----------------------------------------------------------------------------
// ����: ע���߳�
//-----------------------------------------------------------------------------
void CHelperThreadPool::UnregisterThread(CHelperThread *pThread)
{
    CAutoSynchronizer Syncher(m_Lock);

    m_ThreadList.Remove(pThread);
}

//-----------------------------------------------------------------------------
// ����: ֪ͨ�����߳��˳�
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
// ����: �ȴ������߳��˳�
//-----------------------------------------------------------------------------
void CHelperThreadPool::WaitForAllThreads()
{
    const double SLEEP_INTERVAL = 0.5;  // (��)
    double nWaitSecs = 0;
    
    TerminateAllThreads();

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
// ����: ����������
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
// ����: �رշ�����
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
// ����: ���������߳�ִ�к���
// ����:
//   nHelperIndex - �����߳����(0-based)
//-----------------------------------------------------------------------------
void CHelperServer::OnHelperThreadExecute(CHelperThread& HelperThread, int nHelperIndex)
{
    m_pDispatcher->HelperThreadExecute(HelperThread, nHelperIndex);
}

//-----------------------------------------------------------------------------
// ����: ֪ͨ���и����߳��˳�
//-----------------------------------------------------------------------------
void CHelperServer::TerminateAllHelperThreads()
{
    m_ThreadPool.TerminateAllThreads();
}

//-----------------------------------------------------------------------------
// ����: �ȴ����и����߳��˳�
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
// ����: ��������ʼ���к����߳̽��к�̨�ػ�����
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
            // ÿ�� nAdjustThreadInterval ��ִ��һ��
            if ((nSecondCount % nAdjustThreadInterval) == 0)
            {
                // ��ʱ�����˳��ź�
                CSignalMasker SigMasker(true);
                SigMasker.SetSignals(1, SIGTERM);
                SigMasker.Block();

                // ά���������̵߳�����
                if (m_pUdpServer) m_pUdpServer->AdjustWorkerThreadCount(); 
                if (m_pTcpServer) m_pTcpServer->AdjustWorkerThreadCount();
            }
        }
        catch (...)
        { ; }

        nSecondCount++;
        NanoSleep(1, true);  // 1��
    }
    catch (...)
    { ; }
}

//-----------------------------------------------------------------------------
// ����: ��������ʼ�� (����ʼ��ʧ�����׳��쳣)
// ��ע: �� Application.Initialize() ����
//-----------------------------------------------------------------------------
void CMainServer::Initialize()
{
    // �����������ݷ�����
    m_pDispatcher = pExtension->CreateDispatcher();
    
    if (m_pDispatcher)
        m_pDispatcher->Initialize();
    else
        throw CException("CCustomDispatcher object expected", __FILE__, __LINE__);
     
    // ��ʼ�� UDP ������
    if (Application.GetServerOptions().GetServerType() & ST_UDP) 
    {
        m_pUdpServer = new CMainUdpServer(m_pDispatcher);
        m_pUdpServer->SetLocalPort(Application.GetServerOptions().GetUdpServerPort());
        m_pUdpServer->SetListenerThreadCount(Application.GetServerOptions().GetUdpListenerThreadCount());
        m_pUdpServer->Open();
    }
    
    // ��ʼ�� TCP ������
    if (Application.GetServerOptions().GetServerType() & ST_TCP)
    {
        m_pTcpServer = new CMainTcpServer(m_pDispatcher);
        m_pTcpServer->Open();
    }

    // ��ʼ������������
    m_pHelperServer = new CHelperServer(m_pDispatcher);
    m_pHelperServer->Open();
}

//-----------------------------------------------------------------------------
// ����: ������������ 
// ��ע: �� Application.Finalize() ���ã��� CMainServer �����������в��ص���
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
// ����: ��ʼ���з�����
// ��ע: �� Application.Run() ����
//-----------------------------------------------------------------------------
void CMainServer::Run()
{
    RunBackground();
}

///////////////////////////////////////////////////////////////////////////////
// class CDispatcherUtils

//-----------------------------------------------------------------------------
// ����: ����UDP���ݰ�
// ����:
//   nSendTimes - ���ʹ��� (ȱʡ1��)
//-----------------------------------------------------------------------------
void CDispatcherUtils::SendUdpPacket(void *pBuffer, int nSize, 
    const CInetAddress& ToAddr, int nToPort, int nSendTimes)
{
    Application.GetMainServer().GetMainUdpServer().SendBuffer(
        pBuffer, nSize, ToAddr, nToPort, nSendTimes);
}

///////////////////////////////////////////////////////////////////////////////

} // namespace ise
