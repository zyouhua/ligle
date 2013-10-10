//---------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "Alert.h"
//---------------------------------------------------------------------
#pragma link "RzBmpBtn"
#pragma resource "*.dfm"
TAlertDlg *AlertDlg;
//--------------------------------------------------------------------- 
__fastcall TAlertDlg::TAlertDlg(TComponent* AOwner)
	: TForm(AOwner)
{
  IsDelete=false;
}
//---------------------------------------------------------------------
void __fastcall TAlertDlg::OKBtnClick(TObject *Sender)
{
  ModalResult = mrOk;
  if(CheckBox1->Checked)
    IsDelete=true;  
}
//---------------------------------------------------------------------------

void __fastcall TAlertDlg::CancelBtnClick(TObject *Sender)
{
  ModalResult = mrCancel;  
}
//---------------------------------------------------------------------------

void __fastcall TAlertDlg::FormCreate(TObject *Sender)
{
  this->CheckBox1->Checked=false;  
}
//---------------------------------------------------------------------------

void __fastcall TAlertDlg::FormPaint(TObject *Sender)
{
  this->Color=RGB(240,241,243);  
}
//---------------------------------------------------------------------------

