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
// 收到服务器传来的IP的业务
class CTSGetIp : public CTrackerService
{
public:
  CTSGetIp();
  virtual ~CTSGetIp(){};
  virtual void Handle(void* Buffer, int Size);
};


////////////////////////////////////////////////////////
// 收到服务器传来的URL的业务
class CTSGetUrl : public CTrackerService
{
public:
  CTSGetUrl();
  virtual ~CTSGetUrl(){}
  virtual void Handle(void* Buffer, int Size);
};


////////////////////////////////////////////////////////
// 上传资源业务

class CTSUpload : public CTrackerService
{
public:
  CTSUpload();
  virtual ~CTSUpload(){}
  virtual void Handle(void* Buffer, int Size);
};


////////////////////////////////////////////////////////
// 通用应答业务
class CTSAck : public CTrackerService
{
public:
  CTSAck();
  virtual ~CTSAck(){}
  virtual void Handle(void* Buffer, int Size);
};


////////////////////////////////////////////////////////
// 无资源包的业务
class CTSNoop : public CTrackerService
{
public:
  CTSNoop();
  virtual ~CTSNoop(){}
  virtual void Handle(void* Buffer, int Size);
};


////////////////////////////////////////////////////////
//积分包的业务

class CTSIntegral : public CTrackerService
{
public:
  CTSIntegral();
  virtual ~CTSIntegral(){}
  virtual void Handle(void* Buffer, int Size);
};


#endif