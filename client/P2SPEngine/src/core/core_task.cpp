////////////////////////////////////////////////////////////////////////////////
// P2P Client Eegineering (P2P_CE)
// core_task.cpp
//最后修改时间：9-11-2006
////////////////////////////////////////////////////////////////////////////////

#include "core_task.h"
#include "core_channel.h"
#include "calc.hpp"
#include "core_global.h"
#include "icl_classes.h"
#include "tracker.h"
#include "idglobal.hpp"


CTaskPool TaskPool;//全局对象，任务容器
CTaskHandler TaskHandler;
///////////////////////////////////////////////////////////////////////////
//class CTask

CTask::CTask(string& Url,string& Ref,string& FileName)
{
  m_Url=Url;
  m_Ref=Ref;
  m_FileName=FileName;
  m_nLength=0;
  m_Downloaded=0;
  m_Speed=0;
  m_nChannels=0;
  m_StopCode=0;
  Insert();
  SetState(TASKINIT,EVENT_INSERT);
}

CTask::CTask(CMetaData* pMetaData)
{
  m_MetaData = pMetaData;
  m_Url = m_MetaData->Url;
  m_Ref = m_MetaData->Ref;
  m_FileName = m_MetaData->FileName;
  m_nLength = m_MetaData->FileSize;
  m_nFileID = m_MetaData->Index;
  m_FileCode=m_MetaData->FileCode;

  m_Downloaded=0;
  m_Speed=0;
  m_StopCode=0;
  m_nChannels=0;
  if(m_nLength)
  {
    if(FileExists(m_FileName) && FileExists(m_FileName+".blc"))
    {
      m_BlockPool.SetFileName(m_FileName);
      m_Downloaded=m_BlockPool.Load(m_nLength);
    }
    else m_nLength=0;
  }

  SetState(TASKSTOPED,EVENT_INSERT);
}

void CTask::Insert()
{
  m_MetaData=new CMetaData;
  m_MetaData->State=0;
  strcpy(m_MetaData->Url,m_Url.c_str());
  strcpy(m_MetaData->Ref,m_Ref.c_str());
  strcpy(m_MetaData->FileName,m_FileName.c_str());
  MetaFile.Insert(m_MetaData);
  m_nFileID= m_MetaData->Index;
}

CTask::~CTask()
{
  delete m_MetaData;
  TaskPool.RemoveTask(this);
}
//-----------------------------------------------------------------
//任务处理函数
//-----------------------------------------------------------------
void CTask::Handle()
{
  switch(m_State)
  {
	case TASKINIT:
    OnInit();
    break;
	case TASKDOWNLOAD:
    OnDownload();
	  break;
  case TASKSTOP:
    OnStop();
  	break;
  case TASKCOMPLETE:
    OnComplete();
  	break;
  case TASKDELETE:
    OnDelete();
  	break;
  case TASKSTOPED:
    break;
  default: break;
	}
}

void CTask::OnInit()
{
  CResource* pResource=new CResource(m_Url,m_Ref,(bool)m_nLength);
  m_ResourcePool.AddResource(pResource);
  SetState(TASKDOWNLOAD,EVENT_UPDATE);
  if(m_nLength) Tracker.AskResource(m_Url,m_Ref,m_nLength,m_nFileID);
}


void CTask::OnDownload()
{
  try
  {
    if(m_PiecePool.Handle(this))
    {
      m_PiecePool.Save(m_FileName);
      m_BlockPool.Save();
    }
  }
  catch(CException& e)
  {
    Stop(RS_FILEERROR);
  }

  m_ResourcePool.HandleResources(this);
}

void CTask::OnStop()
{
  if(!DoOnStop()) return;
  SetState(TASKSTOPED,EVENT_UPDATE);
}

void CTask::OnDelete()
{
  if(!DoOnStop()) return;

  m_MetaData->State=2;
  MetaFile.UpDate(m_MetaData);
  SetState(TASKSTOPED,EVENT_DELETE);
  throw CException();
}

void CTask::OnComplete()
{
  if(!DoOnStop()) return;

  m_BlockPool.DeleteBlcFile();             //deletefile
  CheckCode();
  m_MetaData->State=1;
  MetaFile.UpDate(m_MetaData);
  SetState(TASKCOMPLETE,EVENT_COMPLETE);

  Tracker.Downloaded(m_Url, m_nLength, m_FileCode);
  throw CException();
}

bool CTask::DoOnStop()
{
  if(m_nChannels) return false;
  m_ResourcePool.Clear();
  try
  {
    if(m_nLength)
    {
      m_PiecePool.Save(m_FileName);
      m_BlockPool.Save();
    }
  }
  catch(CException& e)
  {
    return true;
  }
  return true;
}

bool CTask::CheckCode()
{
  AnsiString FileCode=CalcFileGuid((AnsiString)(m_FileName.c_str()));
  if(m_FileCode.empty())
  {
    m_FileCode=FileCode.c_str();
    strcpy(m_MetaData->FileCode,m_FileCode.c_str());
    return true;
  }
  if(strcmp(FileCode.c_str(),m_FileCode.c_str())==0) return true;
  return false;
}

//-----------------------------------------------------------------
//取得下载块
//-----------------------------------------------------------------
bool CTask::GetBlock(CBlock* &Block)
{
  if(!m_BlockPool.GetBlock(Block))
  {
      SetState(TASKCOMPLETE,EVENT_UPDATE);
      return false;
  }
  return true;
}

bool CTask::DiskFile(int64 Length)
{
  VerifyName();
  try
  {
    CFileStream File(m_FileName,FM_CREATE);
    File.SetSize(Length);
  }
  catch (CException& e)
  {
    DeleteFile(m_FileName);
    Stop(RS_IOERROR);
    return false;
  }
  return true;
}

//-----------------------------------------------------------------
//   设置文件的长度，并创建文件
//-----------------------------------------------------------------
bool CTask::SetLength(int64 Length)
{
  if(!DiskFile(Length)) return false;

  m_nLength=Length;
  m_MetaData->FileSize=m_nLength;
  strcpy(m_MetaData->FileName,m_FileName.c_str());
  MetaFile.UpDate(m_MetaData);

  m_BlockPool.SetFileName(m_FileName);
  CBlock* pBlock=new CBlock(0,m_nLength,0);
  m_BlockPool.AddBlock(pBlock);
  m_BlockPool.Save();

  AppEventPool.Notify(SENDER_CORE,m_nFileID,EVENT_UPDATE);
  Tracker.AskResource(m_Url,m_Ref,m_nLength,m_nFileID);
  return true;
}

void CTask::Start()
{

  if(m_nLength)
  {
    if(!FileExists(m_FileName) ||  !FileExists(m_FileName+".blc"))
    {
      m_nLength=0;
      m_Downloaded=0;
      m_BlockPool.Clear();
      m_PiecePool.Clear();
    }
  }
  m_StopCode=RS_NORMAL;
  m_State=TASKINIT;
}

void CTask::SetState(int State,int Action)
{
  m_State=State;
  AppEventPool.Notify(SENDER_CORE,m_nFileID,Action);
}

//--------------------------------------------------------------
//   写文件
//    参数：Position 起始位置
//          Length   读写的长度
//          pBuffer   数据的缓存
//--------------------------------------------------------------
void CTask::Save(int64 Position,int Length,char* pBuffer)
{
  CPiece* pPiece=new CPiece;
  pPiece->Position=Position;
  pPiece->Length=Length;
  memmove(pPiece->Buffer,pBuffer,Length);
  m_PiecePool.AddPiece(pPiece);
  m_Downloaded+=Length;
}

void CTask::Rename(string Location)
{
  if(m_nLength) return;

  int i=Location.find('?',0);
  if(i>0) Location=Location.substr(0,i);

  i=Location.find_last_of('/',Location.length());
  AnsiString FileName=URLDecode(Location.substr(i+1,Location.length()-i-1).c_str());
  if(FileName.Length()<3) FileName="index.htm";

  i=m_FileName.find_last_of('\\',m_FileName.length());
  m_FileName=m_FileName.substr(0,i+1)+FileName.c_str();

}

void CTask::VerifyName()
{
  int Number=1;
  string MainName=m_FileName,ExtName="";

  int i=m_FileName.find_last_of('.',m_FileName.length());
  if(i!=-1)
  {
    MainName=m_FileName.substr(0,i);
    ExtName=m_FileName.substr(i,m_FileName.length());
  }

  while(FileExists(m_FileName))
  {
    string AddName="";
    AddName="("+icl::IntToStr(Number)+")";
    m_FileName=MainName+AddName+ExtName;
    Number++;
  }
}

/////////////////////////////////////////////////////////////////////////////////
// class CTaskPool
//
CTaskPool::CTaskPool()
{
  m_Tasks=0;
  CMetaList metaList;
  MetaFile.Load(metaList,0);
  AddTask(metaList);
}
//-----------------------------------------------------------------
//  删除一个任务
//-----------------------------------------------------------------
void CTaskPool::RemoveTask(CTask* pTask)
{
  m_TaskList.remove(pTask);
  m_Tasks--;
}

//-----------------------------------------------------------------
//析构函数
//-----------------------------------------------------------------
CTaskPool::~CTaskPool()
{
  list<CTask*>::iterator iter;
  for(iter=m_TaskList.begin();iter!=m_TaskList.end();)
     delete (*iter++);
}
//-----------------------------------------------------------------
//添加任务
//-----------------------------------------------------------------
void CTaskPool::AddTask(string& Url,string& Ref,string& FileName)
{
  CAutoSynchronizer AutoSyncher(m_Lock);
  CTask* task=new CTask(Url,Ref,FileName);
  m_TaskList.push_back(task);
  m_Tasks++;
}

void CTaskPool::AddTask(CMetaList& metaList )
{
  CAutoSynchronizer AutoSyncher(m_Lock);
  list<CMetaData*>::iterator iter;
  for(iter=metaList.begin();iter!=metaList.end();iter++)
  {
    CTask* task=new CTask((*iter));
    m_TaskList.push_back(task);
    m_Tasks++;
  }
}
//-----------------------------------------------------------------
//处理所有任务
//-----------------------------------------------------------------
void CTaskPool::HandleAllTasks()
{
  CAutoSynchronizer AutoSyncher(m_Lock);
  list<CTask*>::iterator iter;
  CTask* pTask=NULL;
  try
  {
    for(iter=m_TaskList.begin();iter!=m_TaskList.end();iter++)
    {
       pTask=(*iter);
       pTask->Handle();
    }
  }
  catch (CException& e)
  {
    delete pTask;
  }
}
//-----------------------------------------------------------------
//停止所有任务
//-----------------------------------------------------------------
void CTaskPool::StopTasks()
{
  list<CTask*>::iterator iter;
  CTask* pTask;
  for(;;)
  {
    bool Stoped=true;
    for(iter=m_TaskList.begin();iter!=m_TaskList.end();iter++)
    {
       pTask=(*iter);
       if((pTask->m_State)!=TASKSTOPED)
       {
          pTask->Stop(RS_NORMAL);
          Stoped=false;
       }
    }
    if(Stoped) return;
  //  std::Sleep(1);
  }
}

bool CTaskPool::GetTask(int TaskID,CTask* &pTask)
{
  list<CTask*>::iterator iter;
  for(iter=m_TaskList.begin();iter!=m_TaskList.end();iter++)
  {
    if((*iter)->GetTaskID()==TaskID)
    {
      pTask=(*iter);
      return true;
    }
  }
  return false;
}
//////////////////////////////////////////////////////////////////
//class CTaskHandler
//下载任务处理器，从线程处理器继承而来，
//它通过下载任务池管理器的接口（HandleAllTask）管理所有正在下载的任务
/////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------
//任务处理器处理函数，虚函数，继承自CThreadHandler类
//-----------------------------------------------------------------
void CTaskHandler::Handle()
{
  TaskPool.HandleAllTasks();
}


