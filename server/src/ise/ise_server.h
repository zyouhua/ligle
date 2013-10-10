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
// 提前声明

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
// 类型定义

// 服务器类型(可多选或不选)
enum ServerType { 
    ST_UDP = 0x0001,     // UDP服务器
    ST_TCP = 0x0002      // TCP服务器
};

///////////////////////////////////////////////////////////////////////////////
// class CServerOptions - 服务器配置类

class CServerOptions
{
public:
    // 服务器常规配置缺省值
    enum {
        DEF_SERVER_TYPE             = ST_UDP,    // 服务器默认类型
        DEF_ADJUST_THREAD_INTERVAL  = 5,         // 后台调整 "工作者线程数量" 的时间间隔缺省值(秒)
        DEF_HELPER_THREAD_COUNT     = 0,         // 辅助线程的个数
    };

    // UDP服务器配置缺省值
    enum {
        DEF_UDP_SERVER_PORT         = 9000,      // UDP服务默认端口
        DEF_UDP_LISTENER_THD_COUNT  = 1,         // 监听线程的数量
        DEF_UDP_REQ_GROUP_COUNT     = 1,         // 请求组别总数的缺省值
        DEF_UDP_REQ_QUEUE_CAPACITY  = 5000,      // 请求队列的缺省容量(即能放下多少数据包)
        DEF_UDP_WORKER_THREADS_MIN  = 1,         // 每个组别中工作者线程的缺省最少个数
        DEF_UDP_WORKER_THREADS_MAX  = 8,         // 每个组别中工作者线程的缺省最多个数
        DEF_UDP_REQ_EFF_WAIT_TIME   = 10,        // 请求在队列中的有效等待时间缺省值(秒)
        DEF_UDP_WORKER_THD_TIMEOUT  = 60,        // 工作者线程的工作超时时间缺省值(秒)
        DEF_UDP_QUEUE_ALERT_LINE    = 500,       // 队列中数据包数量警戒线缺省值，若超过警戒线则尝试增加线程
    };

    // TCP服务器配置缺省值
    enum {
        DEF_TCP_SERVER_PORT         = 9000,      // TCP服务默认端口
        DEF_TCP_REQ_GROUP_COUNT     = 1,         // 请求组别总数的缺省值
        DEF_TCP_REQ_QUEUE_CAPACITY  = 500,       // TCP请求队列的缺省容量(即可容纳多少个连接)
        DEF_TCP_WORKER_THREADS_MIN  = 1,         // 工作者线程的缺省最少个数
        DEF_TCP_WORKER_THREADS_MAX  = 8,         // 工作者线程的缺省最多个数
        DEF_TCP_QUEUE_ALERT_LINE    = 50,        // 队列中连接数量警戒线缺省值，若超过警戒线则尝试增加线程
    };

private:
    /* ------------ 服务器常规配置: ------------ */

    // 服务器类型 (ST_UDP | ST_TCP)
    uint m_nServerType;
    // 后台调整工作者线程数量的时间间隔(秒)
    int m_nAdjustThreadInterval;
    // 辅助线程的个数
    int m_nHelperThreadCount;

    /* ------------ UDP服务器配置: ------------ */

    struct CUdpRequestGroupOpt
    {
        int nRequestQueueCapacity;    // 请求队列的容量(即可容纳多少个数据包)
        int nMinWorkerThreads;        // 工作者线程的最少个数
        int nMaxWorkerThreads;        // 工作者线程的最多个数

        CUdpRequestGroupOpt()
        {
            nRequestQueueCapacity = DEF_UDP_REQ_QUEUE_CAPACITY;
            nMinWorkerThreads = DEF_UDP_WORKER_THREADS_MIN;
            nMaxWorkerThreads = DEF_UDP_WORKER_THREADS_MAX;
        }
    };
    typedef vector<CUdpRequestGroupOpt> CUdpRequestGroupOpts;

    // UDP服务端口
    int m_nUdpServerPort;
    // 监听线程的数量
    int m_nUdpListenerThreadCount;
    // 请求组别的数量
    int m_nUdpRequestGroupCount;
    // 每个组别内的配置
    CUdpRequestGroupOpts m_UdpRequestGroupOpts;
    // 数据包在队列中的有效等待时间，超时则不予处理(秒)
    int m_nUdpRequestEffWaitTime;
    // 工作者线程的工作超时时间(秒)，若为0表示不进行超时检测
    int m_nUdpWorkerThreadTimeout;
    // 请求队列中数据包数量警戒线，若超过警戒线则尝试增加线程
    int m_nUdpRequestQueueAlertLine;

    /* ------------ TCP服务器配置: ------------ */

    struct CTcpRequestGroupOpt
    {
        int nTcpServerPort;           // TCP服务端口号
        int nRequestQueueCapacity;    // 请求队列的容量(即可容纳多少个数据包)
        int nMinWorkerThreads;        // 工作者线程的最少个数
        int nMaxWorkerThreads;        // 工作者线程的最多个数

        CTcpRequestGroupOpt()
        {
            nTcpServerPort = 0;
            nRequestQueueCapacity = DEF_UDP_REQ_QUEUE_CAPACITY;
            nMinWorkerThreads = DEF_UDP_WORKER_THREADS_MIN;
            nMaxWorkerThreads = DEF_UDP_WORKER_THREADS_MAX;
        }
    };
    typedef vector<CTcpRequestGroupOpt> CTcpRequestGroupOpts;

    // 请求组别的数量 (一个请求组别对应一个TCP端口)
    int m_nTcpRequestGroupCount;
    // 每个组别内的配置
    CTcpRequestGroupOpts m_TcpRequestGroupOpts;
    // 请求队列中连接数量警戒线，若超过警戒线则尝试增加线程
    int m_nTcpRequestQueueAlertLine;

public:
    CServerOptions();

    // 服务器配置设置----------------------------------------------------------

    // 设置服务器类型(ST_UDP | ST_TCP)
    void SetServerType(uint nSvrType);
    // 设置后台调整工作者线程数量的时间间隔(秒)
    void SetAdjustThreadInterval(int nSecs);
    // 设置辅助线程的数量
    void SetHelperThreadCount(int nCount);

    // 设置UDP服务端口号
    void SetUdpServerPort(int nPort);
    // 设置UDP监听线程的数量
    void SetUdpListenerThreadCount(int nCount);
    // 设置UDP请求的组别总数
    void SetUdpRequestGroupCount(int nCount);
    // 设置UDP请求队列的最大容量 (即可容纳多少个数据包)
    void SetUdpRequestQueueCapacity(int nGroupIndex, int nCapacity);
    // 设置UDP工作者线程个数的上下限
    void SetUdpWorkerThreadCount(int nGroupIndex, int nMinThreads, int nMaxThreads);
    // 设置UDP请求在队列中的有效等待时间，超时则不予处理(秒)
    void SetUdpRequestEffWaitTime(int nSecs);
    // 设置UDP工作者线程的工作超时时间(秒)，若为0表示不进行超时检测
    void SetUdpWorkerThreadTimeout(int nSecs);
    // 设置UDP请求队列中数据包数量警戒线
    void SetUdpRequestQueueAlertLine(int nCount);

    // 设置TCP请求的组别总数
    void SetTcpRequestGroupCount(int nCount);
    // 设置TCP服务端口号
    void SetTcpServerPort(int nGroupIndex, int nPort);
    // 设置TCP请求队列的容量(即可容纳多少个连接)
    void SetTcpRequestQueueCapacity(int nGroupIndex, int nCapacity);
    // 设置TCP工作者线程个数的上下限
    void SetTcpWorkerThreadCount(int nGroupIndex, int nMinThreads, int nMaxThreads);
    // 设置TCP请求队列中连接数量警戒线
    void SetTcpRequestQueueAlertLine(int nCount);

    // 服务器配置获取----------------------------------------------------------
    
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
// class CThreadTimeoutChecker - 线程超时检测类
//
// 说明:
// 此类用于配合 CUdpWorkerThread/CTcpWorkerThread，进行工作者线程的工作时间超时检测。
// 当工作者线程收到一个请求后，马上进入工作状态。一般而言，工作者线程为单个请求持续工作的
// 时间不宜太长，若太长则会导致服务器空闲工作者线程短缺，使得应付并发请求的能力下降。尤其
// 对于UDP服务来说情况更是如此。通常情况下，线程工作超时，很少是因为程序的流程和逻辑，而
// 是由于外部原因，比如数据库繁忙、资源死锁、网络拥堵等等。当线程工作超时后，应通知其退出，
// 若被通知退出后若干时间内仍未退出，则强行杀死。工作者线程调度中心再适时创建新的线程。

class CThreadTimeoutChecker : public CAutoInvokable
{
private:
    CThread *m_pThread;         // 被检测的线程
    time_t m_tStartTime;        // 开始计时时的时间戳
    bool m_bStarted;            // 是否已开始计时
    uint m_nTimeoutSecs;        // 超过多少秒认为超时 (为0表示不进行超时检测)
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

    // 检测线程是否已超时，若超时则通知其退出
    bool Check();

    // 设置超时时间，若为0则表示不进行超时检测
    void SetTimeoutSecs(uint nValue) { m_nTimeoutSecs = nValue; }
    // 返回是否已开始计时
    bool GetStarted();
};

///////////////////////////////////////////////////////////////////////////////
// class CUdpPacket - UDP数据包类

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
// class CUdpRequestQueue - UDP请求队列类

class CUdpRequestQueue
{
private:
    // 各容器优缺点:
    // deque  - 支持头尾快速增删，但增删中间元素很慢，支持下标访问。
    // vector - 支持尾部快速增删，头部和中间元素增删很慢，支持下标访问。
    // list   - 支持任何元素的快速增删，但不支持下标访问，不支持快速取当前长度(size())；
    typedef deque<CUdpPacket*> PacketList;

    CUdpRequestGroup *m_pOwnGroup;   // 所属组别
    PacketList m_PacketList;         // 数据包列表
    int m_nPacketCount;              // 队列中数据包的个数(为了快速访问)
    int m_nCapacity;                 // 队列的最大容量
    int m_nEffWaitTime;              // 数据包有效等待时间(秒)
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
// class CUdpWorkerThread - UDP工作者线程类
//
// 说明:
// 1. 缺省情况下，UDP工作者线程允许进行超时检测，若某些情况下需禁用超时检测，可以:
//    CUdpWorkerThread::GetTimeoutChecker().SetTimeoutSecs(0);
//
// 名词解释:
// 1. 超时线程: 因某一请求进入工作状态但长久未完成的线程。
// 2. 僵死线程: 已被通知退出但长久不退出的线程。

class CUdpWorkerThread : public CThread
{
private:
    CUdpWorkerThreadPool *m_pOwnPool;        // 所属线程池
    CThreadTimeoutChecker m_TimeoutChecker;  // 超时检测器
protected:
    virtual void Execute();
    virtual void DoTerminate();
    virtual void DoKill();
public:
    explicit CUdpWorkerThread(CUdpWorkerThreadPool *pThreadPool);
    virtual ~CUdpWorkerThread();

    // 返回超时检测器
    CThreadTimeoutChecker& GetTimeoutChecker() { return m_TimeoutChecker; }
    // 返回该线程是否空闲状态(即在等待请求)
    bool IsIdle() { return !m_TimeoutChecker.GetStarted(); }
};

///////////////////////////////////////////////////////////////////////////////
// class CUdpWorkerThreadPool - UDP工作者线程池类

class CUdpWorkerThreadPool
{
public:
    enum {
        MAX_THREAD_TERM_SECS     = 60*3,    // 线程被通知退出后的最长寿命(秒)
        MAX_THREAD_WAIT_FOR_SECS = 2        // 线程池清空时最多等待时间(秒)
    };

private:
    CUdpRequestGroup *m_pOwnGroup;          // 所属组别
    CList m_ThreadList;                     // 线程列表
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

    // 根据负载情况动态调整线程数量
    void AdjustThreadCount();
    // 通知所有线程退出
    void TerminateAllThreads();
    // 等待所有线程退出
    void WaitForAllThreads();

    // 取得当前线程数量
    int GetThreadCount() { return m_ThreadList.Count(); }
    // 取得所属组别
    CUdpRequestGroup& GetRequestGroup() { return *m_pOwnGroup; }
};

///////////////////////////////////////////////////////////////////////////////
// class CUdpRequestGroup - UDP请求组别类

class CUdpRequestGroup
{
private:
    CMainUdpServer *m_pOwnMainUdpSvr;       // 所属UDP服务器
    int m_nGroupIndex;                      // 组别号(0-based)
    CUdpRequestQueue m_RequestQueue;        // 请求队列
    CUdpWorkerThreadPool m_ThreadPool;      // 工作者线程池

public:
    CUdpRequestGroup(CMainUdpServer *pOwnMainUdpSvr, int nGroupIndex);
    virtual ~CUdpRequestGroup() {}

    int GetGroupIndex() { return m_nGroupIndex; }
    CUdpRequestQueue& GetRequestQueue() { return m_RequestQueue; }
    CUdpWorkerThreadPool& GetThreadPool() { return m_ThreadPool; }

    // 取得所属UDP服务器
    CMainUdpServer& GetMainUdpServer() { return *m_pOwnMainUdpSvr; }
};

///////////////////////////////////////////////////////////////////////////////
// class CMainUdpServer - UDP主服务器类

class CMainUdpServer : public CUdpServer
{
private:
    vector<CUdpRequestGroup*> m_RequestGroupList;   // 请求组别列表
    int m_nRequestGroupCount;                       // 请求组别总数
    CCustomDispatcher *m_pDispatcher;               // 程序数据分派器(引用 CMainServer::m_pDispatcher)

    void InitRequestGroupList();
    void ClearRequestGroupList();
public:
    explicit CMainUdpServer(CCustomDispatcher *pDispatcher);
    virtual ~CMainUdpServer();

    // 关闭服务器
    virtual void Close();

    // 收到数据包
    virtual void OnRecvData(void *pPacketBuffer, int nPacketSize, const PeerAddress& RemoteAddr);

    // 根据负载情况动态调整工作者线程数量
    void AdjustWorkerThreadCount();
    // 通知所有工作者线程退出
    void TerminateAllWorkerThreads();
    // 等待所有工作者线程退出
    void WaitForAllWorkerThreads();

    // 取得程序数据分派器
    CCustomDispatcher& GetDispatcher() { return *m_pDispatcher; }
};

///////////////////////////////////////////////////////////////////////////////
// class CTcpRequestQueue - TCP请求队列类

class CTcpRequestQueue
{
private:
    typedef deque<CTcpConnection*> TcpConnList;

    CTcpRequestGroup *m_pOwnGroup;    // 所属组别
    TcpConnList m_TcpConnList;        // TCP连接列表
    int m_nCapacity;                  // 队列的最大容量
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
// class CTcpWorkerThread - TCP工作者线程类
//
// 说明:
// 1. 缺省情况下，TCP工作者线程不进行超时检测，若某些情况下需启用超时检测，可以:
//    CTcpWorkerThread::GetTimeoutChecker().SetTimeoutSecs(nSeconds);

class CTcpWorkerThread : public CThread
{
private:
    CTcpWorkerThreadPool *m_pOwnPool;        // 所属线程池
    CThreadTimeoutChecker m_TimeoutChecker;  // 超时检测器
protected:
    virtual void Execute();
    virtual void DoTerminate();
    virtual void DoKill();
public:
    explicit CTcpWorkerThread(CTcpWorkerThreadPool *pThreadPool);
    virtual ~CTcpWorkerThread();

    // 返回超时检测器
    CThreadTimeoutChecker& GetTimeoutChecker() { return m_TimeoutChecker; }
    // 返回该线程是否空闲状态(即在等待请求)
    bool IsIdle() { return !m_TimeoutChecker.GetStarted(); }
};

///////////////////////////////////////////////////////////////////////////////
// class CTcpWorkerThreadPool - TCP工作者线程池类

class CTcpWorkerThreadPool
{
public:
    enum {
        MAX_THREAD_TERM_SECS     = 60*3,    // 线程被通知退出后的最长寿命(秒)
        MAX_THREAD_WAIT_FOR_SECS = 2        // 线程池清空时最多等待时间(秒)
    };

private:
    CTcpRequestGroup *m_pOwnGroup;          // 所属组别
    CList m_ThreadList;                     // 线程列表
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

    // 根据负载情况动态调整线程数量
    void AdjustThreadCount();
    // 通知所有线程退出
    void TerminateAllThreads();
    // 等待所有线程退出
    void WaitForAllThreads();

    // 取得当前线程数量
    int GetThreadCount() { return m_ThreadList.Count(); }
    // 取得所属组别
    CTcpRequestGroup& GetRequestGroup() { return *m_pOwnGroup; }
};

///////////////////////////////////////////////////////////////////////////////
// class CTcpRequestGroup - TCP请求组别类

class CTcpRequestGroup : public CTcpServer
{
private:
    CMainTcpServer *m_pOwnMainTcpSvr;       // 所属TCP服务器
    int m_nGroupIndex;                      // 组别号(0-based)
    int m_nTcpPort;                         // TCP端口号
    CTcpRequestQueue m_RequestQueue;        // TCP请求队列
    CTcpWorkerThreadPool m_ThreadPool;      // 工作者线程池

public:
    CTcpRequestGroup(CMainTcpServer *pOwnMainTcpSvr, int nGroupIndex, int nTcpPort);
    virtual ~CTcpRequestGroup() {}

    // 关闭服务器
    virtual void Close();
    // 收到连接 (注: pConnection 是堆对象，需使用者释放)
    virtual void OnConnect(CTcpConnection *pConnection);

    int GetGroupIndex() { return m_nGroupIndex; }
    int GetTcpPort() { return m_nTcpPort; }
    CTcpRequestQueue& GetRequestQueue() { return m_RequestQueue; }
    CTcpWorkerThreadPool& GetThreadPool() { return m_ThreadPool; }

    // 取得所属TCP服务器
    CMainTcpServer& GetMainTcpServer() { return *m_pOwnMainTcpSvr; }
};

///////////////////////////////////////////////////////////////////////////////
// class CMainTcpServer - TCP主服务器类

class CMainTcpServer
{
private:
    vector<CTcpRequestGroup*> m_RequestGroupList;   // 请求组别列表
    int m_nRequestGroupCount;                       // 请求组别总数
    CCustomDispatcher *m_pDispatcher;               // 程序数据分派器(引用 CMainServer::m_pDispatcher)

    void InitRequestGroupList();
    void ClearRequestGroupList();
public:
    explicit CMainTcpServer(CCustomDispatcher *pDispatcher);
    virtual ~CMainTcpServer();

    void Open();
    void Close();

    // 根据负载情况动态调整工作者线程数量
    void AdjustWorkerThreadCount();
    // 通知所有工作者线程退出
    void TerminateAllWorkerThreads();
    // 等待所有工作者线程退出
    void WaitForAllWorkerThreads();

    // 取得程序数据分派器
    CCustomDispatcher& GetDispatcher() { return *m_pDispatcher; }
};

///////////////////////////////////////////////////////////////////////////////
// class CHelperThread - 辅助线程类
//
// 说明:
// 1. 对于服务端程序，除了网络服务之外，一般还需要若干后台守护线程，用于后台维护工作，
//    比如垃圾数据回收、数据库过期数据清理等等。这类服务线程统称为 helper thread.

class CHelperThread : public CThread
{
private:
    CHelperThreadPool *m_pOwnPool;          // 所属线程池
    int m_nHelperIndex;                     // 辅助服务序号(0-based)
protected:
    virtual void Execute();
    virtual void DoKill();
public:
    CHelperThread(CHelperThreadPool *pThreadPool, int nHelperIndex);
    virtual ~CHelperThread();
};

///////////////////////////////////////////////////////////////////////////////
// class CHelperThreadPool - 辅助线程池类

class CHelperThreadPool
{
public:
    enum { MAX_THREAD_WAIT_FOR_SECS = 5 };  // 线程池清空时最多等待时间(秒)

private:
    CHelperServer *m_pOwnHelperSvr;         // 所属辅助服务器
    CList m_ThreadList;                     // 线程列表
    CCriticalSection m_Lock;

public:
    explicit CHelperThreadPool(CHelperServer *pOwnHelperServer);
    ~CHelperThreadPool();

    void RegisterThread(CHelperThread *pThread);
    void UnregisterThread(CHelperThread *pThread);

    // 通知所有线程退出
    void TerminateAllThreads();
    // 等待所有线程退出
    void WaitForAllThreads();

    // 取得当前线程数量
    int GetThreadCount() { return m_ThreadList.Count(); }
    // 取得所属辅助服务器
    CHelperServer& GetHelperServer() { return *m_pOwnHelperSvr; }
};

///////////////////////////////////////////////////////////////////////////////
// class CHelperServer - 辅助服务类

class CHelperServer
{
private:
    bool m_bActive;                         // 服务器是否启动
    CCustomDispatcher *m_pDispatcher;       // 程序数据分派器(引用 CMainServer::m_pDispatcher)
    CHelperThreadPool m_ThreadPool;         // 辅助线程池

public:
    explicit CHelperServer(CCustomDispatcher *pDispatcher);
    virtual ~CHelperServer();

    // 启动服务器
    virtual void Open();
    // 关闭服务器
    virtual void Close();

    // 辅助服务线程执行函数
    virtual void OnHelperThreadExecute(CHelperThread& HelperThread, int nHelperIndex);

    // 通知所有辅助线程退出
    void TerminateAllHelperThreads();
    // 等待所有辅助线程退出
    void WaitForAllHelperThreads();
};

///////////////////////////////////////////////////////////////////////////////
// class CMainServer - 主服务器类

class CMainServer
{
private:
    CMainUdpServer *m_pUdpServer;       // UDP服务器
    CMainTcpServer *m_pTcpServer;       // TCP服务器
    CHelperServer *m_pHelperServer;     // 辅助服务器
    CCustomDispatcher *m_pDispatcher;   // 程序数据分派器

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
// class CDispatcherUtils - 程序数据分派器的杂项工具集类

class CDispatcherUtils
{
public:
    // 发送UDP数据包
    void SendUdpPacket(void *pBuffer, int nSize, const CInetAddress& ToAddr, 
        int nToPort, int nSendTimes = 1);
};

///////////////////////////////////////////////////////////////////////////////
// class CCustomDispatcher - 程序数据分派器基类
//
// 说明:
// 1. 此类用于向扩展应用分派所有的程序数据，包括网络数据、线程等。
// 2. 用户须继承此类，从而接收 ISE 分派的数据。
// 3. 推荐用 Initialize()/Finalize() 代替构造/析构函数。C++构造函数若抛出异常，析构
//    函数将不再执行；而 Finalize() 无论如何一定会执行。Initialize()/Finalize() 函
//    数对由 CMainServer 调用。

class CCustomDispatcher
{
private:
    CDispatcherUtils m_Utils;   // 杂项工具集

public:
    CCustomDispatcher() {}
    virtual ~CCustomDispatcher() {}

    // 初始化 (失败则抛出异常)
    virtual void Initialize() {}
    // 结束化 (无论初始化是否有异常，结束时都会执行)
    virtual void Finalize() {}

    // UDP数据包分类
    virtual void ClassifyUdpPacket(void *pPacketBuffer, int nPacketSize, int& nGroupIndex) { nGroupIndex = 0; }
    // UDP数据包分派
    virtual void DispatchUdpPacket(CUdpWorkerThread& WorkerThread, int nGroupIndex, CUdpPacket& Packet) {}
    // TCP连接分派
    virtual void DispatchTcpConnection(CTcpWorkerThread& WorkerThread, int nGroupIndex, CTcpConnection& Connection) {}

    // 辅助服务线程执行(nHelperIndex: 0-based)
    virtual void HelperThreadExecute(CHelperThread& HelperThread, int nHelperIndex) {}

    // 返回杂项工具集
    inline CDispatcherUtils& GetUtils() { return m_Utils; }
};

///////////////////////////////////////////////////////////////////////////////

} // namespace ise

#endif // _ISE_SERVER_H_ 
