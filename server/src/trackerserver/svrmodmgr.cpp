///////////////////////////////////////////////////////////////////////////////
// Msger Main Server
//
// �ļ�����: svrmodmgr.cpp
// ��������: ������ģ�����
// ����޸�: 2005-07-13
///////////////////////////////////////////////////////////////////////////////

#include "svrmodmgr.h"
// ������ģ��:
#include "finder.h"
// TODO: ��������ģ����ڴ˰�����

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
// ����: ��ʼ�� (ʧ�����׳��쳣)
//-----------------------------------------------------------------------------
void CServerModuleMgr::Initialize()
{
    Finalize();

     // ����������ģ�� (��������ģ��ķ�ѡӦע�����±��)
       m_ServerModuleList.push_back((CServerModule*)new CFinder());        // [0]: UDP
    // m_ServerModuleList.push_back((CServerModule*)new CServerModule_NetMgr());       // [1]: TCP
   // m_ServerModuleList.push_back((CServerModule*)new CServerModule_Transfer());     // [2]: UDP

    // ��ѡ�� UDP ������ģ�飬�����η��� m_UdpSvrModuleList ��:
      m_UdpSvrModuleList.resize(UDP_SVR_MODULE_COUNT);
      m_UdpSvrModuleList[UDP_GI_FINDER] = m_ServerModuleList[0];
   // m_UdpSvrModuleList[UDP_GI_TRANSFER] = m_ServerModuleList[2];

    // ��ѡ�� TCP ������ģ�飬�����η��� m_TcpSvrModuleList ��:
    //m_TcpSvrModuleList.resize(TCP_SVR_MODULE_COUNT);
    //m_TcpSvrModuleList[TCP_GI_NETMGR] = m_ServerModuleList[1];
}

//-----------------------------------------------------------------------------
// ����: ������ (���۳�ʼ���Ƿ����쳣������ʱ����ִ��)
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
