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
// ��ǰ����

class CAppDispatcher;
class CAppGlobalData;
class CServerModule;

///////////////////////////////////////////////////////////////////////////////
// ���Ͷ���

// ������������
typedef vector<uint> ActionCodeArray;
// <��������, UDP����ģ������> ӳ���
typedef map<uint, int> ActionCodeMap;

///////////////////////////////////////////////////////////////////////////////
// class CAppDispatcher - �������ݷ�������

class CAppDispatcher : public CCustomDispatcher
{
private:
    CAppGlobalData *m_pGlobalData;              // ȫ������
    CServerModuleMgr *m_pServerModuleMgr;       // ������ģ�������
    ActionCodeMap m_ActionCodeMap;              // <��������, UDP����ģ������> ӳ���

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

    // ��ʼ�� (ʧ�����׳��쳣)
    virtual void Initialize();
    // ������ (���۳�ʼ���Ƿ����쳣������ʱ����ִ��)
    virtual void Finalize();

    // UDP���ݰ�����
    virtual void ClassifyUdpPacket(void *pPacketBuffer, int nPacketSize, int& nGroupIndex);
    // UDP���ݰ�����
    virtual void DispatchUdpPacket(CUdpWorkerThread& WorkerThread, int nGroupIndex, CUdpPacket& Packet);
    // TCP���ӷ���
    virtual void DispatchTcpConnection(CTcpWorkerThread& WorkerThread, int nGroupIndex, CTcpConnection& Connection);

    // ���������߳�ִ�� (nHelperIndex: 0-based)
    virtual void HelperThreadExecute(CHelperThread& HelperThread, int nHelperIndex);

    // ������Ϣ�����еķ�����ģ��
    void DispatchMessage(CBaseMessage& Message);

    // ����ȫ�����ݶ���
    CAppGlobalData& GetGlobalData() { return *m_pGlobalData; }
};

///////////////////////////////////////////////////////////////////////////////
// class CAppGlobalData - ȫ��������
//
// ˵��:
// 1. ���ฺ�����ȫ�����ݡ�
// 2. ȫ�����ݵĴ�������������: (1)��������; (2)���ݳ�ʼ����
//    ֻ����Ҫ��ô��������ΪĳЩȫ�ֶ���ĳ�ʼ������ CAppDispatcher::m_pGlobalData��
//    ��������ȫ�ֶ���Ĺ��캯���У�CAppGlobalData������δ�������(m_pGlobalDataΪNULL)��
//    ���ԣ�ȫ�ֶ���Ĵ����ͳ�ʼ��Ӧ�÷ֿ����С�

class CAppGlobalData
{
private:
    CConfigManager m_ConfigManager;             // ���ù����� 
    CDatabaseManager m_DbManager;               // ���ݿ������

public:
    // ��ʼ��ȫ������ (�� CAppDispatcher::Initialize() ����)
    void Initialize();

    // �������ù�����
    inline CConfigManager& GetConfigManager() { return m_ConfigManager; }
    // �������ݿ������
    inline CDatabaseManager& GetDbManager() { return m_DbManager; }
};

///////////////////////////////////////////////////////////////////////////////
// class CServerModule - ������ģ�����

class CServerModule
{
private:
    CAppDispatcher *m_pDispatcher;    // �������ݷ����� (����)

protected:
    // ����UDP���ݰ�
    void SendUdpPacket(CUdpBizPacket& Packet, const CInetAddress& ToAddr, 
        int nToPort, int nSendTimes = 1);
    // ����UDPͨ��Ӧ���
    void SendUdpRePacket(const CInetAddress& ToAddr, int nToPort, 
        int nSeqNumber, int nResultCode = RET_SUCCESS, int nSendTimes = 1);

    // ���س��ö���
    inline CAppGlobalData& AppGlobalData() { return m_pDispatcher->GetGlobalData(); }
    inline CConfigManager& ConfigManager() { return AppGlobalData().GetConfigManager(); }
public:
    CServerModule();
    virtual ~CServerModule() {}

    // ȡ�ø�ģ���Ͻ��UDP���ݰ���������
    virtual void GetUdpActionCodes(ActionCodeArray& List) {}

    // UDP���ݰ�����
    virtual void DispatchUdpPacket(CUdpWorkerThread& WorkerThread, CUdpPacket& Packet) {}
    // TCP���ӷ���
    virtual void DispatchTcpConnection(CTcpWorkerThread& WorkerThread, CTcpConnection& Connection) {}
    // ��Ϣ����
    virtual void DispatchMessage(CBaseMessage& Message) {}

    // ���ش�ģ�����踨�������̵߳�����
    virtual int GetHelperThreadCount() { return 0; }
    // ���������߳�ִ��(nHelperIndex: 0-based)
    virtual void HelperThreadExecute(CHelperThread& HelperThread, int nHelperIndex) {}
    // ϵͳ�ػ������߳�ִ�� (nSecondCount: 0-based)
    virtual void DaemonHelperExecute(CHelperThread& HelperThread, int nSecondCount) {}
};

///////////////////////////////////////////////////////////////////////////////

#endif // _DISPATCHER_H_ 
