////////////////////////////////////////////////////////////////////////////////
// P2P Client Eegineering (P2P_CE)
// core_task.h
//����޸�ʱ�䣺9-11-2006
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

const double TASK_SLEEP_TIME = 0.1; //��

//Task��״̬
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
//���������࣬�����������񣬸��𱣴������ļ��������Ϣ��
//�������ļ��Ĵ�ȡ����ӵ����Դ����غͿ����ء�
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
//��������أ�����������е���������ȫ��Ψһ��
//��Ҫ�ṩ����ӿڣ�
//һ���ṩ���û�������������������ӣ�ֹͣ�ȡ�
//һ���ṩ��������������̣߳�CTaskhandler�����������ع�������Դ���ӵĵ��ڡ�
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
//�����������������̴߳������̳ж�����
//��ͨ����������ع������Ľӿڣ�HandleAllTask�����������������ص�����
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
