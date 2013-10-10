//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "AutoUpdate_Main.h"
#include "UpdateFile.h"
#include <IniFiles.hpp>
#include <tlhelp32.h>
#include <Registry.hpp>

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;
const AnsiString UpdateUrl="http://lf/sjw/wdg/Update.ini";
//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
  : TForm(Owner)
{
//  DownLoadDir=GetCurrentDir();
  TRegistry *reg=new TRegistry(KEY_READ);
  try
  {
    reg->RootKey=HKEY_CURRENT_USER;
    reg->OpenKey("Software\\Ligle",false);
    DownLoadDir=reg->ReadString("Ligle");
  }
  catch(...)
  {
    DownLoadDir="C:\\";
  }
  delete reg;
}
//---------------------------------------------------------------------------


bool TForm1::Init()
{
  //TODO: Add your source code here
  bool bResult=false,bDown=false;
  int nTimes=3;
  TFileStream *File=NULL;
  while((nTimes>0) && !bDown)
  {
    try{
      File=new TFileStream(DownLoadDir+"Update.ini",fmOpenRead|fmCreate);
      IdHTTP1->Get(UpdateUrl,File);
      bDown=true;
    }
    catch(...)
    {
      IdHTTP1->Disconnect();
      delete File;
      --nTimes;
    }
  }
  if(!bDown)
    Application->Terminate();
  delete File;
  if(IsNeedUpdate())  //网站有最新版本
    bResult=true;
  return bResult;
}

void TForm1::GetFileList()
{
  GetUpdateFileList();
}

void TForm1::GetFile()
{
  m_UpdateFileList.GetAllFile();
}

bool TForm1::Complete()
{
  HANDLE  hProcessSnap=NULL;
  PROCESSENTRY32 p32={0};

  hProcessSnap=CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
  if(hProcessSnap==INVALID_HANDLE_VALUE)
    return false;
  p32.dwSize=sizeof(PROCESSENTRY32);

  bool CanUpdate=true;

  if(Process32First(hProcessSnap,&p32))
  {
    do
    {
      if(strcmp(p32.szExeFile,"Ligle.exe")==0)
      {
        CanUpdate=false;
        break;
      }
    }while(Process32Next(hProcessSnap, &p32));
  }
  CloseHandle(hProcessSnap);
  return CanUpdate;
}

void TForm1::UpdateFile()
{
  m_UpdateFileList.UpdateAllFile();
  TIniFile *OldFile=new TIniFile(DownLoadDir+"config.ini");
  TIniFile *NewFile=new TIniFile(DownLoadDir+"Update.ini");
  OldFile->WriteInteger("infomation","version",NewFile->ReadInteger("Infomation","version",1));
  delete OldFile;
  delete NewFile;
  DeleteFile(DownLoadDir+"Update.ini");
  this->Close();
}

void TForm1::GetUpdateFileList()
{
  TIniFile *File=new TIniFile(DownLoadDir+"Update.ini");
  AnsiString Temp=File->ReadString("Infomation","FileList","");
  AnsiString Line="",FileName="",InstallDir="",Url="";
  bool Complete=false;
  while(Temp.Length()>0)
  {
    int nPos=Temp.Pos(",");
    if(0==nPos)
      Line=Temp;
    else
      Line=Temp.SubString(0,nPos-1);
    Temp=Temp.Delete(1,Line.Length()+1);
    FileName=File->ReadString(Line,"Name","");
    InstallDir=File->ReadString(Line,"Install","");
    Url=File->ReadString(Line,"Url","");
    Complete=File->ReadBool(Line,"Complete",false);
    m_UpdateFileList.AddFile(FileName,InstallDir,Url,Complete,this);
  }
  delete File;
}

bool TForm1::IsNeedUpdate()
{
  TIniFile *OldFile=new TIniFile(DownLoadDir+"config.ini");
  TIniFile *NewFile=new TIniFile(DownLoadDir+"Update.ini");
  int CurVersion=OldFile->ReadInteger("infomation","version",0);
  int UpdateVersion=NewFile->ReadInteger("Infomation","version",0);
  delete OldFile;
  delete NewFile;
  if(CurVersion<UpdateVersion)
    return true;
  return false;
}

bool TForm1::Check()
{
  bool bResult=false;
  if(FileExists(DownLoadDir+"Update.ini"))
    bResult=true;
  return bResult;
}
void __fastcall TForm1::FormCreate(TObject *Sender)
{
  if(Check())   //升级计划在,
  {
    GetFileList();
     if(!Init()) // 网上无最新版本
     {
        GetFile();
        if(!Complete())
        {
          m_UpdateFileList.Clear();
          Application->Terminate();
        }
        else
        {
          Application->ShowMainForm=true;
          return;
        }
     }
     m_UpdateFileList.Clear();
     GetFileList();
     GetFile();
     if(!Complete())
     {
        m_UpdateFileList.Clear();
        Application->Terminate();
     }
     Application->ShowMainForm=true;
  }
  else    //升级计划不在
  {
    if(!Init())
    {
      DeleteFile(DownLoadDir+"Update.ini");
      Application->Terminate();
    }
    GetFileList();
    GetFile();
    if(!Complete())
      Application->Terminate();
    else
      Application->ShowMainForm=true;
  }  
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Button1Click(TObject *Sender)
{
  UpdateFile();    
}
//---------------------------------------------------------------------------
