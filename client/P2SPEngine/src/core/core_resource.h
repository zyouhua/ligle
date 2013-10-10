////////////////////////////////////////////////////////////////////////////////
// P2P Client Eegineering (P2P_CE)
// core_resource.h
//����޸�ʱ�䣺9-11-2006
////////////////////////////////////////////////////////////////////////////////
#ifndef _CORE_RESOURCE_H
#define _CORE_RESOURCE_H

#include <string>
#include <list>
#include "core_global.h"
#include "icl_classes.h"

using namespace icl;

///////////////////////////////////////////////////////////////////////////////
// ��ǰ����
class CTask;

enum ResourceError
{
  RS_NORMAL,               //����
  RS_TIMEOUT,              //���ӳ�ʱ
  RS_NOIP,                 //�����޷�����
  RS_URLERROR,             //URL��ַ����
  RS_IOERROR,              //��������
  RS_FILEERROR,            //�ļ���д����
  RS_ACCESSERROR           //�޷�������վ
};
//-------------------------------------------------------
//һ�����õ���Դ�����𱣴����Դ��������Ϣ��
//��Ҫ�ṩ������ͨ������ʹ��
//-------------------------------------------------------
class CResource
{
public:
  Type m_Type;
  string m_strIp;
  string m_strUrl;
  string m_strPath; //�ļ��洢·��
  string m_strRefe; //����ҳ
  string m_strHostName;//������
  string m_LocationCookie;
  string m_ServerResetCookie;
  int m_nPort;
private:
  int m_nChannels;//channel������
  int m_nMaxChannels ;//����ͨ��������
  int m_nErrorCode; //������� 0--����
  bool m_bReady;     //��Դ�Ƿ���IP��ַ
  bool m_bIsGetLength; //�Ƿ���ɻ�ȡ����
  bool m_bUsable;      //��Դ�Ƿ����
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
//������Դ�����������洢������Դ��������
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
