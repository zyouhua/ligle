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
// ��������

//-----------------------------------------------------------------------------
//-- ���ݿ����ò���:

// ���������
const int MAX_MAIN_DB_POOL_CONNS = 1000;
const int MAX_FRI_DB_POOL_CONNS  = 1000;

///////////////////////////////////////////////////////////////////////////////
// class CDatabaseManager - ���ݿ��������

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

    // ��ʼ�� (�� CAppGlobalData::Initialize() ����)
    void Initialize();

    CDatabase* GetTrackerDb() { return &m_TrackerDb; }
};

///////////////////////////////////////////////////////////////////////////////
// class CDbOperations - ���ݿ�ҵ�������

class CDbOperations
{
private:
    CDatabaseManager *m_pDbManager;     // CDatabaseManager ��������
protected:
    void LogException(const CException& e);
public:
    CDbOperations();
    virtual ~CDbOperations();

    // ����һ���򵥵Ĳ�ѯ������Ƿ�����������
    bool SelfTest();

    // ���� CDatabaseManager ����
    CDatabaseManager* GetDbManager() { return m_pDbManager; }
};

///////////////////////////////////////////////////////////////////////////////

#endif // _DATABASE_H_ 
