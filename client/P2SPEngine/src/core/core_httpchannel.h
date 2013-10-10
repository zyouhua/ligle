////////////////////////////////////////////////////////////////////////////////
// P2P Client Eegineering (P2P_CE)
// core_httpchannel.h
// 最后修改：2006.11.4
////////////////////////////////////////////////////////////////////////////////

#ifndef _CORE_HTTPCHANNEL_H_
#define _CORE_HTTPCHANNEL_H_

#include "core_channel.h"
#include "icl_socket.h"

using namespace icl;

const int HEADLEN=2048;

////////////////////////////////////////////////////////////////////////////////
//枚举类型，代表下载HTTP任务的连接状态

enum CHttpState
{
  HTTPINIT,                          //初始状态
  HTTPCONNECT,                       //连接状态
  HTTPGETLENGTH,                     //获得文件大小
  HTTPGETHEAD,                       //获得回应头
  HTTPRECEIVE,                       //接受数据状态
  HTTPWAIT                           //等待状态
};


////////////////////////////////////////////////////////////////////////////////
//class CHttpChannel ,HTTP下载通道

class CHttpChannel : public CChannel
{
  private:
    CTcpClient m_Connection;
    char m_Head[HEADLEN];
    int  m_Code;
    int64 m_FileLen;
    char m_Location[250];
    char m_ServerResetCookie[2048];
    bool m_Retry;
    unsigned m_Delay;
    bool m_Chunk;
    bool m_ChunkTail;
  private:
    void OnInit();
    void OnConnect();
    void OnGetLength();
    void OnGetHead();
    void OnReceive();
    void OnWait();
    bool SendHead(int64 Start = 0);
    bool GetHead();
    bool HandleHead();
    void GetChunkHead();
    void Saves();
  public:
    virtual void Handle();
    void Clear();
    CHttpChannel(CResource *Resource,CTask *Task);
    virtual ~CHttpChannel(){Clear();};
};

#endif // _CORE_HTTPCHANNEL_H_
