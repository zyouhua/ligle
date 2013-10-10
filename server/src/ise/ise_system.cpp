///////////////////////////////////////////////////////////////////////////////
// Iris Server Engine (ISE)
//
// 文件名称: ise_system.cpp
// 功能描述: 系统框架
// 最后修改: 2005-12-10
///////////////////////////////////////////////////////////////////////////////

#include "ise_system.h"
#include "ise_sysutils.h"
#include "ise_socket.h"

namespace ise
{

///////////////////////////////////////////////////////////////////////////////
// 全局变量定义

// 日志对象
CLogger Logger;
// 应用程序对象
CApplication Application;
// 应用程序功能扩展对象指针 (指向用户创建的对象)
CCustomExtension *pExtension;

// 用于进程退出时长跳转
static sigjmp_buf ProcExitJmpBuf;

///////////////////////////////////////////////////////////////////////////////
// 信号处理器

//-----------------------------------------------------------------------------
// 描述: 正常退出程序 信号处理器
//-----------------------------------------------------------------------------
void ExitProgramSignalHandler(int nSigNo)
{
    static bool bInHandler = false;
    if (bInHandler) return;
    bInHandler = true;

    // 停止主线程循环
    Application.SetTerminated(true);

    Logger.Write("ExitProgramSignalHandler. (signal:%d)", nSigNo);

    siglongjmp(ProcExitJmpBuf, 1);
}

//-----------------------------------------------------------------------------
// 描述: 致命非法操作 信号处理器
//-----------------------------------------------------------------------------
void FatalErrorSignalHandler(int nSigNo)
{
    static bool bInHandler = false;
    if (bInHandler) exit(1);
    bInHandler = true;

    // 停止主线程循环
    Application.SetTerminated(true);

    Logger.Write("Fatal error. (signal:%d)", nSigNo);

    /*
    // 如何 dump core ???
    chdir(Application.GetExePath().c_str());
    signal(nSigNo, SIG_DFL);
    pthread_kill(pthread_self(), nSigNo);
    */

    exit(1);
}

///////////////////////////////////////////////////////////////////////////////
// class CSystemOptions

//-----------------------------------------------------------------------------
// 描述: 初始化默认配置
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
// 描述: 检查用户是否创建了 CCustomExtension 对象
//-----------------------------------------------------------------------------
void CApplication::CheckAppExtensionExists()
{
    if (!pExtension)
        throw CException("CCustomExtension object not found", __FILE__, __LINE__);
}

//-----------------------------------------------------------------------------
// 描述: 检查是否运行了多个程序实体
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
// 描述: 应用 系统配置
//-----------------------------------------------------------------------------
void CApplication::ApplySystemOptions()
{
    Logger.SetFileName(m_SystemOptions.strLogFileName);
}

//-----------------------------------------------------------------------------
// 描述: 创建主服务器
//-----------------------------------------------------------------------------
void CApplication::CreateMainServer()
{
    if (!m_pMainServer)
        m_pMainServer = new CMainServer;
}

//-----------------------------------------------------------------------------
// 描述: 释放主服务器
//-----------------------------------------------------------------------------
void CApplication::FreeMainServer()
{
    if (m_pMainServer) delete m_pMainServer;
    m_pMainServer = NULL;
}

//-----------------------------------------------------------------------------
// 描述: 取得自身文件的全名，并初始化 m_strExeName
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
// 描述: 守护模式初始化
//-----------------------------------------------------------------------------
void CApplication::InitDaemon()
{
    int r;
    
    r = fork();
   
    if (r < 0)
        throw CException("init daemon error", __FILE__, __LINE__);
    else if (r != 0) 
        exit(0);
    
    // 第一子进程后台继续执行

    // 第一子进程成为新的会话组长和进程组长
    r = setsid();
    
    if (r < 0) exit(1);
      
    // 忽略 SIGHUP 信号 (注: 程序与终端断开时发生，比如启动程序后关闭telnet)
    signal(SIGHUP, SIG_IGN);
    
    // 第一子进程退出
    r = fork();
    
    if (r < 0) exit(1); 
    else if (r != 0) exit(0);
   
    // 第二子进程继续执行，它不再是会话组长

    // 改变当前工作目录
    chdir("/");

    // 重设文件创建掩模
    umask(0);
}

//-----------------------------------------------------------------------------
// 描述: 初始化信号 (信号的安装、忽略等)
//
//  信号名称    值                         信号说明
// ---------  ----  -----------------------------------------------------------
// # SIGHUP    1    本信号在用户终端连接(正常或非正常)结束时发出，通常是在终端的控制进程
//                  结束时，通知同一 session 内的各个作业，这时它们与控制终端不再关联。
// # SIGINT    2    程序终止(interrupt)信号，在用户键入INTR字符(通常是Ctrl-C)时发出。
// # SIGQUIT   3    和 SIGINT 类似，但由QUIT字符 (通常是 Ctrl-\) 来控制。进程在因收到
//                  此信号退出时会产生core文件，在这个意义上类似于一个程序错误信号。
// # SIGILL    4    执行了非法指令。通常是因为可执行文件本身出现错误，或者试图执行数据段。
//                  堆栈溢出时也有可能产生这个信号。
// # SIGTRAP   5    由断点指令或其它 trap 指令产生。由 debugger 使用。
// # SIGABRT   6    程序自己发现错误并调用 abort 时产生。
// # SIGIOT    6    在PDP-11上由iot指令产生。在其它机器上和 SIGABRT 一样。
// # SIGBUS    7    非法地址，包括内存地址对齐(alignment)出错。eg: 访问一个四个字长的
//                  整数，但其地址不是 4 的倍数。
// # SIGFPE    8    在发生致命的算术运算错误时发出。不仅包括浮点运算错误，还包括溢出及除
//                  数为 0 等其它所有的算术的错误。
// # SIGKILL   9    用来立即结束程序的运行。本信号不能被阻塞、处理和忽略。
// # SIGUSR1   10   留给用户使用。
// # SIGSEGV   11   试图访问未分配给自己的内存，或试图往没有写权限的内存地址写数据。
// # SIGUSR2   12   留给用户使用。
// # SIGPIPE   13   管道破裂(broken pipe)，写一个没有读端口的管道。
// # SIGALRM   14   时钟定时信号，计算的是实际的时间或时钟时间。alarm 函数使用该信号。
// # SIGTERM   15   程序结束(terminate)信号，与 SIGKILL 不同的是该信号可以被阻塞和处理。
//                  通常用来要求程序自己正常退出。shell 命令 kill 缺省产生这个信号。
// # SIGSTKFLT 16   协处理器堆栈错误(stack fault)。
// # SIGCHLD   17   子进程结束时，父进程会收到这个信号。
// # SIGCONT   18   让一个停止(stopped)的进程继续执行。本信号不能被阻塞。可以用一个
//                  handler 来让程序在由 stopped 状态变为继续执行时完成特定的工作。例如
//                  重新显示提示符。
// # SIGSTOP   19   停止(stopped)进程的执行。注意它和terminate以及interrupt的区别: 
//                  该进程还未结束，只是暂停执行。本信号不能被阻塞、处理或忽略。
// # SIGTSTP   20   停止进程的运行，但该信号可以被处理和忽略。用户键入SUSP字符时(通常是^Z)
//                  发出这个信号。
// # SIGTTIN   21   当后台作业要从用户终端读数据时，该作业中的所有进程会收到此信号。缺省时
//                  这些进程会停止执行。
// # SIGTTOU   22   类似于SIGTTIN，但在写终端(或修改终端模式)时收到。
// # SIGURG    23   有 "紧急" 数据或带外(out-of-band) 数据到达 socket 时产生。
// # SIGXCPU   24   超过CPU时间资源限制。这个限制可以由getrlimit/setrlimit来读取和改变。
// # SIGXFSZ   25   超过文件大小资源限制。
// # SIGVTALRM 26   虚拟时钟信号。类似于 SIGALRM，但是计算的是该进程占用的CPU时间。
// # SIGPROF   27   类似于SIGALRM/SIGVTALRM，但包括该进程用的CPU时间以及系统调用的时间。
// # SIGWINCH  28   终端视窗的改变时发出。
// # SIGIO     29   文件描述符准备就绪，可以开始进行输入/输出操作。
// # SIGPWR    30   Power failure.
// # SIGSYS    31   (?)
//-----------------------------------------------------------------------------
void CApplication::InitSignals()
{
    int i;

    // 忽略某些信号
    int nIgnoreSignals[] = {SIGHUP, SIGINT, SIGQUIT, SIGPIPE, SIGTSTP, SIGTTIN, 
        SIGTTOU, SIGXCPU, SIGCHLD, SIGPWR, SIGALRM};
    for (i = 0; i < sizeof(nIgnoreSignals)/sizeof(int); i++)
        signal(nIgnoreSignals[i], SIG_IGN);

    // 安装致命非法操作信号处理器
    int nFatalSignals[] = {SIGILL, SIGBUS, SIGFPE, SIGSEGV};
    for (i = 0; i < sizeof(nFatalSignals)/sizeof(int); i++)
        signal(nFatalSignals[i], FatalErrorSignalHandler);

    // 安装正常退出信号处理器
    int nExitSignals[] = {SIGTERM/*, SIGABRT*/};
    for (i = 0; i < sizeof(nExitSignals)/sizeof(int); i++)
        signal(nExitSignals[i], ExitProgramSignalHandler);
}

//-----------------------------------------------------------------------------
// 描述: 关闭终端
//-----------------------------------------------------------------------------
void CApplication::CloseTerminal()
{
    close(0);  // 关闭标准输入(stdin)
    /*
    close(1);  // 关闭标准输出(stdout)
    close(2);  // 关闭标准错误输出(stderr)
    */
}

//-----------------------------------------------------------------------------
// 描述: 应用程序结束化 (不检查 m_bInitialized 标志)
//-----------------------------------------------------------------------------
void CApplication::DoFinalize()
{
    try { if (m_pMainServer) m_pMainServer->Finalize(); } catch (...) {}
    try { pExtension->ProgramExit(); } catch (...) {}
    try { FreeMainServer(); } catch (...) {}
}

//-----------------------------------------------------------------------------
// 描述: 解释命令行参数
// 返回:
//   true  - 成功
//   false - 失败，程序退出 (比如命令行参数不正确)
//-----------------------------------------------------------------------------
bool CApplication::ParseArguments(int nArgc, char *sArgv[])
{
    // 先记录命令行参数
    m_ArgList.clear();
    for (int i = 0; i < nArgc; i++)
        m_ArgList.push_back(sArgv[i]);

    // 交给 pExtension 解释
    return pExtension->ParseArguments(nArgc, sArgv);
}

//-----------------------------------------------------------------------------
// 描述: 应用程序初始化 (若初始化失败则抛出异常)
//-----------------------------------------------------------------------------
void CApplication::Initialize()
{
    CheckAppExtensionExists();

    try
    {
        // 在初始化阶段要屏蔽退出信号
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
// 描述: 应用程序结束化
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
// 描述: 开始运行应用程序
//-----------------------------------------------------------------------------
void CApplication::Run()
{
    // 进程被终止时长跳转到此处并立即返回
    if (sigsetjmp(ProcExitJmpBuf, 0)) return;

    if (m_pMainServer)  
        m_pMainServer->Run();
}

//-----------------------------------------------------------------------------
// 描述: 取得可执行文件所在的路径
//-----------------------------------------------------------------------------
string CApplication::GetExePath()
{
    return ExtractFilePath(m_strExeName);
}

//-----------------------------------------------------------------------------
// 描述: 取得命令行参数字符串 (nIndex: 0-based)
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
