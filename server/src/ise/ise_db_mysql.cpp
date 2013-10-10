///////////////////////////////////////////////////////////////////////////////
// Iris Server Engine (ISE)
//
// �ļ�����: ise_db_mysql.cpp
// ��������: MySQL���ݿ��ȡ
// ����޸�: 2005-05-02
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
// ����: �������� (��ʧ�����׳��쳣)
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
// ����: �Ͽ�����
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
// ����: �ͷ����ݼ�
//-----------------------------------------------------------------------------
void CMySqlDataSet::FreeDataSet()
{
    if (m_pRes)
        mysql_free_result(m_pRes);
}

//-----------------------------------------------------------------------------
// ����: ��ʼ�����ݼ� (����ʼ��ʧ�����׳��쳣)
//-----------------------------------------------------------------------------
void CMySqlDataSet::InitDataSet()
{
    // ��MySQL������һ���Ի�ȡ������
    m_pRes = mysql_store_result(&GetConnObject());

    // �����ȡʧ��
    if (!m_pRes)
    {
        if (mysql_errno(&GetConnObject()) != 0)
            throw CDbException(mysql_error(&GetConnObject()), __FILE__, __LINE__);
        else
            throw CDbException("mysql_store_result error", __FILE__, __LINE__);
    }
}

//-----------------------------------------------------------------------------
// ����: ��ʼ�����ݼ����ֶεĶ���
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
// ����: ���α�ָ����ʼλ��(��һ����¼֮ǰ)
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
// ����: ���α�ָ����һ����¼
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
// ����: ȡ�ü�¼����
// ��ע: mysql_num_rows ʵ����ֻ��ֱ�ӷ��� m_pRes->row_count������Ч�ʺܸߡ�
//-----------------------------------------------------------------------------
uint64 CMySqlDataSet::GetRecordCount()
{
    if (m_pRes)
        return (uint64)mysql_num_rows(m_pRes);
    else
        return 0;
}

///////////////////////////////////////////////////////////////////////////////
// class CMySqlQuery - mysql��ѯ��

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
// ����: ִ��SQL (�޲�ѯ�������ʧ�����׳��쳣)
//-----------------------------------------------------------------------------
void CMySqlQuery::DoExecute()
{
    int r;

    for (int nTimes = 0; nTimes < 2; nTimes++)
    {
        r = mysql_real_query(&GetConnObject(), m_strSql.c_str(), m_strSql.length());

        // ���ִ��SQLʧ��
        if (r != 0)
        {
            // ������״Σ����Ҵ�������Ϊ���Ӷ�ʧ������������
            if (nTimes == 0)
            {
                int nErrNo = mysql_errno(&GetConnObject());
                if (nErrNo == CR_SERVER_GONE_ERROR || nErrNo == CR_SERVER_LOST)
                {
                    // ǿ����������
                    GetDbConnection()->ActivateConnection(true);
                    continue;
                }
            }
    
            // �����׳��쳣
            string strErrMsg;
            FormatString(strErrMsg, "%s; Error: %s", m_strSql.c_str(), mysql_error(&GetConnObject()));
            throw CDbException(strErrMsg.c_str(), __FILE__, __LINE__);
        }
        else break;
    }
}

//-----------------------------------------------------------------------------
// ����: ת���ַ���ʹ֮��SQL�кϷ�
//-----------------------------------------------------------------------------
string CMySqlQuery::DoValidateString(const string& str)
{
    int nSrcLen = str.size();
    CBuffer Buffer(nSrcLen * 2 + 1);
    char *pEnd;

    // ע: ��ʹ str �м京�� '\0' �ַ���str.c_str() Ҳ�������������������� 
    // c_str() �Ĳ�������: { data()[size()] = 0; return data(); }
    // ��: string::size() <=> string::length()

    pEnd = (char*)Buffer.Data();
    pEnd += mysql_escape_string(pEnd, str.c_str(), nSrcLen);
    *pEnd = '\0';

    return (char*)Buffer.Data();
}

//-----------------------------------------------------------------------------
// ����: ��ȡִ��SQL����Ӱ�������
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
// ����: ��ȡ���һ��������������ID��ֵ
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
