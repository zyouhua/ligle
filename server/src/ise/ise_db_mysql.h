///////////////////////////////////////////////////////////////////////////////
// Iris Server Engine (ISE)
// ise_db_mysql.h
// Classes:
//   > CMySqlConnection       - MySQL���ݿ�������
//   > CMySqlDataSet          - MySQL���ݼ���
//   > CMySqlQuery            - MySQL���ݲ�ѯ����
//   > CMysqlDatabase         - MySQL���ݿ���
///////////////////////////////////////////////////////////////////////////////

#ifndef _ISE_DB_MYSQL_H_
#define _ISE_DB_MYSQL_H_

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <errmsg.h>
#include <mysql.h>
#include <string>

#include "ise_classes.h"
#include "ise_database.h"

using namespace ise;

namespace ise
{

///////////////////////////////////////////////////////////////////////////////
// ��ǰ����

class CMySqlQuery;

///////////////////////////////////////////////////////////////////////////////
// class CMySqlConnection - MySQL���ݿ�������

class CMySqlConnection : public CDbConnection
{
private:
    MYSQL m_ConnObject;            // MySQL���Ӷ���

public:
    CMySqlConnection(CDatabase *pDatabase);
    virtual ~CMySqlConnection();

    // �������� (��ʧ�����׳��쳣)
    virtual void DoConnect();
    // �Ͽ�����
    virtual void DoDisconnect();

    // ȡ��MySQL���Ӷ���
    MYSQL& GetConnObject() { return m_ConnObject; }
};

///////////////////////////////////////////////////////////////////////////////
// class CMySqlDataSet - MySQL���ݼ���

class CMySqlDataSet : public CDbDataSet
{
private:
    MYSQL_RES* m_pRes;      // MySQL��ѯ�����
    MYSQL_ROW m_pRow;       // MySQL��ѯ�����

private:
    MYSQL& GetConnObject();
    void FreeDataSet();

protected:
    virtual void InitDataSet();
    virtual void InitFieldDefs();

public:
    CMySqlDataSet(CDbQuery* pDbQuery);
    virtual ~CMySqlDataSet();

    virtual bool Rewind();
    virtual bool Next();

    virtual uint64 GetRecordCount();
};

///////////////////////////////////////////////////////////////////////////////
// class CMySqlQuery - MySQL��ѯ����

class CMySqlQuery : public CDbQuery
{
private:
    MYSQL& GetConnObject();

protected:
    virtual void DoExecute();
    virtual string DoValidateString(const string& str);
    virtual uint DoGetAffectedRowCount();
    virtual uint64 DoGetInsertId();

public:
    CMySqlQuery(CDatabase *pDatabase);
    virtual ~CMySqlQuery();

};

///////////////////////////////////////////////////////////////////////////////
// class CMySqlDatabase

class CMySqlDatabase : public CDatabase
{
public:
    virtual CDbConnection* CreateDbConnection() { return new CMySqlConnection(this); }
    virtual CDbQuery* CreateDbQuery() { return new CMySqlQuery(this); }
    virtual CDbDataSet* CreateDbDataSet(CDbQuery* pDbQuery) { return new CMySqlDataSet(pDbQuery); }
};

///////////////////////////////////////////////////////////////////////////////

} // namespace ise

#endif // _ISE_DB_MYSQL_H_ 
