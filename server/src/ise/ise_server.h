///////////////////////////////////////////////////////////////////////////////
// Iris Server Engine (ISE)
// ise_server.h
// Classes:
//   > CServerOptions
//   > CThreadTimeoutChecker
//   > CUdpPacket
//   > CUdpRequestQueue
//   > CUdpWorkerThread
//   > CUdpWorkerThreadPool
//   > CUdpRequestGroup
//   > CMainUdpServer
//   > CTcpRequestQueue
//   > CTcpWorkerThread
//   > CTcpWorkerThreadPool
//   > CTcpRequestGroup
//   > CMainTcpServer
//   > CHelperThread
//   > CHelperThreadPool
//   > CHelperServer
//   > CMainServer
//   > CDispatcherUtils
//   > CCustomDispatcher
///////////////////////////////////////////////////////////////////////////////

#ifndef _ISE_SERVER_H_
#define _ISE_SERVER_H_

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <list>
#include <vector>
#include <deque>

#include "ise_classes.h"
#include "ise_socket.h"

using namespace std;
using namespace ise;

namespace ise
{

///////////////////////////////////////////////////////////////////////////////
// ��ǰ����

class CUdpRequestGroup;
class CUdpWorkerThreadPool;
class CTcpRequestGroup;
class CTcpWorkerThreadPool;
class CMainUdpServer;
class CMainTcpServer;
class CHelperThread;
class CHelperThreadPool;
class CHelperServer;
class CCustomDispatcher;

///////////////////////////////////////////////////////////////////////////////
// ���Ͷ���

// ����������(�ɶ�ѡ��ѡ)
enum ServerType { 
    ST_UDP = 0x0001,     // UDP������
    ST_TCP = 0x0002      // TCP������
};

///////////////////////////////////////////////////////////////////////////////
// class CServerOptions - ������������

class CServerOptions
{
public:
    // ��������������ȱʡֵ
    enum {
        DEF_SERVER_TYPE             = ST_UDP,    // ������Ĭ������
        DEF_ADJUST_THREAD_INTERVAL  = 5,         // ��̨���� "�������߳�����" ��ʱ����ȱʡֵ(��)
        DEF_HELPER_THREAD_COUNT     = 0,         // �����̵߳ĸ���
    };

    // UDP����������ȱʡֵ
    enum {
        DEF_UDP_SERVER_PORT         = 9000,      // UDP����Ĭ�϶˿�
        DEF_UDP_LISTENER_THD_COUNT  = 1,         // �����̵߳�����
        DEF_UDP_REQ_GROUP_COUNT     = 1,         // �������������ȱʡֵ
        DEF_UDP_REQ_QUEUE_CAPACITY  = 5000,      // ������е�ȱʡ����(���ܷ��¶������ݰ�)
        DEF_UDP_WORKER_THREADS_MIN  = 1,         // ÿ������й������̵߳�ȱʡ���ٸ���
        DEF_UDP_WORKER_THREADS_MAX  = 8,         // ÿ������й������̵߳�ȱʡ������
        DEF_UDP_REQ_EFF_WAIT_TIME   = 10,        // �����ڶ����е���Ч�ȴ�ʱ��ȱʡֵ(��)
        DEF_UDP_WORKER_THD_TIMEOUT  = 60,        // �������̵߳Ĺ�����ʱʱ��ȱʡֵ(��)
        DEF_UDP_QUEUE_ALERT_LINE    = 500,       // ���������ݰ�����������ȱʡֵ�����������������������߳�
    };

    // TCP����������ȱʡֵ
    enum {
        DEF_TCP_SERVER_PORT         = 9000,      // TCP����Ĭ�϶˿�
        DEF_TCP_REQ_GROUP_COUNT     = 1,         // �������������ȱʡֵ
        DEF_TCP_REQ_QUEUE_CAPACITY  = 500,       // TCP������е�ȱʡ����(�������ɶ��ٸ�����)
        DEF_TCP_WORKER_THREADS_MIN  = 1,         // �������̵߳�ȱʡ���ٸ���
        DEF_TCP_WORKER_THREADS_MAX  = 8,         // �������̵߳�ȱʡ������
        DEF_TCP_QUEUE_ALERT_LINE    = 50,        // ��������������������ȱʡֵ�����������������������߳�
    };

private:
    /* ------------ ��������������: ------------ */

    // ���������� (ST_UDP | ST_TCP)
    uint m_nServerType;
    // ��̨�����������߳�������ʱ����(��)
    int m_nAdjustThreadInterval;
    // �����̵߳ĸ���
    int m_nHelperThreadCount;

    /* ------------ UDP����������: ------------ */

    struct CUdpRequestGroupOpt
    {
        int nRequestQueueCapacity;    // ������е�����(�������ɶ��ٸ����ݰ�)
        int nMinWorkerThreads;        // �������̵߳����ٸ���
        int nMaxWorkerThreads;        // �������̵߳�������

        CUdpRequestGroupOpt()
        {
            nRequestQueueCapacity = DEF_UDP_REQ_QUEUE_CAPACITY;
            nMinWorkerThreads = DEF_UDP_WORKER_THREADS_MIN;
            nMaxWorkerThreads = DEF_UDP_WORKER_THREADS_MAX;
        }
    };
    typedef vector<CUdpRequestGroupOpt> CUdpRequestGroupOpts;

    // UDP����˿�
    int m_nUdpServerPort;
    // �����̵߳�����
    int m_nUdpListenerThreadCount;
    // ������������
    int m_nUdpRequestGroupCount;
    // ÿ������ڵ�����
    CUdpRequestGroupOpts m_UdpRequestGroupOpts;
    // ���ݰ��ڶ����е���Ч�ȴ�ʱ�䣬��ʱ���账��(��)
    int m_nUdpRequestEffWaitTime;
    // �������̵߳Ĺ�����ʱʱ��(��)����Ϊ0��ʾ�����г�ʱ���
    int m_nUdpWorkerThreadTimeout;
    // ������������ݰ����������ߣ����������������������߳�
    int m_nUdpRequestQueueAlertLine;

    /* ------------ TCP����������: ------------ */

    struct CTcpRequestGroupOpt
    {
        int nTcpServerPort;           // TCP����˿ں�
        int nRequestQueueCapacity;    // ������е�����(�������ɶ��ٸ����ݰ�)
        int nMinWorkerThreads;        // �������̵߳����ٸ���
        int nMaxWorkerThreads;        // �������̵߳�������

        CTcpRequestGroupOpt()
        {
            nTcpServerPort = 0;
            nRequestQueueCapacity = DEF_UDP_REQ_QUEUE_CAPACITY;
            nMinWorkerThreads = DEF_UDP_WORKER_THREADS_MIN;
            nMaxWorkerThreads = DEF_UDP_WORKER_THREADS_MAX;
        }
    };
    typedef vector<CTcpRequestGroupOpt> CTcpRequestGroupOpts;

    // ������������ (һ����������Ӧһ��TCP�˿�)
    int m_nTcpRequestGroupCount;
    // ÿ������ڵ�����
    CTcpRequestGroupOpts m_TcpRequestGroupOpts;
    // ����������������������ߣ����������������������߳�
    int m_nTcpRequestQueueAlertLine;

public:
    CServerOptions();

    // ��������������----------------------------------------------------------

    // ���÷���������(ST_UDP | ST_TCP)
    void SetServerType(uint nSvrType);
    // ���ú�̨�����������߳�������ʱ����(��)
    void SetAdjustThreadInterval(int nSecs);
    // ���ø����̵߳�����
    void SetHelperThreadCount(int nCount);

    // ����UDP����˿ں�
    void SetUdpServerPort(int nPort);
    // ����UDP�����̵߳�����
    void SetUdpListenerThreadCount(int nCount);
    // ����UDP������������
    void SetUdpRequestGroupCount(int nCount);
    // ����UDP������е�������� (�������ɶ��ٸ����ݰ�)
    void SetUdpRequestQueueCapacity(int nGroupIndex, int nCapacity);
    // ����UDP�������̸߳�����������
    void SetUdpWorkerThreadCount(int nGroupIndex, int nMinThreads, int nMaxThreads);
    // ����UDP�����ڶ����е���Ч�ȴ�ʱ�䣬��ʱ���账��(��)
    void SetUdpRequestEffWaitTime(int nSecs);
    // ����UDP�������̵߳Ĺ�����ʱʱ��(��)����Ϊ0��ʾ�����г�ʱ���
    void SetUdpWorkerThreadTimeout(int nSecs);
    // ����UDP������������ݰ�����������
    void SetUdpRequestQueueAlertLine(int nCount);

    // ����TCP������������
    void SetTcpRequestGroupCount(int nCount);
    // ����TCP����˿ں�
    void SetTcpServerPort(int nGroupIndex, int nPort);
    // ����TCP������е�����(�������ɶ��ٸ�����)
    void SetTcpRequestQueueCapacity(int nGroupIndex, int nCapacity);
    // ����TCP�������̸߳�����������
    void SetTcpWorkerThreadCount(int nGroupIndex, int nMinThreads, int nMaxThreads);
    // ����TCP�����������������������
    void SetTcpRequestQueueAlertLine(int nCount);

    // ���������û�ȡ----------------------------------------------------------
    
    uint GetServerType() { return m_nServerType; }
    int GetAdjustThreadInterval() { return m_nAdjustThreadInterval; }
    int GetHelperThreadCount() { return m_nHelperThreadCount; }

    int GetUdpServerPort() { return m_nUdpServerPort; }
    int GetUdpListenerThreadCount() { return m_nUdpListenerThreadCount; }
    int GetUdpRequestGroupCount() { return m_nUdpRequestGroupCount; }
    int GetUdpRequestQueueCapacity(int nGroupIndex);
    void GetUdpWorkerThreadCount(int nGroupIndex, int& nMinThreads, int& nMaxThreads);
    int GetUdpRequestEffWaitTime() { return m_nUdpRequestEffWaitTime; }
    int GetUdpWorkerThreadTimeout() { return m_nUdpWorkerThreadTimeout; }
    int GetUdpRequestQueueAlertLine() { return m_nUdpRequestQueueAlertLine; }
    
    int GetTcpRequestGroupCount() { return m_nTcpRequestGroupCount; }
    int GetTcpServerPort(int nGroupIndex);
    int GetTcpRequestQueueCapacity(int nGroupIndex);
    void GetTcpWorkerThreadCount(int nGroupIndex, int& nMinThreads, int& nMaxThreads);
    int GetTcpRequestQueueAlertLine() { return m_nTcpRequestQueueAlertLine; }
};

///////////////////////////////////////////////////////////////////////////////
// class CThreadTimeoutChecker - �̳߳�ʱ�����
//
// ˵��:
// ����������� CUdpWorkerThread/CTcpWorkerThread�����й������̵߳Ĺ���ʱ�䳬ʱ��⡣
// ���������߳��յ�һ����������Ͻ��빤��״̬��һ����ԣ��������߳�Ϊ�����������������
// ʱ�䲻��̫������̫����ᵼ�·��������й������̶߳�ȱ��ʹ��Ӧ����������������½�������
// ����UDP������˵���������ˡ�ͨ������£��̹߳�����ʱ����������Ϊ��������̺��߼�����
// �������ⲿԭ�򣬱������ݿⷱæ����Դ����������ӵ�µȵȡ����̹߳�����ʱ��Ӧ֪ͨ���˳���
// ����֪ͨ�˳�������ʱ������δ�˳�����ǿ��ɱ�����������̵߳�����������ʱ�����µ��̡߳�

class CThreadTimeoutChecker : public CAutoInvokable
{
private:
    CThread *m_pThread;         // �������߳�
    time_t m_tStartTime;        // ��ʼ��ʱʱ��ʱ���
    bool m_bStarted;            // �Ƿ��ѿ�ʼ��ʱ
    uint m_nTimeoutSecs;        // ������������Ϊ��ʱ (Ϊ0��ʾ�����г�ʱ���)
    CCriticalSection m_Lock;

private:
    void Start();
    void Stop();

protected:
    virtual void InvokeInitialize() { Start(); }
    virtual void InvokeFinalize() { Stop(); }

public:
    explicit CThreadTimeoutChecker(CThread *pThread);
    virtual ~CThreadTimeoutChecker() {}

    // ����߳��Ƿ��ѳ�ʱ������ʱ��֪ͨ���˳�
    bool Check();

    // ���ó�ʱʱ�䣬��Ϊ0���ʾ�����г�ʱ���
    void SetTimeoutSecs(uint nValue) { m_nTimeoutSecs = nValue; }
    // �����Ƿ��ѿ�ʼ��ʱ
    bool GetStarted();
};

///////////////////////////////////////////////////////////////////////////////
// class CUdpPacket - UDP���ݰ���

class CUdpPacket
{
private:
    void *m_pPacketBuffer;

public:
    uint m_nRecvTimeStamp;
    PeerAddress m_RemoteAddr;
    int m_nPacketSize;

public:
    CUdpPacket() : 
        m_pPacketBuffer(NULL),
        m_nRecvTimeStamp(0),
        m_RemoteAddr(0, 0),
        m_nPacketSize(0)
    { ; }
    ~CUdpPacket() 
    { if (m_pPacketBuffer) free(m_pPacketBuffer); }

    void SetPacketBuffer(void *pPakcetBuffer, int nPacketSize);
    inline void* GetPacketBuffer() const { return m_pPacketBuffer; }
};

///////////////////////////////////////////////////////////////////////////////
// class CUdpRequestQueue - UDP���������

class CUdpRequestQueue
{
private:
    // ��������ȱ��:
    // deque  - ֧��ͷβ������ɾ������ɾ�м�Ԫ�غ�����֧���±���ʡ�
    // vector - ֧��β��������ɾ��ͷ�����м�Ԫ����ɾ������֧���±���ʡ�
    // list   - ֧���κ�Ԫ�صĿ�����ɾ������֧���±���ʣ���֧�ֿ���ȡ��ǰ����(size())��
    typedef deque<CUdpPacket*> PacketList;

    CUdpRequestGroup *m_pOwnGroup;   // �������
    PacketList m_PacketList;         // ���ݰ��б�
    int m_nPacketCount;              // ���������ݰ��ĸ���(Ϊ�˿��ٷ���)
    int m_nCapacity;                 // ���е��������
    int m_nEffWaitTime;              // ���ݰ���Ч�ȴ�ʱ��(��)
    CCriticalSection m_Lock;
    CThreadSem m_ThreadSem;

public:
    explicit CUdpRequestQueue(CUdpRequestGroup *pOwnGroup); 
    virtual ~CUdpRequestQueue() { Clear(); }

    void AddPacket(CUdpPacket *pPacket);
    CUdpPacket* ExtractPacket();
    void Clear();
    void BreakWaiting(int nSemCount);

    int GetCount() { return m_nPacketCount; }
};

///////////////////////////////////////////////////////////////////////////////
// class CUdpWorkerThread - UDP�������߳���
//
// ˵��:
// 1. ȱʡ����£�UDP�������߳�������г�ʱ��⣬��ĳЩ���������ó�ʱ��⣬����:
//    CUdpWorkerThread::GetTimeoutChecker().SetTimeoutSecs(0);
//
// ���ʽ���:
// 1. ��ʱ�߳�: ��ĳһ������빤��״̬������δ��ɵ��̡߳�
// 2. �����߳�: �ѱ�֪ͨ�˳������ò��˳����̡߳�

class CUdpWorkerThread : public CThread
{
private:
    CUdpWorkerThreadPool *m_pOwnPool;        // �����̳߳�
    CThreadTimeoutChecker m_TimeoutChecker;  // ��ʱ�����
protected:
    virtual void Execute();
    virtual void DoTerminate();
    virtual void DoKill();
public:
    explicit CUdpWorkerThread(CUdpWorkerThreadPool *pThreadPool);
    virtual ~CUdpWorkerThread();

    // ���س�ʱ�����
    CThreadTimeoutChecker& GetTimeoutChecker() { return m_TimeoutChecker; }
    // ���ظ��߳��Ƿ����״̬(���ڵȴ�����)
    bool IsIdle() { return !m_TimeoutChecker.GetStarted(); }
};

///////////////////////////////////////////////////////////////////////////////
// class CUdpWorkerThreadPool - UDP�������̳߳���

class CUdpWorkerThreadPool
{
public:
    enum {
        MAX_THREAD_TERM_SECS     = 60*3,    // �̱߳�֪ͨ�˳���������(��)
        MAX_THREAD_WAIT_FOR_SECS = 2        // �̳߳����ʱ���ȴ�ʱ��(��)
    };

private:
    CUdpRequestGroup *m_pOwnGroup;          // �������
    CList m_ThreadList;                     // �߳��б�
    CCriticalSection m_Lock;

    void CreateThreads(int nCount);
    void TerminateThreads(int nCount);
    void CheckThreadTimeout();
    void KillZombieThreads();
public:
    explicit CUdpWorkerThreadPool(CUdpRequestGroup *pOwnGroup);
    ~CUdpWorkerThreadPool();

    void RegisterThread(CUdpWorkerThread *pThread);
    void UnregisterThread(CUdpWorkerThread *pThread);

    // ���ݸ��������̬�����߳�����
    void AdjustThreadCount();
    // ֪ͨ�����߳��˳�
    void TerminateAllThreads();
    // �ȴ������߳��˳�
    void WaitForAllThreads();

    // ȡ�õ�ǰ�߳�����
    int GetThreadCount() { return m_ThreadList.Count(); }
    // ȡ���������
    CUdpRequestGroup& GetRequestGroup() { return *m_pOwnGroup; }
};

///////////////////////////////////////////////////////////////////////////////
// class CUdpRequestGroup - UDP���������

class CUdpRequestGroup
{
private:
    CMainUdpServer *m_pOwnMainUdpSvr;       // ����UDP������
    int m_nGroupIndex;                      // ����(0-based)
    CUdpRequestQueue m_RequestQueue;        // �������
    CUdpWorkerThreadPool m_ThreadPool;      // �������̳߳�

public:
    CUdpRequestGroup(CMainUdpServer *pOwnMainUdpSvr, int nGroupIndex);
    virtual ~CUdpRequestGroup() {}

    int GetGroupIndex() { return m_nGroupIndex; }
    CUdpRequestQueue& GetRequestQueue() { return m_RequestQueue; }
    CUdpWorkerThreadPool& GetThreadPool() { return m_ThreadPool; }

    // ȡ������UDP������
    CMainUdpServer& GetMainUdpServer() { return *m_pOwnMainUdpSvr; }
};

///////////////////////////////////////////////////////////////////////////////
// class CMainUdpServer - UDP����������

class CMainUdpServer : public CUdpServer
{
private:
    vector<CUdpRequestGroup*> m_RequestGroupList;   // ��������б�
    int m_nRequestGroupCount;                       // �����������
    CCustomDispatcher *m_pDispatcher;               // �������ݷ�����(���� CMainServer::m_pDispatcher)

    void InitRequestGroupList();
    void ClearRequestGroupList();
public:
    explicit CMainUdpServer(CCustomDispatcher *pDispatcher);
    virtual ~CMainUdpServer();

    // �رշ�����
    virtual void Close();

    // �յ����ݰ�
    virtual void OnRecvData(void *pPacketBuffer, int nPacketSize, const PeerAddress& RemoteAddr);

    // ���ݸ��������̬�����������߳�����
    void AdjustWorkerThreadCount();
    // ֪ͨ���й������߳��˳�
    void TerminateAllWorkerThreads();
    // �ȴ����й������߳��˳�
    void WaitForAllWorkerThreads();

    // ȡ�ó������ݷ�����
    CCustomDispatcher& GetDispatcher() { return *m_pDispatcher; }
};

///////////////////////////////////////////////////////////////////////////////
// class CTcpRequestQueue - TCP���������

class CTcpRequestQueue
{
private:
    typedef deque<CTcpConnection*> TcpConnList;

    CTcpRequestGroup *m_pOwnGroup;    // �������
    TcpConnList m_TcpConnList;        // TCP�����б�
    int m_nCapacity;                  // ���е��������
    CCriticalSection m_Lock;
    CThreadSem m_ThreadSem;

public:
    explicit CTcpRequestQueue(CTcpRequestGroup *pOwnGroup); 
    virtual ~CTcpRequestQueue() { Clear(); }

    void AddTcpConnect(CTcpConnection *pTcpConn);
    CTcpConnection* ExtractTcpConnect();
    void Clear();
    void BreakWaiting(int nSemCount);

    int GetCount() { return m_TcpConnList.size(); }
};

///////////////////////////////////////////////////////////////////////////////
// class CTcpWorkerThread - TCP�������߳���
//
// ˵��:
// 1. ȱʡ����£�TCP�������̲߳����г�ʱ��⣬��ĳЩ����������ó�ʱ��⣬����:
//    CTcpWorkerThread::GetTimeoutChecker().SetTimeoutSecs(nSeconds);

class CTcpWorkerThread : public CThread
{
private:
    CTcpWorkerThreadPool *m_pOwnPool;        // �����̳߳�
    CThreadTimeoutChecker m_TimeoutChecker;  // ��ʱ�����
protected:
    virtual void Execute();
    virtual void DoTerminate();
    virtual void DoKill();
public:
    explicit CTcpWorkerThread(CTcpWorkerThreadPool *pThreadPool);
    virtual ~CTcpWorkerThread();

    // ���س�ʱ�����
    CThreadTimeoutChecker& GetTimeoutChecker() { return m_TimeoutChecker; }
    // ���ظ��߳��Ƿ����״̬(���ڵȴ�����)
    bool IsIdle() { return !m_TimeoutChecker.GetStarted(); }
};

///////////////////////////////////////////////////////////////////////////////
// class CTcpWorkerThreadPool - TCP�������̳߳���

class CTcpWorkerThreadPool
{
public:
    enum {
        MAX_THREAD_TERM_SECS     = 60*3,    // �̱߳�֪ͨ�˳���������(��)
        MAX_THREAD_WAIT_FOR_SECS = 2        // �̳߳����ʱ���ȴ�ʱ��(��)
    };

private:
    CTcpRequestGroup *m_pOwnGroup;          // �������
    CList m_ThreadList;                     // �߳��б�
    CCriticalSection m_Lock;

    void CreateThreads(int nCount);
    void TerminateThreads(int nCount);
    void CheckThreadTimeout();
    void KillZombieThreads();
public:
    explicit CTcpWorkerThreadPool(CTcpRequestGroup *pOwnGroup);
    ~CTcpWorkerThreadPool();

    void RegisterThread(CTcpWorkerThread *pThread);
    void UnregisterThread(CTcpWorkerThread *pThread);

    // ���ݸ��������̬�����߳�����
    void AdjustThreadCount();
    // ֪ͨ�����߳��˳�
    void TerminateAllThreads();
    // �ȴ������߳��˳�
    void WaitForAllThreads();

    // ȡ�õ�ǰ�߳�����
    int GetThreadCount() { return m_ThreadList.Count(); }
    // ȡ���������
    CTcpRequestGroup& GetRequestGroup() { return *m_pOwnGroup; }
};

///////////////////////////////////////////////////////////////////////////////
// class CTcpRequestGroup - TCP���������

class CTcpRequestGroup : public CTcpServer
{
private:
    CMainTcpServer *m_pOwnMainTcpSvr;       // ����TCP������
    int m_nGroupIndex;                      // ����(0-based)
    int m_nTcpPort;                         // TCP�˿ں�
    CTcpRequestQueue m_RequestQueue;        // TCP�������
    CTcpWorkerThreadPool m_ThreadPool;      // �������̳߳�

public:
    CTcpRequestGroup(CMainTcpServer *pOwnMainTcpSvr, int nGroupIndex, int nTcpPort);
    virtual ~CTcpRequestGroup() {}

    // �رշ�����
    virtual void Close();
    // �յ����� (ע: pConnection �ǶѶ�����ʹ�����ͷ�)
    virtual void OnConnect(CTcpConnection *pConnection);

    int GetGroupIndex() { return m_nGroupIndex; }
    int GetTcpPort() { return m_nTcpPort; }
    CTcpRequestQueue& GetRequestQueue() { return m_RequestQueue; }
    CTcpWorkerThreadPool& GetThreadPool() { return m_ThreadPool; }

    // ȡ������TCP������
    CMainTcpServer& GetMainTcpServer() { return *m_pOwnMainTcpSvr; }
};

///////////////////////////////////////////////////////////////////////////////
// class CMainTcpServer - TCP����������

class CMainTcpServer
{
private:
    vector<CTcpRequestGroup*> m_RequestGroupList;   // ��������б�
    int m_nRequestGroupCount;                       // �����������
    CCustomDispatcher *m_pDispatcher;               // �������ݷ�����(���� CMainServer::m_pDispatcher)

    void InitRequestGroupList();
    void ClearRequestGroupList();
public:
    explicit CMainTcpServer(CCustomDispatcher *pDispatcher);
    virtual ~CMainTcpServer();

    void Open();
    void Close();

    // ���ݸ��������̬�����������߳�����
    void AdjustWorkerThreadCount();
    // ֪ͨ���й������߳��˳�
    void TerminateAllWorkerThreads();
    // �ȴ����й������߳��˳�
    void WaitForAllWorkerThreads();

    // ȡ�ó������ݷ�����
    CCustomDispatcher& GetDispatcher() { return *m_pDispatcher; }
};

///////////////////////////////////////////////////////////////////////////////
// class CHelperThread - �����߳���
//
// ˵��:
// 1. ���ڷ���˳��򣬳����������֮�⣬һ�㻹��Ҫ���ɺ�̨�ػ��̣߳����ں�̨ά��������
//    �����������ݻ��ա����ݿ������������ȵȡ���������߳�ͳ��Ϊ helper thread.

class CHelperThread : public CThread
{
private:
    CHelperThreadPool *m_pOwnPool;          // �����̳߳�
    int m_nHelperIndex;                     // �����������(0-based)
protected:
    virtual void Execute();
    virtual void DoKill();
public:
    CHelperThread(CHelperThreadPool *pThreadPool, int nHelperIndex);
    virtual ~CHelperThread();
};

///////////////////////////////////////////////////////////////////////////////
// class CHelperThreadPool - �����̳߳���

class CHelperThreadPool
{
public:
    enum { MAX_THREAD_WAIT_FOR_SECS = 5 };  // �̳߳����ʱ���ȴ�ʱ��(��)

private:
    CHelperServer *m_pOwnHelperSvr;         // ��������������
    CList m_ThreadList;                     // �߳��б�
    CCriticalSection m_Lock;

public:
    explicit CHelperThreadPool(CHelperServer *pOwnHelperServer);
    ~CHelperThreadPool();

    void RegisterThread(CHelperThread *pThread);
    void UnregisterThread(CHelperThread *pThread);

    // ֪ͨ�����߳��˳�
    void TerminateAllThreads();
    // �ȴ������߳��˳�
    void WaitForAllThreads();

    // ȡ�õ�ǰ�߳�����
    int GetThreadCount() { return m_ThreadList.Count(); }
    // ȡ����������������
    CHelperServer& GetHelperServer() { return *m_pOwnHelperSvr; }
};

///////////////////////////////////////////////////////////////////////////////
// class CHelperServer - ����������

class CHelperServer
{
private:
    bool m_bActive;                         // �������Ƿ�����
    CCustomDispatcher *m_pDispatcher;       // �������ݷ�����(���� CMainServer::m_pDispatcher)
    CHelperThreadPool m_ThreadPool;         // �����̳߳�

public:
    explicit CHelperServer(CCustomDispatcher *pDispatcher);
    virtual ~CHelperServer();

    // ����������
    virtual void Open();
    // �رշ�����
    virtual void Close();

    // ���������߳�ִ�к���
    virtual void OnHelperThreadExecute(CHelperThread& HelperThread, int nHelperIndex);

    // ֪ͨ���и����߳��˳�
    void TerminateAllHelperThreads();
    // �ȴ����и����߳��˳�
    void WaitForAllHelperThreads();
};

///////////////////////////////////////////////////////////////////////////////
// class CMainServer - ����������

class CMainServer
{
private:
    CMainUdpServer *m_pUdpServer;       // UDP������
    CMainTcpServer *m_pTcpServer;       // TCP������
    CHelperServer *m_pHelperServer;     // ����������
    CCustomDispatcher *m_pDispatcher;   // �������ݷ�����

    void RunBackground();
public:
    CMainServer();
    ~CMainServer();

    void Initialize();
    void Finalize();
    void Run();

    CMainUdpServer& GetMainUdpServer() { return *m_pUdpServer; }
    CMainTcpServer& GetMainTcpServer() { return *m_pTcpServer; }
    CHelperServer& GetHelperServer() { return *m_pHelperServer; }
    CCustomDispatcher& GetDispatcher() {  return *m_pDispatcher; }
};

///////////////////////////////////////////////////////////////////////////////
// class CDispatcherUtils - �������ݷ�����������߼���

class CDispatcherUtils
{
public:
    // ����UDP���ݰ�
    void SendUdpPacket(void *pBuffer, int nSize, const CInetAddress& ToAddr, 
        int nToPort, int nSendTimes = 1);
};

///////////////////////////////////////////////////////////////////////////////
// class CCustomDispatcher - �������ݷ���������
//
// ˵��:
// 1. ������������չӦ�÷������еĳ������ݣ������������ݡ��̵߳ȡ�
// 2. �û���̳д��࣬�Ӷ����� ISE ���ɵ����ݡ�
// 3. �Ƽ��� Initialize()/Finalize() ���湹��/����������C++���캯�����׳��쳣������
//    ����������ִ�У��� Finalize() �������һ����ִ�С�Initialize()/Finalize() ��
//    ������ CMainServer ���á�

class CCustomDispatcher
{
private:
    CDispatcherUtils m_Utils;   // ����߼�

public:
    CCustomDispatcher() {}
    virtual ~CCustomDispatcher() {}

    // ��ʼ�� (ʧ�����׳��쳣)
    virtual void Initialize() {}
    // ������ (���۳�ʼ���Ƿ����쳣������ʱ����ִ��)
    virtual void Finalize() {}

    // UDP���ݰ�����
    virtual void ClassifyUdpPacket(void *pPacketBuffer, int nPacketSize, int& nGroupIndex) { nGroupIndex = 0; }
    // UDP���ݰ�����
    virtual void DispatchUdpPacket(CUdpWorkerThread& WorkerThread, int nGroupIndex, CUdpPacket& Packet) {}
    // TCP���ӷ���
    virtual void DispatchTcpConnection(CTcpWorkerThread& WorkerThread, int nGroupIndex, CTcpConnection& Connection) {}

    // ���������߳�ִ��(nHelperIndex: 0-based)
    virtual void HelperThreadExecute(CHelperThread& HelperThread, int nHelperIndex) {}

    // ��������߼�
    inline CDispatcherUtils& GetUtils() { return m_Utils; }
};

///////////////////////////////////////////////////////////////////////////////

} // namespace ise

#endif // _ISE_SERVER_H_ 
