//---------------------------------------------------------------------------

#ifndef AutoUpdate_MainH
#define AutoUpdate_MainH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <IdBaseComponent.hpp>
#include <IdComponent.hpp>
#include <IdHTTP.hpp>
#include <IdTCPClient.hpp>
#include <IdTCPConnection.hpp>
//---------------------------------------------------------------------------
AnsiString DownLoadDir;
class TForm1 : public TForm
{
__published:	// IDE-managed Components
  TLabel *Label1;
  TButton *Button1;
  TIdHTTP *IdHTTP1;
  void __fastcall FormCreate(TObject *Sender);
  void __fastcall Button1Click(TObject *Sender);
private:
  bool Init();
  void GetFileList();
  void GetFile();
  bool Complete();
  void UpdateFile();
  void GetUpdateFileList();
  bool IsNeedUpdate();
  bool Check();	// User declarations
public:		// User declarations
  __fastcall TForm1(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif
