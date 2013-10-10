///////////////////////////////////////////////////////////////////////////////
// Msger Main Server
//
// �ļ�����: dispatcher.cpp
// ��������: �������ݷ���
// ����޸�: 2005-07-14
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
// ����: ��ʼ�� (ʧ�����׳��쳣)
//-----------------------------------------------------------------------------
void CAppDispatcher::Initialize()
{
    CCustomDispatcher::Initialize();
   
    // ȫ�����������ȴ�����֮��ĳ�ʼ�����õ�ȫ�����ݡ�
    m_pGlobalData = new CAppGlobalData();
    m_pGlobalData->Initialize();
    
    // ��������ʼ��������ģ���б�
    m_pServerModuleMgr = new CServerModuleMgr();
    m_pServerModuleMgr->Initialize();
    InitActionCodeMap();
    InitDatabase();
    AdjustServerOptions();
    
}

//-----------------------------------------------------------------------------
// ����: ������ (���۳�ʼ���Ƿ����쳣������ʱ����ִ��)
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
// ����: ��ʼ����������ӳ���
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
// ����: ��ʼ�����ݿ��е�����
//-----------------------------------------------------------------------------
void CAppDispatcher::InitDatabase()
{
}

//-----------------------------------------------------------------------------
// ����: �����������������
//-----------------------------------------------------------------------------
void CAppDispatcher::AdjustServerOptions()
{
    // ���ø��������̵߳�����
    int nHelperCount = 0;
    nHelperCount++;  // ϵͳ�ػ��߳� (daemon helper thread)
    for (int i = 0; i < GetServerModuleList().size(); i++)
        nHelperCount += GetServerModuleList()[i]->GetHelperThreadCount();
    Application.GetServerOptions().SetHelperThreadCount(nHelperCount);
}

//-----------------------------------------------------------------------------
// ����: ϵͳ�ػ������̵߳�ִ��
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
        HelperThread.Sleep(1);  // ˯��1��
    }
}

//-----------------------------------------------------------------------------
// ����: UDP���ݰ�����
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
// ����: UDP���ݰ�����
//-----------------------------------------------------------------------------
void CAppDispatcher::DispatchUdpPacket(CUdpWorkerThread& WorkerThread, 
    int nGroupIndex, CUdpPacket& Packet)
{
    if (nGroupIndex >= 0 && nGroupIndex < GetUdpSvrModuleList().size())
    {
        // �ַ�������ģ��
        GetUdpSvrModuleList()[nGroupIndex]->DispatchUdpPacket(WorkerThread, Packet);
    }
}

//-----------------------------------------------------------------------------
// ����: TCP���ӷ���
//-----------------------------------------------------------------------------
void CAppDispatcher::DispatchTcpConnection(CTcpWorkerThread& WorkerThread, 
    int nGroupIndex, CTcpConnection& Connection)
{
   /* if (nGroupIndex >= 0 && nGroupIndex < GetTcpSvrModuleList().size())
    {
        // �ַ���������ģ��
        GetTcpSvrModuleList()[nGroupIndex]->DispatchTcpConnection(WorkerThread, Connection);
    }*/
}

//-----------------------------------------------------------------------------
// ����: ���������߳�ִ�� (nHelperIndex: 0-based)
//-----------------------------------------------------------------------------
void CAppDispatcher::HelperThreadExecute(CHelperThread& HelperThread, int nHelperIndex)
{
    // ϵͳ�ػ������̵߳� nHelperIndex Ϊ 0
    if (nHelperIndex == 0)
    {
        DaemonHelperExecute(HelperThread);
    }
    else
    {
        int nIndex1, nIndex2 = 0;

        // ��ȥϵͳռ�õ��߳���
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
// ����: �����еķ�����ģ��㲥��Ϣ
//-----------------------------------------------------------------------------
void CAppDispatcher::DispatchMessage(CBaseMessage& Message)
{
    for (int i = 0; i < GetServerModuleList().size(); i++)
        GetServerModuleList()[i]->DispatchMessage(Message);
}

///////////////////////////////////////////////////////////////////////////////
// class CAppGlobalData

//-----------------------------------------------------------------------------
// ����: ��ʼ��ȫ������ (�� CAppDispatcher::Initialize() ����)
// ��ע: ����ʼ��ʧ�ܣ����׳��쳣��
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
// ����: ����UDP���ݰ�
// ����:
//   Packet     - ���������ݰ� (��Pack)
//   nSendTimes - ���ʹ��� (ȱʡ1��)
//-----------------------------------------------------------------------------
void CServerModule::SendUdpPacket(CUdpBizPacket& Packet, const CInetAddress& ToAddr, 
    int nToPort, int nSendTimes)
{
    Packet.EnsurePacked();
    m_pDispatcher->GetUtils().SendUdpPacket(Packet.GetBuffer(), Packet.GetSize(), 
        ToAddr, nToPort, nSendTimes);
}

//-----------------------------------------------------------------------------
// ����: ����UDPͨ��Ӧ���
// ����:
//   ToAddr       - �Է���ַ
//   nToPort      - �Է��˿�
//   nSeqNumber   - �Է����������SeqNumber
//   nResultCode  - �������
//   nSendTimes   - ���ʹ��� (ȱʡ1��)
//-----------------------------------------------------------------------------
void CServerModule::SendUdpRePacket(const CInetAddress& ToAddr, int nToPort, 
    int nSeqNumber, int nResultCode, int nSendTimes)
{
    CRePacket RePacket;
    
    // ׼��Ӧ���
    RePacket.InitHeader(SC_ACK, nSeqNumber);
    RePacket.InitData(nResultCode);
    // ����Ӧ���
    SendUdpPacket(RePacket, ToAddr, nToPort, nSendTimes);
}

///////////////////////////////////////////////////////////////////////////////
