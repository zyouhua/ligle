//----------------------------------------------------------------------------
#ifndef ReDownDlgH
#define ReDownDlgH
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
class TReDownDlg : public TForm
{
__published:
  TRzBmpButton *OK;
  TRzBmpButton *Cancel;
  TLabel *Label1;
  void __fastcall FormPaint(TObject *Sender);
private:
public:
	virtual __fastcall TReDownDlg(TComponent* AOwner);
};
//----------------------------------------------------------------------------
extern PACKAGE TReDownDlg *ReDownDlg;
//----------------------------------------------------------------------------
#endif    
