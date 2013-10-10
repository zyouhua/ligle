//---------------------------------------------------------------------------
#pragma hdrstop

#include "UpdateFile.h"
//#include "AutoUpdate_Main.h"
#include <IniFiles.hpp>

CUpdateFilePool m_UpdateFileList;
AnsiString DownLoadDir="";

/////////////////////////////////////////////////////////////////////////////
//// Class CUpdateFile
CUpdateFile::CUpdateFile(AnsiString& FileName,AnsiString& InstallDir,AnsiString& Url,bool C,TForm* Form)
{
  m_FileName=FileName;
  m_InstallDir=InstallDir;
  m_Url=Url;
  m_Complete=C;
  m_IdHttp=new TIdHTTP(Form);
//  m_IdHttp->Port=6000;
}

CUpdateFile::~CUpdateFile()
{
  if(m_IdHttp)
  {
    delete m_IdHttp;
    m_IdHttp=NULL;
  }
  DeleteFile(m_InstallDir+m_FileName+ExName);
  m_UpdateFileList.RemoveFile(this);
}


void CUpdateFile::Get()
{
  TFileStream* File=NULL;

  File=new TFileStream(m_InstallDir+m_FileName+ExName,fmCreate|fmOpenWrite);
  m_IdHttp->Get(m_Url,File);
  TIniFile *file=new TIniFile(DownLoadDir+"Update.ini");
  file->WriteBool(m_FileName,"Complete",true);
  delete File;
}

void CUpdateFile::Update()   //¸Ä³ÉReNameFiles
{
  DeleteFile(m_InstallDir+m_FileName);
  RenameFile((m_InstallDir+m_FileName+ExName),m_InstallDir+m_FileName);
}

void CUpdateFile::Undo()
{
  DeleteFile(m_InstallDir+m_FileName);
}
/////////////////////////////////////////////////////////////////////////////
//// Class CUpdateFilePool
void CUpdateFilePool::AddFile(AnsiString& FileName,AnsiString& InstallDir,AnsiString& Url,bool C,TForm* Form)
{
  CUpdateFile* File=new CUpdateFile(FileName,InstallDir,Url,C,Form);
  m_FileList.push_back(File);
}

void CUpdateFilePool::RemoveFile(CUpdateFile* File)
{
  if(File)
    m_FileList.remove(File);
}

void CUpdateFilePool::GetAllFile()
{
  list<CUpdateFile*>::iterator iter=m_FileList.begin();
  try
  {
    while(iter!=m_FileList.end())
    {
      CUpdateFile* pFile=*iter++;
      if(!pFile->m_Complete)
        pFile->Get();
    }
  }
  catch(...)
  {
    delete *iter;
  }
}

void CUpdateFilePool::UpdateAllFile()
{
 list<CUpdateFile*>::iterator iter=m_FileList.begin();
  while(iter!=m_FileList.end())
  {
    CUpdateFile* pFile=*iter++;
    pFile->Update();
  }
  Clear();
}

void CUpdateFilePool::Clear()
{
  list<CUpdateFile*>::iterator iter=m_FileList.begin();
  while(iter!=m_FileList.end())
    delete (*iter++);
  m_FileList.clear();
}


#pragma package(smart_init)
