///////////////////////////////////////////////////////////////////////////////
// P2SP Tracker Server
//
// 文件名称: database.cpp
// 功能描述: 数据库业务操作
// 最后修改: 2005-04-27
///////////////////////////////////////////////////////////////////////////////

#include "database.h"
#include "utilities.h"
#include "dispatcher.h"

///////////////////////////////////////////////////////////////////////////////
// class CDatabaseManager

CDatabaseManager::CDatabaseManager()
{
}

CDatabaseManager::~CDatabaseManager()
{
}

//----------------------------------------------------------------------------
// 描述: 初始化数据库连接参数
//----------------------------------------------------------------------------
void CDatabaseManager::InitDbConnParams()
{
    CConfigManager& ConfigMgr = GetAppGlobalData()->GetConfigManager();
    m_TrackerDb.GetDbConnParams()->SetHostName(ConfigMgr.GetString("Database.Tracker.HostName"));
    m_TrackerDb.GetDbConnParams()->SetUserName(ConfigMgr.GetString("Database.Tracker.UserName"));
    m_TrackerDb.GetDbConnParams()->SetPassword(ConfigMgr.GetString("Database.Tracker.Password"));
    m_TrackerDb.GetDbConnParams()->SetDbName(ConfigMgr.GetString("Database.Tracker.DbName"));
}

//----------------------------------------------------------------------------
// 描述: 初始化数据库配置参数
//----------------------------------------------------------------------------
void CDatabaseManager::InitDbOptions()
{
    m_TrackerDb.GetDbOptions()->SetMaxDbConnections(MAX_MAIN_DB_POOL_CONNS);
}

//----------------------------------------------------------------------------
// 描述: 初始化 (由 CAppGlobalData::Initialize() 调用)
//----------------------------------------------------------------------------
void CDatabaseManager::Initialize()
{
    InitDbConnParams();
    InitDbOptions();
}

///////////////////////////////////////////////////////////////////////////////
// class CDbOperations

CDbOperations::CDbOperations()
{
    m_pDbManager = &GetAppGlobalData()->GetDbManager();
}

CDbOperations::~CDbOperations()
{
}

//----------------------------------------------------------------------------
// 描述: 将异常写入日志
//----------------------------------------------------------------------------
void CDbOperations::LogException(const CException& e)
{
    Logger.Write(e.MakeLogMsg().c_str());
}

//-----------------------------------------------------------------------------
// 描述: 测试一个简单的查询，检查是否能正常工作
// 返回: 
//   true  - 成功
//   false - 失败
//-----------------------------------------------------------------------------
bool CDbOperations::SelfTest()
{
    bool bResult = true;

    try
    {
        CDbQueryWrapper Query(GetDbManager()->GetTrackerDb()->CreateDbQuery());
        CDbDataSetWrapper DataSet;

        Query->SetSql("SELECT (1+2) AS result");
        DataSet = Query->Query();
        DataSet->Next();

        if (DataSet->GetFields(0)->AsInteger() == 3)
            Logger.Write("CDbOperations self test OK!");
    }
    catch(CException& e)
    {
        bResult = false;      
        LogException(e);
    }

    return bResult;
}

///////////////////////////////////////////////////////////////////////////////
