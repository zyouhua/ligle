////////////////////////////////////////////////////////////////////////////////
//   P2SP Client Engine (P2SP CE)
//   metafile.cpp
//   最后修改时间29-11-2006
////////////////////////////////////////////////////////////////////////////////
#include "core_metafile.h"
#include <Registry.hpp>

CMetaFile MetaFile;

//////////////////////////////////////
//   class CMetaFile
//////////////////////////////////////

//------------------------------------------------------------------------------
//  构造，析构函数
//------------------------------------------------------------------------------
CMetaFile::CMetaFile()
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
  m_FileName=path.c_str();
  m_FileName+="meta.dat";
  m_DataLen=sizeof(CMetaData);
  m_Records=0;
  try
  {
    CFileStream FileStream(m_FileName, FM_OPEN_READ | FM_SHARE_DENY_NONE);
    CMetaData MetaData;
    while(FileStream.Read(&MetaData, sizeof(MetaData)) ) m_Records++;
  }
  catch (CException& e)
  {
    CFileStream FileStream(m_FileName, FM_CREATE | FM_SHARE_DENY_WRITE);
  }
}

void CMetaFile::Pack()
{
  string BackName=m_FileName+"a";
  int Records=0;
  try
  {
    CFileStream BackStream(BackName, FM_CREATE | FM_SHARE_DENY_WRITE);
    CFileStream FileStream(m_FileName, FM_OPEN_READ | FM_SHARE_DENY_NONE);
    CMetaData MetaData;
    while(FileStream.Read(&MetaData, sizeof(MetaData)))
    {
      if(MetaData.State !=5)
      {
        MetaData.Index=Records++;
        BackStream.Write(&MetaData,m_DataLen);
      }
    }
  }
  catch (CException& e)
  {
    return;
  }
  DeleteFile(m_FileName);
  rename(BackName.c_str(),m_FileName.c_str());
}

//------------------------------------------------------------------------------
//  更新数据
//------------------------------------------------------------------------------
void CMetaFile::UpDate(CMetaData* pMetaData)
{
  int Index=m_DataLen*pMetaData->Index;
  CFileStream FileStream(m_FileName, FM_OPEN_WRITE | FM_SHARE_DENY_NONE);
  FileStream.Seek(Index,SO_BEGINNING);
  FileStream.Write(pMetaData,m_DataLen);
}

//------------------------------------------------------------------------------
//  增加数据
//------------------------------------------------------------------------------
void CMetaFile::Insert(CMetaData* pMetaData)
{
  pMetaData->Index=m_Records++;
  CFileStream FileStream(m_FileName, FM_OPEN_WRITE | FM_SHARE_DENY_NONE);
  FileStream.Seek(0,SO_END);
  FileStream.Write(pMetaData,m_DataLen);
}

//------------------------------------------------------------------------------
//  增加数据
//------------------------------------------------------------------------------
void CMetaFile::Delete(int nIndex)
{
  int DeleteState=5;
  int Index=m_DataLen*nIndex;
  CFileStream FileStream(m_FileName, FM_OPEN_WRITE | FM_SHARE_DENY_NONE);
  FileStream.Seek(Index,SO_BEGINNING);
  FileStream.Write(&DeleteState,sizeof(int));
}

//------------------------------------------------------------------------------
// 描述: //查找上传文件
//------------------------------------------------------------------------------
bool CMetaFile::GetFile(char* pFileCode,char* pFileName)
{
  CMetaData MetaData;
  CFileStream FileStream(m_FileName,FM_OPEN_READ);
  while(FileStream.Read(&MetaData,m_DataLen))
  {
    if(strcmp(pFileCode,MetaData.FileCode)==0 && MetaData.State==1)
    {
      strcpy(pFileName,MetaData.FileName);
      return true;
    }
  }
  return false;
}

//------------------------------------------------------------------------------
// 描述: //查找文件
//------------------------------------------------------------------------------

bool CMetaFile::GetFile(char* pUrl)
{
  CMetaData MetaData;
  CFileStream FileStream(m_FileName,FM_OPEN_READ);
  while(FileStream.Read(&MetaData,m_DataLen))
  {
    if((strcmp(pUrl,MetaData.Url)==0) && (MetaData.State!=5))
      return true;
  }
  return false;
}


int CMetaFile::Load(CMetaList& List,int Type)
{
  int Geted=0;
  CMetaData* pMetaData=new CMetaData;
  CFileStream FileStream(m_FileName,FM_OPEN_READ);
  while(FileStream.Read(pMetaData,m_DataLen))
  {
    if( pMetaData->State==Type)
    {
      List.push_back(pMetaData);
      Geted++;
      pMetaData=new CMetaData;
    }
  }
  delete pMetaData;
  return Geted;

}

bool CMetaFile::Load(int TaskID,CMetaData* pMetaData)
{
  CFileStream FileStream(m_FileName,FM_OPEN_READ);
  while(FileStream.Read(pMetaData,m_DataLen))
  {
    if( pMetaData->Index==TaskID) return true;
  }
  return false;
}

