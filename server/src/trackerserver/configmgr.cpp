///////////////////////////////////////////////////////////////////////////////
// Msger Main Server
//
// �ļ�����: configmgr.cpp
// ��������: ���ù��� (ע: �˴�ָӦ�ó������ã�����ISE����)
// ����޸�: 2005-07-01
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
// ����: �������·��
//----------------------------------------------------------------------------
StringArray CConfigManager::SplitNamePath(const string& strNamePath)
{
    StringArray Result;
    SplitString(strNamePath, NAME_PATH_SPLITTER, Result);
    return Result;
}

//----------------------------------------------------------------------------
// ����: ���ⲿ�����ļ�����������Ϣ
// ��ע: ������ʧ�ܣ����׳��쳣��
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
// ����: ��������·��ȡ�������ַ���
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

        // ����·���е����һ�����Ƽȿ��ǽڵ�����Ҳ������������
        pResultNode = pNode->FindChildNode(strLastName);
        if (pResultNode)
            strResult = pResultNode->GetDataString();
        else if (pNode->GetProps()->PropExists(strLastName))
            strResult = pNode->GetProps()->ValueOf(strLastName);
    }

    return strResult;
}

//----------------------------------------------------------------------------
// ����: ��������·��ȡ�������ַ���(�����ͷ���)
//----------------------------------------------------------------------------
int CConfigManager::GetInteger(const string& strNamePath, int nDefault)
{
    return StrToInt(GetString(strNamePath), nDefault);
}

//----------------------------------------------------------------------------
// ����: ��������·��ȡ�������ַ���(�Ը����ͷ���)
//----------------------------------------------------------------------------
double CConfigManager::GetFloat(const string& strNamePath, double fDefault)
{
    return StrToFloat(GetString(strNamePath), fDefault);
}

//----------------------------------------------------------------------------
// ����: ��������·��ȡ�������ַ���(�Բ����ͷ���)
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
