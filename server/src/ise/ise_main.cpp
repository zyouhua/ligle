///////////////////////////////////////////////////////////////////////////////
// Iris Server Engine (ISE)
//
// �ļ�����: ise_main.cpp
// ��������: ������
// ����޸�: 2005-04-19
///////////////////////////////////////////////////////////////////////////////

#include "ise_main.h"
#include "ise_system.h"

///////////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[])
{
    try
    {
        if (Application.ParseArguments(argc, argv))
        {
            struct CAppFinalizer {
                ~CAppFinalizer() { Application.Finalize(); }
            } AppFinalizer;

            Application.Initialize(); 
            Application.Run();
        }
    }
    catch (CException& e)
    {
        cout << "Error: " << e.ErrorMsg() << endl << endl;
        Logger.Write(e);
    }

    return 0;
}

///////////////////////////////////////////////////////////////////////////////
