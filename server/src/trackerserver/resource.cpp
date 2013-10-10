////////////////////////////////////////////////////////////////////////////////
#include "resource.h"

////////////////////////////////////////////////////////////////////////////////
bool CResource::CreateUrl(string& strUrl, string& strRef, int64 nFileLength)
{
   string strSql;
   downloadcount = 0;
    try
    {
        CDbQueryWrapper Query(GetDbManager()->GetTrackerDb()->CreateDbQuery());
        FormatString(strSql,
"insert into resource(UrlId, nFileId,strUrl,strRef,nFileLength,strHashValue, downloadcount)values(%d,%d , '%s', '%s', %d, '%s', %d)",
            NULL, 0, strUrl.c_str(), strRef.c_str(), nFileLength, "0", downloadcount); 
        Query->SetSql(strSql);
        Query->Execute();
    }
    catch (CException& e)
    {
        LogException(e);
        return false;
    }
    return true;
  };

uint CResource::IsOn( string& strUrl, int64 nFileLength)
  {
    string strSql;
    try
    {
        CDbQueryWrapper Query(GetDbManager()->GetTrackerDb()->CreateDbQuery());
        CDbDataSetWrapper DataSet;
        FormatString(strSql, "select count(*) UrlCount from resource where strUrl='%s' and nFileLength=%d ", strUrl.c_str(), nFileLength);
        Query->SetSql(strSql);  
         DataSet = Query->Query();
       if(DataSet->Next())
        {
            UrlCount = DataSet->GetFields(0)->AsInteger();
        } 
      }
    catch (CException& e)
    {
        LogException(e);
    } 
    return UrlCount ;
  };
string CResource::GetHashValue(string& strUrl, int64 nFileLength)
  {
    string strSql;
    try
    {
        CDbQueryWrapper Query(GetDbManager()->GetTrackerDb()->CreateDbQuery());
        CDbDataSetWrapper DataSet;

        FormatString(strSql, "SELECT strHashValue FROM resource where strUrl='%s' and nFileLength=%d ", strUrl.c_str(), nFileLength);
        Query->SetSql(strSql);
        DataSet = Query->Query();
        if(DataSet->Next())
         { 
              strHashValue = DataSet->GetFields(0)->AsString();
          }
      }
    catch (CException& e)
    {
        LogException(e);
    }
    return strHashValue ;
  };

uint CResource::GetUrl(int64 nFileLength, string& strHashValue, UrlDataList&  UrlData)
  {
     string strSql;
    try
    {
        CDbQueryWrapper Query(GetDbManager()->GetTrackerDb()->CreateDbQuery());
        CDbDataSetWrapper DataSet;  
       FormatString(strSql, "select strUrl,strRef from resource where strHashValue='%s' and nFileLength=%d ", strHashValue.c_str(),nFileLength);
        Query->SetSql(strSql);
        DataSet = Query->Query();       

      int i;
      UrlData.clear();
      while (DataSet->Next())
        {
            i = 0;
            sUrlData.strUrl = DataSet->GetFields(i++)->AsString();
            if(strurl == sUrlData.strUrl )  continue ;
            sUrlData.strRef = DataSet->GetFields(i++)->AsString();
            UrlData.push_back(sUrlData);

       }

     }
    catch (CException& e)
    {
        LogException(e);
    }
     return 0 ; 
  };
void CResource::UpdateDownloadCount(int64 nFileLength, string& strHashValue)
  {
    string strSql;
    try
    {  
        CDbQueryWrapper Query(GetDbManager()->GetTrackerDb()->CreateDbQuery());
        CDbDataSetWrapper DataSet;

       FormatString(strSql, "update resource set downloadcount =  downloadcount + 1 where nFileLength=%d and strHashValue='%s'", nFileLength, strHashValue.c_str());
       Query->SetSql(strSql);
       Query->Execute();
      }
    catch (CException& e)
    {
       LogException(e);
    }
  };
uint CResource::GetNextFileId()
  {

    string strSql;
    try
    {
        CDbQueryWrapper Query(GetDbManager()->GetTrackerDb()->CreateDbQuery());
        CDbDataSetWrapper DataSet;

       strSql = "update id set nFileId = nFileId + 1 ;";
       Query->SetSql(strSql);
       Query->Execute();

       strSql = "SELECT nFileId FROM id ;";
       Query->SetSql(strSql);
       DataSet = Query->Query();
        if (DataSet->Next())
        {

            nRet  = DataSet->GetFields(0)->AsInteger();
        }
        else
        {
            Logger.Write("IdTable table 不能正确访问！");
        }
      }
    catch (CException& e)
    {
       LogException(e);
    }
  return  nRet ;
  };

uint CResource::GetFileId(int64 nFileLength, string& strHashValue)
  {
    string strSql;
    try
    {
        CDbQueryWrapper Query(GetDbManager()->GetTrackerDb()->CreateDbQuery());
        CDbDataSetWrapper DataSet;
FormatString(strSql, "SELECT  nFileId nFileId FROM resource where nFileLength=%d  and strHashValue='%s'", nFileLength,strHashValue.c_str());
        Query->SetSql(strSql);
        DataSet = Query->Query();
     if (DataSet->Next())
        {
           nFileId = DataSet->GetFields(0)->AsInteger();
        } 
      }

    catch (CException& e)
    {
        LogException(e);
    }
    return nFileId ;
  };

uint CResource::Downloaded(string& strUrl, int64 nFileLength, string& strHashValue)
  {
    string strSql;
    try
    {  
        int id;   
        CDbQueryWrapper Query(GetDbManager()->GetTrackerDb()->CreateDbQuery());
        CDbDataSetWrapper DataSet;

       FormatString(strSql, "update resource set strHashValue = '%s' where strUrl = '%s' and nFileLength = %d",
       strHashValue.c_str(),strUrl.c_str(), nFileLength);
       Query->SetSql(strSql);
       Query->Execute();
       CResource::UpdateDownloadCount(nFileLength, strHashValue);
      id = GetFileId(nFileLength, strHashValue);
      if(id != 0 )
          {
             FormatString(strSql, "update resource set nFileId = %d  where strUrl = '%s' and nFileLength = %d",
             nFileId ,strUrl.c_str(), nFileLength);
             Query->SetSql(strSql);
             Query->Execute();
             return nFileId;
          }
          
      else
         {   
            nRet = GetNextFileId();     
            FormatString(strSql, "update resource set nFileId = %d where strUrl = '%s' and nFileLength = %d",
            nRet, strUrl.c_str(), nFileLength);
            Query->SetSql(strSql);
            Query->Execute();
            return nRet;  
           } 

    }
    catch (CException& e)
    {
        LogException(e);
    }
  };

uint CResource::GetResource(string& strUrl, string& strRef,int64 nFileLength, UrlDataList&  UrlData, string& HashValue)
  { nUrlCount = 0;
    strurl = strUrl; 
    nUrlCount =  IsOn(strUrl, nFileLength) ; 
    if( nUrlCount == 0 )
      {  
          CResource::CreateUrl(strUrl, strRef , nFileLength);
          return 0;
      }
      strHashValue = GetHashValue(strUrl, nFileLength);
      if (strHashValue != "\0")
         {
            GetUrl(nFileLength,strHashValue, UrlData);
            nFileId = GetFileId(nFileLength,strHashValue) ;
            HashValue = strHashValue ;
            return  nFileId;
         }
      return 0;
  };

////////////////////////////////////////////////////////////////////////////////
int  CResource::Find(char strUserName[10], char strUserPassword[10])
   {
          string strSql;
          int integral= -1;
    try
    {
        CDbQueryWrapper Query(GetDbManager()->GetTrackerDb()->CreateDbQuery());
        CDbDataSetWrapper DataSet;
        FormatString(strSql, "SELECT integral FROM customer where strUserName='%s' and strUserPassword='%s' ", strUserName, strUserPassword);
        Query->SetSql(strSql);  
        DataSet = Query->Query();
        if(DataSet->Next())
         {  
              integral = DataSet->GetFields(0)->AsInteger();
              if(!(integral >=0) )  return -1;
          }
         return integral;
      }
    catch (CException& e)
    {
        LogException(e);
    }
    return integral ;
   } ;

////////////////////////////////////////////////////////////////////////////////
bool  CResource::UpdateIntegral(char strUserName[10], char strUserPassword[10], int nIntegral)
  {
  string strSql;
    try
    {
        CDbQueryWrapper Query(GetDbManager()->GetTrackerDb()->CreateDbQuery());
        CDbDataSetWrapper DataSet;

       FormatString(strSql, "update customer set integral =  integral + nIntegral where strUserName='%s' and strUserPassword='%s'", strUserName, strUserPassword);
       Query->SetSql(strSql);
       Query->Execute();
      }
    catch (CException& e)
    {
       LogException(e);
    }
   return true; 
  };

