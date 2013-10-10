///////////////////////////////////////////////////////////////////////////////
// Iris Server Engine (ISE)
//
// �ļ�����: ise_classes.cpp
// ��������: ͨ�û������
// ����޸�: 2005-04-19
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
// ����: �������� Log ���ַ���
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
// ����: ���ַ���д���ļ�
//-----------------------------------------------------------------------------
void CLogger::WriteToFile(const string& strString)
{
    if (m_strFileName.empty()) return;

    ofstream ofs(m_strFileName.c_str(), ios::out | ios::app);
    ofs << strString;
    ofs.close();
}

//-----------------------------------------------------------------------------
// ����: ���Զ����ı�д����־
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
// ����: ���쳣��Ϣд����־
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
// ����: ���û����С
// ��ע: �µĻ���ᱣ��ԭ������
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

        // ��� m_pBuffer == NULL���� realloc �൱�� malloc��

        pNewBuf = realloc(m_pBuffer, nSize + 1);  // �����һ���ֽ����� c_str()!
        
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
// ����: ���� C �����ַ��� (ĩ�˸��ӽ����� '\0')
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
// ����: �� pBuffer �е� nSize ���ֽڸ��Ƶ� *this �У�������С����Ϊ nSize
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
// ����: ���ص�ǰʱ�� (��1970-01-01 00:00:00 ���������)
//-----------------------------------------------------------------------------
CDateTime CDateTime::CurrentDateTime()
{
    return CDateTime(time(NULL));
}

//-----------------------------------------------------------------------------
// ����: ���ص�ǰʱ���ĵ�ǰʱ��� (��1970-01-01 00:00:00 ���������)
//-----------------------------------------------------------------------------
time_t CDateTime::CurrentZoneTimeStamp()
{
    struct timeb t;
    ftime(&t);
    return t.time - t.timezone * 60;
}

//-----------------------------------------------------------------------------
// ����: ���ַ���ת���� CDateTime
// ע��: strDateTime �ĸ�ʽ����Ϊ YYYY-MM-DD HH:MM:SS
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
// ����: ����ʱ����룬������ *this
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
// ����: ����ʱ����룬�����������
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
// ����: ���������ַ���
// ����:
//   chDateSep - ���ڷָ���
// ��ʽ:
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
// ����: ��������ʱ���ַ���
// ����:
//   chDateSep - ���ڷָ���
//   chTimeSep - ʱ��ָ���
// ��ʽ:
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
// ����: ���캯��
// ����:
//   nSemKey    - �ź�������ϵͳ�е�Ψһ��ʶ��
//   nSemCount  - �ź��������ź����ĸ���
//-----------------------------------------------------------------------------
CSem::CSem(key_t nSemKey, int nSemCount)
    : m_nSemKey(nSemKey), m_nSemCount(nSemCount)
{
}

CSem::~CSem()
{
}

//-----------------------------------------------------------------------------
// ����: �����ź�����
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
// ����: ע���ź�����
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
// ����: �Ե� nSemNum(0-based) ���ź�������P����
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
// ����: �Ե� nSemNum(0-based) ���ź�������V����
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
// ����: �������ź������и�ֵ
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
// ����: �Ե� nSemNum(0-based) ���ź������и�ֵ
//-----------------------------------------------------------------------------
void CSem::SetValue(int nSemNum, int nValue)
{
    semun unSem;
    unSem.val = nValue;
    if (semctl(m_nSemId, nSemNum, SETVAL, unSem) < 0)
        throw CException(strerror(errno), __FILE__, __LINE__);
}

//-----------------------------------------------------------------------------
// ����: ȡ�����ź�����ֵ
//-----------------------------------------------------------------------------
void CSem::GetAll(unsigned short int *pArray)
{
    semun unSem;
    unSem.array = pArray;
    if (semctl(m_nSemId, 0, GETALL, unSem) < 0)
        throw CException(strerror(errno), __FILE__, __LINE__);   
}

//-----------------------------------------------------------------------------
// ����: ȡ�� nSemNum(0-based) ���ź�����ֵ
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
// ����: ��ʼ�������ź�����ֵΪ1
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
// ����: ���캯��
// ����:
//   nShmKey    - �����ڴ���ϵͳ�е�Ψһ��ʶ��
//   nShmSize   - �����ڴ��С(�ֽ���)
//-----------------------------------------------------------------------------
CShm::CShm(key_t nShmKey, int nShmSize)
    : m_nShmKey(nShmKey), m_nShmSize(nShmSize), m_pShmData(NULL), m_bCreate(false)
{
}

CShm::~CShm()
{
}

//-----------------------------------------------------------------------------
// ����: ��ȡ�����ڴ�
// ��ע: ��ָ��KEY�Ĺ����ڴ��Ѵ��ڣ����������й����ڴ档
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
// ����: ���Դ��������ڴ档��ָ����KEY�Ĺ����ڴ��Ѵ��ڣ������KEY�����Դ�����
// ����:
//   nRetryTimes    - ���Զ��ٱ�
//   nMode          - �����ڴ��MODE
//-----------------------------------------------------------------------------
void CShm::TryCreateShm(int nRetryTimes, int nMode)
{
    bool bResult = false;
    int nShmId, nShmKey;
    
    // ��ʼKEYΪ m_nShmKey
    nShmKey = m_nShmKey;

    for (int i = 0; i < nRetryTimes; i++)
    {
        nShmId = shmget(nShmKey, m_nShmSize, IPC_CREAT | IPC_EXCL | nMode);
        if (nShmId < 0)  // ����
        {
            // ����˹����ڴ��Ѵ���
            if (errno == EEXIST)
                nShmKey++;
        }
        else
        {
            // �����ɹ�
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
// ����: ���ӹ����ڴ�
//-----------------------------------------------------------------------------
void CShm::AttachShm()
{
    if ((m_pShmData = (char*)shmat(m_nShmId, NULL, 0)) < 0)
        throw CException(strerror(errno), __FILE__, __LINE__);
}

//-----------------------------------------------------------------------------
// ����: ���빲���ڴ�
//-----------------------------------------------------------------------------
void CShm::DetachShm()
{
    if (shmdt(m_pShmData) < 0)
        throw CException(strerror(errno), __FILE__, __LINE__);
    m_pShmData = NULL;
}

//-----------------------------------------------------------------------------
// ����: �ͷŹ����ڴ�
//-----------------------------------------------------------------------------
void CShm::RemoveShm()
{
    if (shmctl(m_nShmId, IPC_RMID, NULL) < 0)
        throw CException(strerror(errno), __FILE__, __LINE__);
    m_pShmData = NULL;
    m_bCreate = false;
}

//-----------------------------------------------------------------------------
// ����: ��ʼ�������ڴ�����Ϊ0
//-----------------------------------------------------------------------------
void CShm::InitShm()
{
    if (m_pShmData)
    {
        memset(m_pShmData, 0, m_nShmSize);
    }
}

//-----------------------------------------------------------------------------
// ����: �жϹ����ڴ��Ƿ����
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
// ����: ���� Block/UnBlock ����������źż���
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
// ����: �ڽ��̵�ǰ�����źż������ SetSignals ���õ��ź�
//-----------------------------------------------------------------------------
void CSignalMasker::Block()
{
    SigProcMask(SIG_BLOCK, &m_NewSet, &m_OldSet); 
    m_bBlock = true;
}

//-----------------------------------------------------------------------------
// ����: �ڽ��̵�ǰ�����źż��н�� SetSignals ���õ��ź�
//-----------------------------------------------------------------------------
void CSignalMasker::UnBlock()
{
    SigProcMask(SIG_UNBLOCK, &m_NewSet, &m_OldSet);
    m_bBlock = true;
}

//-----------------------------------------------------------------------------
// ����: �����������źż��ָ�Ϊ Block/UnBlock ֮ǰ��״̬
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
// ����: ���캯��
// ����:
//   sFileName - �ļ���(���԰���·��)
//   nMode     - �ļ��򿪷�ʽ
//   nRights   - �ļ�Ȩ��
//-----------------------------------------------------------------------------
CFileLock::CFileLock(const char *sFileName, uint nMode, uint nRights)
{
    ForceDirectories(ExtractFilePath(sFileName));
    umask(0);  // ��ֹ nMode �� umaskֵ ����

    if ((m_nFd = open(sFileName, nMode, nRights)) < 0)
        throw CException(strerror(errno), __FILE__, __LINE__);

    m_bAutoClose = true;
}

CFileLock::~CFileLock()
{
    if (m_bAutoClose) close(m_nFd);
}

//-----------------------------------------------------------------------------
// ����: �Բ����ļ�����
// ����:
//   true   - �ɹ�
//   false  - ʧ��:
//     �� nCmd Ϊ F_SETLKW��������Ϊ�źŶ��жϣ�
//     �� nCmd Ϊ F_SETLK�����ʾ��������δ����
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
// ����: ��ȫ���ļ�����
//-----------------------------------------------------------------------------
bool CFileLock::Flock(int nCmd)
{
    return flock(m_nFd, nCmd) == 0;
}

///////////////////////////////////////////////////////////////////////////////
// class CMultiInstChecker

//-----------------------------------------------------------------------------
// ����: ���ô��������ļ���
// ����:
//   pLockFile  - ���ļ���(��·��)
//   bIsSelfExe - ���ļ��Ƿ�����EXE
//-----------------------------------------------------------------------------
void CMultiInstChecker::SetLockFile(const char *pLockFile, bool bIsSelfExe)
{
    m_strLockFile = pLockFile;
    m_bIsSelfExe = bIsSelfExe;
}

//-----------------------------------------------------------------------------
// ����: �������Ƿ��Ѿ�����
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

    // ������˵��:
    // PTHREAD_MUTEX_TIMED_NP:
    //   ��ͨ����ͬһ�߳��ڱ���ɶԵ��� Lock �� Unlock�������������ö�� Lock�������������
    // PTHREAD_MUTEX_RECURSIVE_NP:
    //   Ƕ�������߳��ڿ���Ƕ�׵��� Lock����һ����Ч��֮����������ͬ������ Unlock ���ɽ�����
    // PTHREAD_MUTEX_ERRORCHECK_NP:
    //   ����������ͬһ�߳�Ƕ�׵��� Lock ���������
    // PTHREAD_MUTEX_ADAPTIVE_NP:
    //   ��Ӧ����
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK_NP);
    pthread_mutex_init(&m_Mutex, &attr);
    pthread_mutexattr_destroy(&attr);
}

CCriticalSection::~CCriticalSection()
{
    // �����δ����������� destroy���˺����᷵�ش��� EBUSY��
    // �� linux �£���ʹ�˺������ش���Ҳ��������Դй©��
    pthread_mutex_destroy(&m_Mutex);
}

//-----------------------------------------------------------------------------
// ����: ����
//-----------------------------------------------------------------------------
void CCriticalSection::Lock()
{
    int r = pthread_mutex_lock(&m_Mutex);

    // �������ѱ��Լ����߳���ס (ֻ�м�����ŷ��ش˴���)
    if (r == EDEADLK)
        throw CException("lock recursived.", __FILE__, __LINE__);
}

//-----------------------------------------------------------------------------
// ����: ����
//-----------------------------------------------------------------------------
void CCriticalSection::Unlock()
{
    int r = pthread_mutex_unlock(&m_Mutex);

    // ��ǰ�߳�δӵ�д��� (ֻ�м�����ŷ��ش˴���)
    if (r == EPERM)
        throw CException("the calling thread does not own the mutex.", __FILE__, __LINE__);
}

//-----------------------------------------------------------------------------
// ����: ���Լ��� (���Ѿ����ڼ���״̬����������)
// ����:
//   true   - �����ɹ�
//   false  - ʧ�ܣ������Ѿ����ڼ���״̬
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
// ����: ��Ʋ������źŵ�ֵԭ�ӵؼ�1����ʾ����һ���ɷ��ʵ���Դ
//-----------------------------------------------------------------------------
void CThreadSem::Increase()
{
    sem_post(&m_Sem);
}

//-----------------------------------------------------------------------------
// ����: �ȴ����� (�źŵ�ֵ����0)��Ȼ���źŵ�ԭ�ӵؼ�1
//-----------------------------------------------------------------------------
void CThreadSem::Wait()
{
    sem_wait(&m_Sem);
}

//-----------------------------------------------------------------------------
// ����: ���Եȴ� (����źŵƼ�������0����ԭ�ӵؼ�1������true��������������false)
//-----------------------------------------------------------------------------
bool CThreadSem::TryWait()
{
    return (sem_trywait(&m_Sem) == 0);
}

//-----------------------------------------------------------------------------
// ����: ȡ���źŵƵĵ�ǰ����
//-----------------------------------------------------------------------------
int CThreadSem::GetValue()
{
    int nValue;
    sem_getvalue(&m_Sem, &nValue);
    return nValue;
}

//-----------------------------------------------------------------------------
// ����: ���źŵƵļ���ֵ��Ϊ��ʼ״̬
//-----------------------------------------------------------------------------
void CThreadSem::Reset()
{
    sem_destroy(&m_Sem);
    sem_init(&m_Sem, 0, m_nInitValue);
}

///////////////////////////////////////////////////////////////////////////////
// class CSeqAllocator

//-----------------------------------------------------------------------------
// ����: ���캯��
// ����:
//   nStartId - ��ʼ���к�
//-----------------------------------------------------------------------------
CSeqAllocator::CSeqAllocator(uint nStartId)
{
    m_nCurrentId = nStartId;
}

//-----------------------------------------------------------------------------
// ����: ����һ���·����ID
//-----------------------------------------------------------------------------
uint CSeqAllocator::AllocId()
{
    CAutoSynchronizer Syncher(m_Lock);
    return m_nCurrentId++;
}

///////////////////////////////////////////////////////////////////////////////
// class CThread

//-----------------------------------------------------------------------------
// ����: �߳��յ� cancel �źź���ƺ�����
// ����:
//   arg - ָ�� CThread ����
//-----------------------------------------------------------------------------
void ThreadFinalProc(void *arg)
{
    CThread *pThread = (CThread*)arg;

    pThread->m_bFinished = true;
    pThread->m_nThreadId = 0;
    if (pThread->m_bFreeOnTerminate) delete pThread;

    // ע: �̱߳� cancel �ź���ֹ�󣬻��Զ�ִ�� pthread_exit��
    // �˴��в��ɵ��� pthread_exit������ᵼ�� ThreadFinalProc ���������޴�ִ�У� 
}

//-----------------------------------------------------------------------------
// ����: �߳�ִ�к���
// ����:
//   arg - �̲߳������˴�ָ�� CThread ����
//-----------------------------------------------------------------------------
void* ThreadExecProc(void *arg)
{
    CThread *pThread = (CThread*)arg;
    int nReturnValue = 0;

    // ����� [push..pop] �����̱߳�ǿ����ֹ(�յ� cancel �ź�)���׳��쳣(throw XXX)��
    // ��֤ ThreadFinalProc ����һ����ִ�С�
    pthread_cleanup_push(ThreadFinalProc, pThread);
    // �̶߳� cancel �źŵ���Ӧ��ʽ������: (1)����Ӧ (2)�Ƴٵ�ȡ��������Ӧ (3)����������Ӧ��
    // �˴������߳�Ϊ��(3)�ַ�ʽ���������ϱ� cancel �ź���ֹ��
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

    if (!pThread->m_bTerminated)
    {
        // �߳���ִ�й��������յ� cancel �źţ����׳�һ���쳣(������ʲô���͵��쳣��)��
        // ���쳣ǧ�򲻿�ȥ������( try{}catch(...){} )��ϵͳ���������쳣�Ƿ񱻳���������
        // ���ǣ���ᷢ�� SIGABRT �źţ������ն���� "FATAL: exception not rethrown"��
        // ���Դ˴��Ĳ�����ֻ���� CException �쳣(��ISE�������쳣�Դ� CException �̳�)��
        // �� pThread->Execute() ��ִ�й����У��û�Ӧ��ע����������:
        // 1. ����մ˴�������ȥ�����쳣�����в��������������͵��쳣( ��catch(...) );
        // 2. �����׳� CException ��������֮����쳣�������׳�һ������( �� throw 5; )��
        //    ϵͳ����Ϊû�д��쳣�Ĵ����������� abort��(������ˣ�ThreadFinalProc
        //    �Ի��� pthread_cleanup_push ����ŵ��������ִ�е���)
        try { pThread->Execute(); } catch (CException& e) { ; }

        // ��¼���̷߳���ֵ
        nReturnValue = pThread->m_nReturnValue;
    }

    // ���� cancel �źţ����� [push..pop] ����󽫲��ɱ�ǿ����ֹ 
    pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
    // �뿪���������ͬʱ��ִ�� ThreadFinalProc ���� (pop�Ĳ���Ϊ1��ʾִ�к���)
    pthread_cleanup_pop(1);

    pthread_exit((void*)nReturnValue);
    return NULL;
}

//-----------------------------------------------------------------------------
// ����: ���캯��
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
// ����: ��������
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
// ����: �̴߳�����
//-----------------------------------------------------------------------------
void CThread::CheckThreadError(int nErrorCode)
{
    if (nErrorCode != 0)
        throw CException(SysErrorMessage(nErrorCode).c_str(), __FILE__, __LINE__);
}

//-----------------------------------------------------------------------------
// ����: �����̲߳�ִ��
// ע��: �˳�Ա�����ڶ�������������ֻ�ɵ���һ�Ρ�
//-----------------------------------------------------------------------------
void CThread::Run()
{
    if (m_nThreadId != 0)
        throw CException("thread already running", __FILE__, __LINE__);
    
    // �����߳�
    int nErrCode;
    nErrCode = pthread_create(&m_nThreadId, NULL, ThreadExecProc, (void*)this);
    if (nErrCode != 0)
        throw CException(SysErrorMessage(nErrCode).c_str(), __FILE__, __LINE__);

    // �����̵߳��Ȳ���
    if (m_nPolicy != THREAD_POLICY_DEFAULT)
        SetPolicy(m_nPolicy);
    // �����߳����ȼ�
    if (m_nPriority != THREAD_PRIORITY_DEFAULT)
        SetPriority(m_nPriority);
}

//-----------------------------------------------------------------------------
// ����: ֪ͨ�߳��˳�
//-----------------------------------------------------------------------------
void CThread::Terminate()
{
    try { DoTerminate(); } catch (CException& e) {}

    if (!m_bTerminated) m_nTermElapsedSecs = time(NULL);
    m_bTerminated = true;
}

//-----------------------------------------------------------------------------
// ����: ǿ��ɱ���߳�
// ע��: 
//   1. ���ô˺����󣬶��߳�������һ�в����Բ�����(Terminate(); WaitFor(); delete pThread; ��)��
//   2. ��ɱ���߳�ǰ��m_bFreeOnTerminate ���Զ���Ϊ true���Ա�������Զ��ͷš�
//   3. �̱߳�ɱ�����û��������ĳЩ��Ҫ��Դ����δ�ܵõ��ͷţ���������Դ (��δ���ü�����
//      �㱻ɱ��)��������Ҫ��Դ���ͷŹ��������� DoKill �н��С�
//   4. �߳����յ� cancel �źź��������Լ�ʱ�����������׳�һ���쳣�����쳣�������̺߳���
//      ��ִ�����̡����ԣ��̺߳����е�����ջ������Ȼ����������˳��������
//-----------------------------------------------------------------------------
void CThread::Kill()
{
    if (m_nThreadId != 0)
    {
        // �û����ڴ˴��ͷ���Ҫ��Դ�������ͷ�����Դ
        try { DoKill(); } catch (CException& e) {}

        pthread_t nThreadId = m_nThreadId;
        m_nThreadId = 0;

        SetFreeOnTerminate(true);
        pthread_cancel(nThreadId);
    }
}

//-----------------------------------------------------------------------------
// ����: �ȴ��߳��˳�
// ����: �̷߳���ֵ
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
// ����: ����˯��״̬ (˯�߹����л��� m_bTerminated ��״̬)
// ����:
//   fSeconds - ˯�ߵ���������ΪС�����ɾ�ȷ������
// ע��:
//   ���ڽ�˯��ʱ��ֳ������ɷݣ�ÿ��˯��ʱ���С����ۼ���������������
//-----------------------------------------------------------------------------
void CThread::Sleep(double fSeconds) const
{
    const double SLEEP_INTERVAL = 0.5;      // ÿ��˯�ߵ�ʱ��(��)
    const double ADJUST_SECONDS = -0.01;    // ����������� (ע: ���޸��� SLEEP_INTERVAL����ֵҲ�����)

    double fOnceSecs;

    while (!GetTerminated() && fSeconds > 0)
    {
        fOnceSecs = (fSeconds >= SLEEP_INTERVAL ? SLEEP_INTERVAL : fSeconds);
        fSeconds -= fOnceSecs;

        NanoSleep(fOnceSecs + ADJUST_SECONDS, true);
    }
}

//-----------------------------------------------------------------------------
// ����: ȡ�ôӵ��� Terminate ����ǰ����������ʱ��(��)
//-----------------------------------------------------------------------------
int CThread::GetTermElapsedSecs() const
{
    int nResult = 0;

    // ����Ѿ�֪ͨ�˳������̻߳�����
    if (m_bTerminated && m_nThreadId != 0)
    {
        nResult = time(NULL) - m_nTermElapsedSecs;
    }

    return nResult;
}

//-----------------------------------------------------------------------------
// ����: �����Ƿ� Terminate
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
// ����: �����̵߳ĵ��Ȳ���
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
// ����: �����̵߳����ȼ�
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
// ����: ���캯��
// ����:
//   nMemoryDelta - �ڴ��������� (�ֽ����������� 2 �� N �η�)
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

        // ��֤ nNewMemoryDelta ��2��N�η�
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
// ����: ���ڴ���
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
// ����: д�ڴ���
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
// ����: �ڴ���ָ�붨λ
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
// ����: �����ڴ�����С
//-----------------------------------------------------------------------------
void CMemoryStream::SetSize(uint64 nSize)
{
    uint64 nOldPos = m_nPosition;

    SetCapacity(nSize);
    m_nSize = nSize;
    if (nOldPos > nSize) Seek(0, SO_END);
}

//-----------------------------------------------------------------------------
// ����: �����������뵽�ڴ�����
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
// ����: ���ļ����뵽�ڴ�����
//-----------------------------------------------------------------------------
void CMemoryStream::LoadFromFile(const string& strFileName)
{
    CFileStream FileStream(strFileName, FM_OPEN_READ | FM_SHARE_DENY_WRITE);
    LoadFromStream(FileStream);
}

//-----------------------------------------------------------------------------
// ����: ���ڴ������浽��������
//-----------------------------------------------------------------------------
void CMemoryStream::SaveToStream(CStream& Stream)
{
    if (m_nSize != 0)
        Stream.Write(m_pMemory, m_nSize);
}

//-----------------------------------------------------------------------------
// ����: ���ڴ������浽�ļ���
//-----------------------------------------------------------------------------
void CMemoryStream::SaveToFile(const string& strFileName)
{
    CFileStream FileStream(strFileName, FM_CREATE);
    SaveToStream(FileStream);
}

//-----------------------------------------------------------------------------
// ����: ����ڴ���
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
// ����: ���캯��
// ����:
//   strFileName - �ļ���
//   nMode       - �ļ����򿪷�ʽ
//   nRights     - �ļ���ȡȨ��
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
// ����: ��������
//-----------------------------------------------------------------------------
CFileStream::~CFileStream()
{
    if (m_nHandle >= 0) FileClose(m_nHandle);
}

//-----------------------------------------------------------------------------
// ����: �����ļ�
//-----------------------------------------------------------------------------
int CFileStream::FileCreate(const string& strFileName, uint nRights)
{
    umask(0);  // ��ֹ nRights �� umask ֵ ����
    return open(strFileName.c_str(), O_RDWR | O_CREAT | O_TRUNC, nRights);
}

//-----------------------------------------------------------------------------
// ����: ���ļ�
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
        umask(0);  // ��ֹ nMode �� umask ֵ ����
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
// ����: �ر��ļ�
//-----------------------------------------------------------------------------
void CFileStream::FileClose(int nHandle)
{
    close(nHandle);
}

//-----------------------------------------------------------------------------
// ����: ���ļ�
//-----------------------------------------------------------------------------
uint64 CFileStream::FileRead(int nHandle, void *pBuffer, uint64 nCount)
{
    return read(nHandle, pBuffer, nCount);
}

//-----------------------------------------------------------------------------
// ����: д�ļ�
//-----------------------------------------------------------------------------
uint64 CFileStream::FileWrite(int nHandle, const void *pBuffer, uint64 nCount)
{
    return write(nHandle, pBuffer, nCount);
}

//-----------------------------------------------------------------------------
// ����: �ļ�ָ�붨λ
//-----------------------------------------------------------------------------
uint64 CFileStream::FileSeek(int nHandle, uint64 nOffset, SeekOrigin nSeekOrigin)
{
    // TODO: lseek Ϊ 32 λ�ĺ������軻�� _llseek.
    return lseek(nHandle, nOffset, nSeekOrigin);
}

//-----------------------------------------------------------------------------
// ����: ���ļ���
//-----------------------------------------------------------------------------
int CFileStream::Read(void *pBuffer, int nCount)
{
    int nResult;

    nResult = FileRead(m_nHandle, pBuffer, nCount);
    if (nResult == -1) nResult = 0;

    return nResult;
}

//-----------------------------------------------------------------------------
// ����: д�ļ���
//-----------------------------------------------------------------------------
int CFileStream::Write(const void *pBuffer, int nCount)
{
    int nResult;

    nResult = FileWrite(m_nHandle, pBuffer, nCount);
    if (nResult == -1) nResult = 0;

    return nResult;
}

//-----------------------------------------------------------------------------
// ����: �ļ���ָ�붨λ
//-----------------------------------------------------------------------------
uint64 CFileStream::Seek(uint64 nOffset, SeekOrigin nSeekOrigin)
{
    return FileSeek(m_nHandle, nOffset, nSeekOrigin);
}

//-----------------------------------------------------------------------------
// ����: �����ļ�����С
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
// ����: ���б������Ԫ��
//-----------------------------------------------------------------------------
void CList::Add(Pointer Item)
{
    if (m_nCount == m_nCapacity) Grow();
    m_pList[m_nCount] = Item;
    m_nCount++;
}

//-----------------------------------------------------------------------------
// ����: ���б��в���Ԫ��
// ����:
//   nIndex - ����λ���±��(0-based)
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
// ����: ���б���ɾ��Ԫ��
// ����:
//   nIndex - �±��(0-based)
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
// ����: ���б���ɾ��Ԫ��
// ����: ��ɾ��Ԫ�����б��е��±��(0-based)����δ�ҵ����򷵻� -1.
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
// ����: ���б���ɾ��Ԫ��
// ����: ��ɾ����Ԫ��ֵ����δ�ҵ����򷵻� NULL.
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
// ����: �ƶ�һ��Ԫ�ص��µ�λ��
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
// ����: �ı��б�Ĵ�С
//-----------------------------------------------------------------------------
void CList::Resize(int nCount)
{
    SetCount(nCount);
}

//-----------------------------------------------------------------------------
// ����: ����б�
//-----------------------------------------------------------------------------
void CList::Clear()
{
    SetCount(0);
    SetCapacity(0);
}

//-----------------------------------------------------------------------------
// ����: �����б��е��׸�Ԫ�� (���б�Ϊ�����׳��쳣)
//-----------------------------------------------------------------------------
Pointer CList::First()
{
    return Get(0);
}

//-----------------------------------------------------------------------------
// ����: �����б��е����Ԫ�� (���б�Ϊ�����׳��쳣)
//-----------------------------------------------------------------------------
Pointer CList::Last()
{
    return Get(m_nCount - 1);
}

//-----------------------------------------------------------------------------
// ����: ����Ԫ�����б��е��±�� (��δ�ҵ��򷵻� -1)
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
// ����: �����б���Ԫ������
//-----------------------------------------------------------------------------
int CList::Count() const
{
    return m_nCount;
}

//-----------------------------------------------------------------------------
// ����: ��ȡ�б�������Ԫ��
//-----------------------------------------------------------------------------
Pointer& CList::operator[](int nIndex)
{
    if (nIndex < 0 || nIndex >= m_nCount)
        throw CException("list index error", __FILE__, __LINE__);

    return m_pList[nIndex];
}

///////////////////////////////////////////////////////////////////////////////

} // namespace ise
