////////////////////////////////////////////////////////////////////////////////
//   P2SP Client Engine(P2SP CE)
//   app_event.cpp
//   最后修改时间：18-12-2006
////////////////////////////////////////////////////////////////////////////////
#include "app_event.h"

//全局变量

CAppEventPool AppEventPool;


//==============================================================================
//class CAppEventPool
//==============================================================================
CAppEventPool::CAppEventPool()
{
  m_Count=0;
}

CAppEventPool::~CAppEventPool()
{
  Clear();
}
//------------------------------------------------------------------------------
//  添加事件
//------------------------------------------------------------------------------
void CAppEventPool::Notify(int Sender,int TaskID,int Action)
{
  CAppEvent *Event=new CAppEvent;
  Event->Sender=Sender;
  Event->TaskID=TaskID;
  Event->Action=Action;

  CAutoSynchronizer AutoSyncher(m_Lock);
  m_EventList.push_back(Event);
  m_Count++;
}

//------------------------------------------------------------------------------
//  取出事件
//  描述：在处理某个事件的时候将其取出，以便处理完将其删除
//------------------------------------------------------------------------------
bool CAppEventPool::GetEvent(CAppEvent* &Event)
{
  CAutoSynchronizer AutoSyncher(m_Lock);
  if(m_Count>0)
  {
    Event=m_EventList.front();
    m_EventList.pop_front();
    m_Count--;
    return true;
  }
  return false;
}
//------------------------------------------------------------------------------
// 清空队列
//------------------------------------------------------------------------------
void CAppEventPool::Clear()
{
  AppEventList::iterator iter;
  for(iter=m_EventList.begin();iter!=m_EventList.end();iter++)
  {
    if(*iter)
      delete (*iter);
  }
  m_EventList.clear();
}
