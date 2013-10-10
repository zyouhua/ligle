///////////////////////////////////////////////////////////////////////////////
// Msger Main Server
// xmldoc.h
// Classes:
//   > CXMLNode
//   > CXMLNodeProps
//   > CXMLDocument
//   > CXMLReader
//   > CXMLWriter
///////////////////////////////////////////////////////////////////////////////

#ifndef _XMLDOC_H_
#define _XMLDOC_H_

#include <stdio.h>
#include <stdlib.h>

#include "ise_system.h"

///////////////////////////////////////////////////////////////////////////////
// 提前声明

class CXMLNode;
class CXMLNodeProps;

///////////////////////////////////////////////////////////////////////////////
// 类型定义

// XML标签类别
enum XMLTagType {
    XTT_START_TAG   = 0x01,      // 起始标签
    XTT_END_TAG     = 0x02,      // 结束标签
};

typedef uint XMLTagTypes;

///////////////////////////////////////////////////////////////////////////////
// 常量定义

const int DEF_XML_INDENT_SPACES = 4;

#define S_CRLF                  "\r\n"
#define S_INVALID_FILE_FORMAT   "Invalid file format"

///////////////////////////////////////////////////////////////////////////////
// 杂项函数

string StrToXML(const string& str);
string XMLToStr(const string& str);

///////////////////////////////////////////////////////////////////////////////
// CXMLNode

class CXMLNode
{
private:
    CXMLNode *m_pParentNode;    // 父节点
    CList *m_pChildNodes;       // 子节点列表 (CXMLNode*)[]
    CXMLNodeProps *m_pProps;    // 属性值列表
    string m_strName;           // 节点名称
    string m_strDataString;     // 节点数据(XMLToStr之后的数据)

    void InitObject();
    void AssignNode(CXMLNode& Src, CXMLNode& Dest);
public:
    CXMLNode();
    CXMLNode(const CXMLNode& src);
    virtual ~CXMLNode();

    CXMLNode& operator = (const CXMLNode& rhs);

    void AddNode(CXMLNode *pNode);
    void InsertNode(int nIndex, CXMLNode *pNode);

    CXMLNode* Add();
    CXMLNode* FindChildNode(const string& strName);
    int IndexOf(const string& strName);
    void Clear();

    CXMLNode* GetRootNode() const;
    CXMLNode* GetParentNode() const { return m_pParentNode; } 
    int GetChildCount() const;
    CXMLNode* GetChildNodes(int nIndex) const;
    CXMLNodeProps* GetProps() const { return m_pProps; }
    string GetName() const { return m_strName; }
    string GetDataString() const { return m_strDataString; }

    void SetParentNode(CXMLNode *pNode);
    void SetName(const string& strValue) { m_strName = strValue; }
    void SetDataString(const string& strValue);
};

///////////////////////////////////////////////////////////////////////////////
// CXMLNodeProps

struct CXMLNodePropItem
{
    string strName;
    string strValue;
};

class CXMLNodeProps
{
private:
    CList m_Items;      // (CXMLNodePropItem*)[]

    CXMLNodePropItem* GetItemPtr(int nIndex) 
        { return (CXMLNodePropItem*)m_Items[nIndex]; }
    void ParsePropString(const string& strPropStr);
public:
    CXMLNodeProps();
    CXMLNodeProps(const CXMLNodeProps& src);
    virtual ~CXMLNodeProps();

    CXMLNodeProps& operator = (const CXMLNodeProps& rhs);
    string& operator[](const string& strName) { return ValueOf(strName); }

    bool Add(const string& strName, const string& strValue);
    void Remove(const string& strName);
    void Clear();
    int IndexOf(const string& strName);
    bool PropExists(const string& strName);
    string& ValueOf(const string& strName);

    int GetCount() const { return m_Items.Count(); }
    CXMLNodePropItem GetItems(int nIndex) const;
    string GetPropString() const;
    void SetPropString(const string& strPropString);
};

///////////////////////////////////////////////////////////////////////////////
// CXMLDocument

class CXMLDocument
{
private:
    bool m_bAutoIndent;     // 是否自动缩进
    int m_nIndentSpaces;    // 缩进空格数
    CXMLNode m_RootNode;    // 根节点
public:
    CXMLDocument();
    CXMLDocument(const CXMLDocument& src);
    virtual ~CXMLDocument();

    CXMLDocument& operator = (const CXMLDocument& rhs);

    void SaveToStream(CStream& Stream);
    void LoadFromStream(CStream& Stream);
    void SaveToString(string& str);
    void LoadFromString(string& str);
    void SaveToFile(const string& strFileName);
    void LoadFromFile(const string& strFileName);
    void Clear();

    bool GetAutoIndent() const { return m_bAutoIndent; }
    int GetIndentSpaces() const { return m_nIndentSpaces; }
    CXMLNode* GetRootNode() { return &m_RootNode; }

    void SetAutoIndent(bool bValue) { m_bAutoIndent = bValue; }
    void SetIndentSpaces(int nValue) { m_nIndentSpaces = nValue; }
};

///////////////////////////////////////////////////////////////////////////////
// CXMLReader

class CXMLReader
{
private:
    CXMLDocument *m_pOwner;
    CStream *m_pStream;
    CBuffer m_Buffer;
    int m_nPosition;

    XMLTagTypes ReadXMLData(string& strName, string& strProp, string& strData);
    XMLTagTypes ReadNode(CXMLNode *pNode);
public:
    CXMLReader(CXMLDocument *pOwner, CStream *pStream);
    virtual ~CXMLReader();

    void ReadHeader();
    void ReadRootNode(CXMLNode *pNode);
};

///////////////////////////////////////////////////////////////////////////////
// CXMLWriter

class CXMLWriter
{
private:
    CXMLDocument *m_pOwner;
    CStream *m_pStream;
    string m_strBuffer;

    void FlushBuffer();
    void WriteLn(const string& str);
    void WriteNode(CXMLNode *pNode, int nIndent);
public:
    CXMLWriter(CXMLDocument *pOwner, CStream *pStream);
    virtual ~CXMLWriter();
    
    void WriteHeader();
    void WriteRootNode(CXMLNode *pNode);
};

///////////////////////////////////////////////////////////////////////////////

#endif // _XMLDOC_H_ 
