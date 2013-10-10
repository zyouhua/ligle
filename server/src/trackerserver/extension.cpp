///////////////////////////////////////////////////////////////////////////////
// P2SP Tracker Server
//
// �ļ�����: extension.cpp
// ��������: ISEӦ����չ�ӿ�ʵ��
// ����޸�: 2005-04-27
///////////////////////////////////////////////////////////////////////////////

#include "extension.h"
#include "global.h"
#include "dispatcher.h" 
#include <time.h>

///////////////////////////////////////////////////////////////////////////////
// ȫ�ֱ���

CAppExtension AppExtension;

///////////////////////////////////////////////////////////////////////////////
// class CAppExtension

//-----------------------------------------------------------------------------
// ����: ���������в�������������ȷ�򷵻� false
//-----------------------------------------------------------------------------
bool CAppExtension::ParseArguments(int nArgc, char *sArgv[])
{
    return true;
}

//-----------------------------------------------------------------------------
// ����: ��������ʱ��ʾ��ӭ��Ϣ
//-----------------------------------------------------------------------------
void CAppExtension::ShowWelcomeMessage()
{
    cout << "******************************************" << endl;
    cout << "*  Tracker Server (1.0 alpha1)           *" << endl;
    cout << "*  (c)2005 All rights reserved.          *" << endl;
    cout << "*  Usage:                                *" << endl;
    cout << "*  Start server:  # ./trackerd           *" << endl;
    cout << "*  Stop server:   # killall trackerd     *" << endl;
    cout << "******************************************" << endl;
    cout << endl;
}

//-----------------------------------------------------------------------------
// ����: ����ɹ���������ʾ��ʾ��Ϣ
//-----------------------------------------------------------------------------
void CAppExtension::ShowStartedMessage()
{
    string msg = "Tracker Server started.";

    cout << endl << msg << endl;
    Logger.Write(msg.c_str());

    // ��ʼ�����������
    Randomize();

    cout << "Load Data From Database...";
    {
       // CDbOprs_NetMgr db;
//       // db.SelfTest();
       // db.LoadIpRangeFromDb();
       // db.LoadResourceFromDB();
    }

    cout << "Complete!" << endl;
}

//-----------------------------------------------------------------------------
// ����: ��������ʧ�ܺ���ʾ��ʾ��Ϣ
//-----------------------------------------------------------------------------
void CAppExtension::ShowStartFailMessage()
{
    string msg = "Fail to start Tracker Server.";

    cout << endl << msg << endl;
    Logger.Write(msg.c_str());
}

//-----------------------------------------------------------------------------
// ����: ��ʼ��ϵͳ������Ϣ
//-----------------------------------------------------------------------------
void CAppExtension::InitSystemOptions(CSystemOptions& SysOpt)
{
    SysOpt.strLogFileName = Application.GetExePath() + SERVER_LOG_FILE;
    SysOpt.bIsDaemon = true;
    SysOpt.bAllowMultiInstance = false;
}

//-----------------------------------------------------------------------------
// ����: ��ʼ��������������Ϣ
//-----------------------------------------------------------------------------
void CAppExtension::InitServerOptions(CServerOptions& SvrOpt)
{
    int i;

    // ���÷���������
    SvrOpt.SetServerType(ST_UDP | ST_TCP);


    // ����UDP����˿�
    SvrOpt.SetUdpServerPort(UDP_SERVER_PORT);

    // ����UDP�����߳�����
    SvrOpt.SetUdpListenerThreadCount(UDP_LISTENER_THREAD_COUNT);

    // ����UDP�����������
    SvrOpt.SetUdpRequestGroupCount(UDP_SVR_MODULE_COUNT);

    // ����UDP���ݰ����е�������� (�������ɶ��ٸ����ݰ�)
    for (i = 0; i < UDP_SVR_MODULE_COUNT; i++)
        SvrOpt.SetUdpRequestQueueCapacity(i, UDP_QUEUE_CAPACITIES[i]);

    // ����UDP�������̸߳�����������
    for (i = 0; i < UDP_SVR_MODULE_COUNT; i++)
        SvrOpt.SetUdpWorkerThreadCount(i, UDP_WORK_THREADS[i].nMin, UDP_WORK_THREADS[i].nMax);


    // ����TCP�����������
    SvrOpt.SetTcpRequestGroupCount(TCP_SVR_MODULE_COUNT);

    // ����TCP����˿�
    for (i = 0; i < TCP_SVR_MODULE_COUNT; i++)
        SvrOpt.SetTcpServerPort(i, TCP_SERVER_PORTS[i]);

    // ����TCP���Ӷ��е�����(�������ɶ��ٸ�����)
    for (i = 0; i < TCP_SVR_MODULE_COUNT; i++)
        SvrOpt.SetTcpRequestQueueCapacity(i, TCP_QUEUE_CAPACITIES[i]);

    // ����TCP�������̸߳�����������
    for (i = 0; i < TCP_SVR_MODULE_COUNT; i++)
        SvrOpt.SetTcpWorkerThreadCount(i, TCP_WORK_THREADS[i].nMin, TCP_WORK_THREADS[i].nMax);


    // ���������߳������� CAppDispatcher::InitServerModuleList() ������������
    SvrOpt.SetHelperThreadCount(0);
}

//-----------------------------------------------------------------------------
// ����: �����������ݷ���������
//-----------------------------------------------------------------------------
CCustomDispatcher* CAppExtension::CreateDispatcher()
{
    return new CAppDispatcher;
}

//-----------------------------------------------------------------------------
// ����: ��չ���ܵĳ�ʼ��������ʼ��ʧ�����׳��쳣 (�˵����� ISE ��ʼ��֮ǰ)
//-----------------------------------------------------------------------------
void CAppExtension::ProgramInit()
{
    // ���õ�ǰʹ�õ��ַ������Ա��������ȷ�����ַ�ת�����й��������ַ�����:
    //   zh_CN            zh_TW
    //   zh_CN.gb18030    zh_TW.big5
    //   zh_CN.gb2312     zh_TW.euctw
    //   zh_CN.gbk        zh_TW.utf8
    //   zh_CN.utf8
    setlocale(LC_CTYPE, "zh_CN.gbk");  // <- ������й���½
}

//-----------------------------------------------------------------------------
// ����: �����˳�ǰ���ƺ��� (����������ѹر�֮�󣬳������ݷ������ͷ�֮ǰ)
//-----------------------------------------------------------------------------
void CAppExtension::ProgramExit()
{
    string msg = "Tracker Server stoped.";

    cout << msg << endl;
    Logger.Write(msg.c_str());
}

///////////////////////////////////////////////////////////////////////////////
