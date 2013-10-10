//---------------------------------------------------------------------------
#ifndef UpdateFileH
#define UpdateFileH

#include <string>
#include <list>
#include <IdHTTP.hpp>
#include <Forms.hpp>

using namespace std;

//---------------------------------------------------------------------------
extern AnsiString DownLoadDir;
const AnsiString ExName=".bak";

class CUpdateFilePool;

class CUpdateFile
{
friend class CUpdateFilePool;
private:
  AnsiString m_FileName;
  AnsiString m_InstallDir;
  AnsiString m_Url;
  bool m_Complete;
  TIdHTTP* m_IdHttp;
public:
  CUpdateFile(AnsiString&,AnsiString&,AnsiString&,bool,TForm*);
  ~CUpdateFile();
private:
  void Undo();
public:
  void Get();
  void Update();
};

//---------------------------------------------------------------------------

class CUpdateFilePool
{
private:
  list<CUpdateFile*> m_FileList;
public:
  CUpdateFilePool(){}
  ~CUpdateFilePool(){}
public:
  void AddFile(AnsiString&,AnsiString&,AnsiString&,bool,TForm*);
  void RemoveFile(CUpdateFile*);
  void GetAllFile();
  void UpdateAllFile();
  void Clear();
};
extern CUpdateFilePool m_UpdateFileList;
#endif
