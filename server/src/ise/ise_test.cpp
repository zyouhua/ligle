///////////////////////////////////////////////////////////////////////////////
// Iris Server Engine (ISE)
//
// �ļ�����: ise_test.cpp
// ��������: ISE ��չģ�����
// ����޸�: 2005-04-19
///////////////////////////////////////////////////////////////////////////////

#include "ise_test.h"
#include "ise_system.h"
#include "ise_sysutils.h"
#include "ise_db_mysql.h"

namespace ise
{

///////////////////////////////////////////////////////////////////////////////
// ȫ�ֱ���

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
