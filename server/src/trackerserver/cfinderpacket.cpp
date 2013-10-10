/////////////////////////////////////////////////////////////////////////////////
//trackerPacket.cpp

#include "cfinderpacket.h"

/////////////////////////////////////////////////////////////////////////////////
//CLogoutPacket - 下线包

void CLogoutPacket::DoPack()
{
    CFinderPacket::DoPack();
};

void CLogoutPacket::DoUnpack()
{
    CFinderPacket::DoUnpack();
};

void CLogoutPacket::InitData()
{
    Pack();
};

////////////////////////////////////////////////////////////////////////////////
//CKeepAlivePacket - 保活包

void CKeepAlivePacket::DoPack()
{
    CFinderPacket::DoPack();
};

void CKeepAlivePacket::DoUnpack()
{
    CFinderPacket::DoUnpack();
};

void CKeepAlivePacket::InitData()
{
    Pack();
};

////////////////////////////////////////////////////////////////////////////////
//CCKeepAlivePacket - 积分包

void CIntegralPacket::DoPack()
{
    CFinderPacket::DoPack();
};

void CIntegralPacket::DoUnpack()
{
    CFinderPacket::DoUnpack();
};

void CIntegralPacket::InitData()
{
    Pack();
};

////////////////////////////////////////////////////////////////////////////////
//CUpLoadFileListPacket - 上传资源列表包

void CUploadFileListPacket::DoPack()
{
    CFinderPacket::DoPack();
    WriteBuffer(&wFileCount, sizeof(word));
    for(int i=0; i<wFileCount; i++)
    {
        WriteBuffer(&FileList.at(i).nFileLength, sizeof(int64));
        WriteString(FileList.at(i).strHashValue);
    }
};

void CUploadFileListPacket::DoUnpack()
{
    CFinderPacket::DoUnpack();
    ReadBuffer(&wFileCount, sizeof(word));
    ////定义了一个结构体对象,将读取的数据与结构体对应,再放入容器中
    CUploadFile UploadFile;
    for(int i=0; i<wFileCount; i++)
    {
        ReadBuffer(&UploadFile.nFileLength, sizeof(int64));
        ReadString(UploadFile.strHashValue);
        FileList.push_back(UploadFile);
    }
};

void CUploadFileListPacket::InitData(word wFileCount, UploadFileList& FileList)
{
    this->wFileCount = wFileCount;
    this->FileList.assign(FileList.begin(), FileList.end());
    Pack();
};

////////////////////////////////////////////////////////////////////////////////
//CDownloadFileCompletePacket - 下载完成包

void CDownloadFileCompletePacket::DoPack()
{
    CFinderPacket::DoPack();
    WriteString(strUrl);
    WriteBuffer(&nFileLength, sizeof(int64));
    WriteString(strHashValue);
};

void CDownloadFileCompletePacket::DoUnpack()
{
    CFinderPacket::DoUnpack();
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
    CFinderPacket::DoPack();
    WriteString(strRef);
    WriteString(strUrl);
    WriteBuffer(&nFileLength, sizeof(int64));
    WriteBuffer(&nTaskId, sizeof(int));
};

void CRequestResourcePacket::DoUnpack()
{
    CFinderPacket::DoUnpack();
    ReadString(strRef);
    ReadString(strUrl);
    ReadBuffer(&nFileLength, sizeof(int64));
    ReadBuffer(&nTaskId, sizeof(int));
};

void CRequestResourcePacket::InitData(string strUrl, string strRef, int64 nFileLength,int nTaskId)
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
    CReFinderPacket::DoPack();
};

void CRequestFilePacket::DoUnpack()
{
    CReFinderPacket::DoUnpack();
};

void CRequestFilePacket::InitData(word nResultCode)
{
    CReFinderPacket::InitData(nResultCode);
};
////////////////////////////////////////////////////////////////////////////////
//CReOnPacket - 通用应答包

void CReOnPacket::DoPack()
{
    CReFinderPacket::DoPack();
};

void CReOnPacket::DoUnpack()
{
    CReFinderPacket::DoUnpack();
};

void CReOnPacket::InitData(word nResultCode)
{
    CReFinderPacket::InitData(nResultCode);
};
////////////////////////////////////////////////////////////////////////////////
// CReGetResoursesPacket - 回给客户端一个URL

void CReGetUrlPacket::DoPack()
{
    CReFinderPacket::DoPack();
    //打一个URL
    WriteString(strReference);
    WriteString(strUrl);
    WriteString(strHashValue);
    WriteBuffer(&nTaskId, sizeof(int));
};

void CReGetUrlPacket::DoUnpack()
{
    CReFinderPacket::DoUnpack();
    //读取一个URL
    ReadString(strReference);
    ReadString(strUrl);
    ReadString(strHashValue);
    ReadBuffer(&nTaskId, sizeof(int));
};

void CReGetUrlPacket::InitData(word nResultCode, string strReference, string strUrl,
                               string strHashValue,int nTaskId)
{
    this->strReference = strReference;
    this->strUrl = strUrl;
    this->strHashValue = strHashValue;
    this->nTaskId = nTaskId;
    CReFinderPacket::InitData(nResultCode);
};



/////////////////////////////////////////////////////////////////////////////////
// CReGetUserIpPacket - 回给客户端一批IP
void CReGetUserIpPacket::DoPack()
{
    CReFinderPacket::DoPack();
    //打一批用户IP
    int Number = UserList.size();
    WriteBuffer(&Number, sizeof(int));
    for(int i=0; i<Number; i++)
    {
        WriteBuffer(&UserList.at(i).nUserIp, sizeof(uint));
        WriteBuffer(&UserList.at(i).wTcpPort, sizeof(word));

    }
    WriteString(strHashValue);
    WriteBuffer(&nTaskId, sizeof(int));
};

void CReGetUserIpPacket::DoUnpack()
{
    CReFinderPacket::DoUnpack();
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
    ReadBuffer(&nTaskId, sizeof(int));
};

void CReGetUserIpPacket::InitData(word nResultCode, UserDataList& UserList, 
                                 string strHashValue, int nTaskId)
{
    this->UserList.assign(UserList.begin(), UserList.end());
    this->strHashValue = strHashValue;
    this->nTaskId = nTaskId;
    CReFinderPacket::InitData(nResultCode);
};


/////////////////////////////////////////////////////////////////////////////////
// CReNoopPacket - 无资源包

void CReNoopPacket::DoPack()
{
    CReFinderPacket::DoPack();
    WriteBuffer(&nCount, sizeof(int));
};

void CReNoopPacket::DoUnpack()
{
    CReFinderPacket::DoUnpack();
    ReadBuffer(&nCount, sizeof(int));
};

void CReNoopPacket::InitData(word nResultCode, int nCount)
{
    this->nCount = nCount;
    CReFinderPacket::InitData(nResultCode);
};



/////////////////////////////////////////////////////////////////////////////////
// CReIntegralPacket - 积分包

void CReIntegralPacket::DoPack()
{
    CReFinderPacket::DoPack();
    WriteBuffer(&nIntegral, sizeof(int));
};

void CReIntegralPacket::DoUnpack()
{
    CReFinderPacket::DoUnpack();
    ReadBuffer(&nIntegral, sizeof(int));
};

void CReIntegralPacket::InitData(word nResultCode, int nIntegral)
{
    this->nIntegral = nIntegral;
    CReFinderPacket::InitData(nResultCode);
};
