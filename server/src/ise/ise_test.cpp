///////////////////////////////////////////////////////////////////////////////
// Iris Server Engine (ISE)
//
// 文件名称: ise_test.cpp
// 功能描述: ISE 扩展模块测试
// 最后修改: 2005-04-19
///////////////////////////////////////////////////////////////////////////////

#include "ise_test.h"
#include "ise_system.h"
#include "ise_sysutils.h"
#include "ise_db_mysql.h"

namespace ise
{

///////////////////////////////////////////////////////////////////////////////
// 全局变量

CAppExtension AppExtension;
CMySqlDatabase MySqlDbP2p;

///////////////////////////////////////////////////////////////////////////////
// class CAppExtension

void CAppExtension::ShowWelcomeMessage()
{
    cout << "test starting..." << endl;
}

void CAppExtension::ShowStartedMessage()
{
    char *pMsg = "test started.";

    cout << pMsg << endl;
    Logger.Write(pMsg);
}

void CAppExtension::ShowStartFailMessage()
{
    char *pMsg = "fail to start test.";

    cout << pMsg << endl;
    Logger.Write(pMsg);
}

void CAppExtension::InitSystemOptions(CSystemOptions& SysOpt)
{
    SysOpt.strLogFileName = Application.GetExePath() + "log";
    SysOpt.bIsDaemon = true;
}

void CAppExtension::InitServerOptions(CServerOptions& SvrOpt)
{
    SvrOpt.SetServerType(ST_UDP | ST_TCP);
}

void CAppExtension::ProgramExit()
{
    char *pMsg = "test stoped.";

    cout << pMsg << endl;
    Logger.Write(pMsg);
}

///////////////////////////////////////////////////////////////////////////////

} // namespace ise
