///////////////////////////////////////////////////////////////////////////////
// Msger Main Server
// svrmodmgr.h
///////////////////////////////////////////////////////////////////////////////

#ifndef _SVRMODMGR_H_
#define _SVRMODMGR_H_

#include <stdio.h>
#include <stdlib.h>

#include "ise_system.h"
#include "global.h"

///////////////////////////////////////////////////////////////////////////////
// 提前声明

class CServerModule;

///////////////////////////////////////////////////////////////////////////////
// 类型定义

typedef vector<CServerModule*> ServerModuleList;

///////////////////////////////////////////////////////////////////////////////
// 常量定义

//-----------------------------------------------------------------------------
//-- UDP服务器模块的组别号(GroupIndex)常量: (必须连续, 有效组别号从0算起)

const int UDP_GI_NONE           = -1;       // (none)
const int UDP_GI_FINDER         = 0;        // finder module
const int UDP_GI_TRANSFER       = 1;        // transfer module


const int UDP_SVR_MODULE_COUNT  = 1;        // UDP服务器模块总数

//-----------------------------------------------------------------------------
//-- TCP服务器模块的组别号(GroupIndex)常量: (必须连续, 有效组别号从0算起)

const int TCP_GI_NONE           = -1;       // (none)
const int TCP_GI_NETMGR         = 0;        // tracker服务器网络管理接口

const int TCP_SVR_MODULE_COUNT  = 1;        // TCP服务器模块总数

//-----------------------------------------------------------------------------
//-- UDP服务器参数:

// UDP服务端口号
const int UDP_SERVER_PORT = 9100;

// UDP监听线程的数量
const int UDP_LISTENER_THREAD_COUNT = 2;

// UDP请求队列的最大容量
const int UDP_QUEUE_CAPACITIES [UDP_SVR_MODULE_COUNT] = {
    100000,         // finder module
//    1000            // transfer module
};

// UDP工作者线程个数的上下限
const struct { int nMin, nMax; } UDP_WORK_THREADS [UDP_SVR_MODULE_COUNT] = {
    {10, 100},      // finder module         (10, 100)
//    {10, 20}        // transfer module
};

//-----------------------------------------------------------------------------
//-- TCP服务器参数:

// TCP服务端口号
const int TCP_SERVER_PORTS [TCP_SVR_MODULE_COUNT] = {
    9100
};

// TCP请求队列的容量
const int TCP_QUEUE_CAPACITIES [TCP_SVR_MODULE_COUNT] = {
    0           // netmgr module
};

// TCP工作者线程个数的上下限
const struct { int nMin, nMax; } TCP_WORK_THREADS [TCP_SVR_MODULE_COUNT] = {
    {2, 2}       // netmgr module
};

///////////////////////////////////////////////////////////////////////////////
// class CServerModuleMgr

class CServerModuleMgr
{
private:
    ServerModuleList m_ServerModuleList;        // 服务器模块列表
    ServerModuleList m_UdpSvrModuleList;        // UDP服务器模块列表(引用 m_ServerModuleList 中的模块)
   // ServerModuleList m_TcpSvrModuleList;        // TCP服务器模块列表(引用 m_ServerModuleList 中的模块)
public:
    CServerModuleMgr();
    virtual ~CServerModuleMgr();

    void Initialize();
    void Finalize();

    inline ServerModuleList& GetServerModuleList() { return m_ServerModuleList; }
    inline ServerModuleList& GetUdpSvrModuleList() { return m_UdpSvrModuleList; }
   // inline ServerModuleList& GetTcpSvrModuleList() { return m_TcpSvrModuleList; }
};

///////////////////////////////////////////////////////////////////////////////

#endif // _SVRMODMGR_H_ 
