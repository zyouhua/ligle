////////////////////////////////////////////////////////////////////////////////
// P2P Client Eegineering (P2P_CE)
// core_task.h
//最后修改时间：9-11-2006
////////////////////////////////////////////////////////////////////////////////
#ifndef _CORE_TASK_H_
#define _CORE_TASK_H_


#include "core_resource.h"
#include "core_block.h"
#include "core_piece.h"
#include "core_metafile.h"
#include "icl_handler.h"
#include "icl_classes.h"
#include "app_event.h"

#include <string>

using namespace icl;

const double TASK_SLEEP_TIME = 0.1; //秒

//Task的状态
enum TaskState
{
  TASKINIT,
  TASkGETLENGTH,
  TASKDOWNLOAD,
  TASKCOMPLETE,
  TASKSTOP,
  TASKSTOPED,
  TASKDELETE
};


//-------------------------------------------------------------------------
//下载任务类，单个下载任务，负责保存下载文件的相关信息，
//并负责文件的存取，他拥有资源管理池和块管理池。
//-------------------------------------------------------------------------
class TMainFrm;

class CTask
{
friend class CPiecePool;
friend class CTaskPool;
friend class TMainFrm;

private:
  string m_Url;
  string m_Ref;
  string m_FileName;
  string m_FileCode;
  int64  m_nLength;
  int64 m_Downloaded;
  long m_Speed;
  int  m_nFileID;
  int  m_State;
  int  m_StopCode;
  int  m_nChannels;
  CBlockPool m_BlockPool;
  CResourcePool m_ResourcePool;
  CPiecePool  m_PiecePool;
  CMetaData* m_MetaData;

private:
  void OnInit();
  void OnGetLength();
  void OnDownload();
  void OnComplete();
  void OnDelete();
  void OnStop();
  void Insert();
  bool CheckCode();
  void SetState(int State,int Action);
  bool DoOnStop();
  void VerifyName();
  bool DiskFile(int64 Length);
  void CheckFile();
public:
  CTask(string& Url,string& Ref,string& FileName);
  CTask(CMetaData* pMetaData);
  ~CTask();
  void Handle();
  void Stop(int Reason){  m_StopCode=Reason; m_State=TASKSTOP;}
  void Start();
  void Delete(){m_State=TASKDELETE;}

  bool GetBlock(CBlock* &Block);
  bool SetLength(int64 Length);
  void Save(int64 Position,int Length,char* pBuffer);
  void Attach(){m_nChannels++;}
  void Detach(){m_nChannels--;}
  bool Check(){return ( m_State == TASKDOWNLOAD ? true : false);}

  int64 GetLength(){return m_nLength;}
  int GetTaskID(){return m_nFileID;}
  void SetCode(string& Hash){ m_FileCode=Hash;strcpy(m_MetaData->FileCode,Hash.c_str());}
  CResourcePool* GetResourcePool(){ return &m_ResourcePool;}
  string GetCode() { return m_FileCode;}
  void Rename(string Location);
};

//-------------------------------------------------------------------------
//下载任务池，负责管理所有的下载任务，全局唯一，
//主要提供两类接口：
//一类提供给用户，控制下载任务的增加，停止等。
//一类提供给下载任务控制线程（CTaskhandler），控制下载过程中资源连接的调节。
//-------------------------------------------------------------------------
class CTaskPool
{
private:
  CCriticalSection m_Lock;
  list<CTask*> m_TaskList;
  int m_Tasks;
  void AddTask(CMetaList& metaList );
public:
  CTaskPool();
  ~CTaskPool();
  void RemoveTask(CTask* pTask);
  void AddTask(string& Url,string& Ref,string& FileName);
  void HandleAllTasks();
  bool GetTask(int TaskID,CTask* &pTask);
  void StopTasks();
};

//-------------------------------------------------------------------------
//下载任务处理器，从线程处理器继承而来，
//它通过下载任务池管理器的接口（HandleAllTask）管理所有正在下载的任务
//-------------------------------------------------------------------------
class CTaskHandler:public CThreadHandler
{
public:
  CTaskHandler(){m_fThreadSleepSeconds=TASK_SLEEP_TIME;};
  ~CTaskHandler(){};
protected:
  virtual void Handle();
};

extern CTaskPool TaskPool;
extern CTaskHandler TaskHandler;
#endif
