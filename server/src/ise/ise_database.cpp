///////////////////////////////////////////////////////////////////////////////
// Iris Server Engine (ISE)
//
// 文件名称: ise_database.cpp
// 功能描述: 数据库存取框架(IDAF: ISE Database Access Framework)
// 最后修改: 2005-05-02
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
// 描述: 建立数据库连接并进行相关设置 (若失败则抛出异常)
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
// 描述: 断开数据库连接并进行相关设置
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
// 描述: 借用连接 (由 ConnectionPool 调用)
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
// 描述: 归还连接 (由 ConnectionPool 调用)
//-----------------------------------------------------------------------------
void CDbConnection::ReturnDbConnection()
{
    m_bBusy = false;
}

//-----------------------------------------------------------------------------
// 描述: 返回连接是否被借用 (由 ConnectionPool 调用)
//-----------------------------------------------------------------------------
bool CDbConnection::IsBorrowed()
{
    return m_bBusy;
}

//-----------------------------------------------------------------------------
// 描述: 激活数据库连接
// 参数:
//   bForce - 是否强制激活
//-----------------------------------------------------------------------------
void CDbConnection::ActivateConnection(bool bForce)
{
    // 没有连接数据库则建立连接
    if (!m_bConnected || bForce)
    {
        Disconnect();
        Connect();
        return;
    }

    if (!m_bBusy && m_pDatabase->GetDbOptions()->GetMaxUsedHours() > 0)
    {
        int nMaxUsedHours = m_pDatabase->GetDbOptions()->GetMaxUsedHours();

        // 如果连接的使用时间超过设定值，则强制重新连接
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
// 描述: 清空连接池
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
// 描述: 分配一个可用的空闲连接 (若失败则抛出异常)
// 返回: 连接对象指针
//-----------------------------------------------------------------------------
CDbConnection* CDbConnectionPool::BorrowConnection()
{
    CDbConnection *pDbConnection = NULL;
    bool bResult = false;

    {
        CAutoSynchronizer Syncher(m_Lock);

        // 检查现有的连接是否能用
        for (int i = 0; i < m_DbConnectionList.Count(); i++)
        {
            pDbConnection = (CDbConnection*)m_DbConnectionList[i];
            bResult = pDbConnection->BorrowDbConnection();  // 借出连接
            if (bResult) break;
        }
        // 增加新的数据库连接到连接池
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
// 描述: 归还数据库连接
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
// 描述: 添加一个字段定义对象
//-----------------------------------------------------------------------------
void CDbFieldDefList::Add(CDbFieldDef* pFieldDef)
{
    m_Items.Add(pFieldDef);
}

//-----------------------------------------------------------------------------
// 描述: 释放并清空所有字段定义对象
//-----------------------------------------------------------------------------
void CDbFieldDefList::Clear()
{
    for (int i = 0; i < m_Items.Count(); i++)
        delete (CDbFieldDef*)m_Items[i];
    m_Items.Clear();
}

//-----------------------------------------------------------------------------
// 描述: 返回字段名对应的字段序号(0-based)，若未找到则返回-1.
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
// 描述: 根据下标号返回字段定义对象 (nIndex: 0-based)
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
// 描述: 以整型返回字段值 (若转换失败则返回缺省值)
//-----------------------------------------------------------------------------
int CDbField::AsInteger(int nDefault) const
{
    return StrToInt(AsString(), nDefault);
}

//-----------------------------------------------------------------------------
// 描述: 以浮点型返回字段值 (若转换失败则返回缺省值)
//-----------------------------------------------------------------------------
double CDbField::AsFloat(double fDefault) const
{
    return StrToFloat(AsString(), fDefault);
}

//-----------------------------------------------------------------------------
// 描述: 以布尔型返回字段值 (若转换失败则返回缺省值)
//-----------------------------------------------------------------------------
bool CDbField::AsBoolean(bool bDefault) const
{
    return AsInteger(bDefault? 1 : 0);
}

//-----------------------------------------------------------------------------
// 描述: 以字符串型返回字段值
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
// 描述: 添加一个字段数据对象
//-----------------------------------------------------------------------------
void CDbFieldList::Add(CDbField* pField)
{
    m_Items.Add(pField);
}

//-----------------------------------------------------------------------------
// 描述: 释放并清空所有字段数据对象
//-----------------------------------------------------------------------------
void CDbFieldList::Clear()
{
    for (int i = 0; i < m_Items.Count(); i++)
        delete (CDbField*)m_Items[i];
    m_Items.Clear();
}

//-----------------------------------------------------------------------------
// 描述: 根据下标号返回字段数据对象 (nIndex: 0-based)
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
// 描述: 取得当前记录中的字段总数
//-----------------------------------------------------------------------------
int CDbDataSet::GetFieldCount()
{
    EnsureInitFieldDefs();
    return m_FieldDefList.Count();
}

//-----------------------------------------------------------------------------
// 描述: 取得当前记录中某个字段的定义 (nIndex: 0-based)
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
// 描述: 取得当前记录中某个字段的数据 (nIndex: 0-based)
//-----------------------------------------------------------------------------
CDbField* CDbDataSet::GetFields(int nIndex) 
{
    if (nIndex >= 0 && nIndex < m_FieldList.Count())
        return m_FieldList[nIndex];
    else
        throw CDbException("field index error.", __FILE__, __LINE__);
}

//-----------------------------------------------------------------------------
// 描述: 取得当前记录中某个字段的数据
// 参数:
//   strName - 字段名
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
// 描述: 执行查询 (返回查询结果数据集，若失败则抛出异常)
//-----------------------------------------------------------------------------
CDbDataSet* CDbQuery::DoQuery()
{
    CDbDataSet* pDataSet;

    // 执行查询
    DoExecute();

    // 初始化数据集
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
// 描述: 设置SQL语句
//-----------------------------------------------------------------------------
void CDbQuery::SetSql(const string& strSql)
{
    m_strSql = strSql;
}

//-----------------------------------------------------------------------------
// 描述: 执行SQL (无返回结果)
//-----------------------------------------------------------------------------
void CDbQuery::Execute()
{
    EnsureConnected();
    DoExecute();    
}

//-----------------------------------------------------------------------------
// 描述: 执行SQL (返回数据集)
//-----------------------------------------------------------------------------
CDbDataSet* CDbQuery::Query()
{
    EnsureConnected();
    return DoQuery();
}

//-----------------------------------------------------------------------------
// 描述: 转换字符串使之在SQL中合法 (str 中可含 '\0' 字符)
//-----------------------------------------------------------------------------
string CDbQuery::ValidateString(const string& str)
{
    return DoValidateString(str);
}

//-----------------------------------------------------------------------------
// 描述: 取得执行SQL后受影响的行数
//-----------------------------------------------------------------------------
uint CDbQuery::GetAffectedRowCount()
{
    EnsureConnected();
    return DoGetAffectedRowCount();
}

//-----------------------------------------------------------------------------
// 描述: 取得最后一条插入语句的自增ID的值
//-----------------------------------------------------------------------------
uint64 CDbQuery::GetInsertId()
{
    EnsureConnected();
    return DoGetInsertId();
}

//-----------------------------------------------------------------------------
// 描述: 取得查询器所用的数据库连接
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
