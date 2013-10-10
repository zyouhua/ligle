//----------------------------------------------------------------------------
#ifndef AlertH
#define AlertH
//----------------------------------------------------------------------------
#include <vcl\ExtCtrls.hpp>
#include <vcl\Buttons.hpp>
#include <vcl\StdCtrls.hpp>
#include <vcl\Controls.hpp>
#include <vcl\Forms.hpp>
#include <vcl\Graphics.hpp>
#include <vcl\Classes.hpp>
#include <vcl\SysUtils.hpp>
#include <vcl\Windows.hpp>
#include <vcl\System.hpp>
#include "RzBmpBtn.hpp"
//----------------------------------------------------------------------------
class TAlertDlg : public TForm
{
__published:
  TLabel *Label1;
  TCheckBox *CheckBox1;
  TRzBmpButton *OKBtn;
  TRzBmpButton *CancelBtn;
  void __fastcall OKBtnClick(TObject *Sender);
  void __fastcall CancelBtnClick(TObject *Sender);
  void __fastcall FormCreate(TObject *Sender);
  void __fastcall FormPaint(TObject *Sender);
private:
public:
  bool IsDelete;
	virtual __fastcall TAlertDlg(TComponent* AOwner);
};
//----------------------------------------------------------------------------
extern PACKAGE TAlertDlg *AlertDlg;
//----------------------------------------------------------------------------
#endif    
