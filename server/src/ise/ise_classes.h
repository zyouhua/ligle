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
// class CException - �쳣����

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

    // �������� Log ���ַ��� 
    string MakeLogMsg() const;
};

///////////////////////////////////////////////////////////////////////////////
// class CLogger - ��־��

class CLogger
{
private:
    string m_strFileName;   // ��־�ļ���

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
// class CBuffer - ������

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
// class CDateTime - ����ʱ����

class CDateTime
{
private:
    time_t m_tTime;     // (��1970-01-01 00:00:00 ���������)

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
// class CAutoInvokable/CAutoInvoker - �Զ���������/�Զ�������
//
// ˵��:
// 1. ������������ʹ�ã������𵽺� "����ָ��" ���Ƶ����ã�������ջ�����Զ����ٵ����ԣ���ջ
//    ����������������Զ����� CAutoInvokable::InvokeInitialize() �� InvokeFinalize()��
//    �˶���һ��ʹ������Ҫ��Դ�ĶԳ��Բ�������(�������/����)��
// 2. ʹ������̳� CAutoInvokable �࣬��д InvokeInitialize() �� InvokeFinalize()
//    ������������Ҫ���õĵط����� CAutoInvoker ��ջ����

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
// class CSem - �ź�����

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
    enum { SEM_DEF_MODE = 0666 };       // �����ź�����ȱʡMODE
    enum { SEM_INIT_VALUE = 1 };        // �ź�����ʼ��֮���ֵ

protected:
    key_t m_nSemKey;    // �ź�������ϵͳ�е�Ψһ��ʶ��
    int m_nSemId;       // �ź������IDֵ
    int m_nSemCount;    // �ź��������ź����ĸ���
    bool m_bCreate;     // �Ƿ񴴽����ź�����

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
// class CShm - �����ڴ���

class CShm
{
public:
    enum { SHM_DEF_MODE = 0666 };       // ���������ڴ��ȱʡMODE
    enum { SHM_RETRY_TIMES = 1000 };    // ���������ڴ�ʱ���Դ���
        
protected:
    key_t m_nShmKey;        // �����ڴ���ϵͳ�е�Ψһ��ʶ��
    int m_nShmId;           // �����ڴ��IDֵ
    const int m_nShmSize;   // �����ڴ��С(�ֽ���)
    char *m_pShmData;       // �����ڴ�ָ��
    bool m_bCreate;         // �Ƿ񴴽��˹����ڴ�
    
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
// class CSignalMasker - �ź�������

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

    // ���� Block/UnBlock ����������źż���
    void SetSignals(int nSigCount, ...);
    void SetSignals(int nSigCount, va_list argList);

    // �ڽ��̵�ǰ�����źż������ SetSignals ���õ��ź�
    void Block();
    // �ڽ��̵�ǰ�����źż��н�� SetSignals ���õ��ź�
    void UnBlock();

    // �����������źż��ָ�Ϊ Block/UnBlock ֮ǰ��״̬
    void Restore();
};

///////////////////////////////////////////////////////////////////////////////
// class CFileLock - �ļ�����
//
// ˵��:
// 1. �ļ����ǻ��ڽ��̵ģ�ͬһ�����еĻ��ⲻ�����ļ�����
// 2. �ļ����Ĳ���ʱ�������ź���(SEM)��������

class CFileLock
{
public:
    enum { 
        DEF_FILELOCK_MODE   = (O_RDWR | O_CREAT),   // �����ļ���ȱʡ Mode
        DEF_FILELOCK_RIGHTS = 0666                  // �����ļ���ȱʡ Rights
    };

protected:
    int m_nFd;          // �ļ�������
    bool m_bAutoClose;  // ��������ʱ�Ƿ�ر��ļ�
    
protected:
    bool Fcntl(int nCmd, int nType, int nOffset, int nLen, int nWhence);
    bool Flock(int nCmd);
    
public:
    CFileLock(const char *sFileName, uint nMode = DEF_FILELOCK_MODE, uint nRights = DEF_FILELOCK_RIGHTS);
    virtual ~CFileLock();
    
    //-- �Բ����ļ�����:
    // ����������(����true:�����ɹ�; false:�����Լ���)
    inline bool ReadLock(int nOffset, int nLen = 1, int nWhence = SEEK_SET)
        { return Fcntl(F_SETLK, F_RDLCK, nOffset, nLen, nWhence); }
    inline bool WriteLock(int nOffset, int nLen = 1, int nWhence = SEEK_SET)
        { return Fcntl(F_SETLK, F_WRLCK, nOffset, nLen, nWhence); } 
    // �ȴ�������(����true:�����ɹ�; false:��Ϊ�źŶ��ж�)
    inline bool ReadLockW(int nOffset, int nLen = 1, int nWhence = SEEK_SET)
        { return Fcntl(F_SETLKW, F_RDLCK, nOffset, nLen, nWhence); }    
    inline bool WriteLockW(int nOffset, int nLen = 1, int nWhence = SEEK_SET)
        { return Fcntl(F_SETLKW, F_WRLCK, nOffset, nLen, nWhence); }
    // ��������
    inline bool Unlock(int nOffset, int nLen = 1, int nWhence = SEEK_SET)
        { return Fcntl(F_SETLK, F_UNLCK, nOffset, nLen, nWhence); }

    //-- �������ļ�����:
    inline bool FileReadLock(bool bBlock = true)
        { return Flock(LOCK_SH | (bBlock? 0 : LOCK_NB)); }
    inline bool FileWriteLock(bool bBlock = true)
        { return Flock(LOCK_EX | (bBlock? 0 : LOCK_NB)); }
    inline bool FileUnLock()
        { return Flock(LOCK_UN); }
};

///////////////////////////////////////////////////////////////////////////////
// class CMultiInstChecker - ����Ķ��ʵ���ظ����м����
//
// ˵��: �����������ڱ���Ϊȫ�ֲſ�����������

class CMultiInstChecker
{
private:
    CFileLock *m_pFileLock;
    string m_strLockFile;
    bool m_bIsSelfExe;
public:
    CMultiInstChecker() : m_pFileLock(NULL) {}
    virtual ~CMultiInstChecker() { delete m_pFileLock; }

    // ���ô��������ļ���
    void SetLockFile(const char *pLockFile, bool bIsSelfExe);
    // �������Ƿ��Ѿ�����
    bool IsAlreayRunning();
};

///////////////////////////////////////////////////////////////////////////////
// class CCriticalSection - �߳��ٽ���������
//
// ˵��:
// 1. �������ڶ��̻߳������ٽ������⣬���������� Lock��Unlock �� TryLock��
// 2. ��������̻߳����� (pthread_mutex_t) ��ʵ�֣������߳���Ч���Խ��̻������ã�
// 3. �߳��ڲ�����Ƕ�׵��� Lock����Ƕ�׵������׳��쳣��
// 4. Unlock ֻ���Լ��̵߳� Lock �н������ã�����Ӱ�������̣߳�
// 5. Unlock ���԰�ȫ���ö�Σ��ʺ����� CThread::DoKill() �У�

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

    // ����
    void Lock();
    // ����
    void Unlock();
    // ���Լ��� (���Ѿ����ڼ���״̬���������� false)
    bool TryLock();
};

///////////////////////////////////////////////////////////////////////////////
// class CAutoSynchronizer - �߳��Զ�������
//
// ˵��:
// 1. ��������C++��ջ�����Զ����ٵ����ԣ��ڶ��̻߳����½��оֲ���Χ�ٽ������⣻
// 2. ʹ�÷���: ����Ҫ����ķ�Χ���Ծֲ�������ʽ���������󼴿ɣ�
// 
// ʹ�÷���:
//   �����Ѷ���: CCriticalSection m_Lock;
//   �Զ������ͽ���:
//   {
//       CAutoSynchronizer Syncher(m_Lock);
//       //...
//   }

typedef CAutoInvoker CAutoSynchronizer;

///////////////////////////////////////////////////////////////////////////////
// class CThreadSem - �߳��źŵ���

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
// class CSeqAllocator - �������кŷ�������
//
// ˵��:
// 1. �������̰߳�ȫ��ʽ����һ�����ϵ������������У��û�����ָ�����е���ʼֵ��
// 2. ����һ���������ݰ���˳��ſ��ƣ�

class CSeqAllocator
{
private:
    CCriticalSection m_Lock;
    uint m_nCurrentId;

public:
    explicit CSeqAllocator(uint nStartId = 0);

    // ����һ���·����ID
    uint AllocId();
};

///////////////////////////////////////////////////////////////////////////////
// class CThread - �߳���
//
// ˵��:
// 1. �����װ�� POSIX �̵߳���ҪAPI���û����Լ̳д��࣬��д Execute() ������
// 2. ���� POSIX ��֧���̵߳� resume/suspend ���������Ը���Ҳδ�ṩ��Ӧ���ܣ�
// 3. ��������Ķ�����̲߳�δ���������� Run() �����̵߳Ĵ�����ִ�У�
// 4. �ڴ����߳������ʱ��ǿ�ҽ����ڶ��ϴ���(new CThread)�������ջ�ϴ���������ջ�����
//    �Զ��ͷţ������̷߳Ƿ�������
// 5. ȱʡ����£��߳�ִ����ϲ����Զ��ͷ��������Ҫ�Զ��ͷţ��� SetFreeOnTerminate(True);
// 6. ���������Զ��ͷ�������򲻿ɵ��� WaitFor()�������п�������Ƿ�������
//
// �̵߳��Ȳ����Լ����ȼ�:
//
//    Linux ���̵߳����ȼ�(priority)�ǻ��ڵ��Ȳ���(policy)�ġ����������ֵ��Ȳ��ԣ�
//
//        Policy          Type         Priority
//      ----------      --------       --------
//      SCHED_OTHER     ��������ʵʱ       0
//      SCHED_RR        ʵʱ����ת��       1-99
//      SCHED_FIFO      ʵʱ�������ȳ�     1-99
//
//    ע: ���Ȳ���ȱʡΪSCHED_OTHER�������ֽ��Գ����û���Ч��

class CThread
{
friend void ThreadFinalProc(void *arg);
friend void* ThreadExecProc(void *arg);

public:
    // �̵߳��Ȳ���
    enum {
        THREAD_POLICY_DEFAULT   = SCHED_OTHER,
        THREAD_POLICY_RR        = SCHED_RR,
        THREAD_POLICY_FIFO      = SCHED_FIFO
    };

    // �߳����ȼ�
    enum {
        THREAD_PRIORITY_DEFAULT = 0,
        THREAD_PRIORITY_MIN     = 0,
        THREAD_PRIORITY_MAX     = 99,
        THREAD_PRIORITY_HIGH    = 80
    };

private:
    pthread_t m_nThreadId;          // �߳�ID
    bool m_bFinished;               // �߳��Ƿ���������̺߳�����ִ��
    int m_nTermElapsedSecs;         // �ӵ��� Terminate ����ǰ����������ʱ��(��)

protected:
    bool m_bFreeOnTerminate;        // �߳��˳�ʱ�Ƿ�ͬʱ�ͷ������
    bool m_bTerminated;             // �Ƿ�Ӧ�˳��ı�־
    int m_nReturnValue;             // �̷߳���ֵ (���� Execute �������޸Ĵ�ֵ������ WaitFor ���ش�ֵ)
    int m_nPolicy;                  // �̵߳��Ȳ��� (THREAD_POLICY_XXX)
    int m_nPriority;                // �߳����ȼ� (0..99)

private:
    void CheckThreadError(int nErrorCode);

protected:
    // �̵߳�ִ�к��������������д��
    // ע: �˺����е��쳣������һЩ��Ҫע���������Դ��ע��:
    // ( ise_classes.cpp::ThreadExecProc() ��)
    virtual void Execute() {}

    // ִ�� Terminate() ǰ�ĸ��Ӳ�����
    // ע: ���� Terminate() ������Ը�˳����ƣ�Ϊ�������߳��ܾ����˳�������
    // m_bTerminated ��־����Ϊ true ֮�⣬��ʱ��Ӧ������һЩ���ӵĲ�����
    // �������߳̾�������������н��ѳ�����
    virtual void DoTerminate() {}

    // ִ�� Kill() ǰ�ĸ��Ӳ�����
    // ע: �̱߳�ɱ�����û��������ĳЩ��Ҫ��Դ����δ�ܵõ��ͷţ���������Դ
    // (��δ���ü������㱻ɱ��)��������Ҫ��Դ���ͷŹ��������� DoKill �н��С�
    virtual void DoKill() {}
public:
    CThread();
    virtual ~CThread();

    // ������ִ���̡߳�
    // ע: �˳�Ա�����ڶ�������������ֻ�ɵ���һ�Ρ�
    void Run();

    // ֪ͨ�߳��˳� (��Ը�˳�����)
    // ע: ���߳�����ĳЩ����ʽ�����ٳٲ��˳����ɵ��� Kill() ǿ���˳���
    void Terminate();
    
    // ǿ��ɱ���߳� (ǿ���˳�����)
    // ע: ע��������� ise_classes.cpp::Kill() Դ��ע�͡�
    void Kill();
    
    // �ȴ��߳��˳�
    int WaitFor();

    // ����˯��״̬ (˯�߹����л��� m_bTerminated ��״̬)
    // ע: �˺����������߳��Լ����÷�����Ч��
    void Sleep(double fSeconds) const;

    // ���� (getter)
    int GetThreadId() const { return m_nThreadId; }
    int GetTerminated() const { return m_bTerminated; }
    int GetReturnValue() const { return m_nReturnValue; }
    bool GetFreeOnTerminate() const { return m_bFreeOnTerminate; }
    int GetPolicy() const { return m_nPolicy; }
    int GetPriority() const { return m_nPriority; }
    int GetTermElapsedSecs() const;
    // ���� (setter)
    void SetTerminated(bool bValue);
    void SetReturnValue(int nValue) { m_nReturnValue = nValue; }
    void SetFreeOnTerminate(bool bValue) { m_bFreeOnTerminate = bValue; }
    void SetPolicy(int nValue);
    void SetPriority(int nValue);
};

///////////////////////////////////////////////////////////////////////////////
// class CStream - �� ����

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
// class CMemoryStream - �ڴ�����

class CMemoryStream : public CStream
{
public:
    enum { DEFAULT_MEMORY_DELTA = 1024 };    // ȱʡ�ڴ��������� (�ֽ����������� 2 �� N �η�)
    enum { MIN_MEMORY_DELTA = 256 };         // ��С�ڴ��������� 

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
// class CFileStream - �ļ�����

// �ļ����򿪷�ʽ (uint nMode)
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
    // ȱʡ�ļ���ȡȨ��
    enum { DEFAULT_FILE_ACCESS_RIGHTS = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH };    

private:
    int m_nHandle;      // �ļ����

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
// class CList  - �б���
// 
// ˵��:
// 1. �����ʵ��ԭ���� Delphi::TList ��ȫ��ͬ��
// 2. ���б����������ŵ�:
//    a. ���з�������ȷ (STL���ޱ�ǿ�����Ի�ɬ)��
//    b. ֧���±������ȡ����Ԫ�� (STL::list��֧��)��
//    c. ֧�ֿ��ٻ�ȡ�б��� (STL::list��֧��)��
//    d. ֧��β��������ɾԪ�أ�
// 3. ���б���������ȱ��:
//    a. ��֧��ͷ�����в��Ŀ�����ɾԪ�أ�
//    b. ֻ֧�ֵ�һ����Ԫ��(Pointer����)��

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
