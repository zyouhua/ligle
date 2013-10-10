/////////////////////////////////////////////////////////////////////////////////
//trackerPacket.cpp

#include "trackerpacket.h"

/////////////////////////////////////////////////////////////////////////////////
//CLogoutPacket - 下线包

void CLogoutPacket::DoPack()
{
    CTrackerPacket::DoPack();
};

void CLogoutPacket::DoUnpack()
{
    CTrackerPacket::DoUnpack();
};

void CLogoutPacket::InitData()
{
    Pack();
};

////////////////////////////////////////////////////////////////////////////////
//CKeepAlivePacket - 保活包

void CKeepAlivePacket::DoPack()
{
    CTrackerPacket::DoPack();
};

void CKeepAlivePacket::DoUnpack()
{
    CTrackerPacket::DoUnpack();
};

void CKeepAlivePacket::InitData()
{
    Pack();
};

////////////////////////////////////////////////////////////////////////////////
//CCIntegralPacket - 积分包

void CIntegralPacket::DoPack()
{
    CTrackerPacket::DoPack();
};

void CIntegralPacket::DoUnpack()
{
    CTrackerPacket::DoUnpack();
};

void CIntegralPacket::InitData()
{
    Pack();
};

////////////////////////////////////////////////////////////////////////////////
//CUpLoadFileListPacket - 上传资源列表包

void CUploadFileListPacket::DoPack()
{
    CTrackerPacket::DoPack();
    WriteBuffer(&wFileCount, sizeof(word));
    for(int i=0; i<wFileCount; i++)
    {
        WriteBuffer(&FileList.at(i).nFileLength, sizeof(int64));
        WriteString(FileList.at(i).strHashValue);
    }
};

void CUploadFileListPacket::DoUnpack()
{
    CTrackerPacket::DoUnpack();
    ReadBuffer(&wFileCount, sizeof(word));
    ////定义了一个结构体对象,将读取的数据与结构体对应,再放入容器中
    CUploadData UploadFile;
    for(int i=0; i<wFileCount; i++)
    {
        ReadBuffer(&UploadFile.nFileLength, sizeof(int64));
        ReadString(UploadFile.strHashValue);
        FileList.push_back(UploadFile);
    }
};

void CUploadFileListPacket::InitData(word wFileCount, CUploadList& FileList)
{
    this->wFileCount = wFileCount;
    this->FileList.assign(FileList.begin(), FileList.end());
    Pack();
};

////////////////////////////////////////////////////////////////////////////////
//CDownloadFileCompletePacket - 下载完成包

void CDownloadFileCompletePacket::DoPack()
{
    CTrackerPacket::DoPack();
    WriteString(strUrl);
    WriteBuffer(&nFileLength, sizeof(int64));
    WriteString(strHashValue);
};

void CDownloadFileCompletePacket::DoUnpack()
{
    CTrackerPacket::DoUnpack();
    ReadString(strUrl);
    ReadBuffer(&nFileLength, sizeof(int64));
    ReadString(strHashValue);
};

void CDownloadFileCompletePacket::InitData(string strUrl, int64 nFileLength, string strHashValue)
{
    this->strUrl = strUrl;
    this->nFileLength = nFileLength;
    this->strHashValue = strHashValue;
    Pack();
};

////////////////////////////////////////////////////////////////////////////////
//CRequestResource - 请求下载资源包

void CRequestResourcePacket::DoPack()
{
    CTrackerPacket::DoPack();
    WriteString(strRef);
    WriteString(strUrl);
    WriteBuffer(&nFileLength, sizeof(int64));
    WriteBuffer(&nTaskId, sizeof(int));
};

void CRequestResourcePacket::DoUnpack()
{
    CTrackerPacket::DoUnpack();
    ReadString(strRef);
    ReadString(strUrl);
    ReadBuffer(&nFileLength, sizeof(int64));
    ReadBuffer(&nTaskId, sizeof(int));
};

void CRequestResourcePacket::InitData(string strUrl, string strRef, int64 nFileLength, int nTaskId)
{
    this->strRef = strRef;
    this->strUrl = strUrl;
    this->nFileLength = nFileLength;
    this->nTaskId = nTaskId;
    Pack();
};


//------------------------------------------------------------------------------
// 以下是服务器所要发送的包


////////////////////////////////////////////////////////////////////////////////
//CRequestFilePacket - 服务器要求客户端上传其曾经下载,现在拥有的资源

void CRequestFilePacket::DoPack()
{
    CReTrackerPacket::DoPack();
};

void CRequestFilePacket::DoUnpack()
{
    CReTrackerPacket::DoUnpack();
};

void CRequestFilePacket::InitData(word nResultCode)
{
    CReTrackerPacket::InitData(nResultCode);
};
////////////////////////////////////////////////////////////////////////////////
//CReOnPacket - 通用应答包

void CReOnPacket::DoPack()
{
    CReTrackerPacket::DoPack();
};

void CReOnPacket::DoUnpack()
{
    CReTrackerPacket::DoUnpack();
};

void CReOnPacket::InitData(word nResultCode)
{
    CReTrackerPacket::InitData(nResultCode);
};
////////////////////////////////////////////////////////////////////////////////
// CReGetResoursesPacket - 回给客户端一个URL

void CReGetUrlPacket::DoPack()
{
    CReTrackerPacket::DoPack();
    //打一个URL
    WriteString(strReference);
    WriteString(strUrl);
    WriteString(strHashValue);
    //WriteBuffer(&nFileLength, sizeof(uint));
    WriteBuffer(&nTaskId, sizeof(int));
};

void CReGetUrlPacket::DoUnpack()
{
    CReTrackerPacket::DoUnpack();
    //读取一个URL
    ReadString(strReference);
    ReadString(strUrl);
    ReadString(strHashValue);
    //ReadBuffer(&nFileLength, sizeof(uint));
    ReadBuffer(&nTaskId, sizeof(int));
};

void CReGetUrlPacket::InitData(word nResultCode, string strReference, string strUrl,
                                string strHashValue, int nTaskId)
{
    this->strReference = strReference;
    this->strUrl = strUrl;
    this->strHashValue = strHashValue;
    //this->nFileLength = nFileLength;
    this->nTaskId = nTaskId;
    CReTrackerPacket::InitData(nResultCode);
};



/////////////////////////////////////////////////////////////////////////////////
// CReGetUserIpPacket - 回给客户端一批IP
void CReGetUserIpPacket::DoPack()
{
    CReTrackerPacket::DoPack();
    //打一批用户IP
    int Number = UserList.size();
    WriteBuffer(&Number, sizeof(int));
    for(int i=0; i<Number; i++)
    {
        WriteBuffer(&UserList.at(i).nUserIp, sizeof(uint));
        WriteBuffer(&UserList.at(i).wTcpPort, sizeof(word));

    }
    WriteString(strHashValue);
    //WriteBuffer(&nFileLength, sizeof(uint));
    WriteBuffer(&nTaskId, sizeof(int));
};

void CReGetUserIpPacket::DoUnpack()
{
    CReTrackerPacket::DoUnpack();
    //读取一批用户IP地址
    int Number;
    ReadBuffer(&Number, sizeof(int));
    CUserData UserData;
    for(int i=0; i<Number; i++)
    {
        ReadBuffer(&UserData.nUserIp, sizeof(uint));
        ReadBuffer(&UserData.wTcpPort, sizeof(word));
        UserList.push_back(UserData);
    }
    ReadString(strHashValue);
    //ReadBuffer(&nFileLength, sizeof(uint));
    ReadBuffer(&nTaskId, sizeof(int));
};

void CReGetUserIpPacket::InitData(word nResultCode, UserDataList& UserList,
                        string strHashValue, int nTaskId)
{
    this->UserList.assign(UserList.begin(), UserList.end());
    this->strHashValue = strHashValue;
    //this->nFileLength = nFileLength;
    this->nTaskId = nTaskId;
    CReTrackerPacket::InitData(nResultCode);
};


/////////////////////////////////////////////////////////////////////////////////
// CReNoopPacket - 无资源包

void CReNoopPacket::DoPack()
{
    CReTrackerPacket::DoPack();
    WriteBuffer(&nCount, sizeof(int));
    //WriteBuffer(&nTaskId, sizeof(int));
};

void CReNoopPacket::DoUnpack()
{
    CReTrackerPacket::DoUnpack();
    ReadBuffer(&nCount, sizeof(int));
    //ReadBuffer(&nTaskId, sizeof(int));
};

void CReNoopPacket::InitData(word nResultCode, int nCount)
{
    this->nCount = nCount;
    CReTrackerPacket::InitData(nResultCode);
};



//////////////////////////////////////////////////////////////////////////////////
// CReIntegralPacket-(Integral)积分包

void CReIntegralPacket::DoPack()
{
    CReTrackerPacket::DoPack();
    WriteBuffer(&nIntegral, sizeof(int));
};

void CReIntegralPacket::DoUnpack()
{
    CReTrackerPacket::DoUnpack();
    ReadBuffer(&nIntegral, sizeof(int));
};

void CReIntegralPacket::InitData(word nResultCode, int nIntegral)
{
    this->nIntegral = nIntegral;
    CReTrackerPacket::InitData(nResultCode);
};

