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
// ��ǰ����

class CServerModule;

///////////////////////////////////////////////////////////////////////////////
// ���Ͷ���

typedef vector<CServerModule*> ServerModuleList;

///////////////////////////////////////////////////////////////////////////////
// ��������

//-----------------------------------------------------------------------------
//-- UDP������ģ�������(GroupIndex)����: (��������, ��Ч���Ŵ�0����)

const int UDP_GI_NONE           = -1;       // (none)
const int UDP_GI_FINDER         = 0;        // finder module
const int UDP_GI_TRANSFER       = 1;        // transfer module


const int UDP_SVR_MODULE_COUNT  = 1;        // UDP������ģ������

//-----------------------------------------------------------------------------
//-- TCP������ģ�������(GroupIndex)����: (��������, ��Ч���Ŵ�0����)

const int TCP_GI_NONE           = -1;       // (none)
const int TCP_GI_NETMGR         = 0;        // tracker�������������ӿ�

const int TCP_SVR_MODULE_COUNT  = 1;        // TCP������ģ������

//-----------------------------------------------------------------------------
//-- UDP����������:

// UDP����˿ں�
const int UDP_SERVER_PORT = 9100;

// UDP�����̵߳�����
const int UDP_LISTENER_THREAD_COUNT = 2;

// UDP������е��������
const int UDP_QUEUE_CAPACITIES [UDP_SVR_MODULE_COUNT] = {
    100000,         // finder module
//    1000            // transfer module
};

// UDP�������̸߳�����������
const struct { int nMin, nMax; } UDP_WORK_THREADS [UDP_SVR_MODULE_COUNT] = {
    {10, 100},      // finder module         (10, 100)
//    {10, 20}        // transfer module
};

//-----------------------------------------------------------------------------
//-- TCP����������:

// TCP����˿ں�
const int TCP_SERVER_PORTS [TCP_SVR_MODULE_COUNT] = {
    9100
};

// TCP������е�����
const int TCP_QUEUE_CAPACITIES [TCP_SVR_MODULE_COUNT] = {
    0           // netmgr module
};

// TCP�������̸߳�����������
const struct { int nMin, nMax; } TCP_WORK_THREADS [TCP_SVR_MODULE_COUNT] = {
    {2, 2}       // netmgr module
};

///////////////////////////////////////////////////////////////////////////////
// class CServerModuleMgr

class CServerModuleMgr
{
private:
    ServerModuleList m_ServerModuleList;        // ������ģ���б�
    ServerModuleList m_UdpSvrModuleList;        // UDP������ģ���б�(���� m_ServerModuleList �е�ģ��)
   // ServerModuleList m_TcpSvrModuleList;        // TCP������ģ���б�(���� m_ServerModuleList �е�ģ��)
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
