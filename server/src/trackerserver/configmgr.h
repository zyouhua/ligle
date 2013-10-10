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
// 常量定义

const char NAME_PATH_SPLITTER = '.';

#define S_TRUE      "true"
#define S_FALSE     "false"

///////////////////////////////////////////////////////////////////////////////
// CConfigManager - 配置管理器类
//
// 说明:
// 1. 此类用于管理应用程序自身的配置信息(而非ISE配置)，配置信息存放在一个XML文件中，此类
//    提供了各种函数方便地读取配置信息。
// 2. XML配置文件名固定为 SERVER_CONFIG_FILE (见 global.h)，且在应用程序当前目录下。
//
// 名词解释:
//   NamePath: 配置值的名称路径。它由一系列XML节点名组成，各名称之间用点号(.)分隔，最后
//   一个名称可以是节点的属性名。XML的根节点名称在名称路径中被省略。名称路径不区分大小写。
//   须注意的是，由于名称之间用点号分隔，所以各名称内部不应含有点号，以免混淆。
//   示例: "Database.MainDb.HostName"

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
