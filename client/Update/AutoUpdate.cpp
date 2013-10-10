//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
//---------------------------------------------------------------------------
USEFORM("AutoUpdate_Main.cpp", Form1);
//---------------------------------------------------------------------------
HANDLE hMutex;

WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
  hMutex=CreateMutex(NULL,false,"AutoUpdate");
  if(ERROR_ALREADY_EXISTS==GetLastError())
    return 0;
//使程序在任务栏隐藏
  DWORD dwStyle=GetWindowLong(Application->Handle,GWL_EXSTYLE);
  dwStyle|=WS_EX_TOOLWINDOW;
//  dwStyle|=WS_EX_LAYERED;
  SetWindowLong(Application->Handle,GWL_EXSTYLE,dwStyle);
//  SetLayeredWindowAttributes(Application->Handle,NULL,0,LWA_ALPHA);
  try
  {
     Application->Initialize();
     Application->ShowMainForm=false;
     Application->CreateForm(__classid(TForm1), &Form1);
     Application->Run();
  }
  catch (Exception &exception)
  {
     Application->ShowException(&exception);
  }
  catch (...)
  {
     try
     {
       throw Exception("");
     }
     catch (Exception &exception)
     {
       Application->ShowException(&exception);
     }
  }
  return 0;
}
//---------------------------------------------------------------------------
