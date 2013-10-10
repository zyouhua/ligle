////////////////////////////////////////////////////////////////////////////////
//   P2SP Client Engine(P2SP CE)
//   app_event.cpp
//   ����޸�ʱ�䣺18-12-2006
////////////////////////////////////////////////////////////////////////////////
#include "app_event.h"

//ȫ�ֱ���

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
//  ����¼�
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
//  ȡ���¼�
//  �������ڴ���ĳ���¼���ʱ����ȡ�����Ա㴦���꽫��ɾ��
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
// ��ն���
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
