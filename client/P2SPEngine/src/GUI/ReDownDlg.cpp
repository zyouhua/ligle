//---------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "ReDownDlg.h"
//---------------------------------------------------------------------
#pragma link "RzBmpBtn"
#pragma resource "*.dfm"
TReDownDlg *ReDownDlg;
//--------------------------------------------------------------------- 
__fastcall TReDownDlg::TReDownDlg(TComponent* AOwner)
	: TForm(AOwner)
{
}
//---------------------------------------------------------------------

void __fastcall TReDownDlg::FormPaint(TObject *Sender)
{
  this->Color=RGB(240,241,243);  
}
//---------------------------------------------------------------------------



