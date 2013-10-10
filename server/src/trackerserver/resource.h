#ifndef _RESOURCE_H_
#define _RESOURCE_H_
#include<stdio.h>
#include<stdlib.h>
#include "database.h"
#include <vector>
struct CUrlData
{
   string strUrl;
   string strRef;
   //string strHashValue;
} ;
typedef  vector<CUrlData> UrlDataList;

class CResource : public CDbOperations
{
public:
    CUrlData     sUrlData;
    UrlDataList  UrlData;
    
private:

    string strHashValue,strUrl,strurl;
    int nRet, nFileId,UrlCount,nUrlCount,downloadcount;

    //插入URL。
    bool CreateUrl(string& strUrl, string& strRef, int64 nFileLength);

    //是否已经存在。
   uint IsOn(string& strurl, int64 nFileLength);

   //查询（URL、FILELENGTH）
    string GetHashValue(string& strurl, int64 nFileLength);


   //查询（HASHVALUES、FILELENGTH）
   uint GetUrl(int64 nFileLength, string& strHashValue, UrlDataList&  UrlData);

  void UpdateDownloadCount(int64 nFileLength,string& strHashValue);
   //获得下一个FILEID
  uint GetNextFileId();

public:
    //利用用户名和密码查找用户的积分。
    int Find(char strUserName[10], char strUserPassword[10]);

    //为用户更新积分。
    bool  UpdateIntegral(char strUserName[10], char strUserPassword[10], int nIntegral);

   // 上传文件列表时获得文件FILEID
   uint GetFileId(int64 nFileLength ,string&  strHashValue);

   // 下载完成后获得文件FILEID
   uint Downloaded(string& strUrl, int64 nFileLength,string& strHashValue);

   //请求资源时获得文件URL FILEID

   uint GetResource(string& strUrl, string& strRef, int64 nFileLength,UrlDataList&  UrlData, string& HashValue);


};


#endif
