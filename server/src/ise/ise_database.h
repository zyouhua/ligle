///////////////////////////////////////////////////////////////////////////////
// Iris Server Engine (ISE)
// ise_database.h
// Classes:
//   > CDbException           - ���ݿ��쳣��
//
//   > CDbConnParams          - ���ݿ����Ӳ�����
//   > CDbOptions             - ���ݿ����ò�����
//   > CDbConnection          - ���ݿ����ӻ���
//   > CDbConnectionPool      - ���ݿ����ӳػ���
//   > CDbFieldDef            - �ֶζ�����
//   > CDbFieldDefList        - �ֶζ����б���
//   > CDbField               - �ֶ�������
//   > CDbFieldList           - �ֶ������б���
//   > CDbDataSet             - ���ݼ���
//   > CDbQuery               - ���ݲ�ѯ����
//   > CDbQueryWrapper        - ���ݲ�ѯ����װ��
//   > CDbDataSetWrapper      - ���ݼ���װ��
//
//   > CDatabase              - ���ݿ���
///////////////////////////////////////////////////////////////////////////////

#ifndef _ISE_DATABASE_H_
#define _ISE_DATABASE_H_

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string>

#include "ise_classes.h"

using namespace ise;

namespace ise
{

///////////////////////////////////////////////////////////////////////////////
// ��ǰ����

class CDatabase;
class CDbConnectionPool;
class CDatabase;
class CDbQuery;

///////////////////////////////////////////////////////////////////////////////
// class CDbException - ���ݿ��쳣��
//
// ע: ���е����ݿ�����쳣��ʹ�ø�������������ࡣ

class CDbException : public CException
{
public:
    CDbException(int nErrorNo, const char *sErrorMsg, const char *sFileName = NULL, int nLineNo = EMPTY_LINE_NO) :
		CException(nErrorNo, sErrorMsg, sFileName, nLineNo) {}
    CDbException(const char *sErrorMsg, const char *sFileName = NULL, int nLineNo = EMPTY_LINE_NO) :
	    CException(sErrorMsg, sFileName, nLineNo) {}
};

///////////////////////////////////////////////////////////////////////////////
// class CDbConnParams - ���ݿ����Ӳ�����

class CDbConnParams
{
private:
    string m_strHostName;       // ������ַ
    string m_strUserName;       // �û���
    string m_strPassword;       // �û�����
    string m_strDbName;         // ���ݿ���
    int m_nPort;                // ���Ӷ˿ں�

public:
    CDbConnParams();
    CDbConnParams(const CDbConnParams& src); 
    CDbConnParams(const string& strHostName, const string& strUserName,
        const string& strPassword, const string& strDbName, const int nPort);

    string GetHostName() const { return m_strHostName; }
    string GetUserName() const { return m_strUserName; }
    string GetPassword() const { return m_strPassword; }
    string GetDbName() const { return m_strDbName; }
    int GetPort() const { return m_nPort; }

    void SetHostName(const string& strValue) { m_strHostName = strValue; }
    void SetUserName(const string& strValue) { m_strUserName = strValue; }
    void SetPassword(const string& strValue) { m_strPassword = strValue; }
    void SetDbName(const string& strValue) { m_strDbName = strValue; }
    void SetPort(const int nValue) { m_nPort = nValue; }
};

///////////////////////////////////////////////////////////////////////////////
// class CDbOptions - ���ݿ����ò�����

class CDbOptions
{
public:
    // ����ȱʡֵ:
    enum { 
        DEF_MAX_DB_CONNECTIONS     = 100,       // ���ӳ����������ȱʡֵ
        DEF_MAX_DB_CONN_USED_HOURS = 10         // ����ʹ�õ��ʱ��(Сʱ)
    };

private:
    int m_nMaxDbConnections;                    // ���ӳ�����������������
    int m_nMaxUsedHours;                        // һ�����ӵ��ʹ��ʱ��(Сʱ)��<=0 ��ʾ���Ըò���

public:
    CDbOptions();

    int GetMaxDbConnections() const { return m_nMaxDbConnections; }
    int GetMaxUsedHours() const { return m_nMaxUsedHours; }

    void SetMaxDbConnections(int nValue);
    void SetMaxUsedHours(int nValue);
};

///////////////////////////////////////////////////////////////////////////////
// class CDbConnection - ���ݿ����ӻ���

class CDbConnection
{
friend class CDbConnectionPool;

private:
    bool m_bConnected;                  // �Ƿ��ѽ�������
    bool m_bBusy;                       // �����ӵ�ǰ�Ƿ�����ռ��
    uint m_nConnTimeStamp;              // �����Ӽ���ʱ��ʱ��� (time())

protected:
    CDatabase *m_pDatabase;             // CDatabase ���������

    // �������ݿ����Ӳ������������ (��ʧ�����׳��쳣)
    void Connect();
    // �Ͽ����ݿ����Ӳ������������
    void Disconnect();

    // ConnectionPool ���������к������������ӵ�ʹ�����
    bool BorrowDbConnection();          // ��������
    void ReturnDbConnection();          // �黹����
    bool IsBorrowed();                  // �����Ƿ񱻽���

protected:
    // �����ݿ⽨������(��ʧ�����׳��쳣)
    virtual void DoConnect() = 0;
    // �����ݿ�Ͽ�����
    virtual void DoDisconnect() = 0;

public:
    CDbConnection(CDatabase *pDatabase);
    virtual ~CDbConnection();

    // �������ݿ�����
    void ActivateConnection(bool bForce = false);
};

///////////////////////////////////////////////////////////////////////////////
// class CDbConnectionPool - ���ݿ����ӳػ���
//
// ����ԭ��:
// 1. ����ά��һ�������б�����ǰ����������(�������ӡ�æ����)����ʼΪ�գ�����һ���������ޡ�
// 2. ��������: 
//    ���ȳ��Դ������б����ҳ�һ���������ӣ����ҵ������ɹ���ͬʱ��������Ϊæ״̬����û�ҵ�
//    ��: (1)��������δ�ﵽ���ޣ��򴴽�һ���µĿ������ӣ�(2)���������Ѵﵽ���ޣ������ʧ�ܡ�
//    ������ʧ��(�������������޷����������ӵ�)�����׳��쳣��
// 3. �黹����:
//    ֻ�轫������Ϊ����״̬���ɣ�����(Ҳ����)�Ͽ����ݿ����ӡ�

class CDbConnectionPool
{
protected:
    CDatabase* m_pDatabase;         // ���� CDatabase ����
    CList m_DbConnectionList;       // ��ǰ�����б� (CDbConnection*[])�������������Ӻ�æ����
    CCriticalSection m_Lock;        // ������

    void ClearPool();
public:
    CDbConnectionPool(CDatabase* pDatabase);
    virtual ~CDbConnectionPool();

    // ����һ�����õĿ������� (��ʧ�����׳��쳣)
    CDbConnection* BorrowConnection();
    // �黹���ݿ�����
    void ReturnConnection(CDbConnection* pDbConnection);

    // ȡ�����ӳ��е�ǰ���ӵ����� (���������Ӻ�æ����)
    int Count() const { return m_DbConnectionList.Count(); }
};

///////////////////////////////////////////////////////////////////////////////
// class CDbFieldDef - �ֶζ�����

class CDbFieldDef
{
protected:
    string m_strName;          // �ֶ�����

public:
    CDbFieldDef() {}
    CDbFieldDef(const string& strName);
    CDbFieldDef(const CDbFieldDef& src);
    virtual ~CDbFieldDef() {}

    void SetData(char *sName) { m_strName = sName; }

    string GetName() const { return m_strName; }
};

///////////////////////////////////////////////////////////////////////////////
// class CDbFieldDefList - �ֶζ����б���

class CDbFieldDefList
{
private:
    CList m_Items;       // (CDbFieldDef* [])

public:
    CDbFieldDefList();
    virtual ~CDbFieldDefList();

    // ���һ���ֶζ������
    void Add(CDbFieldDef* pFieldDef);
    // �ͷŲ���������ֶζ������
    void Clear();
    // �����ֶ�����Ӧ���ֶ����(0-based)
    int IndexOfName(const string& strName);

    CDbFieldDef* operator [] (int nIndex);
    int Count() const { return m_Items.Count(); }
};

///////////////////////////////////////////////////////////////////////////////
// class CDbField - �ֶ�������

class CDbField
{
protected:
    void* m_pData;       // ָ���ֶ�����
    int m_nSize;         // �ֶ����ݵ����ֽ���

public:
    CDbField();
    CDbField(void* pData, int nSize);
    CDbField(const CDbField& src);
    virtual ~CDbField() {}

    void SetData(void* pData, int nSize);

    virtual bool IsNull() const { return (m_pData == NULL); }
    virtual int AsInteger(int nDefault = 0) const;
    virtual double AsFloat(double fDefault = 0) const;
    virtual bool AsBoolean(bool bDefault = false) const;
    virtual string AsString() const;
    void* AsPointer() const { return m_pData; }
    int GetSize() const { return m_nSize; }
};

///////////////////////////////////////////////////////////////////////////////
// class CDbFieldList - �ֶ������б���

class CDbFieldList
{
private:
    CList m_Items;       // (CDbField* [])

public:
    CDbFieldList();
    virtual ~CDbFieldList();

    // ���һ���ֶ����ݶ���
    void Add(CDbField* pField);
    // �ͷŲ���������ֶ����ݶ���
    void Clear();

    CDbField* operator [] (int nIndex);
    int Count() const { return m_Items.Count(); }
};

///////////////////////////////////////////////////////////////////////////////
// class CDbDataSet - ���ݼ���
// 
// ˵��:
// 1. ����ֻ�ṩ����������ݼ��Ĺ��ܡ�
// 2. ���ݼ���ʼ��(InitDataSet)���α�ָ���һ����¼֮ǰ������� Next() ��ָ���һ����¼��
//    ���͵����ݼ���������Ϊ: while(DataSet.Next()) { ... }

class CDbDataSet
{
friend class CDbQuery;

protected:
    CDbQuery* m_pDbQuery;             // CDbQuery ��������
    CDbFieldDefList m_FieldDefList;   // �ֶζ�������б�
    CDbFieldList m_FieldList;         // �ֶ����ݶ����б�
    bool m_bInitFieldDefs;            // �Ƿ��ʼ���� m_FieldDefList

protected:
    // ��ʼ�����ݼ� (��ʧ�����׳��쳣)
    virtual void InitDataSet() = 0;
    // ��ʼ�����ݼ����ֶεĶ���
    virtual void InitFieldDefs() = 0;

private:
    void EnsureInitFieldDefs();

public:
    CDbDataSet(CDbQuery* pDbQuery);
    virtual ~CDbDataSet();

    // ���α�ָ����ʼλ��(��һ����¼֮ǰ)
    virtual bool Rewind() = 0;
    // ���α�ָ����һ����¼
    virtual bool Next() = 0;

    // ȡ�õ�ǰ���ݼ��еļ�¼����
    virtual uint64 GetRecordCount() = 0;

    // �������ݼ��Ƿ�Ϊ��
    bool IsEmpty() { return GetRecordCount() == 0; }
    // ȡ�õ�ǰ��¼�е��ֶ�����
    int GetFieldCount();
    // ȡ�õ�ǰ��¼��ĳ���ֶεĶ��� (nIndex: 0-based)
    CDbFieldDef* GetFieldDefs(int nIndex);
    // ȡ�õ�ǰ��¼��ĳ���ֶε����� (nIndex: 0-based)
    CDbField* GetFields(int nIndex);
    CDbField* GetFields(const string& strName);
};

///////////////////////////////////////////////////////////////////////////////
// class CDbQuery - ���ݲ�ѯ����
//
// ����ԭ��:
// 1. ִ��SQL: �����ӳ�ȡ��һ���������ӣ�Ȼ�����ô�����ִ��SQL�����黹���ӡ�

class CDbQuery
{
protected:
    CDatabase* m_pDatabase;                     // CDatabase ��������
    CDbConnection* m_pDbConnection;             // CDbConnection ��������
    string m_strSql;                            // ��ִ�е�SQL���

    // ִ��SQL (�޷��ؽ��, ��ʧ�����׳��쳣)
    virtual void DoExecute() {}
    // ִ��SQL (�������ݼ�, ��ʧ�����׳��쳣)
    virtual CDbDataSet* DoQuery();

    // ת���ַ���ʹ֮��SQL�кϷ�
    virtual string DoValidateString(const string& str) { return str; }

    // ȡ��ִ��SQL����Ӱ�������
    virtual uint DoGetAffectedRowCount() = 0;
    // ȡ�����һ��������������ID��ֵ
    virtual uint64 DoGetInsertId() = 0;

private:
    void EnsureConnected();

public:
    CDbQuery(CDatabase *pDatabase);
    virtual ~CDbQuery();

    // ����SQL���
    void SetSql(const string& strSql);

    // ִ��SQL (�޷��ؽ��, ��ʧ�����׳��쳣)
    void Execute();
    // ִ��SQL (�������ݼ�, ��ʧ�����׳��쳣)
    CDbDataSet* Query();

    // ת���ַ���ʹ֮��SQL�кϷ� (str �пɺ� '\0' �ַ�)
    string ValidateString(const string& str);

    // ȡ��ִ��SQL����Ӱ�������
    uint GetAffectedRowCount();
    // ȡ�����һ��������������ID��ֵ
    uint64 GetInsertId();

    // ȡ�ò�ѯ�����õ����ݿ�����
    CDbConnection* GetDbConnection();
    // ȡ�� CDatabase ����
    CDatabase* GetDatabase() { return m_pDatabase; }
};

///////////////////////////////////////////////////////////////////////////////
// class CDbQueryWrapper - ��ѯ����װ��
//
// ˵��: �������ڰ�װ CDbQuery �����Զ��ͷű���װ�Ķ��󣬷�ֹ��Դй©��
// ʾ��:
//      int main()
//      {
//          CDbQueryWrapper qry( MyDatabase.CreateDbQuery() );
//          qry->SetSql("select * from users");
//          /* ... */
//          // ջ���� qry ���Զ����٣����ͬʱ����װ�ĶѶ���Ҳ�Զ��ͷš�
//      }

class CDbQueryWrapper
{
private:
    CDbQuery* m_pQuery;
public:
    CDbQueryWrapper(CDbQuery *pQuery) : m_pQuery(pQuery) {}
    virtual ~CDbQueryWrapper() { delete m_pQuery; }
    
    CDbQuery* operator -> () { return m_pQuery; }
};

///////////////////////////////////////////////////////////////////////////////
// class CDbDataSetWrapper - ���ݼ���װ��
//
// ˵��: 
// 1. �������ڰ�װ CDbDataSet �����Զ��ͷű���װ�Ķ��󣬷�ֹ��Դй©��
// 2. ÿ�θ���װ����ֵ(Wrapper = DataSet)���ϴα���װ�Ķ����Զ��ͷš�
// ʾ��:
//      int main()
//      {
//          CDbQueryWrapper qry( MyDatabase.CreateDbQuery() );
//          CDbDataSetWrapper ds;
//
//          qry->SetSql("select * from users");
//          ds = qry->Query();
//          /* ... */
//
//          // ջ���� qry �� ds ���Զ����٣����ͬʱ����װ�ĶѶ���Ҳ�Զ��ͷš�
//      }

class CDbDataSetWrapper
{
private:
    CDbDataSet* m_pDataSet;
public:
    CDbDataSetWrapper() : m_pDataSet(NULL) {}
    virtual ~CDbDataSetWrapper() { delete m_pDataSet; }

    CDbDataSetWrapper& operator = (CDbDataSet *pDataSet) 
    { 
        if (m_pDataSet) delete m_pDataSet; 
        m_pDataSet = pDataSet; 
        return *this; 
    }
    
    CDbDataSet* operator -> () { return m_pDataSet; }
};

///////////////////////////////////////////////////////////////////////////////
// class CDatabase - ���ݿ���

class CDatabase
{
protected:
    CDbConnParams* m_pDbConnParams;             // ���ݿ����Ӳ���
    CDbOptions* m_pDbOptions;                   // ���ݿ����ò���
    CDbConnectionPool* m_pDbConnectionPool;     // ���ݿ����ӳ�

private:
    void EnsureInit();

public:
    CDatabase();
    virtual ~CDatabase();

    // �๤������:
    virtual CDbConnParams* CreateDbConnParams() { return new CDbConnParams(); }
    virtual CDbOptions* CreateDbOptions() { return new CDbOptions(); }
    virtual CDbConnection* CreateDbConnection() = 0;
    virtual CDbConnectionPool* CreateDbConnectionPool() { return new CDbConnectionPool(this); }
    virtual CDbQuery* CreateDbQuery() = 0;
    virtual CDbDataSet* CreateDbDataSet(CDbQuery* pDbQuery) = 0;
    virtual CDbField* CreateDbField() { return new CDbField(); }
    virtual CDbFieldDef* CreateDbFieldDef() { return new CDbFieldDef(); }

    // �������ݿ����Ӳ���
    CDbConnParams* GetDbConnParams();
    // �������ݿ����ò���
    CDbOptions* GetDbOptions();
    // �������ӳ�
    CDbConnectionPool* GetDbConnectionPool();
};

///////////////////////////////////////////////////////////////////////////////

} // namespace ise

#endif // _ISE_DATABASE_H_ 
