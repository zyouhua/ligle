////////////////////////////////////////////////////////////////////////////////
// P2P Client Eegineering (P2P_CE)
// core_channel.h
// 最后修改：2006.11.4
////////////////////////////////////////////////////////////////////////////////

#ifndef _CORE_CHANNEL_H_
#define _CORE_CHANNEL_H_

#include "core_task.h"
#include <stdlib>

using namespace icl;

///////////////////////////////////////////////////////////////////////////////
// 提前声明

class CChannelPool;

const int STATE_TIMEOUT          = 1000*60*1;  //10秒

const double CHANNEL_SLEEP_TIME          = 0.001;  //1毫秒

////////////////////////////////////////////////////////////////////////////////
//class CChannel下载通道类

class CChannel
{
  protected:
    CResource *m_Resource;      //资源对象
    CTask *m_Task;              //任务对象
    CBlock *m_Block;
    int m_State;                //任务下载状态
    uint m_nLastTicks;          //上次操作时间
    char m_Buffer[PIECELEN];    //数据缓存
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
//class CChannelPool全局唯一的下载通道池管理器

class CChannelPool
{
  private:
    CCriticalSection m_Lock;
    list<CChannel*> m_Channels;     //声明一个list容器
  public:
    bool HandleAllChannels();
    void AddChannel(CChannel *Channel);
    void RemoveChannel(CChannel *Channel);
    CChannelPool(){};
    ~CChannelPool(){};
};

/////////////////////////////////////////////////////
// class CChannelHandle线程下载通道处理器

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
