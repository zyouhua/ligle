///////////////////////////////////////////////////////////////////////////////
// Msger Main Server
//
// 文件名称: xmldoc.cpp
// 功能描述: XML文档支持
// 最后修改: 2005-07-01
///////////////////////////////////////////////////////////////////////////////

#include "xmldoc.h"

///////////////////////////////////////////////////////////////////////////////
// 杂项函数

//-----------------------------------------------------------------------------
// 描述: 抛出异常信息
//-----------------------------------------------------------------------------
void ThrowXMLException(const string& strMsg)
{
    throw CException(strMsg.c_str(), __FILE__, __LINE__);
}

//-----------------------------------------------------------------------------
// 描述: 任意字符串 -> 合法的XML字符串
//-----------------------------------------------------------------------------
string StrToXML(const string& str)
{
    string s = str;

    for (int i = s.size() - 1; i >= 0; i--)
    {
        switch (s[i])
        {
        case '<':
            s.insert(i + 1, "lt;");
            s[i] = '&';
            break;

        case '>':
            s.insert(i + 1, "gt;");
            s[i] = '&';
            break;
        
        case '\"':
            s.insert(i + 1, "quot;");
            s[i] = '&';
            break;
        
        case 10:
        case 13:
            string strRep;
            strRep = string("#") + IntToStr(s[i]) + ";";
            s.insert(i + 1, strRep);
            s[i] = '&';
            break;
        };
    }

    return s;
}

//-----------------------------------------------------------------------------
// 描述: XML字符串 -> 实际字符串
// 备注:
//   &gt;   -> '>'
//   &lt;   -> '<'
//   &quot; -> '"'
//   &#XXX; -> 十进制值所对应的ASCII字符
//-----------------------------------------------------------------------------
string XMLToStr(const string& str)
{
    string s = str;
    int i, j, h, n;

    i = 0;
    n = s.size();
    while (i < n - 1)
    {
        if (s[i] == '&')
        {
            if (i + 3 < n && s[i + 1] == '#')
            {
                j = i + 3;
                while (j < n && s[j] != ';') j++;
                if (s[j] == ';')
                {
                    h = StrToInt(s.substr(i + 2, j - i - 2).c_str());
                    s.erase(i, j - i);
                    s[i] = h;
                    n -= (j - i);
                }
            }
            else if (i + 3 < n && s.substr(i + 1, 3) == "gt;")
            {
                s.erase(i, 3);
                s[i] = '>';
                n -= 3;
            }
            else if (i + 3 < n && s.substr(i + 1, 3) == "lt;")
            {
                s.erase(i, 3);
                s[i] = '<';
                n -= 3;
            }
            else if (i + 5 < n && s.substr(i + 1, 5) == "quot;")
            {
                s.erase(i, 5);
                s[i] = '\"';
                n -= 5;
            }
        }

        i++;
    }

    return s;
}

///////////////////////////////////////////////////////////////////////////////
// CXMLNode

CXMLNode::CXMLNode() :
    m_pParentNode(NULL),
    m_pChildNodes(NULL),
    m_pProps(NULL)
{
    InitObject();
}

CXMLNode::CXMLNode(const CXMLNode& src)
{
    InitObject();
    *this = src;
}

CXMLNode::~CXMLNode()
{
    Clear();
    delete m_pProps;
    if (m_pParentNode && m_pParentNode->m_pChildNodes)
        m_pParentNode->m_pChildNodes->Remove(this);
}

void CXMLNode::InitObject()
{
    m_pProps = new CXMLNodeProps();
}

void CXMLNode::AssignNode(CXMLNode& Src, CXMLNode& Dest)
{
    Dest.m_strName = Src.m_strName;
    Dest.m_strDataString = Src.m_strDataString;
    *(Dest.m_pProps) = *(Src.m_pProps);
    for (int i = 0; i < Src.GetChildCount(); i++)
        AssignNode(*(Src.GetChildNodes(i)), *(Dest.Add()));
}

CXMLNode& CXMLNode::operator = (const CXMLNode& rhs)
{
    Clear();
    AssignNode((CXMLNode&)rhs, *this);
    return *this;
}

//-----------------------------------------------------------------------------
// 描述: 在当前节点下添加子节点(Node)
//-----------------------------------------------------------------------------
void CXMLNode::AddNode(CXMLNode *pNode)
{
    if (!m_pChildNodes)
        m_pChildNodes = new CList();

    m_pChildNodes->Add(pNode);
    if (pNode->m_pParentNode != NULL)
        pNode->m_pParentNode->m_pChildNodes->Remove(pNode);
    pNode->m_pParentNode = this;
}

//-----------------------------------------------------------------------------
// 描述: 在当前节点下插入子节点
//-----------------------------------------------------------------------------
void CXMLNode::InsertNode(int nIndex, CXMLNode *pNode)
{
    AddNode(pNode);
    m_pChildNodes->Delete(m_pChildNodes->Count() - 1);
    m_pChildNodes->Insert(nIndex, pNode);
}

//-----------------------------------------------------------------------------
// 描述: 在当前节点下添加一个新的子节点
//-----------------------------------------------------------------------------
CXMLNode* CXMLNode::Add()
{
    CXMLNode *pNode = new CXMLNode();
    AddNode(pNode);
    return pNode;
}

//-----------------------------------------------------------------------------
// 描述: 根据子节点名称，查找子节点。若未找到则返回 NULL。
//-----------------------------------------------------------------------------
CXMLNode* CXMLNode::FindChildNode(const string& strName)
{
    CXMLNode *pResult;
    int i;

    i = IndexOf(strName);
    if (i >= 0)
        pResult = GetChildNodes(i);
    else
        pResult = NULL;

    return pResult;
}

//-----------------------------------------------------------------------------
// 描述: 在当前节点下查找名称为 strName 的节点，返回其序号(0-based)。若未找到则返回-1。
//-----------------------------------------------------------------------------
int CXMLNode::IndexOf(const string& strName)
{
    int nResult = -1;

    for (int i = 0; i < GetChildCount(); i++)
        if (SameText(strName, GetChildNodes(i)->GetName()))
        {
            nResult = i;
            break;
        }

    return nResult;
}

//-----------------------------------------------------------------------------
// 描述: 清空当前节点及其所有子节点的数据
//-----------------------------------------------------------------------------
void CXMLNode::Clear()
{
    if (m_pChildNodes)
    {
        while (m_pChildNodes->Count() > 0)
            delete (CXMLNode*)(*m_pChildNodes)[0];
        delete m_pChildNodes;
        m_pChildNodes = NULL;
    }

    m_strName.clear();
    m_strDataString.clear();
    m_pProps->Clear();
}

CXMLNode* CXMLNode::GetRootNode() const
{
    CXMLNode *pResult = (CXMLNode*)this;
  
    while (pResult->GetParentNode() != NULL)
        pResult = pResult->GetParentNode();

    return pResult;
}

int CXMLNode::GetChildCount() const
{
    if (!m_pChildNodes)
        return 0;
    else
        return m_pChildNodes->Count();
}

CXMLNode* CXMLNode::GetChildNodes(int nIndex) const
{
    if (!m_pChildNodes) 
        return NULL;
    else
        return (CXMLNode*)(*m_pChildNodes)[nIndex];
}

void CXMLNode::SetParentNode(CXMLNode *pNode)
{
    if (m_pParentNode && m_pParentNode->m_pChildNodes)
        m_pParentNode->m_pChildNodes->Remove(this);
    m_pParentNode = pNode;
}

void CXMLNode::SetDataString(const string& strValue)
{
    m_strDataString = XMLToStr(strValue);
}

///////////////////////////////////////////////////////////////////////////////
// CXMLNodeProps

CXMLNodeProps::CXMLNodeProps()
{
}

CXMLNodeProps::CXMLNodeProps(const CXMLNodeProps& src)
{
    *this = src;
}

CXMLNodeProps::~CXMLNodeProps()
{
    Clear();
}

void CXMLNodeProps::ParsePropString(const string& strPropStr)
{
    string s, strName, strValue;
    int i;

    Clear();
    s = strPropStr;
    while (true)
    {
        i = s.find("=", 0);
        if (i >= 0)
        {
            strName = TrimString(s.substr(0, i));
            s.erase(0, i + 1);
            s = TrimString(s);
            if (s.size() > 0 && s[0] == '\"')
            {
                s.erase(0, 1);
                i = s.find("\"", 0);
                if (i >= 0)
                {
                    strValue = XMLToStr(s.substr(0, i));
                    s.erase(0, i + 1);
                }
                else
                    ThrowXMLException(S_INVALID_FILE_FORMAT);
            }
            else
                ThrowXMLException(S_INVALID_FILE_FORMAT);

            Add(strName, strValue);
        }
        else
        {
            if (TrimString(s).size() > 0)
                ThrowXMLException(S_INVALID_FILE_FORMAT);
            else
                break;
        }
    }
}

CXMLNodeProps& CXMLNodeProps::operator = (const CXMLNodeProps& rhs)
{
    CXMLNodePropItem Item;

    Clear();
    for (int i = 0; i < rhs.GetCount(); i++)
    {
        Item = rhs.GetItems(i);
        Add(Item.strName, Item.strValue);
    }
}

bool CXMLNodeProps::Add(const string& strName, const string& strValue)
{
    bool bResult;

    bResult = (IndexOf(strName) < 0);
    if (bResult)
    {
        CXMLNodePropItem *pItem = new CXMLNodePropItem();
        pItem->strName = strName;
        pItem->strValue = strValue;
        m_Items.Add(pItem);
    }

    return bResult;
}

void CXMLNodeProps::Remove(const string& strName)
{
    int i;

    i = IndexOf(strName);
    if (i >= 0)
    {
        delete GetItemPtr(i);
        m_Items.Delete(i);
    }
}

void CXMLNodeProps::Clear()
{
    for (int i = 0; i < m_Items.Count(); i++)
        delete GetItemPtr(i);
    m_Items.Clear();
}

int CXMLNodeProps::IndexOf(const string& strName)
{
    int nResult = -1;

    for (int i = 0; i < m_Items.Count(); i++)
        if (SameText(strName, GetItemPtr(i)->strName))
        {
            nResult = i;
            break;
        }

    return nResult;
}

bool CXMLNodeProps::PropExists(const string& strName)
{
    return (IndexOf(strName) >= 0);
}

string& CXMLNodeProps::ValueOf(const string& strName)
{
    int i;

    i = IndexOf(strName);
    if (i >= 0)
        return GetItemPtr(i)->strValue;
    else
    {
        Add(strName, "");
        return GetItemPtr(GetCount()-1)->strValue;
    }
}

CXMLNodePropItem CXMLNodeProps::GetItems(int nIndex) const
{
    CXMLNodePropItem Result;

    if (nIndex >= 0 && nIndex < m_Items.Count())
        Result = *(((CXMLNodeProps*)this)->GetItemPtr(nIndex));

    return Result;
}

string CXMLNodeProps::GetPropString() const
{
    string strResult;
    CXMLNodePropItem *pItem;

    for (int i = 0; i < m_Items.Count(); i++)
    {
        pItem = ((CXMLNodeProps*)this)->GetItemPtr(i);
        if (i > 0) strResult = strResult + " ";
        strResult = strResult + FormatString("%s=\"%s\"", pItem->strName.c_str(), pItem->strValue.c_str());
    }

    return strResult;
}

void CXMLNodeProps::SetPropString(const string& strPropString)
{
    ParsePropString(strPropString);
}

///////////////////////////////////////////////////////////////////////////////
// CXMLDocument

CXMLDocument::CXMLDocument() :
    m_bAutoIndent(true),
    m_nIndentSpaces(DEF_XML_INDENT_SPACES)
{
}

CXMLDocument::CXMLDocument(const CXMLDocument& src)
{
    *this = src;
}

CXMLDocument::~CXMLDocument()
{
}

CXMLDocument& CXMLDocument::operator = (const CXMLDocument& rhs)
{
    m_bAutoIndent = rhs.m_bAutoIndent;
    m_nIndentSpaces = rhs.m_nIndentSpaces;
    m_RootNode = rhs.m_RootNode;

    return *this;
}

void CXMLDocument::SaveToStream(CStream& Stream)
{
    CXMLWriter Writer(this, &Stream);

    Writer.WriteHeader();
    Writer.WriteRootNode(&m_RootNode);
}

void CXMLDocument::LoadFromStream(CStream& Stream)
{
    CXMLReader Reader(this, &Stream);

    m_RootNode.Clear();
    Reader.ReadHeader();
    Reader.ReadRootNode(&m_RootNode);
}

void CXMLDocument::SaveToString(string& str)
{
    CMemoryStream Stream;
    SaveToStream(Stream);
    str.assign(Stream.GetMemory(), Stream.GetSize());
}

void CXMLDocument::LoadFromString(string& str)
{
    CMemoryStream Stream;
    Stream.Write(str.c_str(), str.size());
    Stream.SetPosition(0);
    LoadFromStream(Stream);
}

void CXMLDocument::SaveToFile(const string& strFileName)
{
    CFileStream Stream(strFileName, FM_CREATE | FM_SHARE_DENY_WRITE);
    SaveToStream(Stream);
}

void CXMLDocument::LoadFromFile(const string& strFileName)
{
    CFileStream Stream(strFileName, FM_OPEN_READ | FM_SHARE_DENY_NONE);
    LoadFromStream(Stream);
}

void CXMLDocument::Clear()
{
    m_RootNode.Clear();
}

///////////////////////////////////////////////////////////////////////////////
// CXMLReader

CXMLReader::CXMLReader(CXMLDocument *pOwner, CStream *pStream)
{
    m_pOwner = pOwner;
    m_pStream = pStream;

    m_Buffer.SetSize(pStream->GetSize());
    pStream->SetPosition(0);
    pStream->Read(m_Buffer.Data(), m_Buffer.GetSize());

    m_nPosition = 0;
}

CXMLReader::~CXMLReader()
{
}

//-----------------------------------------------------------------------------
// 描述: 读取 XML 标签
//-----------------------------------------------------------------------------
XMLTagTypes CXMLReader::ReadXMLData(string& strName, string& strProp, string& strData)
{
    XMLTagTypes nResult = XTT_START_TAG;
    enum { FIND_LEFT, FIND_RIGHT, FIND_DATA, FIND_COMMENT, DONE } nState;
    char c;
    int i, n, nBufferSize, nComment;

    strName = "";
    strProp = "";
    strData = "";
    nComment = 0;
    nBufferSize = m_Buffer.GetSize();
    nState = FIND_LEFT;

    while (m_nPosition < nBufferSize && nState != DONE)
    {
        c = m_Buffer[m_nPosition];
        m_nPosition++;

        switch (nState)
        {
        case FIND_LEFT:
            if (c == '<') nState = FIND_RIGHT;
            break;

        case FIND_RIGHT:
            if (c == '>')
                nState = FIND_DATA;
            else if (c == '<')
                ThrowXMLException(S_INVALID_FILE_FORMAT);
            else
            {
                strName += c;
                if (strName == "!--")
                {
                    nState = FIND_COMMENT;
                    nComment = 0;
                    strName.clear();
                }
            }
            break;
        
        case FIND_DATA:
            if (c == '<')
            {
                m_nPosition--;
                nState = DONE;
                break;
            }
            else
                strData += c;
            break;
        
        case FIND_COMMENT:
            if (nComment == 2)
            {
                if (c == '>') nState = FIND_LEFT;
            }
            else if (c == '-')
                nComment++;
            else
                nComment = 0;
            break;
        }
    }

    if (nState == FIND_RIGHT)
        ThrowXMLException(S_INVALID_FILE_FORMAT);

    strName = TrimString(strName);
    n = strName.size();
    if (n > 0 && strName[n - 1] == '/')
    {
        strName.resize(n - 1);
        n--;
        nResult |= XTT_END_TAG;
    }
    if (n > 0 && strName[0] == '/')
    {
        strName.erase(0, 1);
        n--;
        nResult = XTT_END_TAG;
    }
    if (n <= 0)
        nResult = 0;

    if (nResult != XTT_START_TAG)
        strData.clear();

    if ((nResult & XTT_START_TAG) != 0)
    {
        i = strName.find(" ", 0);
        if (i == string::npos)
            i = strName.find("\t", 0);
        if (i != string::npos)
        {
            strProp = TrimString(strName.substr(i + 1));
            strName.erase(i);
            strName = TrimString(strName);
        }
        strData = TrimString(strData);
    }

    return nResult;
}

XMLTagTypes CXMLReader::ReadNode(CXMLNode *pNode)
{
    XMLTagTypes nResult, TagTypes;
    CXMLNode *pChildNode;
    string strName, strProp, strData;
    string strEndName;

    nResult = ReadXMLData(strName, strProp, strData);

    pNode->SetName(strName);
    if ((nResult & XTT_START_TAG) != 0)
    {
        pNode->GetProps()->SetPropString(strProp);
        pNode->SetDataString(strData);
    }
    else
        return nResult;

    if ((nResult & XTT_END_TAG) != 0) return nResult;

    do
    {
        pChildNode = new CXMLNode();
        try
        {
            TagTypes = 0;
            TagTypes = ReadNode(pChildNode);
        }
        catch (CException& e)
        {
            delete pChildNode;
            throw;
        }

        if ((TagTypes & XTT_START_TAG) != 0)
            pNode->AddNode(pChildNode);
        else
        {
            strEndName = pChildNode->GetName();
            delete pChildNode;
        }
    }
    while ((TagTypes & XTT_START_TAG) != 0);

    if (!SameText(strName, strEndName))
        ThrowXMLException(S_INVALID_FILE_FORMAT);

    return nResult;
}

void CXMLReader::ReadHeader()
{
    string s1, s2, s3;

    ReadXMLData(s1, s2, s3);
    if (s1.find("?xml", 0) != 0)
        ThrowXMLException(S_INVALID_FILE_FORMAT);
}

void CXMLReader::ReadRootNode(CXMLNode *pNode)
{
    ReadNode(pNode);
}

///////////////////////////////////////////////////////////////////////////////
// CXMLWriter

CXMLWriter::CXMLWriter(CXMLDocument *pOwner, CStream *pStream)
{
    m_pOwner = pOwner;
    m_pStream = pStream;
}

CXMLWriter::~CXMLWriter()
{
}

void CXMLWriter::FlushBuffer()
{
    if (!m_strBuffer.empty())
        m_pStream->Write(m_strBuffer.c_str(), m_strBuffer.size());
    m_strBuffer.clear();
}

void CXMLWriter::WriteLn(const string& str)
{
    string s = str;

    m_strBuffer += s;
    if (m_pOwner->GetAutoIndent())
        m_strBuffer += S_CRLF;
    FlushBuffer();
}

void CXMLWriter::WriteNode(CXMLNode *pNode, int nIndent)
{
    string s;

    if (!m_pOwner->GetAutoIndent())
        nIndent = 0;

    if (pNode->GetProps()->GetCount() > 0)
    {
        s = pNode->GetProps()->GetPropString();
        if (s.empty() || s[0] != ' ')
            s = string(" ") + s;
    }

    if (pNode->GetDataString().size() > 0 && pNode->GetChildCount() == 0)
        s = s + ">" + StrToXML(pNode->GetDataString()) + "</" + pNode->GetName() + ">";
    else if (pNode->GetChildCount() == 0)
        s = s + "/>";
    else
        s = s + ">";

    s = string(nIndent, ' ') + "<" + pNode->GetName() + s;
    WriteLn(s);

    for (int i = 0; i < pNode->GetChildCount(); i++)
        WriteNode(pNode->GetChildNodes(i), nIndent + m_pOwner->GetIndentSpaces());

    if (pNode->GetChildCount() > 0)
    {
        s = string(nIndent, ' ') + "</" + pNode->GetName() + ">";
        WriteLn(s);
    }
}

void CXMLWriter::WriteHeader()
{
    WriteLn("<?xml version=\"1.0\"?>");
}

void CXMLWriter::WriteRootNode(CXMLNode *pNode)
{
    WriteNode(pNode, 0);
    FlushBuffer();
}

///////////////////////////////////////////////////////////////////////////////
