///////////////////////////////////////////////////////////////////////////////
// Msger Main Server
//
// 文件名称: dispatcher.cpp
// 功能描述: 程序数据分派
// 最后修改: 2005-07-14
///////////////////////////////////////////////////////////////////////////////

#include "dispatcher.h"
#include "utilities.h"

///////////////////////////////////////////////////////////////////////////////
// class CAppDispatcher

CAppDispatcher::CAppDispatcher() :
    m_pGlobalData(NULL),
    m_pServerModuleMgr(NULL)
{
}

CAppDispatcher::~CAppDispatcher()
{
}

//-----------------------------------------------------------------------------
// 描述: 初始化 (失败则抛出异常)
//-----------------------------------------------------------------------------
void CAppDispatcher::Initialize()
{
    CCustomDispatcher::Initialize();
   
    // 全局数据须首先创建！之后的初始化会用到全局数据。
    m_pGlobalData = new CAppGlobalData();
    m_pGlobalData->Initialize();
    
    // 创建并初始化服务器模块列表
    m_pServerModuleMgr = new CServerModuleMgr();
    m_pServerModuleMgr->Initialize();
    InitActionCodeMap();
    InitDatabase();
    AdjustServerOptions();
    
}

//-----------------------------------------------------------------------------
// 描述: 结束化 (无论初始化是否有异常，结束时都会执行)
//-----------------------------------------------------------------------------
void CAppDispatcher::Finalize()
{
    m_pServerModuleMgr->Finalize();
    delete m_pServerModuleMgr;
    m_pServerModuleMgr = NULL;

    delete m_pGlobalData;
    m_pGlobalData = NULL;

    CCustomDispatcher::Finalize();
}

//-----------------------------------------------------------------------------
// 描述: 初始化动作代码映射表
//-----------------------------------------------------------------------------
void CAppDispatcher::InitActionCodeMap()
{
    m_ActionCodeMap.clear();
    
    for (int i = 0; i < GetUdpSvrModuleList().size(); i++)
                       // GetUdpSvrModuleList().size()
    {
        ActionCodeArray List;
        GetUdpSvrModuleList()[i]->GetUdpActionCodes(List);
        
        for (int j = 0; j < List.size(); j++)
        {
            m_ActionCodeMap[List[j]] = i;
        }
     
    }
}

//-----------------------------------------------------------------------------
// 描述: 初始化数据库中的数据
//-----------------------------------------------------------------------------
void CAppDispatcher::InitDatabase()
{
}

//-----------------------------------------------------------------------------
// 描述: 调整服务器相关配置
//-----------------------------------------------------------------------------
void CAppDispatcher::AdjustServerOptions()
{
    // 设置辅助服务线程的数量
    int nHelperCount = 0;
    nHelperCount++;  // 系统守护线程 (daemon helper thread)
    for (int i = 0; i < GetServerModuleList().size(); i++)
        nHelperCount += GetServerModuleList()[i]->GetHelperThreadCount();
    Application.GetServerOptions().SetHelperThreadCount(nHelperCount);
}

//-----------------------------------------------------------------------------
// 描述: 系统守护辅助线程的执行
//-----------------------------------------------------------------------------
void CAppDispatcher::DaemonHelperExecute(CHelperThread& HelperThread)
{
    int nSecondCount = 0;

    while (!HelperThread.GetTerminated())
    {
        for (int i = 0; i < GetServerModuleList().size(); i++)
        {
            try
            {
                GetServerModuleList()[i]->DaemonHelperExecute(HelperThread, nSecondCount);
            }
            catch (CException& e)
            {
                Logger.Write(e);
            }
        }

        nSecondCount++;
        HelperThread.Sleep(1);  // 睡眠1秒
    }
}

//-----------------------------------------------------------------------------
// 描述: UDP数据包分类
//-----------------------------------------------------------------------------
void CAppDispatcher::ClassifyUdpPacket(void *pPacketBuffer, int nPacketSize, int& nGroupIndex)
{
    nGroupIndex = UDP_GI_NONE;
    if (nPacketSize <= 0) return;
    CUdpPacketHeader* pHeader = (CUdpPacketHeader*)pPacketBuffer;
    
    if (pHeader->nProtoType == PP_UDP)
    {
        if (nPacketSize < sizeof(CUdpPacketHeader)) return;

        ActionCodeMap::iterator iter;
        iter = m_ActionCodeMap.find(pHeader->nActionCode);
        if (iter != m_ActionCodeMap.end())
        {
            nGroupIndex = iter->second;
        }
    }
}

//-----------------------------------------------------------------------------
// 描述: UDP数据包分派
//-----------------------------------------------------------------------------
void CAppDispatcher::DispatchUdpPacket(CUdpWorkerThread& WorkerThread, 
    int nGroupIndex, CUdpPacket& Packet)
{
    if (nGroupIndex >= 0 && nGroupIndex < GetUdpSvrModuleList().size())
    {
        // 分发给处理模块
        GetUdpSvrModuleList()[nGroupIndex]->DispatchUdpPacket(WorkerThread, Packet);
    }
}

//-----------------------------------------------------------------------------
// 描述: TCP连接分派
//-----------------------------------------------------------------------------
void CAppDispatcher::DispatchTcpConnection(CTcpWorkerThread& WorkerThread, 
    int nGroupIndex, CTcpConnection& Connection)
{
   /* if (nGroupIndex >= 0 && nGroupIndex < GetTcpSvrModuleList().size())
    {
        // 分发给服务器模块
        GetTcpSvrModuleList()[nGroupIndex]->DispatchTcpConnection(WorkerThread, Connection);
    }*/
}

//-----------------------------------------------------------------------------
// 描述: 辅助服务线程执行 (nHelperIndex: 0-based)
//-----------------------------------------------------------------------------
void CAppDispatcher::HelperThreadExecute(CHelperThread& HelperThread, int nHelperIndex)
{
    // 系统守护辅助线程的 nHelperIndex 为 0
    if (nHelperIndex == 0)
    {
        DaemonHelperExecute(HelperThread);
    }
    else
    {
        int nIndex1, nIndex2 = 0;

        // 减去系统占用的线程数
        nHelperIndex -= 1; 

        for (int i = 0; i < GetServerModuleList().size(); i++)
        {
            nIndex1 = nIndex2;
            nIndex2 += GetServerModuleList()[i]->GetHelperThreadCount();

            if (nHelperIndex >= nIndex1 && nHelperIndex < nIndex2)
                GetServerModuleList()[i]->HelperThreadExecute(HelperThread, nHelperIndex - nIndex1);
        }
    }
}

//-----------------------------------------------------------------------------
// 描述: 对所有的服务器模块广播消息
//-----------------------------------------------------------------------------
void CAppDispatcher::DispatchMessage(CBaseMessage& Message)
{
    for (int i = 0; i < GetServerModuleList().size(); i++)
        GetServerModuleList()[i]->DispatchMessage(Message);
}

///////////////////////////////////////////////////////////////////////////////
// class CAppGlobalData

//-----------------------------------------------------------------------------
// 描述: 初始化全局数据 (由 CAppDispatcher::Initialize() 调用)
// 备注: 若初始化失败，则抛出异常。
//-----------------------------------------------------------------------------
void CAppGlobalData::Initialize()
{
    m_ConfigManager.LoadFromFile();
    m_DbManager.Initialize();
}

///////////////////////////////////////////////////////////////////////////////
// class CServerModule

CServerModule::CServerModule()
{
    m_pDispatcher = GetAppDispatcher();
}

//-----------------------------------------------------------------------------
// 描述: 发送UDP数据包
// 参数:
//   Packet     - 待发的数据包 (已Pack)
//   nSendTimes - 发送次数 (缺省1次)
//-----------------------------------------------------------------------------
void CServerModule::SendUdpPacket(CUdpBizPacket& Packet, const CInetAddress& ToAddr, 
    int nToPort, int nSendTimes)
{
    Packet.EnsurePacked();
    m_pDispatcher->GetUtils().SendUdpPacket(Packet.GetBuffer(), Packet.GetSize(), 
        ToAddr, nToPort, nSendTimes);
}

//-----------------------------------------------------------------------------
// 描述: 发送UDP通用应答包
// 参数:
//   ToAddr       - 对方地址
//   nToPort      - 对方端口
//   nSeqNumber   - 对方的请求包的SeqNumber
//   nResultCode  - 结果代码
//   nSendTimes   - 发送次数 (缺省1次)
//-----------------------------------------------------------------------------
void CServerModule::SendUdpRePacket(const CInetAddress& ToAddr, int nToPort, 
    int nSeqNumber, int nResultCode, int nSendTimes)
{
    CRePacket RePacket;
    
    // 准备应答包
    RePacket.InitHeader(SC_ACK, nSeqNumber);
    RePacket.InitData(nResultCode);
    // 发送应答包
    SendUdpPacket(RePacket, ToAddr, nToPort, nSendTimes);
}

///////////////////////////////////////////////////////////////////////////////
