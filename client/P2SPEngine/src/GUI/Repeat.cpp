//---------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "Repeat.h"
//--------------------------------------------------------------------- 
#pragma link "RzBmpBtn"
#pragma resource "*.dfm"
TOKBottomDlg *OKBottomDlg;
//---------------------------------------------------------------------
__fastcall TOKBottomDlg::TOKBottomDlg(TComponent* AOwner)
	: TForm(AOwner)
{
}
//---------------------------------------------------------------------

void __fastcall TOKBottomDlg::FormPaint(TObject *Sender)
{
  this->Color=RGB(240,241,243);  
}
//---------------------------------------------------------------------------



