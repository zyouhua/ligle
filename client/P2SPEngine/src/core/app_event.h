////////////////////////////////////////////////////////////////////////////////
//   P2SP Client Engine(P2SP CE)
//   app_event.h
//   最后修改时间：18-12-2006
////////////////////////////////////////////////////////////////////////////////

#ifndef _APP_EVENT_H_
#define _APP_EVENT_H_

#include "icl_classes.h"

using namespace icl;

//Task的事件类型

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
  int Sender;    //事件的发送者: 0、Task,1、ListView
  int TaskID;    //任务号
  int Action;    //事件类型： 0、增加，1、删除，2、修改，3、暂停，4、开始
};

//------------------------------------------------------------------------------
//  事件池
//  用来存储APP事件
//------------------------------------------------------------------------------
typedef deque<CAppEvent*>  AppEventList; //事件队列

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
