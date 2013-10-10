///////////////////////////////////////////////////////////////////////////////
// Iris Server Engine (ISE)
// ise_system.h
///////////////////////////////////////////////////////////////////////////////

#ifndef _ISE_SYSTEM_H_
#define _ISE_SYSTEM_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <signal.h>
#include <ctype.h>
#include <setjmp.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <iostream>
#include <string>

#include "ise_classes.h"
#include "ise_server.h"
#include "ise_sysutils.h"

using namespace std;
using namespace ise;

namespace ise
{

///////////////////////////////////////////////////////////////////////////////
// 全局变量声明

class CApplication;
class CCustomExtension;

// 日志对象
extern CLogger Logger;
// 应用程序对象
extern CApplication Application;
// 应用程序功能扩展对象指针 (指向用户创建的对象)
extern CCustomExtension *pExtension;

///////////////////////////////////////////////////////////////////////////////
// class CSystemOptions - 系统配置类

class CSystemOptions
{
public:
    string strLogFileName;      // 日志文件名 (含路径)
    bool bIsDaemon;             // 是否后台守护程序(daemon)
    bool bAllowMultiInstance;   // 是否允许多个程序实体同时运行

public:
    CSystemOptions() {}
    void InitDefaultOptions();
};

///////////////////////////////////////////////////////////////////////////////
// class CApplication - 应用程序类
//
// 说明:
// 1. 此类是整个服务程序的主框架，全局单例对象(Application)在程序启动时即被创建；
// 2. 此类在 main() 函数中由 CAppController 类控制执行；
// 3. 一般来说，服务程序是由外部发出命令(kill)而退出的。在ISE中，程序收到kill退出命令后
//    (此时当前执行点一定在 Application.Run() 中)，会触发 ExitProgramSignalHandler 
//    信号处理器，进而利用 longjmp 方法使执行点模拟从 Run() 中退出，继而执行 Finalize。
// 4. 若程序发生致命的非法操作错误，会先触发 FatalErrorSignalHandler 信号处理器，
//    然后同样按照正常的析构顺序退出。
// 5. 若程序内部想正常退出，不推荐使用exit函数。而是 Application.SetTeraminted(true).
//    这样才能让程序按照正常的析构顺序退出。

class CApplication
{
private:
    CSystemOptions m_SystemOptions;         // 系统配置
    CServerOptions m_ServerOptions;         // 服务器配置 
    CSeqAllocator m_SeqAllocator;           // 数据包顺序号分配器
    CMultiInstChecker m_MultiInstChecker;   // 程序多个实体重复运行检测器
    CMainServer *m_pMainServer;             // 主服务器
    StringArray m_ArgList;                  // 命令行参数
    string m_strExeName;                    // 可执行文件的全名(含绝对路径)
    bool m_bInitialized;                    // 是否成功初始化
    bool m_bTerminated;                     // 是否应退出的标志

private:
    void CheckAppExtensionExists();
    void CheckMultiInstance();
    void ApplySystemOptions();
    void CreateMainServer();
    void FreeMainServer();
    void InitExeName();
    void InitDaemon();
    void InitSignals();
    void CloseTerminal();
    void DoFinalize();

public:
    CApplication();
    ~CApplication();

    bool ParseArguments(int nArgc, char *sArgv[]);
    void Initialize();
    void Finalize();
    void Run();

    inline CSystemOptions& GetSystemOptions() { return m_SystemOptions; }
    inline CServerOptions& GetServerOptions() { return m_ServerOptions; }
    inline CSeqAllocator& GetSeqAllocator() { return m_SeqAllocator; }
    inline CMainServer& GetMainServer() { return *m_pMainServer; }

    inline void SetTerminated(bool bValue) { m_bTerminated = bValue; }
    inline bool GetTerminated() { return m_bTerminated; }

    // 取得可执行文件的全名(含绝对路径)
    string GetExeName() { return m_strExeName; }
    // 取得可执行文件所在的路径
    string GetExePath();
    // 取得命令行参数个数
    int GetArgCount() { return m_ArgList.size(); }
    // 取得命令行参数字符串 (nIndex: 0-based)
    string GetArgString(int nIndex);
};

///////////////////////////////////////////////////////////////////////////////
// class CCustomExtension - 应用程序业务扩展基类
//
// 说明:
// 1. 此类用于在ISE的基础上扩展实际业务功能；
// 2. ISE的使用者必须继承此类，并在全局范围内创建此对象(在 main 函数运行之前创建)；

class CCustomExtension
{
public:
    CCustomExtension() { pExtension = this; }
    virtual ~CCustomExtension() { pExtension = NULL; }

    // 解释命令行参数，参数不正确则返回 false
    virtual bool ParseArguments(int nArgc, char *sArgv[]) { return true; }

    // 程序启动时显示欢迎信息
    virtual void ShowWelcomeMessage() {}
    // 程序成功启动后显示提示信息
    virtual void ShowStartedMessage() {}
    // 程序启动失败后显示提示信息
    virtual void ShowStartFailMessage() {}

    // 初始化系统配置信息
    virtual void InitSystemOptions(CSystemOptions& SysOpt) = 0;
    // 初始化服务器配置信息
    virtual void InitServerOptions(CServerOptions& SvrOpt) = 0;
    // 创建程序数据分派器对象 (用户可定义自己的程序数据分派器，以便处理业务逻辑)
    virtual CCustomDispatcher* CreateDispatcher() { return new CCustomDispatcher; }

    // 扩展功能的初始化，若初始化失败则抛出异常 (此调用在 ISE 初始化之前)
    virtual void ProgramInit() {}
    // 程序退出前的善后处理 (网络服务器已关闭之后，程序数据分派器释放之前)
    virtual void ProgramExit() {}
};

///////////////////////////////////////////////////////////////////////////////

} // namespace ise

#endif // _ISE_SYSTEM_H_ 
