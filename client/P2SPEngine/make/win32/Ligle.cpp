//---------------------------------------------------------------------------

#include <vcl.h>
#include <Clipbrd.hpp>


#include "core_engine.h"
#include "core_task.h"

#include "MainForm.h"
#include "NewTask.h"

#pragma hdrstop
//---------------------------------------------------------------------------
USEFORM("..\..\src\GUI\MainForm.cpp", MainFrm);
USEFORM("..\..\src\GUI\NewTask.cpp", NewTaskDlg);
USEFORM("..\..\src\GUI\ScanDirDlg.cpp", ScanDir);
USEFORM("..\..\src\GUI\Alert.cpp", AlertDlg);
USEFORM("..\..\src\GUI\Repeat.cpp", OKBottomDlg);
USEFORM("..\..\src\GUI\ReDownDlg.cpp", ReDownDlg);
//---------------------------------------------------------------------------
HANDLE hMutex;

bool CheckPreInstance(char* cCmd);
void SendMessageDownLoad(HANDLE,char*);

WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR cmdline, int)
{
  try
  {
     if(CheckPreInstance(cmdline))
      return 0;
      
     Engine.Initialize();

     Application->Initialize();
     Application->CreateForm(__classid(TMainFrm), &MainFrm);
     Application->CreateForm(__classid(TScanDir), &ScanDir);
     Application->CreateForm(__classid(TAlertDlg), &AlertDlg);
     Application->CreateForm(__classid(TOKBottomDlg), &OKBottomDlg);
     Application->CreateForm(__classid(TReDownDlg), &ReDownDlg);
     string str=cmdline;
     string Ref,Url;
     int n;
     n=str.find(" ",0);
     if(n>0)
     {
      Url=str.substr(0,n);
      Ref=str.substr(n+1,str.length()-n+1);
      TNewTaskDlg *NewTaskDlg=new TNewTaskDlg(MainFrm);
      NewTaskDlg->m_Ref->Text=Ref.c_str();
      NewTaskDlg->m_Url->Text=Url.c_str();
      NewTaskDlg->ShowModal();
      delete NewTaskDlg;
     }
//==============================================================================
     Application->Run();

     Engine.Finalize();
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
bool  CheckPreInstance(char* cCmd)
{
  bool bResult=false;

  HWND Handle,Ret;
  hMutex=CreateMutex(NULL,false,APP_NAME);
  if(ERROR_ALREADY_EXISTS==GetLastError())
  {
    bResult=true;
    Handle=GetDesktopWindow();
    Handle=GetWindow(Handle,GW_CHILD);
    while(IsWindow(Handle))
    {
      Ret=GetProp(Handle,APP_NAME);
      if(Ret==Handle)
      {
        SetForegroundWindow(Handle);
        SendMessageDownLoad(Handle,cCmd);
        break;
      }
      Handle=GetWindow(Handle,GW_HWNDNEXT);
    }
  }
  return bResult;
}
void SendMessageDownLoad(HANDLE handle,char* cCmd)
{
  string CmdLine="";
  CmdLine=cCmd;
  if(!(CmdLine.length()>0))
    return;
  TClipboard* clip=Clipboard();
  clip->AsText="";
  clip->AsText=CmdLine.c_str();
  SendMessage(handle,WM_NEWTASK,0,0);
}
