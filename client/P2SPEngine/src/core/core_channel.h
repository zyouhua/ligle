////////////////////////////////////////////////////////////////////////////////
// P2P Client Eegineering (P2P_CE)
// core_channel.h
// ����޸ģ�2006.11.4
////////////////////////////////////////////////////////////////////////////////

#ifndef _CORE_CHANNEL_H_
#define _CORE_CHANNEL_H_

#include "core_task.h"
#include <stdlib>

using namespace icl;

///////////////////////////////////////////////////////////////////////////////
// ��ǰ����

class CChannelPool;

const int STATE_TIMEOUT          = 1000*60*1;  //10��

const double CHANNEL_SLEEP_TIME          = 0.001;  //1����

////////////////////////////////////////////////////////////////////////////////
//class CChannel����ͨ����

class CChannel
{
  protected:
    CResource *m_Resource;      //��Դ����
    CTask *m_Task;              //�������
    CBlock *m_Block;
    int m_State;                //��������״̬
    uint m_nLastTicks;          //�ϴβ���ʱ��
    char m_Buffer[PIECELEN];    //���ݻ���
    int64 m_nBlockStart;
    int m_nPieceIndex;
    int m_nPieceRemain;
    bool m_NeedSleep;

  protected:
    bool GetBlock();
    void SetPiece();
    void SetState(int nState);
    void CheckTimeOut();
    void DoOnError(int Error);
    void Save();
    bool SetLength(int64 FileLen);
  public:
    virtual void Handle() = 0;
    inline bool NeedSleep(){return m_NeedSleep;}
    CChannel(CResource *Resource,CTask *Task);
    virtual ~CChannel();
};

////////////////////////////////////////////////////////////////////////////////
//class CChannelPoolȫ��Ψһ������ͨ���ع�����

class CChannelPool
{
  private:
    CCriticalSection m_Lock;
    list<CChannel*> m_Channels;     //����һ��list����
  public:
    bool HandleAllChannels();
    void AddChannel(CChannel *Channel);
    void RemoveChannel(CChannel *Channel);
    CChannelPool(){};
    ~CChannelPool(){};
};

/////////////////////////////////////////////////////
// class CChannelHandle�߳�����ͨ��������

class CChannelHandler : public CThreadHandler
{
  protected:
    virtual void Handle();
  public:
    CChannelHandler(){ m_fThreadSleepSeconds=CHANNEL_SLEEP_TIME;}
    ~CChannelHandler(){}
};

extern  CChannelHandler ChannelHandler;
#endif // _CORE_CHANNEL_H_
