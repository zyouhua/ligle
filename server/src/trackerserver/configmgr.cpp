///////////////////////////////////////////////////////////////////////////////
// Msger Main Server
//
// 文件名称: configmgr.cpp
// 功能描述: 配置管理 (注: 此处指应用程序配置，而非ISE配置)
// 最后修改: 2005-07-01
///////////////////////////////////////////////////////////////////////////////

#include "configmgr.h"

///////////////////////////////////////////////////////////////////////////////
// CConfigManager

CConfigManager::CConfigManager()
{
}

CConfigManager::~CConfigManager()
{
}

//----------------------------------------------------------------------------
// 描述: 拆分名称路径
//----------------------------------------------------------------------------
StringArray CConfigManager::SplitNamePath(const string& strNamePath)
{
    StringArray Result;
    SplitString(strNamePath, NAME_PATH_SPLITTER, Result);
    return Result;
}

//----------------------------------------------------------------------------
// 描述: 从外部配置文件载入配置信息
// 备注: 若载入失败，则抛出异常。
//----------------------------------------------------------------------------
void CConfigManager::LoadFromFile()
{
    string strFileName;
    CXMLDocument XMLDoc;

    try
    {
        strFileName = Application.GetExePath() + SERVER_CONFIG_FILE;
        XMLDoc.LoadFromFile(strFileName);
    }
    catch (CException& e)
    {
        string s = FormatString("fail to load config file (%s).", strFileName.c_str());
        throw CException(s.c_str(), __FILE__, __LINE__);
    }

    m_XMLDoc = XMLDoc;
}

//----------------------------------------------------------------------------
// 描述: 根据名称路径取得配置字符串
//----------------------------------------------------------------------------
string CConfigManager::GetString(const string& strNamePath)
{
    string strResult;
    StringArray NameList = SplitNamePath(strNamePath);
    CXMLNode *pNode, *pResultNode;

    if (NameList.size() == 0) return strResult;

    pNode = m_XMLDoc.GetRootNode();
    for (int i = 0; (i < NameList.size() - 1) && pNode; i++)
        pNode = pNode->FindChildNode(NameList[i]);

    if (pNode)
    {
        string strLastName = NameList[NameList.size() - 1];

        // 名称路径中的最后一个名称既可是节点名，也可以是属性名
        pResultNode = pNode->FindChildNode(strLastName);
        if (pResultNode)
            strResult = pResultNode->GetDataString();
        else if (pNode->GetProps()->PropExists(strLastName))
            strResult = pNode->GetProps()->ValueOf(strLastName);
    }

    return strResult;
}

//----------------------------------------------------------------------------
// 描述: 根据名称路径取得配置字符串(以整型返回)
//----------------------------------------------------------------------------
int CConfigManager::GetInteger(const string& strNamePath, int nDefault)
{
    return StrToInt(GetString(strNamePath), nDefault);
}

//----------------------------------------------------------------------------
// 描述: 根据名称路径取得配置字符串(以浮点型返回)
//----------------------------------------------------------------------------
double CConfigManager::GetFloat(const string& strNamePath, double fDefault)
{
    return StrToFloat(GetString(strNamePath), fDefault);
}

//----------------------------------------------------------------------------
// 描述: 根据名称路径取得配置字符串(以布尔型返回)
//----------------------------------------------------------------------------
bool CConfigManager::GetBoolean(const string& strNamePath, bool bDefault)
{
    string str = GetString(strNamePath);

    if (SameText(str, S_TRUE))
        return true;
    else if (SameText(str, S_FALSE))
        return false;
    else
        return bDefault;
}

///////////////////////////////////////////////////////////////////////////////
