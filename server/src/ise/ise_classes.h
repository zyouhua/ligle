///////////////////////////////////////////////////////////////////////////////
// Iris Server Engine (ISE)
// ise_classes.h
// Classes:
//   > CException
//   > CLogger
//   > CBuffer
//   > CDateTime
//   > CAutoInvokable
//   > CAutoInvoker
//   > CSem
//   > CShm
//   > CSignalMasker
//   > CFileLock
//   > CMultiInstChecker
//   > CCriticalSection
//   > CAutoSynchronizer
//   > CThreadSem
//   > CSeqAllocator
//   > CThread
//   > CStream
//   > CMemoryStream
//   > CFileStream
//   > CList
///////////////////////////////////////////////////////////////////////////////

#ifndef _ISE_CLASSES_H_
#define _ISE_CLASSES_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/timeb.h>
#include <sys/file.h>
#include <linux/unistd.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "ise_types.h"

using namespace std;

namespace ise
{

///////////////////////////////////////////////////////////////////////////////
// class CException - 异常基类

class CException
{
public:
    enum { EMPTY_ERROR_NO = 32767,  EMPTY_LINE_NO = -1 };
    
protected:
    int m_nErrorNo;
    string m_strErrorMsg;
    string m_strFileName;
    int m_nLineNo;    

public:
    CException(int nErrorNo, const char *sErrorMsg, const char *sFileName = NULL, int nLineNo = EMPTY_LINE_NO);
    CException(const char *sErrorMsg, const char *sFileName = NULL, int nLineNo = EMPTY_LINE_NO);
    CException(const CException& src);
    virtual ~CException() {}

    int ErrorNo() const { return m_nErrorNo; }
    string ErrorMsg() const { return m_strErrorMsg; }
    string FileName() const { return m_strFileName; }
    int LineNo() const { return m_nLineNo; }

    // 返回用于 Log 的字符串 
    string MakeLogMsg() const;
};

///////////////////////////////////////////////////////////////////////////////
// class CLogger - 日志类

class CLogger
{
private:
    string m_strFileName;   // 日志文件名

    void WriteToFile(const string& strString);
public:
    CLogger() {}
    ~CLogger() {}

    void SetFileName(const string& strFileName)
        { m_strFileName = strFileName; }

    void Write(const char* sFormatString, ...);
    void Write(const CException& e);
};

///////////////////////////////////////////////////////////////////////////////
// class CBuffer - 缓存类

class CBuffer
{
protected:
    void *m_pBuffer;
    int m_nSize;

public:
    CBuffer();
    CBuffer(const CBuffer& src);
    explicit CBuffer(int nSize);
    CBuffer(const void *pBuffer, int nSize);
    virtual ~CBuffer();

    CBuffer& operator = (const CBuffer& rhs);
    char& operator[](int nIndex) { return ((char*)m_pBuffer)[nIndex]; }
    operator char*() const { return (char*)m_pBuffer; }
    char* Data() const { return (char*)m_pBuffer; }
    char* c_str() const;
    void Assign(const void *pBuffer, int nSize);
    void SetSize(int nSize);
    int GetSize() const { return m_nSize; }
};

///////////////////////////////////////////////////////////////////////////////
// class CDateTime - 日期时间类

class CDateTime
{
private:
    time_t m_tTime;     // (从1970-01-01 00:00:00 算起的秒数)

public:
    CDateTime()                     { m_tTime = 0; }
    CDateTime(const CDateTime& src) { m_tTime = src.m_tTime; }
    explicit CDateTime(time_t src)  { m_tTime = src; }

    static CDateTime CurrentDateTime();
    static time_t CurrentZoneTimeStamp();

    CDateTime& operator = (const CDateTime& rhs) 
        { m_tTime = rhs.m_tTime; return *this; }
    CDateTime& operator = (time_t rhs) 
        { m_tTime = rhs; return *this; }
    CDateTime& operator = (const string& strDateTime);

    CDateTime operator + (int nSeconds) { return CDateTime(m_tTime + nSeconds); }
    CDateTime operator - (int nSeconds) { return CDateTime(m_tTime - nSeconds); }

    bool operator == (const CDateTime& rhs) const { return m_tTime == rhs.m_tTime; }
    bool operator != (const CDateTime& rhs) const { return m_tTime != rhs.m_tTime; }
    bool operator > (const CDateTime& rhs) const  { return m_tTime > rhs.m_tTime; }
    bool operator < (const CDateTime& rhs) const  { return m_tTime < rhs.m_tTime; }
    bool operator >= (const CDateTime& rhs) const { return m_tTime >= rhs.m_tTime; }
    bool operator <= (const CDateTime& rhs) const { return m_tTime <= rhs.m_tTime; }

    operator time_t() const { return m_tTime; }

    void EncodeDateTime(int nYear, int nMonth, int nDay, 
        int nHour = 0, int nMinute = 0, int nSecond = 0);
    void DecodeDateTime(int& nYear, int& nMonth, int& nDay, 
        int& nHour, int& nMinute, int& nSecond) const;

    string DateString(char chDateSep = '-') const;
    string DateTimeString(char chDateSep = '-', char chTimeSep = ':') const;
};

///////////////////////////////////////////////////////////////////////////////
// class CAutoInvokable/CAutoInvoker - 自动被调对象/自动调用者
//
// 说明:
// 1. 这两个类联合使用，可以起到和 "智能指针" 类似的作用，即利用栈对象自动销毁的特性，在栈
//    对象的生命周期中自动调用 CAutoInvokable::InvokeInitialize() 和 InvokeFinalize()。
//    此二类一般使用在重要资源的对称性操作场合(比如加锁/解锁)。
// 2. 使用者需继承 CAutoInvokable 类，重写 InvokeInitialize() 和 InvokeFinalize()
//    函数。并在需要调用的地方定义 CAutoInvoker 的栈对象。

class CAutoInvokable
{
friend class CAutoInvoker;

protected:
    virtual void InvokeInitialize() {}
    virtual void InvokeFinalize() {}
};

class CAutoInvoker
{
private:
    CAutoInvokable *m_pObject;
public:
    explicit CAutoInvoker(CAutoInvokable& Object)
    { m_pObject = &Object; m_pObject->InvokeInitialize(); }

    virtual ~CAutoInvoker()
    { m_pObject->InvokeFinalize(); }
};

///////////////////////////////////////////////////////////////////////////////
// class CSem - 信号量类

#if defined(__GNU_LIBRARY__) && !defined(_SEM_SEMUN_UNDEFINED)
// union semun is defined by including <sys/sem.h>
#else
// according to X/OPEN we have to define it ourselves
union semun {
    int val;                    // value for SETVAL
    struct semid_ds *buf;       // buffer for IPC_STAT, IPC_SET
    unsigned short int *array;  // array for GETALL, SETALL
    struct seminfo *__buf;      // buffer for IPC_INFO
};
#endif

class CSem
{
public:
    enum { SEM_DEF_MODE = 0666 };       // 创建信号量的缺省MODE
    enum { SEM_INIT_VALUE = 1 };        // 信号量初始化之后的值

protected:
    key_t m_nSemKey;    // 信号量组在系统中的唯一标识符
    int m_nSemId;       // 信号量组的ID值
    int m_nSemCount;    // 信号量组中信号量的个数
    bool m_bCreate;     // 是否创建了信号量组

public: 
    CSem(key_t nSemKey, int nSemCount = 1);
    virtual ~CSem();
    
    void GetSem(int nMode = SEM_DEF_MODE);
    void RemoveSem();
    bool Lock(int nSemNum);
    void Unlock(int nSemNum);
    void SetAll(int nSemCount, ...);
    void SetValue(int nSemNum, int nValue);
    void GetAll(unsigned short int *pArray);
    int GetValue(int nSemNum);
    void InitSem();

    key_t GetSemKey() { return m_nSemKey; }
    int GetSemCount() { return m_nSemCount; }
    bool IsCreate() { return m_bCreate; }
};

///////////////////////////////////////////////////////////////////////////////
// class CShm - 共享内存类

class CShm
{
public:
    enum { SHM_DEF_MODE = 0666 };       // 创建共享内存的缺省MODE
    enum { SHM_RETRY_TIMES = 1000 };    // 创建共享内存时重试次数
        
protected:
    key_t m_nShmKey;        // 共享内存在系统中的唯一标识符
    int m_nShmId;           // 共享内存的ID值
    const int m_nShmSize;   // 共享内存大小(字节数)
    char *m_pShmData;       // 共享内存指针
    bool m_bCreate;         // 是否创建了共享内存
    
public:
    CShm(key_t nShmKey, int nShmSize);
    virtual ~CShm();
    
    void GetShm(int nMode = SHM_DEF_MODE);
    void AttachShm();
    void DetachShm();
    void RemoveShm();
    void InitShm();
    void TryCreateShm(int nRetryTimes = SHM_RETRY_TIMES, int nMode = SHM_DEF_MODE);
    inline char *GetData() { return m_pShmData; }
    inline bool IsCreate() { return m_bCreate; }

    static bool ShmExists(key_t nShmKey);
};

///////////////////////////////////////////////////////////////////////////////
// class CSignalMasker - 信号屏蔽类

class CSignalMasker
{
private:
    sigset_t m_OldSet;
    sigset_t m_NewSet;
    bool m_bBlock;
    bool m_bAutoRestore;

    int SigProcMask(int nHow, const sigset_t* pNewSet, sigset_t* pOldSet);
public:
    explicit CSignalMasker(bool bAutoRestore = false);
    virtual ~CSignalMasker();

    // 设置 Block/UnBlock 操作所需的信号集合
    void SetSignals(int nSigCount, ...);
    void SetSignals(int nSigCount, va_list argList);

    // 在进程当前阻塞信号集中添加 SetSignals 设置的信号
    void Block();
    // 在进程当前阻塞信号集中解除 SetSignals 设置的信号
    void UnBlock();

    // 将进程阻塞信号集恢复为 Block/UnBlock 之前的状态
    void Restore();
};

///////////////////////////////////////////////////////////////////////////////
// class CFileLock - 文件锁类
//
// 说明:
// 1. 文件锁是基于进程的，同一进程中的互斥不能用文件锁。
// 2. 文件锁的操作时间大概是信号量(SEM)的两倍。

class CFileLock
{
public:
    enum { 
        DEF_FILELOCK_MODE   = (O_RDWR | O_CREAT),   // 创建文件的缺省 Mode
        DEF_FILELOCK_RIGHTS = 0666                  // 创建文件的缺省 Rights
    };

protected:
    int m_nFd;          // 文件描述符
    bool m_bAutoClose;  // 对象析构时是否关闭文件
    
protected:
    bool Fcntl(int nCmd, int nType, int nOffset, int nLen, int nWhence);
    bool Flock(int nCmd);
    
public:
    CFileLock(const char *sFileName, uint nMode = DEF_FILELOCK_MODE, uint nRights = DEF_FILELOCK_RIGHTS);
    virtual ~CFileLock();
    
    //-- 对部分文件加锁:
    // 立即返回型(返回true:加锁成功; false:需重试加锁)
    inline bool ReadLock(int nOffset, int nLen = 1, int nWhence = SEEK_SET)
        { return Fcntl(F_SETLK, F_RDLCK, nOffset, nLen, nWhence); }
    inline bool WriteLock(int nOffset, int nLen = 1, int nWhence = SEEK_SET)
        { return Fcntl(F_SETLK, F_WRLCK, nOffset, nLen, nWhence); } 
    // 等待解锁型(返回true:加锁成功; false:因为信号而中断)
    inline bool ReadLockW(int nOffset, int nLen = 1, int nWhence = SEEK_SET)
        { return Fcntl(F_SETLKW, F_RDLCK, nOffset, nLen, nWhence); }    
    inline bool WriteLockW(int nOffset, int nLen = 1, int nWhence = SEEK_SET)
        { return Fcntl(F_SETLKW, F_WRLCK, nOffset, nLen, nWhence); }
    // 解锁操作
    inline bool Unlock(int nOffset, int nLen = 1, int nWhence = SEEK_SET)
        { return Fcntl(F_SETLK, F_UNLCK, nOffset, nLen, nWhence); }

    //-- 对整个文件加锁:
    inline bool FileReadLock(bool bBlock = true)
        { return Flock(LOCK_SH | (bBlock? 0 : LOCK_NB)); }
    inline bool FileWriteLock(bool bBlock = true)
        { return Flock(LOCK_EX | (bBlock? 0 : LOCK_NB)); }
    inline bool FileUnLock()
        { return Flock(LOCK_UN); }
};

///////////////////////////////////////////////////////////////////////////////
// class CMultiInstChecker - 程序的多个实体重复运行检测类
//
// 说明: 类对象的生命期必须为全局才可正常工作。

class CMultiInstChecker
{
private:
    CFileLock *m_pFileLock;
    string m_strLockFile;
    bool m_bIsSelfExe;
public:
    CMultiInstChecker() : m_pFileLock(NULL) {}
    virtual ~CMultiInstChecker() { delete m_pFileLock; }

    // 设置待加锁的文件名
    void SetLockFile(const char *pLockFile, bool bIsSelfExe);
    // 检查程序是否已经运行
    bool IsAlreayRunning();
};

///////////////////////////////////////////////////////////////////////////////
// class CCriticalSection - 线程临界区互斥类
//
// 说明:
// 1. 此类用于多线程环境下临界区互斥，基本操作有 Lock、Unlock 和 TryLock；
// 2. 此类采用线程互斥锁 (pthread_mutex_t) 来实现，仅对线程有效，对进程互斥无用；
// 3. 线程内不允许嵌套调用 Lock，若嵌套调用则抛出异常；
// 4. Unlock 只对自己线程的 Lock 有解锁作用，不会影响其它线程；
// 5. Unlock 可以安全调用多次，适合用于 CThread::DoKill() 中；

class CCriticalSection : public CAutoInvokable
{
private:
    pthread_mutex_t m_Mutex;

protected:
    virtual void InvokeInitialize() { Lock(); }
    virtual void InvokeFinalize() { Unlock(); }

public:
    CCriticalSection();
    ~CCriticalSection();

    // 加锁
    void Lock();
    // 解锁
    void Unlock();
    // 尝试加锁 (若已经处于加锁状态则立即返回 false)
    bool TryLock();
};

///////////////////////////////////////////////////////////////////////////////
// class CAutoSynchronizer - 线程自动互斥类
//
// 说明:
// 1. 此类利用C++的栈对象自动销毁的特性，在多线程环境下进行局部范围临界区互斥；
// 2. 使用方法: 在需要互斥的范围中以局部变量方式定义此类对象即可；
// 
// 使用范例:
//   假设已定义: CCriticalSection m_Lock;
//   自动加锁和解锁:
//   {
//       CAutoSynchronizer Syncher(m_Lock);
//       //...
//   }

typedef CAutoInvoker CAutoSynchronizer;

///////////////////////////////////////////////////////////////////////////////
// class CThreadSem - 线程信号灯类

class CThreadSem
{
private:
    sem_t m_Sem;
    int m_nInitValue;
public:
    explicit CThreadSem(int nInitValue = 0);
    virtual ~CThreadSem();

    void Increase();
    void Wait();
    bool TryWait();
    int GetValue();
    void Reset();
};

///////////////////////////////////////////////////////////////////////////////
// class CSeqAllocator - 整数序列号分配器类
//
// 说明:
// 1. 此类以线程安全方式生成一个不断递增的整数序列，用户可以指定序列的起始值；
// 2. 此类一般用于数据包的顺序号控制；

class CSeqAllocator
{
private:
    CCriticalSection m_Lock;
    uint m_nCurrentId;

public:
    explicit CSeqAllocator(uint nStartId = 0);

    // 返回一个新分配的ID
    uint AllocId();
};

///////////////////////////////////////////////////////////////////////////////
// class CThread - 线程类
//
// 说明:
// 1. 此类封装了 POSIX 线程的主要API，用户可以继承此类，重写 Execute() 函数；
// 2. 由于 POSIX 不支持线程的 resume/suspend 操作，所以该类也未提供相应功能；
// 3. 创建此类的对象后，线程并未创建。函数 Run() 负责线程的创建和执行；
// 4. 在创建线程类对象时，强烈建议在堆上创建(new CThread)。如果在栈上创建，由于栈对象会
//    自动释放，导致线程非法操作；
// 5. 缺省情况下，线程执行完毕不会自动释放类对象。若要自动释放，可 SetFreeOnTerminate(True);
// 6. 若设置了自动释放类对象，则不可调用 WaitFor()，否则有可能引起非法操作；
//
// 线程调度策略以及优先级:
//
//    Linux 下线程的优先级(priority)是基于调度策略(policy)的。有如下三种调度策略：
//
//        Policy          Type         Priority
//      ----------      --------       --------
//      SCHED_OTHER     正常、非实时       0
//      SCHED_RR        实时、轮转法       1-99
//      SCHED_FIFO      实时、先入先出     1-99
//
//    注: 调度策略缺省为SCHED_OTHER。后两种仅对超级用户有效。

class CThread
{
friend void ThreadFinalProc(void *arg);
friend void* ThreadExecProc(void *arg);

public:
    // 线程调度策略
    enum {
        THREAD_POLICY_DEFAULT   = SCHED_OTHER,
        THREAD_POLICY_RR        = SCHED_RR,
        THREAD_POLICY_FIFO      = SCHED_FIFO
    };

    // 线程优先级
    enum {
        THREAD_PRIORITY_DEFAULT = 0,
        THREAD_PRIORITY_MIN     = 0,
        THREAD_PRIORITY_MAX     = 99,
        THREAD_PRIORITY_HIGH    = 80
    };

private:
    pthread_t m_nThreadId;          // 线程ID
    bool m_bFinished;               // 线程是否已完成了线程函数的执行
    int m_nTermElapsedSecs;         // 从调用 Terminate 到当前共经过多少时间(秒)

protected:
    bool m_bFreeOnTerminate;        // 线程退出时是否同时释放类对象
    bool m_bTerminated;             // 是否应退出的标志
    int m_nReturnValue;             // 线程返回值 (可在 Execute 函数中修改此值，函数 WaitFor 返回此值)
    int m_nPolicy;                  // 线程调度策略 (THREAD_POLICY_XXX)
    int m_nPriority;                // 线程优先级 (0..99)

private:
    void CheckThreadError(int nErrorCode);

protected:
    // 线程的执行函数，子类必须重写。
    // 注: 此函数中的异常处理有一些需要注意的事项，详见源码注释:
    // ( ise_classes.cpp::ThreadExecProc() 中)
    virtual void Execute() {}

    // 执行 Terminate() 前的附加操作。
    // 注: 由于 Terminate() 属于自愿退出机制，为了能让线程能尽快退出，除了
    // m_bTerminated 标志被设为 true 之外，有时还应当补充一些附加的操作以
    // 便能让线程尽快从阻塞操作中解脱出来。
    virtual void DoTerminate() {}

    // 执行 Kill() 前的附加操作。
    // 注: 线程被杀死后，用户所管理的某些重要资源可能未能得到释放，比如锁资源
    // (还未来得及解锁便被杀了)，所以重要资源的释放工作必须在 DoKill 中进行。
    virtual void DoKill() {}
public:
    CThread();
    virtual ~CThread();

    // 创建并执行线程。
    // 注: 此成员方法在对象声明周期中只可调用一次。
    void Run();

    // 通知线程退出 (自愿退出机制)
    // 注: 若线程由于某些阻塞式操作迟迟不退出，可调用 Kill() 强行退出。
    void Terminate();
    
    // 强行杀死线程 (强行退出机制)
    // 注: 注意事项详见 ise_classes.cpp::Kill() 源码注释。
    void Kill();
    
    // 等待线程退出
    int WaitFor();

    // 进入睡眠状态 (睡眠过程中会检测 m_bTerminated 的状态)
    // 注: 此函数必须由线程自己调用方可生效。
    void Sleep(double fSeconds) const;

    // 属性 (getter)
    int GetThreadId() const { return m_nThreadId; }
    int GetTerminated() const { return m_bTerminated; }
    int GetReturnValue() const { return m_nReturnValue; }
    bool GetFreeOnTerminate() const { return m_bFreeOnTerminate; }
    int GetPolicy() const { return m_nPolicy; }
    int GetPriority() const { return m_nPriority; }
    int GetTermElapsedSecs() const;
    // 属性 (setter)
    void SetTerminated(bool bValue);
    void SetReturnValue(int nValue) { m_nReturnValue = nValue; }
    void SetFreeOnTerminate(bool bValue) { m_bFreeOnTerminate = bValue; }
    void SetPolicy(int nValue);
    void SetPriority(int nValue);
};

///////////////////////////////////////////////////////////////////////////////
// class CStream - 流 基类

enum SeekOrigin { 
    SO_BEGINNING    = 0, 
    SO_CURRENT      = 1, 
    SO_END          = 2
};

class CStream
{
public:
    virtual ~CStream() {}

    virtual int Read(void *pBuffer, int nCount) = 0;
    virtual int Write(const void *pBuffer, int nCount) = 0;
    virtual uint64 Seek(uint64 nOffset, SeekOrigin nSeekOrigin) = 0;

    uint64 GetPosition() { return Seek(0, SO_CURRENT); }
    void SetPosition(uint64 nPos) { Seek(nPos, SO_BEGINNING); }

    virtual uint64 GetSize();
    virtual void SetSize(uint64 nSize) {}
};

///////////////////////////////////////////////////////////////////////////////
// class CMemoryStream - 内存流类

class CMemoryStream : public CStream
{
public:
    enum { DEFAULT_MEMORY_DELTA = 1024 };    // 缺省内存增长步长 (字节数，必须是 2 的 N 次方)
    enum { MIN_MEMORY_DELTA = 256 };         // 最小内存增长步长 

private:
    char *m_pMemory;
    uint64 m_nCapacity;
    uint64 m_nSize;
    uint64 m_nPosition;
    uint64 m_nMemoryDelta;

    void SetMemoryDelta(uint64 nNewMemoryDelta);
    void SetPointer(char* pMemory, uint64 nSize);
    void SetCapacity(uint64 nNewCapacity);
    char* Realloc(uint64& nNewCapacity);
public:
    explicit CMemoryStream(uint64 nMemoryDelta = DEFAULT_MEMORY_DELTA);
    virtual ~CMemoryStream();

    virtual int Read(void *pBuffer, int nCount);
    virtual int Write(const void *pBuffer, int nCount);
    virtual uint64 Seek(uint64 nOffset, SeekOrigin nSeekOrigin);
    virtual void SetSize(uint64 nSize);
    void LoadFromStream(CStream& Stream);
    void LoadFromFile(const string& strFileName);
    void SaveToStream(CStream& Stream);
    void SaveToFile(const string& strFileName);
    void Clear();
    char* GetMemory() { return m_pMemory; }
};

///////////////////////////////////////////////////////////////////////////////
// class CFileStream - 文件流类

// 文件流打开方式 (uint nMode)
enum {
    FM_CREATE           = 0xFFFF,
    FM_OPEN_READ        = O_RDONLY,  // 0
    FM_OPEN_WRITE       = O_WRONLY,  // 1
    FM_OPEN_READ_WRITE  = O_RDWR,    // 2
    
    FM_SHARE_EXCLUSIVE  = 0x0010,
    FM_SHARE_DENY_WRITE = 0x0020,
    FM_SHARE_DENY_NONE  = 0x0030
};

class CFileStream : public CStream
{
public:
    // 缺省文件存取权限
    enum { DEFAULT_FILE_ACCESS_RIGHTS = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH };    

private:
    int m_nHandle;      // 文件句柄

    int FileCreate(const string& strFileName, uint nRights);
    int FileOpen(const string& strFileName, uint nMode);
    void FileClose(int nHandle);
    uint64 FileRead(int nHandle, void *pBuffer, uint64 nCount);
    uint64 FileWrite(int nHandle, const void *pBuffer, uint64 nCount);
    uint64 FileSeek(int nHandle, uint64 nOffset, SeekOrigin nSeekOrigin);
public:
    CFileStream(const string& strFileName, uint nMode, uint nRights = DEFAULT_FILE_ACCESS_RIGHTS);
    virtual ~CFileStream();

    virtual int Read(void *pBuffer, int nCount);
    virtual int Write(const void *pBuffer, int nCount);
    virtual uint64 Seek(uint64 nOffset, SeekOrigin nSeekOrigin);
    virtual void SetSize(uint64 nSize);
};

///////////////////////////////////////////////////////////////////////////////
// class CList  - 列表类
// 
// 说明:
// 1. 此类的实现原理与 Delphi::TList 完全相同；
// 2. 此列表类有如下优点:
//    a. 公有方法简单明确 (STL虽无比强大但稍显晦涩)；
//    b. 支持下标随机存取各个元素 (STL::list不支持)；
//    c. 支持快速获取列表长度 (STL::list不支持)；
//    d. 支持尾部快速增删元素；
// 3. 此列表类有如下缺点:
//    a. 不支持头部和中部的快速增删元素；
//    b. 只支持单一类型元素(Pointer类型)；

class CList
{
private:
    Pointer *m_pList;
    int m_nCount;
    int m_nCapacity;

protected:
    virtual void Grow();

    Pointer Get(int nIndex);
    void Put(int nIndex, Pointer Item);
    void SetCapacity(int nNewCapacity);
    void SetCount(int nNewCount);

public:
    CList();
    virtual ~CList();

    void Add(Pointer Item);
    void Insert(int nIndex, Pointer Item);
    void Delete(int nIndex);
    int Remove(Pointer Item);
    Pointer Extract(Pointer Item);
    void Move(int nCurIndex, int nNewIndex);
    void Resize(int nCount);
    void Clear();

    Pointer First();
    Pointer Last();
    int IndexOf(Pointer Item);
    int Count() const;

    Pointer& operator[](int nIndex);
};

///////////////////////////////////////////////////////////////////////////////

} // namespace ise

#endif // _ISE_CLASSES_H_ 
