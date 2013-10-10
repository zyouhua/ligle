///////////////////////////////////////////////////////////////////////////////
// P2SP Tracker Server
// database.h
///////////////////////////////////////////////////////////////////////////////

#ifndef _DATABASE_H_
#define _DATABASE_H_

#include <stdio.h>
#include <stdlib.h>

#include "ise_system.h"
#include "ise_db_mysql.h"
#include "global.h"

///////////////////////////////////////////////////////////////////////////////
// 常量定义

//-----------------------------------------------------------------------------
//-- 数据库配置参数:

// 最大连接数
const int MAX_MAIN_DB_POOL_CONNS = 1000;
const int MAX_FRI_DB_POOL_CONNS  = 1000;

///////////////////////////////////////////////////////////////////////////////
// class CDatabaseManager - 数据库管理器类

class CDatabaseManager
{
private:
    CMySqlDatabase m_TrackerDb;

private:
    void InitDbConnParams();
    void InitDbOptions();

public:
    CDatabaseManager();
    virtual ~CDatabaseManager();

    // 初始化 (由 CAppGlobalData::Initialize() 调用)
    void Initialize();

    CDatabase* GetTrackerDb() { return &m_TrackerDb; }
};

///////////////////////////////////////////////////////////////////////////////
// class CDbOperations - 数据库业务操作类

class CDbOperations
{
private:
    CDatabaseManager *m_pDbManager;     // CDatabaseManager 对象引用
protected:
    void LogException(const CException& e);
public:
    CDbOperations();
    virtual ~CDbOperations();

    // 测试一个简单的查询，检查是否能正常工作
    bool SelfTest();

    // 返回 CDatabaseManager 对象
    CDatabaseManager* GetDbManager() { return m_pDbManager; }
};

///////////////////////////////////////////////////////////////////////////////

#endif // _DATABASE_H_ 
