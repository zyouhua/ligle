////////////////////////////////////////////////////////////////////////////////
// P2P Client Eegineering (P2P_CE)
// core_resource.h
//最后修改时间：9-11-2006
////////////////////////////////////////////////////////////////////////////////
#ifndef _CORE_RESOURCE_H
#define _CORE_RESOURCE_H

#include <string>
#include <list>
#include "core_global.h"
#include "icl_classes.h"

using namespace icl;

///////////////////////////////////////////////////////////////////////////////
// 提前声明
class CTask;

enum ResourceError
{
  RS_NORMAL,               //正常
  RS_TIMEOUT,              //连接超时
  RS_NOIP,                 //域名无法解析
  RS_URLERROR,             //URL地址错误
  RS_IOERROR,              //磁盘已满
  RS_FILEERROR,            //文件读写错误
  RS_ACCESSERROR           //无法访问网站
};
//-------------------------------------------------------
//一个可用的资源，负责保存该资源的描述信息，
//主要提供给下载通道对象使用
//-------------------------------------------------------
class CResource
{
public:
  Type m_Type;
  string m_strIp;
  string m_strUrl;
  string m_strPath; //文件存储路径
  string m_strRefe; //引用页
  string m_strHostName;//主机名
  string m_LocationCookie;
  string m_ServerResetCookie;
  int m_nPort;
private:
  int m_nChannels;//channel的数量
  int m_nMaxChannels ;//最大的通道数量　
  int m_nErrorCode; //错误代码 0--正常
  bool m_bReady;     //资源是否有IP地址
  bool m_bIsGetLength; //是否完成获取长度
  bool m_bUsable;      //资源是否可用
private:
  bool GetHostIp();
  bool AnalyseUrl();
  void GetCookie();
  void Init(CTask* pTask);
  void DoCheckLength(CTask*);
  void CreateChannel(CTask*);
public:
  CResource(string strUrl,string strRef,bool bIsCetLength=true);
  CResource(string strIP,int Port);
  ~CResource();
  void ReSetURL(string strURL);
  void ReSetRequestCookie(string strCookie){ m_ServerResetCookie = strCookie; }

  void SetUsable(){m_bUsable=true;}
  void LenghtGeted(){ m_bIsGetLength=true; }
  void SetError(int Error){m_nErrorCode=Error;}
  int GetError(){return m_nErrorCode;}
  void Attach(){m_nChannels++;}
  void Detach(){m_nChannels--;}
  void Handle(CTask* pTask);
  
};
//-----------------------------------------------------
//下载资源容器，用来存储下载资源，并管理
//-----------------------------------------------
class CResourcePool
{
private:
	list<CResource*> m_ResourcePool;
	int m_nSize;
private:
	int GetSize(){return m_ResourcePool.size();}
	void DeleteResource(CResource*& res);
public:
	CResourcePool();
	~CResourcePool(){Clear();}
	void AddResource(CResource *Resource);
        void HandleResources(CTask* pTask);
	void Clear();
};
#endif
