//---------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "NewTask.h"
#include "ScanDirDlg.h"
#include "MainForm.h"
#include <IniFiles.hpp>
#include <Registry.hpp>

#include "core_task.h"
#include "core_metafile.h"
//--------------------------------------------------------------------- 
#pragma link "RzBmpBtn"
#pragma link "RzBmpBtn"
#pragma resource "*.dfm"
//TNewTaskDlg *NewTaskDlg;
//---------------------------------------------------------------------
__fastcall TNewTaskDlg::TNewTaskDlg(TComponent* AOwner)
	: TForm(AOwner)
{
  strFileName="C:\\";
}
//---------------------------------------------------------------------
void __fastcall TNewTaskDlg::OKBtnClick(TObject *Sender)
{
  string Url,Ref,FileName;
  Url=m_Url->Text.c_str();
  Ref=m_Ref->Text.c_str();
  m_FileName->Text=ValidateFileName(m_FileName->Text);   //不合法字符
  m_FileName->Text=AutoRenameExtName(m_FileName->Text,m_Url->Text); //文件后缀名
  FileName=(strFileName+m_FileName->Text).c_str();
  if(MetaFile.GetFile(const_cast<char*>(Url.c_str())))
  {
    PostMessage(MainFrm->Handle,WM_MULITPTASK,0,(LPARAM)Url.c_str());
    return;
  }
  TaskPool.AddTask(Url,Ref,FileName);
  m_Url->Text="";
  m_Ref->Text="";
  
}
//---------------------------------------------------------------------------

void __fastcall TNewTaskDlg::CancelBtnClick(TObject *Sender)
{
  m_Url->Text="";
  m_Ref->Text="";
  m_FileName->Text="";  
}
//---------------------------------------------------------------------------

void __fastcall TNewTaskDlg::m_UrlChange(TObject *Sender)
{
  string str=m_Url->Text.c_str();
  int nPos=str.find_last_of('/',str.length());
  int nPos2=str.find("?",nPos);
  if(nPos+1==str.length())
    m_FileName->Text="index.htm";
  else
  {
    if(nPos2>nPos)
      m_FileName->Text="index.htm";
    else
    {
      m_FileName->Text=str.substr(nPos+1,str.length()-nPos).c_str();
      m_FileName->Text=URLDecode(m_FileName->Text);
    }
  }
}
//---------------------------------------------------------------------------

void __fastcall TNewTaskDlg::ScanClick(TObject *Sender)
{
  if(!ScanDir->ShowModal())
    return;
  m_Path->Text=ScanDir->DirTree->SelectedPathName;
  strFileName=ScanDir->DirTree->SelectedPathName;
  AnsiString strTemp=strFileName.AnsiLastChar();
  if(strTemp!="\\")
    strFileName+="\\";
  AnsiString path="";
  TRegistry *reg=new TRegistry(KEY_READ);
  try
  {
    reg->RootKey=HKEY_CURRENT_USER;
    reg->OpenKey("Software\\Ligle",false);
    path=reg->ReadString("Ligle");
  }
  catch(...)
  {
    path="C:\\";
  }
  delete reg;
  TIniFile *file=new TIniFile(path+"\\Config.ini");
  AnsiString temp=file->ReadString("NewTaskDir","newtask","");
  if(m_Path->Items->Count>5)
  {
    AnsiString l=temp.SubString(0,temp.Pos("|")-1);
    temp.Delete(1,l.Length()+1);
  }
  temp=temp+"|"+ScanDir->DirTree->SelectedPathName;
  file->WriteString("NewTaskDir","newtask",temp);
  file->WriteString("TaskDefaultDir","DefaultFolder",ScanDir->DirTree->SelectedPathName);
  delete file;
}
//---------------------------------------------------------------------------

void __fastcall TNewTaskDlg::OKClick(TObject *Sender)
{
  m_Url->Text=m_Url->Text.Trim();
  m_Ref->Text=m_Ref->Text.Trim();
  m_FileName->Text=m_FileName->Text.LowerCase().Trim();

  AnsiString strTemp=m_Url->Text.SubString(0,m_Url->Text.Pos("//")+1).LowerCase();
  m_Url->Text=strTemp+m_Url->Text.SubString(m_Url->Text.Pos("//")+2,m_Url->Text.Length());
  
  AnsiString HTTP="http://",FTP="ftp://";
  if((strTemp.AnsiCompare(HTTP)==0 || strTemp.AnsiCompare(FTP)==0) && m_FileName->Text!="")
    OKBtn->Click();
  else
  {
    if(m_FileName->Text=="")
    {
      Label4->Caption="请输入文件名!";
      Label4->Visible=true;
      return;
    }
    else
    {
      Label4->Caption="输入的URL不合法，请重新输入!";
      Label4->Visible=true;
      return;
    }
  }
}
//---------------------------------------------------------------------------

/*
AnsiString TNewTaskDlg::AutoRenameFileName(AnsiString& strPath, AnsiString& strFileName)
{
  //TODO: Add your source code here
  int Number=1;
  AnsiString Name=strFileName;
  AnsiString Path=strPath;
  AnsiString strTemp=Path.AnsiLastChar();
  if(strTemp!="\\")
    Path+="\\";
  AnsiString ExtName=ExtractFileExt(Name);
  AnsiString MainName=Name.SubString(0,Name.Pos(ExtName)-1);
  AnsiString AddName="";
  while(FileExists(Path+Name))
  {
    AddName="";
    AddName="("+Sysutils::IntToStr(Number)+")";
    Name=MainName+AddName+ExtName;
    Number++;
  }
  return Path+Name;
}
*/
AnsiString TNewTaskDlg::ValidateFileName(AnsiString& FileName)
{
  //TODO: Add your source code here
  AnsiString strResult=FileName;
  for(int i=1;i<strResult.Length();i++)
  {
    if(strResult[i]=='?' || strResult[i]=='/' || strResult[i]=='<' || strResult[i]=='>' ||
       strResult[i]=='|' || strResult[i]=='*' || strResult[i]==':' || strResult[i]=='"' ||
       strResult[i]=='\\')
      strResult[i]='_';
  }
  return strResult;
}

AnsiString TNewTaskDlg::AutoRenameExtName(AnsiString& OldName, AnsiString& Url)
{
  //TODO: Add your source code here
  AnsiString strResult=OldName;
  string strFileName=Url.c_str();
  if(Url.Pos("?")<=0)
  {
    AnsiString strExt=ExtractFileExt(OldName);
    strExt.LowerCase();
    int nPos=strFileName.find_last_of('/',strFileName.length());
    strFileName=strFileName.substr(nPos+1,strFileName.length()-nPos);
    AnsiString strTemp=strFileName.c_str();
    AnsiString strExtOld=ExtractFileExt(strTemp);
    if(strExtOld.AnsiCompare(strExt)!=0)
      strResult=OldName.SubString(0,OldName.Pos(strExt)-1)+strExtOld;
  }
  else
    strResult="index.htm";
  return strResult;
}
void __fastcall TNewTaskDlg::FormCreate(TObject *Sender)
{
  AnsiString path="";
  TRegistry *reg=new TRegistry(KEY_READ);
  try
  {
    reg->RootKey=HKEY_CURRENT_USER;
    reg->OpenKey("Software\\Ligle",false);
    path=reg->ReadString("Ligle");
  }
  catch(...)
  {
    path="C:\\";
  }
  delete reg;
  TIniFile *file=new TIniFile(path+"\\Config.ini");
  m_Path->Text=file->ReadString("TaskDefaultDir","DefaultFolder","");
  strFileName=m_Path->Text;
  AnsiString temp=file->ReadString("NewTaskDir","newtask","");
  while(temp.Length()>0)
  {
    AnsiString l="";
    if(temp.Pos("|"))
    {
      l=temp.SubString(0,temp.Pos("|")-1);
      m_Path->Items->Add(l);
    }
    else
    {
      l=temp;
      m_Path->Items->Add(l);
    }
    temp.Delete(1,l.Length()+1);
  }
  delete file;
}
//---------------------------------------------------------------------------

void __fastcall TNewTaskDlg::m_PathSelect(TObject *Sender)
{
  strFileName=m_Path->Text;
}
//---------------------------------------------------------------------------

void __fastcall TNewTaskDlg::FormPaint(TObject *Sender)
{
  this->Color=RGB(240,241,243);   
}
//---------------------------------------------------------------------------

