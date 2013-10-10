///////////////////////////////////////////////////////////////////////////////
// Iris Server Engine (ISE)
//
// �ļ�����: ise_system.cpp
// ��������: ϵͳ���
// ����޸�: 2005-12-10
///////////////////////////////////////////////////////////////////////////////

#include "ise_system.h"
#include "ise_sysutils.h"
#include "ise_socket.h"

namespace ise
{

///////////////////////////////////////////////////////////////////////////////
// ȫ�ֱ�������

// ��־����
CLogger Logger;
// Ӧ�ó������
CApplication Application;
// Ӧ�ó�������չ����ָ�� (ָ���û������Ķ���)
CCustomExtension *pExtension;

// ���ڽ����˳�ʱ����ת
static sigjmp_buf ProcExitJmpBuf;

///////////////////////////////////////////////////////////////////////////////
// �źŴ�����

//-----------------------------------------------------------------------------
// ����: �����˳����� �źŴ�����
//-----------------------------------------------------------------------------
void ExitProgramSignalHandler(int nSigNo)
{
    static bool bInHandler = false;
    if (bInHandler) return;
    bInHandler = true;

    // ֹͣ���߳�ѭ��
    Application.SetTerminated(true);

    Logger.Write("ExitProgramSignalHandler. (signal:%d)", nSigNo);

    siglongjmp(ProcExitJmpBuf, 1);
}

//-----------------------------------------------------------------------------
// ����: �����Ƿ����� �źŴ�����
//-----------------------------------------------------------------------------
void FatalErrorSignalHandler(int nSigNo)
{
    static bool bInHandler = false;
    if (bInHandler) exit(1);
    bInHandler = true;

    // ֹͣ���߳�ѭ��
    Application.SetTerminated(true);

    Logger.Write("Fatal error. (signal:%d)", nSigNo);

    /*
    // ��� dump core ???
    chdir(Application.GetExePath().c_str());
    signal(nSigNo, SIG_DFL);
    pthread_kill(pthread_self(), nSigNo);
    */

    exit(1);
}

///////////////////////////////////////////////////////////////////////////////
// class CSystemOptions

//-----------------------------------------------------------------------------
// ����: ��ʼ��Ĭ������
//-----------------------------------------------------------------------------
void CSystemOptions::InitDefaultOptions()
{
    strLogFileName = "";
    bIsDaemon = false;
    bAllowMultiInstance = false;
}

///////////////////////////////////////////////////////////////////////////////
// class CApplication

CApplication::CApplication() :
    m_pMainServer(NULL),
    m_bInitialized(false),
    m_bTerminated(false)
{
}

CApplication::~CApplication()
{
    Finalize();
}

//-----------------------------------------------------------------------------
// ����: ����û��Ƿ񴴽��� CCustomExtension ����
//-----------------------------------------------------------------------------
void CApplication::CheckAppExtensionExists()
{
    if (!pExtension)
        throw CException("CCustomExtension object not found", __FILE__, __LINE__);
}

//-----------------------------------------------------------------------------
// ����: ����Ƿ������˶������ʵ��
//-----------------------------------------------------------------------------
void CApplication::CheckMultiInstance()
{
    if (!m_SystemOptions.bAllowMultiInstance)
    {
        m_MultiInstChecker.SetLockFile(Application.GetExeName().c_str(), true);
        if (m_MultiInstChecker.IsAlreayRunning())
            throw CException("already running.", __FILE__, __LINE__);
    }
}

//-----------------------------------------------------------------------------
// ����: Ӧ�� ϵͳ����
//-----------------------------------------------------------------------------
void CApplication::ApplySystemOptions()
{
    Logger.SetFileName(m_SystemOptions.strLogFileName);
}

//-----------------------------------------------------------------------------
// ����: ������������
//-----------------------------------------------------------------------------
void CApplication::CreateMainServer()
{
    if (!m_pMainServer)
        m_pMainServer = new CMainServer;
}

//-----------------------------------------------------------------------------
// ����: �ͷ���������
//-----------------------------------------------------------------------------
void CApplication::FreeMainServer()
{
    if (m_pMainServer) delete m_pMainServer;
    m_pMainServer = NULL;
}

//-----------------------------------------------------------------------------
// ����: ȡ�������ļ���ȫ��������ʼ�� m_strExeName
//-----------------------------------------------------------------------------
void CApplication::InitExeName()
{
    const int BUFFER_SIZE = 500;

    int r;
    char sBuffer[BUFFER_SIZE];

    r = readlink("/proc/self/exe", sBuffer, BUFFER_SIZE);
    if (r != -1)
    {
        if (r >= BUFFER_SIZE) r = BUFFER_SIZE - 1;
        sBuffer[r] = 0;
        m_strExeName = sBuffer;
    }
    else
    {
        throw CException("no permission to read /proc/self/exe.", __FILE__, __LINE__);
    }
}

//-----------------------------------------------------------------------------
// ����: �ػ�ģʽ��ʼ��
//-----------------------------------------------------------------------------
void CApplication::InitDaemon()
{
    int r;
    
    r = fork();
   
    if (r < 0)
        throw CException("init daemon error", __FILE__, __LINE__);
    else if (r != 0) 
        exit(0);
    
    // ��һ�ӽ��̺�̨����ִ��

    // ��һ�ӽ��̳�Ϊ�µĻỰ�鳤�ͽ����鳤
    r = setsid();
    
    if (r < 0) exit(1);
      
    // ���� SIGHUP �ź� (ע: �������ն˶Ͽ�ʱ�������������������ر�telnet)
    signal(SIGHUP, SIG_IGN);
    
    // ��һ�ӽ����˳�
    r = fork();
    
    if (r < 0) exit(1); 
    else if (r != 0) exit(0);
   
    // �ڶ��ӽ��̼���ִ�У��������ǻỰ�鳤

    // �ı䵱ǰ����Ŀ¼
    chdir("/");

    // �����ļ�������ģ
    umask(0);
}

//-----------------------------------------------------------------------------
// ����: ��ʼ���ź� (�źŵİ�װ�����Ե�)
//
//  �ź�����    ֵ                         �ź�˵��
// ---------  ----  -----------------------------------------------------------
// # SIGHUP    1    ���ź����û��ն�����(�����������)����ʱ������ͨ�������ն˵Ŀ��ƽ���
//                  ����ʱ��֪ͨͬһ session �ڵĸ�����ҵ����ʱ����������ն˲��ٹ�����
// # SIGINT    2    ������ֹ(interrupt)�źţ����û�����INTR�ַ�(ͨ����Ctrl-C)ʱ������
// # SIGQUIT   3    �� SIGINT ���ƣ�����QUIT�ַ� (ͨ���� Ctrl-\) �����ơ����������յ�
//                  ���ź��˳�ʱ�����core�ļ��������������������һ����������źš�
// # SIGILL    4    ִ���˷Ƿ�ָ�ͨ������Ϊ��ִ���ļ�������ִ��󣬻�����ͼִ�����ݶΡ�
//                  ��ջ���ʱҲ�п��ܲ�������źš�
// # SIGTRAP   5    �ɶϵ�ָ������� trap ָ��������� debugger ʹ�á�
// # SIGABRT   6    �����Լ����ִ��󲢵��� abort ʱ������
// # SIGIOT    6    ��PDP-11����iotָ������������������Ϻ� SIGABRT һ����
// # SIGBUS    7    �Ƿ���ַ�������ڴ��ַ����(alignment)����eg: ����һ���ĸ��ֳ���
//                  �����������ַ���� 4 �ı�����
// # SIGFPE    8    �ڷ��������������������ʱ������������������������󣬻������������
//                  ��Ϊ 0 ���������е������Ĵ���
// # SIGKILL   9    ��������������������С����źŲ��ܱ�����������ͺ��ԡ�
// # SIGUSR1   10   �����û�ʹ�á�
// # SIGSEGV   11   ��ͼ����δ������Լ����ڴ棬����ͼ��û��дȨ�޵��ڴ��ַд���ݡ�
// # SIGUSR2   12   �����û�ʹ�á�
// # SIGPIPE   13   �ܵ�����(broken pipe)��дһ��û�ж��˿ڵĹܵ���
// # SIGALRM   14   ʱ�Ӷ�ʱ�źţ��������ʵ�ʵ�ʱ���ʱ��ʱ�䡣alarm ����ʹ�ø��źš�
// # SIGTERM   15   �������(terminate)�źţ��� SIGKILL ��ͬ���Ǹ��źſ��Ա������ʹ���
//                  ͨ������Ҫ������Լ������˳���shell ���� kill ȱʡ��������źš�
// # SIGSTKFLT 16   Э��������ջ����(stack fault)��
// # SIGCHLD   17   �ӽ��̽���ʱ�������̻��յ�����źš�
// # SIGCONT   18   ��һ��ֹͣ(stopped)�Ľ��̼���ִ�С����źŲ��ܱ�������������һ��
//                  handler ���ó������� stopped ״̬��Ϊ����ִ��ʱ����ض��Ĺ���������
//                  ������ʾ��ʾ����
// # SIGSTOP   19   ֹͣ(stopped)���̵�ִ�С�ע������terminate�Լ�interrupt������: 
//                  �ý��̻�δ������ֻ����ִͣ�С����źŲ��ܱ��������������ԡ�
// # SIGTSTP   20   ֹͣ���̵����У������źſ��Ա�����ͺ��ԡ��û�����SUSP�ַ�ʱ(ͨ����^Z)
//                  ��������źš�
// # SIGTTIN   21   ����̨��ҵҪ���û��ն˶�����ʱ������ҵ�е����н��̻��յ����źš�ȱʡʱ
//                  ��Щ���̻�ִֹͣ�С�
// # SIGTTOU   22   ������SIGTTIN������д�ն�(���޸��ն�ģʽ)ʱ�յ���
// # SIGURG    23   �� "����" ���ݻ����(out-of-band) ���ݵ��� socket ʱ������
// # SIGXCPU   24   ����CPUʱ����Դ���ơ�������ƿ�����getrlimit/setrlimit����ȡ�͸ı䡣
// # SIGXFSZ   25   �����ļ���С��Դ���ơ�
// # SIGVTALRM 26   ����ʱ���źš������� SIGALRM�����Ǽ�����Ǹý���ռ�õ�CPUʱ�䡣
// # SIGPROF   27   ������SIGALRM/SIGVTALRM���������ý����õ�CPUʱ���Լ�ϵͳ���õ�ʱ�䡣
// # SIGWINCH  28   �ն��Ӵ��ĸı�ʱ������
// # SIGIO     29   �ļ�������׼�����������Կ�ʼ��������/���������
// # SIGPWR    30   Power failure.
// # SIGSYS    31   (?)
//-----------------------------------------------------------------------------
void CApplication::InitSignals()
{
    int i;

    // ����ĳЩ�ź�
    int nIgnoreSignals[] = {SIGHUP, SIGINT, SIGQUIT, SIGPIPE, SIGTSTP, SIGTTIN, 
        SIGTTOU, SIGXCPU, SIGCHLD, SIGPWR, SIGALRM};
    for (i = 0; i < sizeof(nIgnoreSignals)/sizeof(int); i++)
        signal(nIgnoreSignals[i], SIG_IGN);

    // ��װ�����Ƿ������źŴ�����
    int nFatalSignals[] = {SIGILL, SIGBUS, SIGFPE, SIGSEGV};
    for (i = 0; i < sizeof(nFatalSignals)/sizeof(int); i++)
        signal(nFatalSignals[i], FatalErrorSignalHandler);

    // ��װ�����˳��źŴ�����
    int nExitSignals[] = {SIGTERM/*, SIGABRT*/};
    for (i = 0; i < sizeof(nExitSignals)/sizeof(int); i++)
        signal(nExitSignals[i], ExitProgramSignalHandler);
}

//-----------------------------------------------------------------------------
// ����: �ر��ն�
//-----------------------------------------------------------------------------
void CApplication::CloseTerminal()
{
    close(0);  // �رձ�׼����(stdin)
    /*
    close(1);  // �رձ�׼���(stdout)
    close(2);  // �رձ�׼�������(stderr)
    */
}

//-----------------------------------------------------------------------------
// ����: Ӧ�ó�������� (����� m_bInitialized ��־)
//-----------------------------------------------------------------------------
void CApplication::DoFinalize()
{
    try { if (m_pMainServer) m_pMainServer->Finalize(); } catch (...) {}
    try { pExtension->ProgramExit(); } catch (...) {}
    try { FreeMainServer(); } catch (...) {}
}

//-----------------------------------------------------------------------------
// ����: ���������в���
// ����:
//   true  - �ɹ�
//   false - ʧ�ܣ������˳� (���������в�������ȷ)
//-----------------------------------------------------------------------------
bool CApplication::ParseArguments(int nArgc, char *sArgv[])
{
    // �ȼ�¼�����в���
    m_ArgList.clear();
    for (int i = 0; i < nArgc; i++)
        m_ArgList.push_back(sArgv[i]);

    // ���� pExtension ����
    return pExtension->ParseArguments(nArgc, sArgv);
}

//-----------------------------------------------------------------------------
// ����: Ӧ�ó����ʼ�� (����ʼ��ʧ�����׳��쳣)
//-----------------------------------------------------------------------------
void CApplication::Initialize()
{
    CheckAppExtensionExists();

    try
    {
        // �ڳ�ʼ���׶�Ҫ�����˳��ź�
        CSignalMasker SigMasker(true);
        SigMasker.SetSignals(1, SIGTERM);
        SigMasker.Block();

        InitExeName();
        pExtension->ShowWelcomeMessage();
        m_SystemOptions.InitDefaultOptions();
        pExtension->InitSystemOptions(m_SystemOptions);
        pExtension->InitServerOptions(m_ServerOptions);
        CheckMultiInstance();
        if (m_SystemOptions.bIsDaemon) InitDaemon();
        
        InitSignals();
        ApplySystemOptions();
        CreateMainServer();
        pExtension->ProgramInit();
        m_pMainServer->Initialize();
        pExtension->ShowStartedMessage();
        if (m_SystemOptions.bIsDaemon) CloseTerminal();
        m_bInitialized = true;
        
    }
    catch (CException& e)
    {
        pExtension->ShowStartFailMessage();
        DoFinalize();
        throw e;
    }
}

//-----------------------------------------------------------------------------
// ����: Ӧ�ó��������
//-----------------------------------------------------------------------------
void CApplication::Finalize()
{
    if (m_bInitialized)
    {
        DoFinalize();
        m_bInitialized = false;
    }
}

//-----------------------------------------------------------------------------
// ����: ��ʼ����Ӧ�ó���
//-----------------------------------------------------------------------------
void CApplication::Run()
{
    // ���̱���ֹʱ����ת���˴�����������
    if (sigsetjmp(ProcExitJmpBuf, 0)) return;

    if (m_pMainServer)  
        m_pMainServer->Run();
}

//-----------------------------------------------------------------------------
// ����: ȡ�ÿ�ִ���ļ����ڵ�·��
//-----------------------------------------------------------------------------
string CApplication::GetExePath()
{
    return ExtractFilePath(m_strExeName);
}

//-----------------------------------------------------------------------------
// ����: ȡ�������в����ַ��� (nIndex: 0-based)
//-----------------------------------------------------------------------------
string CApplication::GetArgString(int nIndex)
{
    if (nIndex >= 0 && nIndex < m_ArgList.size())
        return m_ArgList[nIndex];
    else
        return "";
}

///////////////////////////////////////////////////////////////////////////////

} // namespace ise
