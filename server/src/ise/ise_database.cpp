///////////////////////////////////////////////////////////////////////////////
// Iris Server Engine (ISE)
//
// �ļ�����: ise_database.cpp
// ��������: ���ݿ��ȡ���(IDAF: ISE Database Access Framework)
// ����޸�: 2005-05-02
///////////////////////////////////////////////////////////////////////////////

#include "ise_database.h"
#include "ise_classes.h"
#include "ise_system.h"

using namespace ise;

namespace ise
{

///////////////////////////////////////////////////////////////////////////////
// class CDbConnParams

CDbConnParams::CDbConnParams() :
    m_nPort(0)
{
}

CDbConnParams::CDbConnParams(const CDbConnParams& src) 
{
    m_strHostName = src.m_strHostName;
    m_strUserName = src.m_strUserName;
    m_strPassword = src.m_strPassword;
    m_strDbName = src.m_strDbName;
    m_nPort = src.m_nPort;
}

CDbConnParams::CDbConnParams(const string& strHostName, const string& strUserName,
    const string& strPassword, const string& strDbName, int nPort)
{
    m_strHostName = strHostName;
    m_strUserName = strUserName;
    m_strPassword = strPassword;
    m_strDbName = strDbName;
    m_nPort = nPort;
}

///////////////////////////////////////////////////////////////////////////////
// class CDbOptions

CDbOptions::CDbOptions()
{
    SetMaxDbConnections(DEF_MAX_DB_CONNECTIONS);
    SetMaxUsedHours(DEF_MAX_DB_CONN_USED_HOURS);
}

void CDbOptions::SetMaxDbConnections(int nValue)
{
    if (nValue < 1) nValue = 1;
    m_nMaxDbConnections = nValue;
}

void CDbOptions::SetMaxUsedHours(int nValue)
{
    if (nValue < 0) nValue = 0;
    m_nMaxUsedHours = nValue; 
}

///////////////////////////////////////////////////////////////////////////////
// class CDbConnection

CDbConnection::CDbConnection(CDatabase *pDatabase) 
{
    m_pDatabase = pDatabase;
    m_bConnected = false;
    m_bBusy = false;
    m_nConnTimeStamp = 0;
}

CDbConnection::~CDbConnection()
{
}

//-----------------------------------------------------------------------------
// ����: �������ݿ����Ӳ������������ (��ʧ�����׳��쳣)
//-----------------------------------------------------------------------------
void CDbConnection::Connect()
{
    if (!m_bConnected)
    {
        DoConnect();
        m_bConnected = true;
        m_bBusy = false;
        m_nConnTimeStamp = time(NULL);
    }
}

//-----------------------------------------------------------------------------
// ����: �Ͽ����ݿ����Ӳ������������
//-----------------------------------------------------------------------------
void CDbConnection::Disconnect()
{
    if (m_bConnected)
    {
        DoDisconnect();
        m_bConnected = false;
        m_bBusy = false;
        m_nConnTimeStamp = 0;
    }
}

//-----------------------------------------------------------------------------
// ����: �������� (�� ConnectionPool ����)
//-----------------------------------------------------------------------------
bool CDbConnection::BorrowDbConnection()
{
    ActivateConnection();

    if (!m_bBusy && m_bConnected)
    {
        m_bBusy = true;
        return true;
    }
    return false;
}

//-----------------------------------------------------------------------------
// ����: �黹���� (�� ConnectionPool ����)
//-----------------------------------------------------------------------------
void CDbConnection::ReturnDbConnection()
{
    m_bBusy = false;
}

//-----------------------------------------------------------------------------
// ����: ���������Ƿ񱻽��� (�� ConnectionPool ����)
//-----------------------------------------------------------------------------
bool CDbConnection::IsBorrowed()
{
    return m_bBusy;
}

//-----------------------------------------------------------------------------
// ����: �������ݿ�����
// ����:
//   bForce - �Ƿ�ǿ�Ƽ���
//-----------------------------------------------------------------------------
void CDbConnection::ActivateConnection(bool bForce)
{
    // û���������ݿ���������
    if (!m_bConnected || bForce)
    {
        Disconnect();
        Connect();
        return;
    }

    if (!m_bBusy && m_pDatabase->GetDbOptions()->GetMaxUsedHours() > 0)
    {
        int nMaxUsedHours = m_pDatabase->GetDbOptions()->GetMaxUsedHours();

        // ������ӵ�ʹ��ʱ�䳬���趨ֵ����ǿ����������
        if ((uint)time(NULL) - m_nConnTimeStamp > (nMaxUsedHours * 60 * 60))
        {
            Disconnect();
            Connect();
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
// class CDbConnectionPool

CDbConnectionPool::CDbConnectionPool(CDatabase* pDatabase) :
    m_pDatabase(pDatabase)
{
}

CDbConnectionPool::~CDbConnectionPool()
{
    ClearPool();
}

//-----------------------------------------------------------------------------
// ����: ������ӳ�
//-----------------------------------------------------------------------------
void CDbConnectionPool::ClearPool()
{
    CAutoSynchronizer Syncher(m_Lock);

    for (int i = 0; i < m_DbConnectionList.Count(); i++)
    {
        CDbConnection *pDbConnection;
        pDbConnection = (CDbConnection*)m_DbConnectionList[i];
        pDbConnection->DoDisconnect();
        delete pDbConnection;
    }

    m_DbConnectionList.Clear();
}

//-----------------------------------------------------------------------------
// ����: ����һ�����õĿ������� (��ʧ�����׳��쳣)
// ����: ���Ӷ���ָ��
//-----------------------------------------------------------------------------
CDbConnection* CDbConnectionPool::BorrowConnection()
{
    CDbConnection *pDbConnection = NULL;
    bool bResult = false;

    {
        CAutoSynchronizer Syncher(m_Lock);

        // ������е������Ƿ�����
        for (int i = 0; i < m_DbConnectionList.Count(); i++)
        {
            pDbConnection = (CDbConnection*)m_DbConnectionList[i];
            bResult = pDbConnection->BorrowDbConnection();  // �������
            if (bResult) break;
        }
        // �����µ����ݿ����ӵ����ӳ�
        if (!bResult && (m_DbConnectionList.Count() < m_pDatabase->GetDbOptions()->GetMaxDbConnections()))
        {
            pDbConnection = m_pDatabase->CreateDbConnection();
            m_DbConnectionList.Add(pDbConnection);
            bResult = pDbConnection->BorrowDbConnection();
        }
    }

    if (!bResult)
    {
        throw CDbException("cannot borrow connection from connection pool.", __FILE__, __LINE__);
    }

    return pDbConnection;
}

//-----------------------------------------------------------------------------
// ����: �黹���ݿ�����
//-----------------------------------------------------------------------------
void CDbConnectionPool::ReturnConnection(CDbConnection* pDbConnection)
{
    CAutoSynchronizer Syncher(m_Lock);

    pDbConnection->ReturnDbConnection();
}

///////////////////////////////////////////////////////////////////////////////
// class CDbFieldDef

CDbFieldDef::CDbFieldDef(const string& strName)
{
    m_strName = strName;
}

CDbFieldDef::CDbFieldDef(const CDbFieldDef& src)
{
    m_strName = src.m_strName;
}

///////////////////////////////////////////////////////////////////////////////
// class CDbFieldDefList

CDbFieldDefList::CDbFieldDefList()
{
}

CDbFieldDefList::~CDbFieldDefList()
{
    Clear();
}

//-----------------------------------------------------------------------------
// ����: ���һ���ֶζ������
//-----------------------------------------------------------------------------
void CDbFieldDefList::Add(CDbFieldDef* pFieldDef)
{
    m_Items.Add(pFieldDef);
}

//-----------------------------------------------------------------------------
// ����: �ͷŲ���������ֶζ������
//-----------------------------------------------------------------------------
void CDbFieldDefList::Clear()
{
    for (int i = 0; i < m_Items.Count(); i++)
        delete (CDbFieldDef*)m_Items[i];
    m_Items.Clear();
}

//-----------------------------------------------------------------------------
// ����: �����ֶ�����Ӧ���ֶ����(0-based)����δ�ҵ��򷵻�-1.
//-----------------------------------------------------------------------------
int CDbFieldDefList::IndexOfName(const string& strName)
{
    int nIndex = -1;

    for (int i = 0; i < m_Items.Count(); i++)
    {
        if (((CDbFieldDef*)m_Items[i])->GetName() == strName)
        {
            nIndex = i;
            break;
        }
    }

    return nIndex;
}

//-----------------------------------------------------------------------------
// ����: �����±�ŷ����ֶζ������ (nIndex: 0-based)
//-----------------------------------------------------------------------------
CDbFieldDef* CDbFieldDefList::operator [] (int nIndex)
{
    if (nIndex >= 0 && nIndex < m_Items.Count())
        return (CDbFieldDef*)m_Items[nIndex];
    else
        return NULL;
}

///////////////////////////////////////////////////////////////////////////////
// class CDbField

CDbField::CDbField()
{
    m_pData = NULL;
    m_nSize = 0;
}

CDbField::CDbField(void* pData, int nSize)
{
    SetData(pData, nSize);
}

CDbField::CDbField(const CDbField& src)
{
    m_pData = src.m_pData;
    m_nSize = src.m_nSize;
}

void CDbField::SetData(void* pData, int nSize)
{
    m_pData = pData;
    m_nSize = nSize;
}

//-----------------------------------------------------------------------------
// ����: �����ͷ����ֶ�ֵ (��ת��ʧ���򷵻�ȱʡֵ)
//-----------------------------------------------------------------------------
int CDbField::AsInteger(int nDefault) const
{
    return StrToInt(AsString(), nDefault);
}

//-----------------------------------------------------------------------------
// ����: �Ը����ͷ����ֶ�ֵ (��ת��ʧ���򷵻�ȱʡֵ)
//-----------------------------------------------------------------------------
double CDbField::AsFloat(double fDefault) const
{
    return StrToFloat(AsString(), fDefault);
}

//-----------------------------------------------------------------------------
// ����: �Բ����ͷ����ֶ�ֵ (��ת��ʧ���򷵻�ȱʡֵ)
//-----------------------------------------------------------------------------
bool CDbField::AsBoolean(bool bDefault) const
{
    return AsInteger(bDefault? 1 : 0);
}

//-----------------------------------------------------------------------------
// ����: ���ַ����ͷ����ֶ�ֵ
//-----------------------------------------------------------------------------
string CDbField::AsString() const
{
    string strResult;
    if (m_pData && m_nSize > 0) strResult.assign((char*)m_pData, m_nSize);
    return strResult;
}

///////////////////////////////////////////////////////////////////////////////
// class CDbFieldList

CDbFieldList::CDbFieldList()
{
}

CDbFieldList::~CDbFieldList()
{
    Clear();
}

//-----------------------------------------------------------------------------
// ����: ���һ���ֶ����ݶ���
//-----------------------------------------------------------------------------
void CDbFieldList::Add(CDbField* pField)
{
    m_Items.Add(pField);
}

//-----------------------------------------------------------------------------
// ����: �ͷŲ���������ֶ����ݶ���
//-----------------------------------------------------------------------------
void CDbFieldList::Clear()
{
    for (int i = 0; i < m_Items.Count(); i++)
        delete (CDbField*)m_Items[i];
    m_Items.Clear();
}

//-----------------------------------------------------------------------------
// ����: �����±�ŷ����ֶ����ݶ��� (nIndex: 0-based)
//-----------------------------------------------------------------------------
CDbField* CDbFieldList::operator [] (int nIndex)
{
    if (nIndex >= 0 && nIndex < m_Items.Count())
        return (CDbField*)m_Items[nIndex];
    else
        return NULL;
}

///////////////////////////////////////////////////////////////////////////////
// class CDbDataSet

CDbDataSet::CDbDataSet(CDbQuery* pDbQuery) :
    m_pDbQuery(pDbQuery),
    m_bInitFieldDefs(false)
{
}

CDbDataSet::~CDbDataSet()
{   
}

void CDbDataSet::EnsureInitFieldDefs()
{
    if (!m_bInitFieldDefs)
    {
        InitFieldDefs();
        m_bInitFieldDefs = true;
    }
}

//-----------------------------------------------------------------------------
// ����: ȡ�õ�ǰ��¼�е��ֶ�����
//-----------------------------------------------------------------------------
int CDbDataSet::GetFieldCount()
{
    EnsureInitFieldDefs();
    return m_FieldDefList.Count();
}

//-----------------------------------------------------------------------------
// ����: ȡ�õ�ǰ��¼��ĳ���ֶεĶ��� (nIndex: 0-based)
//-----------------------------------------------------------------------------
CDbFieldDef* CDbDataSet::GetFieldDefs(int nIndex)
{
    EnsureInitFieldDefs();

    if (nIndex >= 0 && nIndex < m_FieldDefList.Count())
        return m_FieldDefList[nIndex];
    else
        throw CDbException("field index error.", __FILE__, __LINE__);
}

//-----------------------------------------------------------------------------
// ����: ȡ�õ�ǰ��¼��ĳ���ֶε����� (nIndex: 0-based)
//-----------------------------------------------------------------------------
CDbField* CDbDataSet::GetFields(int nIndex) 
{
    if (nIndex >= 0 && nIndex < m_FieldList.Count())
        return m_FieldList[nIndex];
    else
        throw CDbException("field index error.", __FILE__, __LINE__);
}

//-----------------------------------------------------------------------------
// ����: ȡ�õ�ǰ��¼��ĳ���ֶε�����
// ����:
//   strName - �ֶ���
//-----------------------------------------------------------------------------
CDbField* CDbDataSet::GetFields(const string& strName)
{
    int nIndex;

    EnsureInitFieldDefs();
    nIndex = m_FieldDefList.IndexOfName(strName);
    
    if (nIndex >= 0)
        return GetFields(nIndex);
    else
        throw CDbException("field name error.", __FILE__, __LINE__);
}

///////////////////////////////////////////////////////////////////////////////
// class CDbQuery

CDbQuery::CDbQuery(CDatabase *pDatabase) :
    m_pDatabase(pDatabase),
    m_pDbConnection(NULL)
{
}

CDbQuery::~CDbQuery()
{
    if (m_pDbConnection)
        m_pDatabase->GetDbConnectionPool()->ReturnConnection(m_pDbConnection);
}

void CDbQuery::EnsureConnected()
{
    if (!m_pDbConnection)
        m_pDbConnection = m_pDatabase->GetDbConnectionPool()->BorrowConnection();
}

//-----------------------------------------------------------------------------
// ����: ִ�в�ѯ (���ز�ѯ������ݼ�����ʧ�����׳��쳣)
//-----------------------------------------------------------------------------
CDbDataSet* CDbQuery::DoQuery()
{
    CDbDataSet* pDataSet;

    // ִ�в�ѯ
    DoExecute();

    // ��ʼ�����ݼ�
    pDataSet = m_pDatabase->CreateDbDataSet(this);
    try
    {
        pDataSet->InitDataSet();
    }
    catch (CException& e)
    {
        delete pDataSet;
        pDataSet = NULL;
        throw;
    }

    return pDataSet;
}

//-----------------------------------------------------------------------------
// ����: ����SQL���
//-----------------------------------------------------------------------------
void CDbQuery::SetSql(const string& strSql)
{
    m_strSql = strSql;
}

//-----------------------------------------------------------------------------
// ����: ִ��SQL (�޷��ؽ��)
//-----------------------------------------------------------------------------
void CDbQuery::Execute()
{
    EnsureConnected();
    DoExecute();    
}

//-----------------------------------------------------------------------------
// ����: ִ��SQL (�������ݼ�)
//-----------------------------------------------------------------------------
CDbDataSet* CDbQuery::Query()
{
    EnsureConnected();
    return DoQuery();
}

//-----------------------------------------------------------------------------
// ����: ת���ַ���ʹ֮��SQL�кϷ� (str �пɺ� '\0' �ַ�)
//-----------------------------------------------------------------------------
string CDbQuery::ValidateString(const string& str)
{
    return DoValidateString(str);
}

//-----------------------------------------------------------------------------
// ����: ȡ��ִ��SQL����Ӱ�������
//-----------------------------------------------------------------------------
uint CDbQuery::GetAffectedRowCount()
{
    EnsureConnected();
    return DoGetAffectedRowCount();
}

//-----------------------------------------------------------------------------
// ����: ȡ�����һ��������������ID��ֵ
//-----------------------------------------------------------------------------
uint64 CDbQuery::GetInsertId()
{
    EnsureConnected();
    return DoGetInsertId();
}

//-----------------------------------------------------------------------------
// ����: ȡ�ò�ѯ�����õ����ݿ�����
//-----------------------------------------------------------------------------
CDbConnection* CDbQuery::GetDbConnection()
{
    EnsureConnected();
    return m_pDbConnection; 
}

///////////////////////////////////////////////////////////////////////////////
// class CDatabase

CDatabase::CDatabase()
{ 
    m_pDbConnParams = NULL;
    m_pDbOptions = NULL;
    m_pDbConnectionPool = NULL; 
}

CDatabase::~CDatabase()
{
    delete m_pDbConnParams;
    delete m_pDbOptions;
    delete m_pDbConnectionPool;
}

void CDatabase::EnsureInit()
{
    if (!m_pDbConnParams)
    {
        m_pDbConnParams = CreateDbConnParams();
        m_pDbOptions = CreateDbOptions();
        m_pDbConnectionPool = CreateDbConnectionPool(); 
    }
}

CDbConnParams* CDatabase::GetDbConnParams()
{ 
    EnsureInit();
    return m_pDbConnParams; 
}

CDbOptions* CDatabase::GetDbOptions() 
{ 
    EnsureInit();
    return m_pDbOptions; 
}

CDbConnectionPool* CDatabase::GetDbConnectionPool() 
{ 
    EnsureInit();
    return m_pDbConnectionPool; 
}

///////////////////////////////////////////////////////////////////////////////

} // namespace ise
