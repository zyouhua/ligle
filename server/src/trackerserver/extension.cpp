///////////////////////////////////////////////////////////////////////////////
// P2SP Tracker Server
//
// 文件名称: extension.cpp
// 功能描述: ISE应用扩展接口实现
// 最后修改: 2005-04-27
///////////////////////////////////////////////////////////////////////////////

#include "extension.h"
#include "global.h"
#include "dispatcher.h" 
#include <time.h>

///////////////////////////////////////////////////////////////////////////////
// 全局变量

CAppExtension AppExtension;

///////////////////////////////////////////////////////////////////////////////
// class CAppExtension

//-----------------------------------------------------------------------------
// 描述: 解释命令行参数，参数不正确则返回 false
//-----------------------------------------------------------------------------
bool CAppExtension::ParseArguments(int nArgc, char *sArgv[])
{
    return true;
}

//-----------------------------------------------------------------------------
// 描述: 程序启动时显示欢迎信息
//-----------------------------------------------------------------------------
void CAppExtension::ShowWelcomeMessage()
{
    cout << "******************************************" << endl;
    cout << "*  Tracker Server (1.0 alpha1)           *" << endl;
    cout << "*  (c)2005 All rights reserved.          *" << endl;
    cout << "*  Usage:                                *" << endl;
    cout << "*  Start server:  # ./trackerd           *" << endl;
    cout << "*  Stop server:   # killall trackerd     *" << endl;
    cout << "******************************************" << endl;
    cout << endl;
}

//-----------------------------------------------------------------------------
// 描述: 程序成功启动后显示提示信息
//-----------------------------------------------------------------------------
void CAppExtension::ShowStartedMessage()
{
    string msg = "Tracker Server started.";

    cout << endl << msg << endl;
    Logger.Write(msg.c_str());

    // 初始化随机数种子
    Randomize();

    cout << "Load Data From Database...";
    {
       // CDbOprs_NetMgr db;
//       // db.SelfTest();
       // db.LoadIpRangeFromDb();
       // db.LoadResourceFromDB();
    }

    cout << "Complete!" << endl;
}

//-----------------------------------------------------------------------------
// 描述: 程序启动失败后显示提示信息
//-----------------------------------------------------------------------------
void CAppExtension::ShowStartFailMessage()
{
    string msg = "Fail to start Tracker Server.";

    cout << endl << msg << endl;
    Logger.Write(msg.c_str());
}

//-----------------------------------------------------------------------------
// 描述: 初始化系统配置信息
//-----------------------------------------------------------------------------
void CAppExtension::InitSystemOptions(CSystemOptions& SysOpt)
{
    SysOpt.strLogFileName = Application.GetExePath() + SERVER_LOG_FILE;
    SysOpt.bIsDaemon = true;
    SysOpt.bAllowMultiInstance = false;
}

//-----------------------------------------------------------------------------
// 描述: 初始化服务器配置信息
//-----------------------------------------------------------------------------
void CAppExtension::InitServerOptions(CServerOptions& SvrOpt)
{
    int i;

    // 设置服务器类型
    SvrOpt.SetServerType(ST_UDP | ST_TCP);


    // 设置UDP服务端口
    SvrOpt.SetUdpServerPort(UDP_SERVER_PORT);

    // 设置UDP监听线程数量
    SvrOpt.SetUdpListenerThreadCount(UDP_LISTENER_THREAD_COUNT);

    // 设置UDP请求组别总数
    SvrOpt.SetUdpRequestGroupCount(UDP_SVR_MODULE_COUNT);

    // 设置UDP数据包队列的最大容量 (即可容纳多少个数据包)
    for (i = 0; i < UDP_SVR_MODULE_COUNT; i++)
        SvrOpt.SetUdpRequestQueueCapacity(i, UDP_QUEUE_CAPACITIES[i]);

    // 设置UDP工作者线程个数的上下限
    for (i = 0; i < UDP_SVR_MODULE_COUNT; i++)
        SvrOpt.SetUdpWorkerThreadCount(i, UDP_WORK_THREADS[i].nMin, UDP_WORK_THREADS[i].nMax);


    // 设置TCP请求组别总数
    SvrOpt.SetTcpRequestGroupCount(TCP_SVR_MODULE_COUNT);

    // 设置TCP服务端口
    for (i = 0; i < TCP_SVR_MODULE_COUNT; i++)
        SvrOpt.SetTcpServerPort(i, TCP_SERVER_PORTS[i]);

    // 设置TCP连接队列的容量(即可容纳多少个连接)
    for (i = 0; i < TCP_SVR_MODULE_COUNT; i++)
        SvrOpt.SetTcpRequestQueueCapacity(i, TCP_QUEUE_CAPACITIES[i]);

    // 设置TCP工作者线程个数的上下限
    for (i = 0; i < TCP_SVR_MODULE_COUNT; i++)
        SvrOpt.SetTcpWorkerThreadCount(i, TCP_WORK_THREADS[i].nMin, TCP_WORK_THREADS[i].nMax);


    // 辅助服务线程数量在 CAppDispatcher::InitServerModuleList() 中须重新设置
    SvrOpt.SetHelperThreadCount(0);
}

//-----------------------------------------------------------------------------
// 描述: 创建程序数据分派器对象
//-----------------------------------------------------------------------------
CCustomDispatcher* CAppExtension::CreateDispatcher()
{
    return new CAppDispatcher;
}

//-----------------------------------------------------------------------------
// 描述: 扩展功能的初始化，若初始化失败则抛出异常 (此调用在 ISE 初始化之前)
//-----------------------------------------------------------------------------
void CAppExtension::ProgramInit()
{
    // 设置当前使用的字符集，以便程序能正确进行字符转换。中国地区的字符集有:
    //   zh_CN            zh_TW
    //   zh_CN.gb18030    zh_TW.big5
    //   zh_CN.gb2312     zh_TW.euctw
    //   zh_CN.gbk        zh_TW.utf8
    //   zh_CN.utf8
    setlocale(LC_CTYPE, "zh_CN.gbk");  // <- 仅针对中国大陆
}

//-----------------------------------------------------------------------------
// 描述: 程序退出前的善后处理 (网络服务器已关闭之后，程序数据分派器释放之前)
//-----------------------------------------------------------------------------
void CAppExtension::ProgramExit()
{
    string msg = "Tracker Server stoped.";

    cout << msg << endl;
    Logger.Write(msg.c_str());
}

///////////////////////////////////////////////////////////////////////////////
