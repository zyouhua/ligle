//----------------------------------------------------------------------------
#ifndef NewTaskH
#define NewTaskH
//----------------------------------------------------------------------------
#include <vcl\System.hpp>
#include <vcl\Windows.hpp>
#include <vcl\SysUtils.hpp>
#include <vcl\Classes.hpp>
#include <vcl\Graphics.hpp>
#include <vcl\StdCtrls.hpp>
#include <vcl\Forms.hpp>
#include <vcl\Controls.hpp>
#include <vcl\Buttons.hpp>
#include <vcl\ExtCtrls.hpp>
#include <Dialogs.hpp>
#include <IdGlobal.hpp>
#include "RzBmpBtn.hpp"
//#include "RzBmpBtn.hpp"
//----------------------------------------------------------------------------
class TNewTaskDlg : public TForm
{
__published:
  TLabel *Label1;
  TLabel *Label2;
  TLabel *Label3;
  TButton *OKBtn;
  TEdit *m_Url;
  TComboBox *m_Path;
  TEdit *m_FileName;
  TLabel *Label4;
  TLabel *Label5;
  TEdit *m_Ref;
  TRzBmpButton *Scan;
  TRzBmpButton *OK;
  TRzBmpButton *CancelBtn;
  void __fastcall OKBtnClick(TObject *Sender);
  void __fastcall CancelBtnClick(TObject *Sender);
  void __fastcall m_UrlChange(TObject *Sender);
  void __fastcall ScanClick(TObject *Sender);
  void __fastcall OKClick(TObject *Sender);
  void __fastcall FormCreate(TObject *Sender);
  void __fastcall m_PathSelect(TObject *Sender);
  void __fastcall FormPaint(TObject *Sender);
private:
public:
  AnsiString strFileName;
	virtual __fastcall TNewTaskDlg(TComponent* AOwner);
//  AnsiString AutoRenameFileName(AnsiString& strPath, AnsiString& strFileName);
  AnsiString ValidateFileName(AnsiString& FileName);
  AnsiString AutoRenameExtName(AnsiString& OldName, AnsiString& Url);
};
//----------------------------------------------------------------------------
//extern PACKAGE TNewTaskDlg *NewTaskDlg;
//----------------------------------------------------------------------------
#endif    
