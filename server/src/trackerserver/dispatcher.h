///////////////////////////////////////////////////////////////////////////////
// Msger Main Server
// dispatcher.h
///////////////////////////////////////////////////////////////////////////////

#ifndef _DISPATCHER_H_
#define _DISPATCHER_H_

#include <stdio.h>
#include <stdlib.h>

#include "ise_system.h"
#include "global.h"
#include "configmgr.h"
#include "database.h"
#include "packet_udp.h"
#include "svrmodmgr.h"
#include "messages.h"

///////////////////////////////////////////////////////////////////////////////
// 提前声明

class CAppDispatcher;
class CAppGlobalData;
class CServerModule;

///////////////////////////////////////////////////////////////////////////////
// 类型定义

// 动作代码数组
typedef vector<uint> ActionCodeArray;
// <动作代码, UDP服务模块组别号> 映射表
typedef map<uint, int> ActionCodeMap;

///////////////////////////////////////////////////////////////////////////////
// class CAppDispatcher - 程序数据分派器类

class CAppDispatcher : public CCustomDispatcher
{
private:
    CAppGlobalData *m_pGlobalData;              // 全局数据
    CServerModuleMgr *m_pServerModuleMgr;       // 服务器模块管理器
    ActionCodeMap m_ActionCodeMap;              // <动作代码, UDP服务模块组别号> 映射表

private:
    void InitActionCodeMap();
    void InitDatabase();
    void AdjustServerOptions();
    void DaemonHelperExecute(CHelperThread& HelperThread);

    inline ServerModuleList& GetServerModuleList() { return m_pServerModuleMgr->GetServerModuleList(); }
    inline ServerModuleList& GetUdpSvrModuleList() { return m_pServerModuleMgr->GetUdpSvrModuleList(); }
//inline ServerModuleList& GetTcpSvrModuleList() { return m_pServerModuleMgr->GetTcpSvrModuleList(); }
public:
    CAppDispatcher();
    virtual ~CAppDispatcher();

    // 初始化 (失败则抛出异常)
    virtual void Initialize();
    // 结束化 (无论初始化是否有异常，结束时都会执行)
    virtual void Finalize();

    // UDP数据包分类
    virtual void ClassifyUdpPacket(void *pPacketBuffer, int nPacketSize, int& nGroupIndex);
    // UDP数据包分派
    virtual void DispatchUdpPacket(CUdpWorkerThread& WorkerThread, int nGroupIndex, CUdpPacket& Packet);
    // TCP连接分派
    virtual void DispatchTcpConnection(CTcpWorkerThread& WorkerThread, int nGroupIndex, CTcpConnection& Connection);

    // 辅助服务线程执行 (nHelperIndex: 0-based)
    virtual void HelperThreadExecute(CHelperThread& HelperThread, int nHelperIndex);

    // 分派消息给所有的服务器模块
    void DispatchMessage(CBaseMessage& Message);

    // 返回全局数据对象
    CAppGlobalData& GetGlobalData() { return *m_pGlobalData; }
};

///////////////////////////////////////////////////////////////////////////////
// class CAppGlobalData - 全局数据类
//
// 说明:
// 1. 此类负责管理全局数据。
// 2. 全局数据的创建分两个步骤: (1)创建对象; (2)数据初始化。
//    只所以要这么做，是因为某些全局对象的初始化依赖 CAppDispatcher::m_pGlobalData，
//    而在上述全局对象的构造函数中，CAppGlobalData对象尚未创建完毕(m_pGlobalData为NULL)。
//    所以，全局对象的创建和初始化应该分开进行。

class CAppGlobalData
{
private:
    CConfigManager m_ConfigManager;             // 配置管理器 
    CDatabaseManager m_DbManager;               // 数据库管理器

public:
    // 初始化全局数据 (由 CAppDispatcher::Initialize() 调用)
    void Initialize();

    // 返回配置管理器
    inline CConfigManager& GetConfigManager() { return m_ConfigManager; }
    // 返回数据库管理器
    inline CDatabaseManager& GetDbManager() { return m_DbManager; }
};

///////////////////////////////////////////////////////////////////////////////
// class CServerModule - 服务器模块基类

class CServerModule
{
private:
    CAppDispatcher *m_pDispatcher;    // 程序数据分派器 (引用)

protected:
    // 发送UDP数据包
    void SendUdpPacket(CUdpBizPacket& Packet, const CInetAddress& ToAddr, 
        int nToPort, int nSendTimes = 1);
    // 发送UDP通用应答包
    void SendUdpRePacket(const CInetAddress& ToAddr, int nToPort, 
        int nSeqNumber, int nResultCode = RET_SUCCESS, int nSendTimes = 1);

    // 返回常用对象
    inline CAppGlobalData& AppGlobalData() { return m_pDispatcher->GetGlobalData(); }
    inline CConfigManager& ConfigManager() { return AppGlobalData().GetConfigManager(); }
public:
    CServerModule();
    virtual ~CServerModule() {}

    // 取得该模块管辖的UDP数据包动作代码
    virtual void GetUdpActionCodes(ActionCodeArray& List) {}

    // UDP数据包分派
    virtual void DispatchUdpPacket(CUdpWorkerThread& WorkerThread, CUdpPacket& Packet) {}
    // TCP连接分派
    virtual void DispatchTcpConnection(CTcpWorkerThread& WorkerThread, CTcpConnection& Connection) {}
    // 消息分派
    virtual void DispatchMessage(CBaseMessage& Message) {}

    // 返回此模块所需辅助服务线程的数量
    virtual int GetHelperThreadCount() { return 0; }
    // 辅助服务线程执行(nHelperIndex: 0-based)
    virtual void HelperThreadExecute(CHelperThread& HelperThread, int nHelperIndex) {}
    // 系统守护辅助线程执行 (nSecondCount: 0-based)
    virtual void DaemonHelperExecute(CHelperThread& HelperThread, int nSecondCount) {}
};

///////////////////////////////////////////////////////////////////////////////

#endif // _DISPATCHER_H_ 
