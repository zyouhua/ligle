///////////////////////////////////////////////////////////////////////////////
// Msger Main Server
// configmgr.h
///////////////////////////////////////////////////////////////////////////////

#ifndef _CONFIGMGR_H_
#define _CONFIGMGR_H_

#include <stdio.h>
#include <stdlib.h>

#include "ise_system.h"
#include "xmldoc.h"
#include "global.h"

///////////////////////////////////////////////////////////////////////////////
// ��������

const char NAME_PATH_SPLITTER = '.';

#define S_TRUE      "true"
#define S_FALSE     "false"

///////////////////////////////////////////////////////////////////////////////
// CConfigManager - ���ù�������
//
// ˵��:
// 1. �������ڹ���Ӧ�ó��������������Ϣ(����ISE����)��������Ϣ�����һ��XML�ļ��У�����
//    �ṩ�˸��ֺ�������ض�ȡ������Ϣ��
// 2. XML�����ļ����̶�Ϊ SERVER_CONFIG_FILE (�� global.h)������Ӧ�ó���ǰĿ¼�¡�
//
// ���ʽ���:
//   NamePath: ����ֵ������·��������һϵ��XML�ڵ�����ɣ�������֮���õ��(.)�ָ������
//   һ�����ƿ����ǽڵ����������XML�ĸ��ڵ�����������·���б�ʡ�ԡ�����·�������ִ�Сд��
//   ��ע����ǣ���������֮���õ�ŷָ������Ը������ڲ���Ӧ���е�ţ����������
//   ʾ��: "Database.MainDb.HostName"

class CConfigManager
{
private:
    CXMLDocument m_XMLDoc;

    StringArray SplitNamePath(const string& strNamePath);
public:
    CConfigManager();
    virtual ~CConfigManager();

    void LoadFromFile();

    string GetString(const string& strNamePath);
    int GetInteger(const string& strNamePath, int nDefault = 0);
    double GetFloat(const string& strNamePath, double fDefault = 0);
    bool GetBoolean(const string& strNamePath, bool bDefault = 0);

    CXMLDocument& GetXMLDoc() { return m_XMLDoc; }
};

///////////////////////////////////////////////////////////////////////////////

#endif // _CONFIGMGR_H_ 
