///////////////////////////////////////////////////////////////////////////////
// Iris Server Engine (ISE)
// ise_system.h
///////////////////////////////////////////////////////////////////////////////

#ifndef _ISE_SYSTEM_H_
#define _ISE_SYSTEM_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <signal.h>
#include <ctype.h>
#include <setjmp.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <iostream>
#include <string>

#include "ise_classes.h"
#include "ise_server.h"
#include "ise_sysutils.h"

using namespace std;
using namespace ise;

namespace ise
{

///////////////////////////////////////////////////////////////////////////////
// ȫ�ֱ�������

class CApplication;
class CCustomExtension;

// ��־����
extern CLogger Logger;
// Ӧ�ó������
extern CApplication Application;
// Ӧ�ó�������չ����ָ�� (ָ���û������Ķ���)
extern CCustomExtension *pExtension;

///////////////////////////////////////////////////////////////////////////////
// class CSystemOptions - ϵͳ������

class CSystemOptions
{
public:
    string strLogFileName;      // ��־�ļ��� (��·��)
    bool bIsDaemon;             // �Ƿ��̨�ػ�����(daemon)
    bool bAllowMultiInstance;   // �Ƿ�����������ʵ��ͬʱ����

public:
    CSystemOptions() {}
    void InitDefaultOptions();
};

///////////////////////////////////////////////////////////////////////////////
// class CApplication - Ӧ�ó�����
//
// ˵��:
// 1. ����������������������ܣ�ȫ�ֵ�������(Application)�ڳ�������ʱ����������
// 2. ������ main() �������� CAppController �����ִ�У�
// 3. һ����˵��������������ⲿ��������(kill)���˳��ġ���ISE�У������յ�kill�˳������
//    (��ʱ��ǰִ�е�һ���� Application.Run() ��)���ᴥ�� ExitProgramSignalHandler 
//    �źŴ��������������� longjmp ����ʹִ�е�ģ��� Run() ���˳����̶�ִ�� Finalize��
// 4. �������������ķǷ��������󣬻��ȴ��� FatalErrorSignalHandler �źŴ�������
//    Ȼ��ͬ����������������˳���˳���
// 5. �������ڲ��������˳������Ƽ�ʹ��exit���������� Application.SetTeraminted(true).
//    ���������ó���������������˳���˳���

class CApplication
{
private:
    CSystemOptions m_SystemOptions;         // ϵͳ����
    CServerOptions m_ServerOptions;         // ���������� 
    CSeqAllocator m_SeqAllocator;           // ���ݰ�˳��ŷ�����
    CMultiInstChecker m_MultiInstChecker;   // ������ʵ���ظ����м����
    CMainServer *m_pMainServer;             // ��������
    StringArray m_ArgList;                  // �����в���
    string m_strExeName;                    // ��ִ���ļ���ȫ��(������·��)
    bool m_bInitialized;                    // �Ƿ�ɹ���ʼ��
    bool m_bTerminated;                     // �Ƿ�Ӧ�˳��ı�־

private:
    void CheckAppExtensionExists();
    void CheckMultiInstance();
    void ApplySystemOptions();
    void CreateMainServer();
    void FreeMainServer();
    void InitExeName();
    void InitDaemon();
    void InitSignals();
    void CloseTerminal();
    void DoFinalize();

public:
    CApplication();
    ~CApplication();

    bool ParseArguments(int nArgc, char *sArgv[]);
    void Initialize();
    void Finalize();
    void Run();

    inline CSystemOptions& GetSystemOptions() { return m_SystemOptions; }
    inline CServerOptions& GetServerOptions() { return m_ServerOptions; }
    inline CSeqAllocator& GetSeqAllocator() { return m_SeqAllocator; }
    inline CMainServer& GetMainServer() { return *m_pMainServer; }

    inline void SetTerminated(bool bValue) { m_bTerminated = bValue; }
    inline bool GetTerminated() { return m_bTerminated; }

    // ȡ�ÿ�ִ���ļ���ȫ��(������·��)
    string GetExeName() { return m_strExeName; }
    // ȡ�ÿ�ִ���ļ����ڵ�·��
    string GetExePath();
    // ȡ�������в�������
    int GetArgCount() { return m_ArgList.size(); }
    // ȡ�������в����ַ��� (nIndex: 0-based)
    string GetArgString(int nIndex);
};

///////////////////////////////////////////////////////////////////////////////
// class CCustomExtension - Ӧ�ó���ҵ����չ����
//
// ˵��:
// 1. ����������ISE�Ļ�������չʵ��ҵ���ܣ�
// 2. ISE��ʹ���߱���̳д��࣬����ȫ�ַ�Χ�ڴ����˶���(�� main ��������֮ǰ����)��

class CCustomExtension
{
public:
    CCustomExtension() { pExtension = this; }
    virtual ~CCustomExtension() { pExtension = NULL; }

    // ���������в�������������ȷ�򷵻� false
    virtual bool ParseArguments(int nArgc, char *sArgv[]) { return true; }

    // ��������ʱ��ʾ��ӭ��Ϣ
    virtual void ShowWelcomeMessage() {}
    // ����ɹ���������ʾ��ʾ��Ϣ
    virtual void ShowStartedMessage() {}
    // ��������ʧ�ܺ���ʾ��ʾ��Ϣ
    virtual void ShowStartFailMessage() {}

    // ��ʼ��ϵͳ������Ϣ
    virtual void InitSystemOptions(CSystemOptions& SysOpt) = 0;
    // ��ʼ��������������Ϣ
    virtual void InitServerOptions(CServerOptions& SvrOpt) = 0;
    // �����������ݷ��������� (�û��ɶ����Լ��ĳ������ݷ��������Ա㴦��ҵ���߼�)
    virtual CCustomDispatcher* CreateDispatcher() { return new CCustomDispatcher; }

    // ��չ���ܵĳ�ʼ��������ʼ��ʧ�����׳��쳣 (�˵����� ISE ��ʼ��֮ǰ)
    virtual void ProgramInit() {}
    // �����˳�ǰ���ƺ��� (����������ѹر�֮�󣬳������ݷ������ͷ�֮ǰ)
    virtual void ProgramExit() {}
};

///////////////////////////////////////////////////////////////////////////////

} // namespace ise

#endif // _ISE_SYSTEM_H_ 
