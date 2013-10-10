////////////////////////////////////////////////////////////////////////////////
//   P2SP Client Engine(P2SP CE)
//   app_event.h
//   ����޸�ʱ�䣺18-12-2006
////////////////////////////////////////////////////////////////////////////////

#ifndef _APP_EVENT_H_
#define _APP_EVENT_H_

#include "icl_classes.h"

using namespace icl;

//Task���¼�����

enum EventAction
{
  EVENT_INSERT,
  EVENT_UPDATE,
  EVENT_COMPLETE,
  EVENT_DELETE,
//  EVENT_START,
//  EVENT_STOP
};

enum EventSender
{
  SENDER_CORE,
  SENDER_VIEW
};

struct CAppEvent
{
  int Sender;    //�¼��ķ�����: 0��Task,1��ListView
  int TaskID;    //�����
  int Action;    //�¼����ͣ� 0�����ӣ�1��ɾ����2���޸ģ�3����ͣ��4����ʼ
};

//------------------------------------------------------------------------------
//  �¼���
//  �����洢APP�¼�
//------------------------------------------------------------------------------
typedef deque<CAppEvent*>  AppEventList; //�¼�����

class CAppEventPool
{
private:
  CCriticalSection m_Lock;
  AppEventList m_EventList;
  int m_Count;

private:
  void Clear();
public:
  CAppEventPool();
  ~CAppEventPool();
  bool GetEvent(CAppEvent*&);
  void Notify(int Sender,int TaskID,int Action);
};

extern CAppEventPool AppEventPool;

#endif
