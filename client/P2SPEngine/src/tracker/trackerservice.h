//////////////////////////////////////////////////////
//class CTrackerService.h
//////////////////////////////////////////////////////
#ifndef _TRACKER_SERVICE_H_
#define _TRACKER_SERVICE_H_
//////////////////////////////////////////////////////
#include "tracker.h"
#include "icl_classes.h"
#include "core_task.h"

using namespace icl;
////////////////////////////////////////////////////////
// �յ�������������IP��ҵ��
class CTSGetIp : public CTrackerService
{
public:
  CTSGetIp();
  virtual ~CTSGetIp(){};
  virtual void Handle(void* Buffer, int Size);
};


////////////////////////////////////////////////////////
// �յ�������������URL��ҵ��
class CTSGetUrl : public CTrackerService
{
public:
  CTSGetUrl();
  virtual ~CTSGetUrl(){}
  virtual void Handle(void* Buffer, int Size);
};


////////////////////////////////////////////////////////
// �ϴ���Դҵ��

class CTSUpload : public CTrackerService
{
public:
  CTSUpload();
  virtual ~CTSUpload(){}
  virtual void Handle(void* Buffer, int Size);
};


////////////////////////////////////////////////////////
// ͨ��Ӧ��ҵ��
class CTSAck : public CTrackerService
{
public:
  CTSAck();
  virtual ~CTSAck(){}
  virtual void Handle(void* Buffer, int Size);
};


////////////////////////////////////////////////////////
// ����Դ����ҵ��
class CTSNoop : public CTrackerService
{
public:
  CTSNoop();
  virtual ~CTSNoop(){}
  virtual void Handle(void* Buffer, int Size);
};


////////////////////////////////////////////////////////
//���ְ���ҵ��

class CTSIntegral : public CTrackerService
{
public:
  CTSIntegral();
  virtual ~CTSIntegral(){}
  virtual void Handle(void* Buffer, int Size);
};


#endif