///////////////////////////////////////////////////////////////////////////////
// Iris Server Engine (ISE)
//
// 文件名称: ise_db_mysql.cpp
// 功能描述: MySQL数据库存取
// 最后修改: 2005-05-02
///////////////////////////////////////////////////////////////////////////////

#include "ise_db_mysql.h"
#include "ise_system.h"
#include "ise_sysutils.h"

using namespace ise;

namespace ise
{

///////////////////////////////////////////////////////////////////////////////
// class CMySqlConnection

CMySqlConnection::CMySqlConnection(CDatabase* pDatabase) :
    CDbConnection(pDatabase)
{
}

CMySqlConnection::~CMySqlConnection()
{
}

//-----------------------------------------------------------------------------
// 描述: 建立连接 (若失败则抛出异常)
//-----------------------------------------------------------------------------
void CMySqlConnection::DoConnect()
{
    if (mysql_init(&m_ConnObject) == NULL)
        throw CDbException("mysql_init error", __FILE__, __LINE__);
    
    if (mysql_real_connect(&m_ConnObject, 
        m_pDatabase->GetDbConnParams()->GetHostName().c_str(), 
        m_pDatabase->GetDbConnParams()->GetUserName().c_str(), 
        m_pDatabase->GetDbConnParams()->GetPassword().c_str(), 
        m_pDatabase->GetDbConnParams()->GetDbName().c_str(), 
        m_pDatabase->GetDbConnParams()->GetPort(), NULL, 0) == NULL)
    {
        throw CDbException(mysql_error(&m_ConnObject), __FILE__, __LINE__);
    }
}

//-----------------------------------------------------------------------------
// 描述: 断开连接
//-----------------------------------------------------------------------------
void CMySqlConnection::DoDisconnect()
{
    mysql_close(&m_ConnObject);
}

///////////////////////////////////////////////////////////////////////////////
// class CMySqlDataSet

CMySqlDataSet::CMySqlDataSet(CDbQuery* pDbQuery) :
    CDbDataSet(pDbQuery),
    m_pRes(NULL),
    m_pRow(NULL)
{
}

CMySqlDataSet::~CMySqlDataSet()
{
    FreeDataSet();
}

MYSQL& CMySqlDataSet::GetConnObject()
{
    return ((CMySqlConnection*)m_pDbQuery->GetDbConnection())->GetConnObject();
}

//-----------------------------------------------------------------------------
// 描述: 释放数据集
//-----------------------------------------------------------------------------
void CMySqlDataSet::FreeDataSet()
{
    if (m_pRes)
        mysql_free_result(m_pRes);
}

//-----------------------------------------------------------------------------
// 描述: 初始化数据集 (若初始化失败则抛出异常)
//-----------------------------------------------------------------------------
void CMySqlDataSet::InitDataSet()
{
    // 从MySQL服务器一次性获取所有行
    m_pRes = mysql_store_result(&GetConnObject());

    // 如果获取失败
    if (!m_pRes)
    {
        if (mysql_errno(&GetConnObject()) != 0)
            throw CDbException(mysql_error(&GetConnObject()), __FILE__, __LINE__);
        else
            throw CDbException("mysql_store_result error", __FILE__, __LINE__);
    }
}

//-----------------------------------------------------------------------------
// 描述: 初始化数据集各字段的定义
//-----------------------------------------------------------------------------
void CMySqlDataSet::InitFieldDefs()
{
    MYSQL_FIELD *pMySqlField;
    CDbFieldDef* pFieldDef;
    int nFieldCount;

    nFieldCount = mysql_num_fields(m_pRes);
    for (int i = 0; i < nFieldCount; i++)
    {
        pMySqlField = mysql_fetch_field(m_pRes);
        pFieldDef = m_pDbQuery->GetDatabase()->CreateDbFieldDef(); 
        pFieldDef->SetData(pMySqlField->name);
        m_FieldDefList.Add(pFieldDef);
    }
}

//-----------------------------------------------------------------------------
// 描述: 将游标指向起始位置(第一条记录之前)
//-----------------------------------------------------------------------------
bool CMySqlDataSet::Rewind()
{
    if (GetRecordCount() > 0)
    {
        mysql_data_seek(m_pRes, 0);
        return true;
    }
    else
        return false;
}

//-----------------------------------------------------------------------------
// 描述: 将游标指向下一条记录
//-----------------------------------------------------------------------------
bool CMySqlDataSet::Next()
{
    m_pRow = mysql_fetch_row(m_pRes);
    if (m_pRow)
    {
        CDbField* pField;
        int nFieldCount;
        uint* pLengths;

        nFieldCount = mysql_num_fields(m_pRes);
        pLengths = (uint*)mysql_fetch_lengths(m_pRes);

        for (int i = 0; i < nFieldCount; i++)
        {
            if (i < m_FieldList.Count())
            {
                pField = m_FieldList[i];
            }
            else
            {
                pField = m_pDbQuery->GetDatabase()->CreateDbField();
                m_FieldList.Add(pField);
            }

            pField->SetData(m_pRow[i], pLengths[i]);
        }
    }

    return (m_pRow != NULL);
}

//-----------------------------------------------------------------------------
// 描述: 取得记录总数
// 备注: mysql_num_rows 实际上只是直接返回 m_pRes->row_count，所以效率很高。
//-----------------------------------------------------------------------------
uint64 CMySqlDataSet::GetRecordCount()
{
    if (m_pRes)
        return (uint64)mysql_num_rows(m_pRes);
    else
        return 0;
}

///////////////////////////////////////////////////////////////////////////////
// class CMySqlQuery - mysql查询类

CMySqlQuery::CMySqlQuery(CDatabase* pDatabase) :
    CDbQuery(pDatabase)
{
}

CMySqlQuery::~CMySqlQuery()
{
}

MYSQL& CMySqlQuery::GetConnObject()
{
    return ((CMySqlConnection*)m_pDbConnection)->GetConnObject();
}

//-----------------------------------------------------------------------------
// 描述: 执行SQL (无查询结果，若失败则抛出异常)
//-----------------------------------------------------------------------------
void CMySqlQuery::DoExecute()
{
    int r;

    for (int nTimes = 0; nTimes < 2; nTimes++)
    {
        r = mysql_real_query(&GetConnObject(), m_strSql.c_str(), m_strSql.length());

        // 如果执行SQL失败
        if (r != 0)
        {
            // 如果是首次，并且错误类型为连接丢失，则重试连接
            if (nTimes == 0)
            {
                int nErrNo = mysql_errno(&GetConnObject());
                if (nErrNo == CR_SERVER_GONE_ERROR || nErrNo == CR_SERVER_LOST)
                {
                    // 强制重新连接
                    GetDbConnection()->ActivateConnection(true);
                    continue;
                }
            }
    
            // 否则抛出异常
            string strErrMsg;
            FormatString(strErrMsg, "%s; Error: %s", m_strSql.c_str(), mysql_error(&GetConnObject()));
            throw CDbException(strErrMsg.c_str(), __FILE__, __LINE__);
        }
        else break;
    }
}

//-----------------------------------------------------------------------------
// 描述: 转换字符串使之在SQL中合法
//-----------------------------------------------------------------------------
string CMySqlQuery::DoValidateString(const string& str)
{
    int nSrcLen = str.size();
    CBuffer Buffer(nSrcLen * 2 + 1);
    char *pEnd;

    // 注: 即使 str 中间含有 '\0' 字符，str.c_str() 也照样返回整个缓冲区， 
    // c_str() 的操作如下: { data()[size()] = 0; return data(); }
    // 另: string::size() <=> string::length()

    pEnd = (char*)Buffer.Data();
    pEnd += mysql_escape_string(pEnd, str.c_str(), nSrcLen);
    *pEnd = '\0';

    return (char*)Buffer.Data();
}

//-----------------------------------------------------------------------------
// 描述: 获取执行SQL后受影响的行数
//-----------------------------------------------------------------------------
uint CMySqlQuery::DoGetAffectedRowCount()
{
    uint nResult = 0;

    if (m_pDbConnection)
    {
        nResult = (uint)mysql_affected_rows(&GetConnObject());
        if (nResult < 0) nResult = 0;
    }

    return nResult;
}

//-----------------------------------------------------------------------------
// 描述: 获取最后一条插入语句的自增ID的值
//-----------------------------------------------------------------------------
uint64 CMySqlQuery::DoGetInsertId()
{
    uint64 nResult = 0;

    if (m_pDbConnection)
        nResult = (uint64)mysql_insert_id(&GetConnObject());

    return nResult;
}

///////////////////////////////////////////////////////////////////////////////

} // namespace ise
