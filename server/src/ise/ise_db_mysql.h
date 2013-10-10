///////////////////////////////////////////////////////////////////////////////
// Iris Server Engine (ISE)
// ise_db_mysql.h
// Classes:
//   > CMySqlConnection       - MySQL数据库连接类
//   > CMySqlDataSet          - MySQL数据集类
//   > CMySqlQuery            - MySQL数据查询器类
//   > CMysqlDatabase         - MySQL数据库类
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
// 提前声明

class CMySqlQuery;

///////////////////////////////////////////////////////////////////////////////
// class CMySqlConnection - MySQL数据库连接类

class CMySqlConnection : public CDbConnection
{
private:
    MYSQL m_ConnObject;            // MySQL连接对象

public:
    CMySqlConnection(CDatabase *pDatabase);
    virtual ~CMySqlConnection();

    // 建立连接 (若失败则抛出异常)
    virtual void DoConnect();
    // 断开连接
    virtual void DoDisconnect();

    // 取得MySQL连接对象
    MYSQL& GetConnObject() { return m_ConnObject; }
};

///////////////////////////////////////////////////////////////////////////////
// class CMySqlDataSet - MySQL数据集类

class CMySqlDataSet : public CDbDataSet
{
private:
    MYSQL_RES* m_pRes;      // MySQL查询结果集
    MYSQL_ROW m_pRow;       // MySQL查询结果行

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
// class CMySqlQuery - MySQL查询器类

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
