///////////////////////////////////////////////////////////////////////////////
// P2SP Tracker Server
//
// �ļ�����: database.cpp
// ��������: ���ݿ�ҵ�����
// ����޸�: 2005-04-27
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
// ����: ��ʼ�����ݿ����Ӳ���
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
// ����: ��ʼ�����ݿ����ò���
//----------------------------------------------------------------------------
void CDatabaseManager::InitDbOptions()
{
    m_TrackerDb.GetDbOptions()->SetMaxDbConnections(MAX_MAIN_DB_POOL_CONNS);
}

//----------------------------------------------------------------------------
// ����: ��ʼ�� (�� CAppGlobalData::Initialize() ����)
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
// ����: ���쳣д����־
//----------------------------------------------------------------------------
void CDbOperations::LogException(const CException& e)
{
    Logger.Write(e.MakeLogMsg().c_str());
}

//-----------------------------------------------------------------------------
// ����: ����һ���򵥵Ĳ�ѯ������Ƿ�����������
// ����: 
//   true  - �ɹ�
//   false - ʧ��
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
