///////////////////////////////////////////////////////////////////////////////
// Iris Server Engine (ISE)
//
// 文件名称: ise_classes.cpp
// 功能描述: 通用基础类库
// 最后修改: 2005-04-19
///////////////////////////////////////////////////////////////////////////////

#include "ise_classes.h"
#include "ise_sysutils.h"

namespace ise
{

///////////////////////////////////////////////////////////////////////////////
// class CException

CException::CException(int nErrorNo, const char *sErrorMsg, const char *sFileName, int nLineNo)
{
    m_nErrorNo = nErrorNo;
    if (sErrorMsg)
        m_strErrorMsg = sErrorMsg;
    if (sFileName)
        m_strFileName = sFileName;
    m_nLineNo = nLineNo;
}

CException::CException(const char *sErrorMsg, const char *sFileName, int nLineNo)
{
    m_nErrorNo = EMPTY_ERROR_NO;
    if (sErrorMsg)
        m_strErrorMsg = sErrorMsg;
    if (sFileName)
        m_strFileName = sFileName;
    m_nLineNo = nLineNo;
}

CException::CException(const CException& src)
{
    m_nErrorNo = src.m_nErrorNo;
    m_strErrorMsg = src.m_strErrorMsg;
    m_strFileName = src.m_strFileName;
    m_nLineNo = src.m_nLineNo;
}

//-----------------------------------------------------------------------------
// 描述: 返回用于 Log 的字符串
//-----------------------------------------------------------------------------
string CException::MakeLogMsg() const
{
    string strText;

    if (m_nErrorNo != EMPTY_ERROR_NO)
        strText = IntToStr(m_nErrorNo) + ":";
    strText = strText + m_strErrorMsg;

    if (!m_strFileName.empty() && m_nLineNo != EMPTY_LINE_NO)
        strText = strText + " (" + m_strFileName + ":" + IntToStr(m_nLineNo) + ")";

    return strText;
}

///////////////////////////////////////////////////////////////////////////////
// class CLogger

//-----------------------------------------------------------------------------
// 描述: 将字符串写入文件
//-----------------------------------------------------------------------------
void CLogger::WriteToFile(const string& strString)
{
    if (m_strFileName.empty()) return;

    ofstream ofs(m_strFileName.c_str(), ios::out | ios::app);
    ofs << strString;
    ofs.close();
}

//-----------------------------------------------------------------------------
// 描述: 将自定义文本写入日志
//-----------------------------------------------------------------------------
void CLogger::Write(const char* sFormatString, ...)
{
    string strText;

    va_list argList;
    va_start(argList, sFormatString);
    FormatStringV(strText, sFormatString, argList);
    va_end(argList);

    strText = FormatString("[%s](%d|%u)<%s>\r\n",
        CDateTime::CurrentDateTime().DateTimeString().c_str(),
        getpid(), pthread_self(), strText.c_str());

    WriteToFile(strText);
}

//-----------------------------------------------------------------------------
// 描述: 将异常信息写入日志
//-----------------------------------------------------------------------------
void CLogger::Write(const CException& e)
{
    Write(e.MakeLogMsg().c_str());
}

///////////////////////////////////////////////////////////////////////////////
// class CBuffer

CBuffer::CBuffer()
{
    m_pBuffer = NULL;
    m_nSize = 0;
}

CBuffer::CBuffer(const CBuffer& src)
{
    m_pBuffer = NULL;
    m_nSize = 0;
    (*this) = src;
}

CBuffer::CBuffer(int nSize)
{
    m_pBuffer = NULL;
    m_nSize = 0;
    
    SetSize(nSize);
}

CBuffer::CBuffer(const void *pBuffer, int nSize)
{
    m_pBuffer = NULL;
    m_nSize = 0;
    
    Assign(pBuffer, nSize);
}

CBuffer::~CBuffer()
{ 
    if (m_pBuffer)
        free(m_pBuffer);
}

CBuffer& CBuffer::operator = (const CBuffer& rhs)
{
    SetSize(rhs.GetSize());
    if (m_nSize > 0)
        memmove(m_pBuffer, rhs.m_pBuffer, m_nSize);
    return *this;
}

//-----------------------------------------------------------------------------
// 描述: 设置缓存大小
// 备注: 新的缓存会保留原有内容
//-----------------------------------------------------------------------------
void CBuffer::SetSize(int nSize)
{
    if (nSize <= 0)
    {
        if (m_pBuffer) free(m_pBuffer);
        m_pBuffer = NULL;
        m_nSize = 0;
    }
    else if (nSize != m_nSize)
    {
        void *pNewBuf;

        // 如果 m_pBuffer == NULL，则 realloc 相当于 malloc。

        pNewBuf = realloc(m_pBuffer, nSize + 1);  // 多分配一个字节用于 c_str()!
        
        if (pNewBuf)
        {
            m_pBuffer = pNewBuf;
            m_nSize = nSize;
        }
        else
        {
            throw CException("out of memory", __FILE__, __LINE__);
        }
    }
}

//-----------------------------------------------------------------------------
// 描述: 返回 C 风格的字符串 (末端附加结束符 '\0')
//-----------------------------------------------------------------------------
char* CBuffer::c_str() const
{
    if (m_nSize <= 0 || !m_pBuffer)
        return "";
    else
    {
        ((char*)m_pBuffer)[m_nSize] = 0;
        return (char*)m_pBuffer;
    }
}

//-----------------------------------------------------------------------------
// 描述: 将 pBuffer 中的 nSize 个字节复制到 *this 中，并将大小设置为 nSize
//-----------------------------------------------------------------------------
void CBuffer::Assign(const void *pBuffer, int nSize)
{
    SetSize(nSize);
    if (m_nSize > 0)
        memmove(m_pBuffer, pBuffer, m_nSize);
}

///////////////////////////////////////////////////////////////////////////////
// class CDateTime

//-----------------------------------------------------------------------------
// 描述: 返回当前时间 (从1970-01-01 00:00:00 算起的秒数)
//-----------------------------------------------------------------------------
CDateTime CDateTime::CurrentDateTime()
{
    return CDateTime(time(NULL));
}

//-----------------------------------------------------------------------------
// 描述: 返回当前时区的当前时间戳 (从1970-01-01 00:00:00 算起的秒数)
//-----------------------------------------------------------------------------
time_t CDateTime::CurrentZoneTimeStamp()
{
    struct timeb t;
    ftime(&t);
    return t.time - t.timezone * 60;
}

//-----------------------------------------------------------------------------
// 描述: 将字符串转换成 CDateTime
// 注意: strDateTime 的格式必须为 YYYY-MM-DD HH:MM:SS
//-----------------------------------------------------------------------------
CDateTime& CDateTime::operator = (const string& strDateTime)
{
    int nYear, nMonth, nDay, nHour, nMinute, nSecond;

    if (strDateTime.length() == 19)
    {
        nYear = StrToInt(strDateTime.substr(0, 4), 0);
        nMonth = StrToInt(strDateTime.substr(5, 2), 0);
        nDay = StrToInt(strDateTime.substr(8, 2), 0);
        nHour = StrToInt(strDateTime.substr(11, 2), 0);
        nMinute = StrToInt(strDateTime.substr(14, 2), 0);
        nSecond = StrToInt(strDateTime.substr(17, 2), 0);

        EncodeDateTime(nYear, nMonth, nDay, nHour, nMinute, nSecond);
        return *this;
    }
    else
    {
        throw CException("invalid datetime string", __FILE__, __LINE__);
    }
}

//-----------------------------------------------------------------------------
// 描述: 日期时间编码，并存入 *this
//-----------------------------------------------------------------------------
void CDateTime::EncodeDateTime(int nYear, int nMonth, int nDay,
    int nHour, int nMinute, int nSecond)
{
    struct tm tm;

    tm.tm_year = nYear - 1900;
    tm.tm_mon = nMonth - 1;
    tm.tm_mday = nDay;
    tm.tm_hour = nHour;
    tm.tm_min = nMinute;
    tm.tm_sec = nSecond;

    m_tTime = mktime(&tm);
}

//-----------------------------------------------------------------------------
// 描述: 日期时间解码，并存入各参数
//-----------------------------------------------------------------------------
void CDateTime::DecodeDateTime(int& nYear, int& nMonth, int& nDay,
    int& nHour, int& nMinute, int& nSecond) const
{
    struct tm tm;

    localtime_r(&m_tTime, &tm);

    nYear = tm.tm_year + 1900;
    nMonth = tm.tm_mon + 1;
    nDay = tm.tm_mday;
    nHour = tm.tm_hour;
    nMinute = tm.tm_min;
    nSecond = tm.tm_sec;
}

//-----------------------------------------------------------------------------
// 描述: 返回日期字符串
// 参数:
//   chDateSep - 日期分隔符
// 格式:
//   YYYY-MM-DD
//-----------------------------------------------------------------------------
string CDateTime::DateString(char chDateSep) const
{
    char sDate[11];
    int nYear, nMonth, nDay, nHour, nMinute, nSecond;

    DecodeDateTime(nYear, nMonth, nDay, nHour, nMinute, nSecond);
    sprintf(sDate, "%04d%c%02d%c%02d", nYear, chDateSep, nMonth, chDateSep, nDay);

    return string(sDate);
}
    
//-----------------------------------------------------------------------------
// 描述: 返回日期时间字符串
// 参数:
//   chDateSep - 日期分隔符
//   chTimeSep - 时间分隔符
// 格式:
//   YYYY-MM-DD HH:MM:SS
//-----------------------------------------------------------------------------
string CDateTime::DateTimeString(char chDateSep, char chTimeSep) const
{
    char sDateTime[20];
    int nYear, nMonth, nDay, nHour, nMinute, nSecond;

    DecodeDateTime(nYear, nMonth, nDay, nHour, nMinute, nSecond);
    sprintf(sDateTime, "%04d%c%02d%c%02d %02d%c%02d%c%02d", 
        nYear, chDateSep, nMonth, chDateSep, nDay,
        nHour, chTimeSep, nMinute, chTimeSep, nSecond);

    return string(sDateTime);
}

///////////////////////////////////////////////////////////////////////////////
// class CSem

//-----------------------------------------------------------------------------
// 描述: 构造函数
// 参数:
//   nSemKey    - 信号量组在系统中的唯一标识符
//   nSemCount  - 信号量组中信号量的个数
//-----------------------------------------------------------------------------
CSem::CSem(key_t nSemKey, int nSemCount)
    : m_nSemKey(nSemKey), m_nSemCount(nSemCount)
{
}

CSem::~CSem()
{
}

//-----------------------------------------------------------------------------
// 描述: 创建信号量组
//-----------------------------------------------------------------------------
void CSem::GetSem(int nMode)
{
    if ((m_nSemId = semget(m_nSemKey, m_nSemCount, IPC_CREAT | IPC_EXCL | nMode)) < 0)
    {
        if (errno != EEXIST)
            throw CException(strerror(errno), __FILE__, __LINE__);

        if ((m_nSemId = semget(m_nSemKey, 0, nMode)) < 0)
            throw CException(strerror(errno), __FILE__, __LINE__);
        
        m_bCreate = false;
        return;
    }

    m_bCreate = true;
}

//-----------------------------------------------------------------------------
// 描述: 注销信号量组
//-----------------------------------------------------------------------------
void CSem::RemoveSem()
{
    if (m_bCreate)
    {
        union semun unSem;
        if (semctl(m_nSemId, 0, IPC_RMID, unSem) < 0)
            throw CException(strerror(errno), __FILE__, __LINE__);
        m_bCreate = false;
    }
}

//-----------------------------------------------------------------------------
// 描述: 对第 nSemNum(0-based) 个信号量进行P操作
//-----------------------------------------------------------------------------
bool CSem::Lock(int nSemNum)
{
    struct sembuf SemBuf[1];
    
    SemBuf[0].sem_num = nSemNum;
    SemBuf[0].sem_op = -1;
    SemBuf[0].sem_flg = SEM_UNDO;
    
    if (semop(m_nSemId, SemBuf, 1) < 0)
    {
        if (errno == EAGAIN || errno == EINTR) 
            return false;
        else
            throw CException(strerror(errno), __FILE__, __LINE__);   
    }
    
    return true;
}

//-----------------------------------------------------------------------------
// 描述: 对第 nSemNum(0-based) 个信号量进行V操作
//-----------------------------------------------------------------------------
void CSem::Unlock(int nSemNum)
{
    struct sembuf SemBuf[1];
    
    SemBuf[0].sem_num = nSemNum;
    SemBuf[0].sem_op = 1;
    SemBuf[0].sem_flg = SEM_UNDO;
    
    if (semop(m_nSemId, SemBuf, 1) < 0)
    {
        throw CException(strerror(errno), __FILE__, __LINE__);   
    }
}

//-----------------------------------------------------------------------------
// 描述: 对所有信号量进行赋值
//-----------------------------------------------------------------------------
void CSem::SetAll(int nSemCount, ...)
{
    if (nSemCount != m_nSemCount)
        throw CException("argument error", __FILE__, __LINE__);
    
    unsigned short int *pValue = new unsigned short int [nSemCount];
    
    va_list ap;
    va_start(ap, nSemCount);
    for (int i = 0; i < nSemCount; i++)
        pValue[i] = (unsigned short int)va_arg(ap, int);
    va_end(ap);
    
    semun unSem;
    unSem.array = pValue;
    if (semctl(m_nSemId, 0, SETALL, unSem) < 0)
    {
        delete[] pValue;
        throw CException(strerror(errno), __FILE__, __LINE__);
    }
    
    delete[] pValue;
}

//-----------------------------------------------------------------------------
// 描述: 对第 nSemNum(0-based) 个信号量进行赋值
//-----------------------------------------------------------------------------
void CSem::SetValue(int nSemNum, int nValue)
{
    semun unSem;
    unSem.val = nValue;
    if (semctl(m_nSemId, nSemNum, SETVAL, unSem) < 0)
        throw CException(strerror(errno), __FILE__, __LINE__);
}

//-----------------------------------------------------------------------------
// 描述: 取所有信号量的值
//-----------------------------------------------------------------------------
void CSem::GetAll(unsigned short int *pArray)
{
    semun unSem;
    unSem.array = pArray;
    if (semctl(m_nSemId, 0, GETALL, unSem) < 0)
        throw CException(strerror(errno), __FILE__, __LINE__);   
}

//-----------------------------------------------------------------------------
// 描述: 取第 nSemNum(0-based) 个信号量的值
//-----------------------------------------------------------------------------
int CSem::GetValue(int nSemNum)
{
    semun unSem;
    int nValue = semctl(m_nSemId, nSemNum, GETVAL, unSem);
    if (nValue < 0)
        throw CException(strerror(errno), __FILE__, __LINE__);   
    
    return nValue;
}

//-----------------------------------------------------------------------------
// 描述: 初始化所有信号量的值为1
//-----------------------------------------------------------------------------
void CSem::InitSem()
{
    unsigned short int *pValue = new unsigned short int [m_nSemCount];
    
    for (int i = 0; i < m_nSemCount; i++)
        pValue[i] = (unsigned short int)SEM_INIT_VALUE;

    semun unSem;
    unSem.array = pValue;
    if (semctl(m_nSemId, 0, SETALL, unSem) < 0)
    {
        delete[] pValue;
        throw CException(strerror(errno), __FILE__, __LINE__);
    }
    
    delete[] pValue;
}

///////////////////////////////////////////////////////////////////////////////
// class CShm

//-----------------------------------------------------------------------------
// 描述: 构造函数
// 参数:
//   nShmKey    - 共享内存在系统中的唯一标识符
//   nShmSize   - 共享内存大小(字节数)
//-----------------------------------------------------------------------------
CShm::CShm(key_t nShmKey, int nShmSize)
    : m_nShmKey(nShmKey), m_nShmSize(nShmSize), m_pShmData(NULL), m_bCreate(false)
{
}

CShm::~CShm()
{
}

//-----------------------------------------------------------------------------
// 描述: 获取共享内存
// 备注: 若指定KEY的共享内存已存在，则利用已有共享内存。
//-----------------------------------------------------------------------------
void CShm::GetShm(int nMode)
{
    if ((m_nShmId = shmget(m_nShmKey, m_nShmSize, IPC_CREAT | IPC_EXCL | nMode)) < 0)   //try to create
    {
        if (errno != EEXIST)
            throw CException(strerror(errno), __FILE__, __LINE__);

        if ((m_nShmId = shmget(m_nShmKey, m_nShmSize, nMode)) < 0)
            throw CException(strerror(errno), __FILE__, __LINE__);

        m_bCreate = false;
        return;
    }

    m_bCreate = true;
}

//-----------------------------------------------------------------------------
// 描述: 重试创建共享内存。若指定的KEY的共享内存已存在，则递增KEY，重试创建。
// 参数:
//   nRetryTimes    - 重试多少遍
//   nMode          - 共享内存的MODE
//-----------------------------------------------------------------------------
void CShm::TryCreateShm(int nRetryTimes, int nMode)
{
    bool bResult = false;
    int nShmId, nShmKey;
    
    // 起始KEY为 m_nShmKey
    nShmKey = m_nShmKey;

    for (int i = 0; i < nRetryTimes; i++)
    {
        nShmId = shmget(nShmKey, m_nShmSize, IPC_CREAT | IPC_EXCL | nMode);
        if (nShmId < 0)  // 错误
        {
            // 如果此共享内存已存在
            if (errno == EEXIST)
                nShmKey++;
        }
        else
        {
            // 创建成功
            m_nShmKey = nShmKey;
            m_nShmId = nShmId;
            m_bCreate = true;
            m_pShmData = NULL;

            bResult = true;
            break;
        }
    }

    if (!bResult)
        throw CException("Cannot create share memory", __FILE__, __LINE__);
}

//-----------------------------------------------------------------------------
// 描述: 连接共享内存
//-----------------------------------------------------------------------------
void CShm::AttachShm()
{
    if ((m_pShmData = (char*)shmat(m_nShmId, NULL, 0)) < 0)
        throw CException(strerror(errno), __FILE__, __LINE__);
}

//-----------------------------------------------------------------------------
// 描述: 分离共享内存
//-----------------------------------------------------------------------------
void CShm::DetachShm()
{
    if (shmdt(m_pShmData) < 0)
        throw CException(strerror(errno), __FILE__, __LINE__);
    m_pShmData = NULL;
}

//-----------------------------------------------------------------------------
// 描述: 释放共享内存
//-----------------------------------------------------------------------------
void CShm::RemoveShm()
{
    if (shmctl(m_nShmId, IPC_RMID, NULL) < 0)
        throw CException(strerror(errno), __FILE__, __LINE__);
    m_pShmData = NULL;
    m_bCreate = false;
}

//-----------------------------------------------------------------------------
// 描述: 初始化共享内存内容为0
//-----------------------------------------------------------------------------
void CShm::InitShm()
{
    if (m_pShmData)
    {
        memset(m_pShmData, 0, m_nShmSize);
    }
}

//-----------------------------------------------------------------------------
// 描述: 判断共享内存是否存在
//-----------------------------------------------------------------------------
bool CShm::ShmExists(key_t nShmKey)
{
    return (shmget(nShmKey, 0, 0) >= 0);
}

///////////////////////////////////////////////////////////////////////////////
// class CSignalMasker

CSignalMasker::CSignalMasker(bool bAutoRestore) : 
    m_bBlock(false),
    m_bAutoRestore(bAutoRestore)
{
    sigemptyset(&m_OldSet);
    sigemptyset(&m_NewSet);
}

CSignalMasker::~CSignalMasker()
{
    if (m_bAutoRestore) Restore();
}

int CSignalMasker::SigProcMask(int nHow, const sigset_t* pNewSet, sigset_t* pOldSet)
{
    int nRet;
    if ((nRet = sigprocmask(nHow, pNewSet, pOldSet)) < 0)
        throw CException(strerror(errno), __FILE__, __LINE__);
        
    return nRet;
}

//-----------------------------------------------------------------------------
// 描述: 设置 Block/UnBlock 操作所需的信号集合
//-----------------------------------------------------------------------------
void CSignalMasker::SetSignals(int nSigCount, va_list argList)
{
    sigemptyset(&m_NewSet);
    for (int i = 0; i < nSigCount; i++)
        sigaddset(&m_NewSet, va_arg(argList, int));
}

void CSignalMasker::SetSignals(int nSigCount, ...)
{
    va_list argList;
    va_start(argList, nSigCount);
    SetSignals(nSigCount, argList);
    va_end(argList);
}

//-----------------------------------------------------------------------------
// 描述: 在进程当前阻塞信号集中添加 SetSignals 设置的信号
//-----------------------------------------------------------------------------
void CSignalMasker::Block()
{
    SigProcMask(SIG_BLOCK, &m_NewSet, &m_OldSet); 
    m_bBlock = true;
}

//-----------------------------------------------------------------------------
// 描述: 在进程当前阻塞信号集中解除 SetSignals 设置的信号
//-----------------------------------------------------------------------------
void CSignalMasker::UnBlock()
{
    SigProcMask(SIG_UNBLOCK, &m_NewSet, &m_OldSet);
    m_bBlock = true;
}

//-----------------------------------------------------------------------------
// 描述: 将进程阻塞信号集恢复为 Block/UnBlock 之前的状态
//-----------------------------------------------------------------------------
void CSignalMasker::Restore()
{
    if (m_bBlock)
    {
        SigProcMask(SIG_SETMASK, &m_OldSet, NULL); 
        m_bBlock = false;
    }
}

///////////////////////////////////////////////////////////////////////////////
// class CFileLock

//-----------------------------------------------------------------------------
// 描述: 构造函数
// 参数:
//   sFileName - 文件名(可以包含路径)
//   nMode     - 文件打开方式
//   nRights   - 文件权限
//-----------------------------------------------------------------------------
CFileLock::CFileLock(const char *sFileName, uint nMode, uint nRights)
{
    ForceDirectories(ExtractFilePath(sFileName));
    umask(0);  // 防止 nMode 被 umask值 遮掩

    if ((m_nFd = open(sFileName, nMode, nRights)) < 0)
        throw CException(strerror(errno), __FILE__, __LINE__);

    m_bAutoClose = true;
}

CFileLock::~CFileLock()
{
    if (m_bAutoClose) close(m_nFd);
}

//-----------------------------------------------------------------------------
// 描述: 对部分文件加锁
// 返回:
//   true   - 成功
//   false  - 失败:
//     若 nCmd 为 F_SETLKW，则是因为信号而中断；
//     若 nCmd 为 F_SETLK，则表示其它进程未解锁
//-----------------------------------------------------------------------------
bool CFileLock::Fcntl(int nCmd, int nType, int nOffset, int nLen, int nWhence)
{
    struct flock flk;

    flk.l_type = nType;
    flk.l_start = nOffset;
    flk.l_whence = nWhence;
    flk.l_len = nLen;

    if (fcntl(m_nFd, nCmd, &flk) < 0)
    {
        if (errno == EINTR || errno == EAGAIN || errno == EACCES) return false;
        throw CException(strerror(errno), __FILE__, __LINE__);
    }
    
    return true;
}

//-----------------------------------------------------------------------------
// 描述: 对全部文件加锁
//-----------------------------------------------------------------------------
bool CFileLock::Flock(int nCmd)
{
    return flock(m_nFd, nCmd) == 0;
}

///////////////////////////////////////////////////////////////////////////////
// class CMultiInstChecker

//-----------------------------------------------------------------------------
// 描述: 设置待加锁的文件名
// 参数:
//   pLockFile  - 锁文件名(含路径)
//   bIsSelfExe - 锁文件是否自身EXE
//-----------------------------------------------------------------------------
void CMultiInstChecker::SetLockFile(const char *pLockFile, bool bIsSelfExe)
{
    m_strLockFile = pLockFile;
    m_bIsSelfExe = bIsSelfExe;
}

//-----------------------------------------------------------------------------
// 描述: 检查程序是否已经运行
//-----------------------------------------------------------------------------
bool CMultiInstChecker::IsAlreayRunning()
{
    if (!m_pFileLock)
    {
        uint nMode = (m_bIsSelfExe? O_RDONLY : CFileLock::DEF_FILELOCK_MODE);
        m_pFileLock = new CFileLock(m_strLockFile.c_str(), nMode);
    }

    return !m_pFileLock->FileWriteLock(false);
}

///////////////////////////////////////////////////////////////////////////////
// class CCriticalSection

CCriticalSection::CCriticalSection()
{
    pthread_mutexattr_t attr;

    // 锁属性说明:
    // PTHREAD_MUTEX_TIMED_NP:
    //   普通锁。同一线程内必须成对调用 Lock 和 Unlock。不可连续调用多次 Lock，否则会死锁。
    // PTHREAD_MUTEX_RECURSIVE_NP:
    //   嵌套锁。线程内可以嵌套调用 Lock，第一次生效，之后必须调用相同次数的 Unlock 方可解锁。
    // PTHREAD_MUTEX_ERRORCHECK_NP:
    //   检错锁。如果同一线程嵌套调用 Lock 则产生错误。
    // PTHREAD_MUTEX_ADAPTIVE_NP:
    //   适应锁。
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK_NP);
    pthread_mutex_init(&m_Mutex, &attr);
    pthread_mutexattr_destroy(&attr);
}

CCriticalSection::~CCriticalSection()
{
    // 如果在未解锁的情况下 destroy，此函数会返回错误 EBUSY。
    // 在 linux 下，即使此函数返回错误，也不会有资源泄漏。
    pthread_mutex_destroy(&m_Mutex);
}

//-----------------------------------------------------------------------------
// 描述: 加锁
//-----------------------------------------------------------------------------
void CCriticalSection::Lock()
{
    int r = pthread_mutex_lock(&m_Mutex);

    // 当此锁已被自己的线程锁住 (只有检错锁才返回此错误)
    if (r == EDEADLK)
        throw CException("lock recursived.", __FILE__, __LINE__);
}

//-----------------------------------------------------------------------------
// 描述: 解锁
//-----------------------------------------------------------------------------
void CCriticalSection::Unlock()
{
    int r = pthread_mutex_unlock(&m_Mutex);

    // 当前线程未拥有此锁 (只有检错锁才返回此错误)
    if (r == EPERM)
        throw CException("the calling thread does not own the mutex.", __FILE__, __LINE__);
}

//-----------------------------------------------------------------------------
// 描述: 尝试加锁 (若已经处于加锁状态则立即返回)
// 返回:
//   true   - 加锁成功
//   false  - 失败，此锁已经处于加锁状态
//-----------------------------------------------------------------------------
bool CCriticalSection::TryLock()
{
    return pthread_mutex_trylock(&m_Mutex) != EBUSY;
}

///////////////////////////////////////////////////////////////////////////////
// class CThreadSem

CThreadSem::CThreadSem(int nInitValue)
{
    m_nInitValue = nInitValue;
    int r = sem_init(&m_Sem, 0, nInitValue);
    if (r != 0)
        throw CException("sem_init error", __FILE__, __LINE__);
}

CThreadSem::~CThreadSem()
{
    sem_destroy(&m_Sem);
}

//-----------------------------------------------------------------------------
// 描述: 点灯操作将信号灯值原子地加1，表示增加一个可访问的资源
//-----------------------------------------------------------------------------
void CThreadSem::Increase()
{
    sem_post(&m_Sem);
}

//-----------------------------------------------------------------------------
// 描述: 等待灯亮 (信号灯值大于0)，然后将信号灯原子地减1
//-----------------------------------------------------------------------------
void CThreadSem::Wait()
{
    sem_wait(&m_Sem);
}

//-----------------------------------------------------------------------------
// 描述: 尝试等待 (如果信号灯计数大于0，则原子地减1并返回true，否则立即返回false)
//-----------------------------------------------------------------------------
bool CThreadSem::TryWait()
{
    return (sem_trywait(&m_Sem) == 0);
}

//-----------------------------------------------------------------------------
// 描述: 取得信号灯的当前计数
//-----------------------------------------------------------------------------
int CThreadSem::GetValue()
{
    int nValue;
    sem_getvalue(&m_Sem, &nValue);
    return nValue;
}

//-----------------------------------------------------------------------------
// 描述: 将信号灯的计数值设为初始状态
//-----------------------------------------------------------------------------
void CThreadSem::Reset()
{
    sem_destroy(&m_Sem);
    sem_init(&m_Sem, 0, m_nInitValue);
}

///////////////////////////////////////////////////////////////////////////////
// class CSeqAllocator

//-----------------------------------------------------------------------------
// 描述: 构造函数
// 参数:
//   nStartId - 起始序列号
//-----------------------------------------------------------------------------
CSeqAllocator::CSeqAllocator(uint nStartId)
{
    m_nCurrentId = nStartId;
}

//-----------------------------------------------------------------------------
// 描述: 返回一个新分配的ID
//-----------------------------------------------------------------------------
uint CSeqAllocator::AllocId()
{
    CAutoSynchronizer Syncher(m_Lock);
    return m_nCurrentId++;
}

///////////////////////////////////////////////////////////////////////////////
// class CThread

//-----------------------------------------------------------------------------
// 描述: 线程收到 cancel 信号后的善后处理函数
// 参数:
//   arg - 指向 CThread 对象
//-----------------------------------------------------------------------------
void ThreadFinalProc(void *arg)
{
    CThread *pThread = (CThread*)arg;

    pThread->m_bFinished = true;
    pThread->m_nThreadId = 0;
    if (pThread->m_bFreeOnTerminate) delete pThread;

    // 注: 线程被 cancel 信号终止后，会自动执行 pthread_exit。
    // 此处切不可调用 pthread_exit，否则会导致 ThreadFinalProc 函数的无限次执行！ 
}

//-----------------------------------------------------------------------------
// 描述: 线程执行函数
// 参数:
//   arg - 线程参数，此处指向 CThread 对象
//-----------------------------------------------------------------------------
void* ThreadExecProc(void *arg)
{
    CThread *pThread = (CThread*)arg;
    int nReturnValue = 0;

    // 如果在 [push..pop] 区间线程被强行终止(收到 cancel 信号)或抛出异常(throw XXX)，
    // 保证 ThreadFinalProc 函数一定被执行。
    pthread_cleanup_push(ThreadFinalProc, pThread);
    // 线程对 cancel 信号的响应方式有三种: (1)不响应 (2)推迟到取消点再响应 (3)尽量立即响应。
    // 此处设置线程为第(3)种方式，即可马上被 cancel 信号终止。
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

    if (!pThread->m_bTerminated)
    {
        // 线程在执行过程中若收到 cancel 信号，会抛出一个异常(究竟是什么类型的异常？)，
        // 此异常千万不可去阻拦它( try{}catch(...){} )，系统能侦测出此异常是否被彻底阻拦，
        // 若是，则会发出 SIGABRT 信号，并在终端输出 "FATAL: exception not rethrown"。
        // 所以此处的策略是只阻拦 CException 异常(在ISE中所有异常皆从 CException 继承)。
        // 在 pThread->Execute() 的执行过程中，用户应该注意如下事项:
        // 1. 请参照此处的做法去拦截异常，而切不可阻拦所有类型的异常( 即catch(...) );
        // 2. 不可抛出 CException 及其子类之外的异常。假如抛出一个整数( 如 throw 5; )，
        //    系统会因为没有此异常的处理程序而调用 abort。(尽管如此，ThreadFinalProc
        //    仍会象 pthread_cleanup_push 所承诺的那样被执行到。)
        try { pThread->Execute(); } catch (CException& e) { ; }

        // 记录下线程返回值
        nReturnValue = pThread->m_nReturnValue;
    }

    // 屏蔽 cancel 信号，出了 [push..pop] 区间后将不可被强行终止 
    pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
    // 离开保护区域的同时，执行 ThreadFinalProc 函数 (pop的参数为1表示执行函数)
    pthread_cleanup_pop(1);

    pthread_exit((void*)nReturnValue);
    return NULL;
}

//-----------------------------------------------------------------------------
// 描述: 构造函数
//-----------------------------------------------------------------------------
CThread::CThread() :
    m_nThreadId(0),
    m_bFinished(false),
    m_nTermElapsedSecs(0),
    m_bFreeOnTerminate(false),
    m_bTerminated(false),
    m_nReturnValue(0),
    m_nPolicy(THREAD_POLICY_DEFAULT),
    m_nPriority(THREAD_PRIORITY_DEFAULT)
{
}

//-----------------------------------------------------------------------------
// 描述: 析构函数
//-----------------------------------------------------------------------------
CThread::~CThread()
{
    if (m_nThreadId != 0 && !m_bFinished)
    {
        Terminate();
        WaitFor();
    }

    if (m_nThreadId != 0)
        pthread_detach(m_nThreadId);
}

//-----------------------------------------------------------------------------
// 描述: 线程错误处理
//-----------------------------------------------------------------------------
void CThread::CheckThreadError(int nErrorCode)
{
    if (nErrorCode != 0)
        throw CException(SysErrorMessage(nErrorCode).c_str(), __FILE__, __LINE__);
}

//-----------------------------------------------------------------------------
// 描述: 创建线程并执行
// 注意: 此成员方法在对象声明周期中只可调用一次。
//-----------------------------------------------------------------------------
void CThread::Run()
{
    if (m_nThreadId != 0)
        throw CException("thread already running", __FILE__, __LINE__);
    
    // 创建线程
    int nErrCode;
    nErrCode = pthread_create(&m_nThreadId, NULL, ThreadExecProc, (void*)this);
    if (nErrCode != 0)
        throw CException(SysErrorMessage(nErrCode).c_str(), __FILE__, __LINE__);

    // 设置线程调度策略
    if (m_nPolicy != THREAD_POLICY_DEFAULT)
        SetPolicy(m_nPolicy);
    // 设置线程优先级
    if (m_nPriority != THREAD_PRIORITY_DEFAULT)
        SetPriority(m_nPriority);
}

//-----------------------------------------------------------------------------
// 描述: 通知线程退出
//-----------------------------------------------------------------------------
void CThread::Terminate()
{
    try { DoTerminate(); } catch (CException& e) {}

    if (!m_bTerminated) m_nTermElapsedSecs = time(NULL);
    m_bTerminated = true;
}

//-----------------------------------------------------------------------------
// 描述: 强行杀死线程
// 注意: 
//   1. 调用此函数后，对线程类对象的一切操作皆不可用(Terminate(); WaitFor(); delete pThread; 等)。
//   2. 在杀死线程前，m_bFreeOnTerminate 会自动设为 true，以便对象能自动释放。
//   3. 线程被杀死后，用户所管理的某些重要资源可能未能得到释放，比如锁资源 (还未来得及解锁
//      便被杀了)，所以重要资源的释放工作必须在 DoKill 中进行。
//   4. 线程在收到 cancel 信号后，在销毁自己时，本质上是抛出一个异常，让异常来干扰线程函数
//      的执行流程。所以，线程函数中的所有栈对象依然按照正常的顺序析构。
//-----------------------------------------------------------------------------
void CThread::Kill()
{
    if (m_nThreadId != 0)
    {
        // 用户须在此处释放重要资源，比如释放锁资源
        try { DoKill(); } catch (CException& e) {}

        pthread_t nThreadId = m_nThreadId;
        m_nThreadId = 0;

        SetFreeOnTerminate(true);
        pthread_cancel(nThreadId);
    }
}

//-----------------------------------------------------------------------------
// 描述: 等待线程退出
// 返回: 线程返回值
//-----------------------------------------------------------------------------
int CThread::WaitFor()
{
    pthread_t nThreadId = m_nThreadId;
    int nReturnValue;

    m_nThreadId = 0;
    CheckThreadError(pthread_join(nThreadId, (void**)&nReturnValue));
    return nReturnValue;
}

//-----------------------------------------------------------------------------
// 描述: 进入睡眠状态 (睡眠过程中会检测 m_bTerminated 的状态)
// 参数:
//   fSeconds - 睡眠的秒数，可为小数，可精确到毫秒
// 注意:
//   由于将睡眠时间分成了若干份，每次睡眠时间的小误差累加起来将扩大总误差。
//-----------------------------------------------------------------------------
void CThread::Sleep(double fSeconds) const
{
    const double SLEEP_INTERVAL = 0.5;      // 每次睡眠的时间(秒)
    const double ADJUST_SECONDS = -0.01;    // 用来纠正误差 (注: 若修改了 SLEEP_INTERVAL，此值也须调整)

    double fOnceSecs;

    while (!GetTerminated() && fSeconds > 0)
    {
        fOnceSecs = (fSeconds >= SLEEP_INTERVAL ? SLEEP_INTERVAL : fSeconds);
        fSeconds -= fOnceSecs;

        NanoSleep(fOnceSecs + ADJUST_SECONDS, true);
    }
}

//-----------------------------------------------------------------------------
// 描述: 取得从调用 Terminate 到当前共经过多少时间(秒)
//-----------------------------------------------------------------------------
int CThread::GetTermElapsedSecs() const
{
    int nResult = 0;

    // 如果已经通知退出，但线程还活着
    if (m_bTerminated && m_nThreadId != 0)
    {
        nResult = time(NULL) - m_nTermElapsedSecs;
    }

    return nResult;
}

//-----------------------------------------------------------------------------
// 描述: 设置是否 Terminate
//-----------------------------------------------------------------------------
void CThread::SetTerminated(bool bValue)
{
    if (bValue != m_bTerminated)
    {
        if (bValue)
            Terminate();
        else
        {
            m_bTerminated = false;
            m_nTermElapsedSecs = 0;
        }
    }
}

//-----------------------------------------------------------------------------
// 描述: 设置线程的调度策略
//-----------------------------------------------------------------------------
void CThread::SetPolicy(int nValue)
{
    if (nValue != THREAD_POLICY_DEFAULT &&
        nValue != THREAD_POLICY_RR &&
        nValue != THREAD_POLICY_FIFO)
    {
        nValue = THREAD_POLICY_DEFAULT;
    }

    m_nPolicy = nValue;

    if (m_nThreadId != 0)
    {
        struct sched_param param;
        param.sched_priority = m_nPriority;
        pthread_setschedparam(m_nThreadId, m_nPolicy, &param);
    }
}

//-----------------------------------------------------------------------------
// 描述: 设置线程的优先级
//-----------------------------------------------------------------------------
void CThread::SetPriority(int nValue)
{
    if (nValue < THREAD_PRIORITY_MIN || nValue > THREAD_PRIORITY_MAX)
        nValue = THREAD_PRIORITY_DEFAULT;

    m_nPriority = nValue;

    if (m_nThreadId != 0)
    {
        struct sched_param param;
        param.sched_priority = m_nPriority;
        pthread_setschedparam(m_nThreadId, m_nPolicy, &param);
    }
}

///////////////////////////////////////////////////////////////////////////////
// class CStream

uint64 CStream::GetSize()
{
    uint64 nPos, nResult;

    nPos = Seek(0, SO_CURRENT);
    nResult = Seek(0, SO_END);
    Seek(nPos, SO_BEGINNING);

    return nResult;
}

///////////////////////////////////////////////////////////////////////////////
// class CMemoryStream

//-----------------------------------------------------------------------------
// 描述: 构造函数
// 参数:
//   nMemoryDelta - 内存增长步长 (字节数，必须是 2 的 N 次方)
//-----------------------------------------------------------------------------
CMemoryStream::CMemoryStream(uint64 nMemoryDelta) :
    m_pMemory(NULL),
    m_nCapacity(0),
    m_nSize(0),
    m_nPosition(0)
{
    SetMemoryDelta(nMemoryDelta);
}

CMemoryStream::~CMemoryStream()
{
    Clear();
}

void CMemoryStream::SetMemoryDelta(uint64 nNewMemoryDelta)
{
    if (nNewMemoryDelta != DEFAULT_MEMORY_DELTA)
    {
        if (nNewMemoryDelta < MIN_MEMORY_DELTA)
            nNewMemoryDelta = MIN_MEMORY_DELTA;

        // 保证 nNewMemoryDelta 是2的N次方
        for (int i = sizeof(uint64) * 8 - 1; i >= 0; i--)
            if (((1 << i) & nNewMemoryDelta) != 0)
            {
                nNewMemoryDelta &= (1 << i);
                break;
            }
    }

    m_nMemoryDelta = nNewMemoryDelta;
}

void CMemoryStream::SetPointer(char* pMemory, uint64 nSize)
{
    m_pMemory = pMemory;
    m_nSize = nSize;
}

void CMemoryStream::SetCapacity(uint64 nNewCapacity)
{
    SetPointer(Realloc(nNewCapacity), m_nSize);
    m_nCapacity = nNewCapacity;
}

char* CMemoryStream::Realloc(uint64& nNewCapacity)
{
    char* pResult;

    if (nNewCapacity > 0 && nNewCapacity != m_nSize)
        nNewCapacity = (nNewCapacity + (m_nMemoryDelta - 1)) & ~(m_nMemoryDelta - 1);
    
    pResult = m_pMemory;
    if (nNewCapacity != m_nCapacity)
    {
        if (nNewCapacity == 0)
        {
            free(m_pMemory);
            pResult = NULL;
        }
        else
        {
            if (m_nCapacity == 0)
                pResult = (char*)malloc(nNewCapacity);
            else
                pResult = (char*)realloc(m_pMemory, nNewCapacity);

            if (!pResult)
                throw CException("out of memory while expanding memory stream", __FILE__, __LINE__);
        }
    }

    return pResult;
}

//-----------------------------------------------------------------------------
// 描述: 读内存流
//-----------------------------------------------------------------------------
int CMemoryStream::Read(void *pBuffer, int nCount)
{
    int nResult = 0;

    if (m_nPosition >= 0 && nCount >= 0)
    {
        nResult = m_nSize - m_nPosition;
        if (nResult > 0)
        {
            if (nResult > nCount) nResult = nCount;
            memmove(pBuffer, m_pMemory + m_nPosition, nResult);
            m_nPosition += nResult;
        }
    }

    return nResult;
}

//-----------------------------------------------------------------------------
// 描述: 写内存流
//-----------------------------------------------------------------------------
int CMemoryStream::Write(const void *pBuffer, int nCount)
{
    int nResult = 0;
    uint64 nPos;

    if (m_nPosition >= 0 && nCount >= 0)
    {
        nPos = m_nPosition + nCount;
        if (nPos > 0)
        {
            if (nPos > m_nSize)
            {
                if (nPos > m_nCapacity)
                    SetCapacity(nPos);
                m_nSize = nPos;
            }
            memmove(m_pMemory + m_nPosition, pBuffer, nCount);
            m_nPosition = nPos;
            nResult = nCount;
        }
    }

    return nResult;
}

//-----------------------------------------------------------------------------
// 描述: 内存流指针定位
//-----------------------------------------------------------------------------
uint64 CMemoryStream::Seek(uint64 nOffset, SeekOrigin nSeekOrigin)
{
    switch (nSeekOrigin)
    {
    case SO_BEGINNING:
        m_nPosition = nOffset;
        break;
    case SO_CURRENT:
        m_nPosition += nOffset;
        break;
    case SO_END:
        m_nPosition = m_nSize + nOffset;
        break;
    }

    return m_nPosition;
}

//-----------------------------------------------------------------------------
// 描述: 设置内存流大小
//-----------------------------------------------------------------------------
void CMemoryStream::SetSize(uint64 nSize)
{
    uint64 nOldPos = m_nPosition;

    SetCapacity(nSize);
    m_nSize = nSize;
    if (nOldPos > nSize) Seek(0, SO_END);
}

//-----------------------------------------------------------------------------
// 描述: 将其它流读入到内存流中
//-----------------------------------------------------------------------------
void CMemoryStream::LoadFromStream(CStream& Stream)
{
    uint64 nCount;

    Stream.SetPosition(0);
    nCount = Stream.GetSize();
    SetSize(nCount);
    if (nCount != 0)
        Stream.Read(m_pMemory, nCount);
}

//-----------------------------------------------------------------------------
// 描述: 将文件读入到内存流中
//-----------------------------------------------------------------------------
void CMemoryStream::LoadFromFile(const string& strFileName)
{
    CFileStream FileStream(strFileName, FM_OPEN_READ | FM_SHARE_DENY_WRITE);
    LoadFromStream(FileStream);
}

//-----------------------------------------------------------------------------
// 描述: 将内存流保存到其它流中
//-----------------------------------------------------------------------------
void CMemoryStream::SaveToStream(CStream& Stream)
{
    if (m_nSize != 0)
        Stream.Write(m_pMemory, m_nSize);
}

//-----------------------------------------------------------------------------
// 描述: 将内存流保存到文件中
//-----------------------------------------------------------------------------
void CMemoryStream::SaveToFile(const string& strFileName)
{
    CFileStream FileStream(strFileName, FM_CREATE);
    SaveToStream(FileStream);
}

//-----------------------------------------------------------------------------
// 描述: 清空内存流
//-----------------------------------------------------------------------------
void CMemoryStream::Clear()
{
    SetCapacity(0);
    m_nSize = 0;
    m_nPosition = 0;
}

///////////////////////////////////////////////////////////////////////////////
// class CFileStream

//-----------------------------------------------------------------------------
// 描述: 构造函数
// 参数:
//   strFileName - 文件名
//   nMode       - 文件流打开方式
//   nRights     - 文件存取权限
//-----------------------------------------------------------------------------
CFileStream::CFileStream(const string& strFileName, uint nMode, uint nRights)
{
    if (nMode == FM_CREATE)
    {
        m_nHandle = FileCreate(strFileName, nRights);
        if (m_nHandle < 0)
        {
            throw CException(FormatString("cannot create file '%s'. %s", 
                strFileName.c_str(), SysErrorMessage(errno).c_str()).c_str(), __FILE__, __LINE__);
        }
    }
    else
    {
        m_nHandle = FileOpen(strFileName, nMode);
        if (m_nHandle < 0)
        {
            throw CException(FormatString("cannot open file '%s'. %s", 
                strFileName.c_str(), SysErrorMessage(errno).c_str()).c_str(), __FILE__, __LINE__);
        }
    }
}

//-----------------------------------------------------------------------------
// 描述: 析构函数
//-----------------------------------------------------------------------------
CFileStream::~CFileStream()
{
    if (m_nHandle >= 0) FileClose(m_nHandle);
}

//-----------------------------------------------------------------------------
// 描述: 创建文件
//-----------------------------------------------------------------------------
int CFileStream::FileCreate(const string& strFileName, uint nRights)
{
    umask(0);  // 防止 nRights 被 umask 值 遮掩
    return open(strFileName.c_str(), O_RDWR | O_CREAT | O_TRUNC, nRights);
}

//-----------------------------------------------------------------------------
// 描述: 打开文件
//-----------------------------------------------------------------------------
int CFileStream::FileOpen(const string& strFileName, uint nMode)
{
    byte nShareModes[4] = { 
        0,          // none
        F_WRLCK,    // FM_SHARE_EXCLUSIVE
        F_RDLCK,    // FM_SHARE_DENY_WRITE
        0           // FM_SHARE_DENY_NONE
    };

    int nFileHandle = -1;
    byte nShareMode;

    if (FileExists(strFileName) &&
        (nMode & 0x03) <= FM_OPEN_READ_WRITE &&
        (nMode & 0xF0) <= FM_SHARE_DENY_NONE)
    {
        umask(0);  // 防止 nMode 被 umask 值 遮掩
        nFileHandle = open(strFileName.c_str(), (nMode & 0x03), DEFAULT_FILE_ACCESS_RIGHTS);
        if (nFileHandle != -1)
        {
            nShareMode = ((nMode & 0xF0) >> 4);
            if (nShareModes[nShareMode] != 0)
            {
                struct flock flk;

                flk.l_type = nShareModes[nShareMode];
                flk.l_whence = SEEK_SET;
                flk.l_start = 0;
                flk.l_len = 0;
                
                if (fcntl(nFileHandle, F_SETLK, &flk) < 0)
                {
                    FileClose(nFileHandle);
                    nFileHandle = -1;
                }
            }
        }
    }

    return nFileHandle;
}

//-----------------------------------------------------------------------------
// 描述: 关闭文件
//-----------------------------------------------------------------------------
void CFileStream::FileClose(int nHandle)
{
    close(nHandle);
}

//-----------------------------------------------------------------------------
// 描述: 读文件
//-----------------------------------------------------------------------------
uint64 CFileStream::FileRead(int nHandle, void *pBuffer, uint64 nCount)
{
    return read(nHandle, pBuffer, nCount);
}

//-----------------------------------------------------------------------------
// 描述: 写文件
//-----------------------------------------------------------------------------
uint64 CFileStream::FileWrite(int nHandle, const void *pBuffer, uint64 nCount)
{
    return write(nHandle, pBuffer, nCount);
}

//-----------------------------------------------------------------------------
// 描述: 文件指针定位
//-----------------------------------------------------------------------------
uint64 CFileStream::FileSeek(int nHandle, uint64 nOffset, SeekOrigin nSeekOrigin)
{
    // TODO: lseek 为 32 位的函数，需换成 _llseek.
    return lseek(nHandle, nOffset, nSeekOrigin);
}

//-----------------------------------------------------------------------------
// 描述: 读文件流
//-----------------------------------------------------------------------------
int CFileStream::Read(void *pBuffer, int nCount)
{
    int nResult;

    nResult = FileRead(m_nHandle, pBuffer, nCount);
    if (nResult == -1) nResult = 0;

    return nResult;
}

//-----------------------------------------------------------------------------
// 描述: 写文件流
//-----------------------------------------------------------------------------
int CFileStream::Write(const void *pBuffer, int nCount)
{
    int nResult;

    nResult = FileWrite(m_nHandle, pBuffer, nCount);
    if (nResult == -1) nResult = 0;

    return nResult;
}

//-----------------------------------------------------------------------------
// 描述: 文件流指针定位
//-----------------------------------------------------------------------------
uint64 CFileStream::Seek(uint64 nOffset, SeekOrigin nSeekOrigin)
{
    return FileSeek(m_nHandle, nOffset, nSeekOrigin);
}

//-----------------------------------------------------------------------------
// 描述: 设置文件流大小
//-----------------------------------------------------------------------------
void CFileStream::SetSize(uint64 nSize)
{
    Seek(nSize, SO_BEGINNING);
    if (ftruncate(m_nHandle, GetPosition()) == -1)
        throw CException("error setting file stream size", __FILE__, __LINE__);
}

///////////////////////////////////////////////////////////////////////////////
// class CList

CList::CList() :
    m_pList(NULL),
    m_nCount(0),
    m_nCapacity(0)
{
}

CList::~CList()
{
    Clear();
}

void CList::Grow()
{
    int nDelta;

    if (m_nCapacity > 64)
        nDelta = m_nCapacity / 4;
    else if (m_nCapacity > 8)
        nDelta = 16;
    else
        nDelta = 4;

    SetCapacity(m_nCapacity + nDelta);
}

Pointer CList::Get(int nIndex)
{
    if (nIndex < 0 || nIndex >= m_nCount)
        throw CException("list index error", __FILE__, __LINE__);

    return m_pList[nIndex];
}

void CList::Put(int nIndex, Pointer Item)
{
    if (nIndex < 0 || nIndex >= m_nCount)
        throw CException("list index error", __FILE__, __LINE__);

    m_pList[nIndex] = Item;
}

void CList::SetCapacity(int nNewCapacity)
{
    if (nNewCapacity < m_nCount)
        throw CException("list capacity error", __FILE__, __LINE__);

    if (nNewCapacity != m_nCapacity)
    {
        Pointer *p;
        p = (Pointer*)realloc(m_pList, nNewCapacity * sizeof(Pointer));
        if (p || nNewCapacity == 0)
        {
            m_pList = p;
            m_nCapacity = nNewCapacity;
        }
        else
        {
            throw CException("out of memory", __FILE__, __LINE__);
        }
    }
}

void CList::SetCount(int nNewCount)
{
    if (nNewCount < 0)
        throw CException("list count error", __FILE__, __LINE__);

    if (nNewCount > m_nCapacity)
        SetCapacity(nNewCount);
    if (nNewCount > m_nCount)
        bzero(&m_pList[m_nCount], (nNewCount - m_nCount) * sizeof(Pointer));
    else
        for (int i = m_nCount - 1; i >= nNewCount; i--) Delete(i);

    m_nCount = nNewCount;
}

//-----------------------------------------------------------------------------
// 描述: 向列表中添加元素
//-----------------------------------------------------------------------------
void CList::Add(Pointer Item)
{
    if (m_nCount == m_nCapacity) Grow();
    m_pList[m_nCount] = Item;
    m_nCount++;
}

//-----------------------------------------------------------------------------
// 描述: 向列表中插入元素
// 参数:
//   nIndex - 插入位置下标号(0-based)
//-----------------------------------------------------------------------------
void CList::Insert(int nIndex, Pointer Item)
{
    if (nIndex < 0 || nIndex > m_nCount)
        throw CException("list index error", __FILE__, __LINE__);

    if (m_nCount == m_nCapacity) Grow();
    if (nIndex < m_nCount)
        MoveBuffer(&m_pList[nIndex], &m_pList[nIndex + 1], (m_nCount - nIndex) * sizeof(Pointer));
    m_pList[nIndex] = Item;
    m_nCount++;
}

//-----------------------------------------------------------------------------
// 描述: 从列表中删除元素
// 参数:
//   nIndex - 下标号(0-based)
//-----------------------------------------------------------------------------
void CList::Delete(int nIndex)
{
    if (nIndex < 0 || nIndex >= m_nCount)
        throw CException("list index error", __FILE__, __LINE__);

    m_nCount--;
    if (nIndex < m_nCount)
        MoveBuffer(&m_pList[nIndex + 1], &m_pList[nIndex], (m_nCount - nIndex) * sizeof(Pointer));
}

//-----------------------------------------------------------------------------
// 描述: 从列表中删除元素
// 返回: 被删除元素在列表中的下标号(0-based)，若未找到，则返回 -1.
//-----------------------------------------------------------------------------
int CList::Remove(Pointer Item)
{
    int nResult;

    nResult = IndexOf(Item);
    if (nResult >= 0)
        Delete(nResult);

    return nResult;
}

//-----------------------------------------------------------------------------
// 描述: 从列表中删除元素
// 返回: 被删除的元素值，若未找到，则返回 NULL.
//-----------------------------------------------------------------------------
Pointer CList::Extract(Pointer Item)
{
    int i;
    Pointer pResult = NULL;

    i = IndexOf(Item);
    if (i >= 0)
    {
        pResult = Item;
        m_pList[i] = NULL;
        Delete(i);
    }

    return pResult;
}

//-----------------------------------------------------------------------------
// 描述: 移动一个元素到新的位置
//-----------------------------------------------------------------------------
void CList::Move(int nCurIndex, int nNewIndex)
{
    Pointer pItem;

    if (nCurIndex != nNewIndex)
    {
        if (nNewIndex < 0 || nNewIndex >= m_nCount)
            throw CException("list index error", __FILE__, __LINE__);

        pItem = Get(nCurIndex);
        m_pList[nCurIndex] = NULL;
        Delete(nCurIndex);
        Insert(nNewIndex, NULL);
        m_pList[nNewIndex] = pItem;
    }
}

//-----------------------------------------------------------------------------
// 描述: 改变列表的大小
//-----------------------------------------------------------------------------
void CList::Resize(int nCount)
{
    SetCount(nCount);
}

//-----------------------------------------------------------------------------
// 描述: 清空列表
//-----------------------------------------------------------------------------
void CList::Clear()
{
    SetCount(0);
    SetCapacity(0);
}

//-----------------------------------------------------------------------------
// 描述: 返回列表中的首个元素 (若列表为空则抛出异常)
//-----------------------------------------------------------------------------
Pointer CList::First()
{
    return Get(0);
}

//-----------------------------------------------------------------------------
// 描述: 返回列表中的最后元素 (若列表为空则抛出异常)
//-----------------------------------------------------------------------------
Pointer CList::Last()
{
    return Get(m_nCount - 1);
}

//-----------------------------------------------------------------------------
// 描述: 返回元素在列表中的下标号 (若未找到则返回 -1)
//-----------------------------------------------------------------------------
int CList::IndexOf(Pointer Item)
{
    int nResult = 0;

    while (nResult < m_nCount && m_pList[nResult] != Item) nResult++;
    if (nResult == m_nCount)
        nResult = -1;

    return nResult;
}

//-----------------------------------------------------------------------------
// 描述: 返回列表中元素总数
//-----------------------------------------------------------------------------
int CList::Count() const
{
    return m_nCount;
}

//-----------------------------------------------------------------------------
// 描述: 存取列表中任意元素
//-----------------------------------------------------------------------------
Pointer& CList::operator[](int nIndex)
{
    if (nIndex < 0 || nIndex >= m_nCount)
        throw CException("list index error", __FILE__, __LINE__);

    return m_pList[nIndex];
}

///////////////////////////////////////////////////////////////////////////////

} // namespace ise
