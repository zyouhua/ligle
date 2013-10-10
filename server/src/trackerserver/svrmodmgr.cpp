///////////////////////////////////////////////////////////////////////////////
// Msger Main Server
//
// 文件名称: svrmodmgr.cpp
// 功能描述: 服务器模块管理
// 最后修改: 2005-07-13
///////////////////////////////////////////////////////////////////////////////

#include "svrmodmgr.h"
// 服务器模块:
#include "finder.h"
// TODO: 其它服务模块可在此包含。

///////////////////////////////////////////////////////////////////////////////
// classs CServerModuleMgr

CServerModuleMgr::CServerModuleMgr()
{
}

CServerModuleMgr::~CServerModuleMgr()
{
    Finalize();
}

//-----------------------------------------------------------------------------
// 描述: 初始化 (失败则抛出异常)
//-----------------------------------------------------------------------------
void CServerModuleMgr::Initialize()
{
    Finalize();

     // 创建服务器模块 (创浸务器模块的分选应注意其下标号)
       m_ServerModuleList.push_back((CServerModule*)new CFinder());        // [0]: UDP
    // m_ServerModuleList.push_back((CServerModule*)new CServerModule_NetMgr());       // [1]: TCP
   // m_ServerModuleList.push_back((CServerModule*)new CServerModule_Transfer());     // [2]: UDP

    // 挑选出 UDP 服务器模块，并依次放入 m_UdpSvrModuleList 中:
      m_UdpSvrModuleList.resize(UDP_SVR_MODULE_COUNT);
      m_UdpSvrModuleList[UDP_GI_FINDER] = m_ServerModuleList[0];
   // m_UdpSvrModuleList[UDP_GI_TRANSFER] = m_ServerModuleList[2];

    // 挑选出 TCP 服务器模块，并依次放入 m_TcpSvrModuleList 中:
    //m_TcpSvrModuleList.resize(TCP_SVR_MODULE_COUNT);
    //m_TcpSvrModuleList[TCP_GI_NETMGR] = m_ServerModuleList[1];
}

//-----------------------------------------------------------------------------
// 描述: 结束化 (无论初始化是否有异常，结束时都会执行)
//-----------------------------------------------------------------------------
void CServerModuleMgr::Finalize()
{
   for (int i = 0; i < m_ServerModuleList.size(); i++)
        delete m_ServerModuleList[i];
    m_ServerModuleList.clear();
    m_UdpSvrModuleList.clear();
    //m_TcpSvrModuleList.clear();
}

///////////////////////////////////////////////////////////////////////////////
