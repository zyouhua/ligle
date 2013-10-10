//---------------------------------------------------------------------------

#include <vcl.h>
#include <Clipbrd.hpp>
#pragma hdrstop

#include "MainForm.h"
#include "NewTask.h"
#include "Alert.h"
#include "Repeat.h"
#include "ReDownDlg.h"
#include "ReDownDlg.h"

#include "app_event.h"
#include "core_task.h"
#include "core_engine.h"
#include "core_metafile.h"


const double K=1024;
const double M=1024*1024;
const double G=1024*1024*1024;
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "trayicon"
#pragma resource "*.dfm"
TMainFrm *MainFrm;
//---------------------------------------------------------------------------
__fastcall TMainFrm::TMainFrm(TComponent* Owner)
  : TForm(Owner)
{
  CanExit=false;
  TrayShow=false;
}
//---------------------------------------------------------------------------
bool TMainFrm::SerachTask(int TaskId,int& Index,TListView* ListView)
{
  bool bResult=false;
  for(int i=0;i<ListView->Items->Count;i++)
  {
    if(TaskId!=ListView->Items->Item[i]->ImageIndex)
      Index++;
    else
    {
      bResult=true;
      break;
    }
  }
  return bResult;
}
void __fastcall TMainFrm::Timer1Timer(TObject *Sender)
{
//................
  
  CAppEvent* pEvent=NULL;

  if(!AppEventPool.GetEvent(pEvent)) return;

  switch(pEvent->Action)
  {
  case EVENT_INSERT:
    DoInsert(pEvent->TaskID);
    break;
  case EVENT_UPDATE:
    DoUpDate(pEvent->TaskID);
    break;
  case EVENT_COMPLETE:
    DoComplete(pEvent->TaskID);
    break;
  case EVENT_DELETE:
    DoDelete(pEvent->TaskID);
    break;
  default:break;
  }
}
//---------------------------------------------------------------------------


void TMainFrm::DoUpDate(int TaskID)
{
  //TODO: Add your source code here
  CTask* pTask=NULL;
  if(!TaskPool.GetTask(TaskID,pTask)) return;

  int nIndex=0;
  if(!this->SerachTask(TaskID,nIndex,DownLoadView)) return;

  TListItem* Item;
  Item=DownLoadView->Items->Item[nIndex];

  if(TASKSTOPED==pTask->m_State)  //状态
  {
    Item->StateIndex=1;
    Item->SubItems->Strings[3]="";
  }
  else
  {
    Item->StateIndex=0;
    Item->SubItems->Strings[3]=Speed(pTask->m_Speed);
  }
  Item->SubItems->Strings[0]=GetFileName(pTask->m_FileName.c_str());
  Item->SubItems->Strings[1]=Size(pTask->m_nLength);
  Item->SubItems->Strings[2]=Rate(pTask->m_Downloaded,pTask->m_nLength);
  Item->SubItems->Strings[4]=(AnsiString)pTask->m_nChannels;
  Item->SubItems->Strings[5]=GetReMainTime(pTask->m_Downloaded,pTask->m_nLength,pTask->m_Speed);
  Item->SubItems->Strings[6]=ErrorInfomation[pTask->m_StopCode];
}
//---------------------------------------------------------------------------
void TMainFrm::DoComplete(int TaskID)
{
  int nIndex=0;
  if(!this->SerachTask(TaskID,nIndex,DownLoadView)) return;


  TListItem* Item;
  Item=CompleteView->Items->Add();

  Item->ImageIndex=DownLoadView->Items->Item[nIndex]->ImageIndex;  //任务ID
  
  Item->StateIndex=3;
  Item->SubItems->Add(DownLoadView->Items->Item[nIndex]->SubItems->Strings[0]);
  Item->SubItems->Add(DownLoadView->Items->Item[nIndex]->SubItems->Strings[1]);


  DownLoadView->Items->Item[nIndex]->Delete();
}
//---------------------------------------------------------------------------
void TMainFrm::DoInsert(int  TaskID)
{
  //TODO: Add your source code here
  CTask* pTask=NULL;
  if(!TaskPool.GetTask(TaskID,pTask)) return;

  TListItem* Item;
  Item=DownLoadView->Items->Add();


  Item->ImageIndex=TaskID;

  if(TASKSTOPED==pTask->m_State)    //状态
  {
    Item->StateIndex=1;
  }
  else
  {
    Item->StateIndex=0;
  }
  Item->SubItems->Add(GetFileName(pTask->m_FileName.c_str()));      //文件名称
  Item->SubItems->Add(Size(pTask->m_nLength));         //大小
  Item->SubItems->Add(Rate(pTask->m_Downloaded,pTask->m_nLength)); //进度
  Item->SubItems->Add("");  //速度
  Item->SubItems->Add(""); //连接数
  Item->SubItems->Add(""); //剩余时间
  Item->SubItems->Add("");  //错误信息
  Item->SubItems->Add(""); //用时

  DownLoadPanel->Visible=true;
  DownLoadView->Visible=true;
  CompletePanel->Visible=false;
  CompleteView->Visible=false;
  RecyclePanel->Visible=false;
  RecycleView->Visible=false;
}
//---------------------------------------------------------------------------
void TMainFrm::DoDelete(int TaskID)
{
  int nIndex=0;
  if(!SerachTask(TaskID,nIndex,DownLoadView)) return;

  TListItem* Item;
  Item=RecycleView->Items->Add();

  Item->ImageIndex=TaskID;//任务ID
  Item->StateIndex=2;
  Item->SubItems->Add(DownLoadView->Items->Item[nIndex]->SubItems->Strings[0]);
  Item->SubItems->Add(DownLoadView->Items->Item[nIndex]->SubItems->Strings[1]);

  DownLoadView->Items->Item[nIndex]->Delete();

}
//---------------------------------------------------------------------------
AnsiString TMainFrm::Rate(double DownLoad,double Length)
{
  AnsiString strRate="";
  double dRate=(Length>0) ? (DownLoad*100)/Length : 0;
  if(dRate>=99.9)
    dRate=99.9;
  strRate=Sysutils::FloatToStr(dRate);
  if(Length>0)
  {
    int nLen=strRate.SubString(0,strRate.Pos(".")-1).Length();
    strRate=strRate.FloatToStrF(dRate,ffFixed,nLen+1,1);
  }
  return strRate+"%";
}
//---------------------------------------------------------------------------
AnsiString TMainFrm::Speed(int Speed)
{
   AnsiString strRate="",strUnit=" B/s";
   float fRate=0l;
   if(Speed/(float)K>1)
   {
      if((float)Speed/(float)M>1)
      {
        fRate=(float)Speed/(float)M;
        strUnit=" M/S";
      }
      else
      {
        fRate=(float)Speed/(float)K;
        strUnit=" K/S";
      }
   }
   else
    fRate=Speed;
   strRate=Sysutils::FloatToStr(fRate);
   int nPos=strRate.Pos(".");
   if(nPos>0)
   {
    int nLen=strRate.SubString(0,nPos-1).Length();
    strRate=strRate.FloatToStrF(fRate,ffFixed,nLen+1,1);
   }
   return strRate+strUnit;
}
//---------------------------------------------------------------------------
AnsiString TMainFrm::Size(double Length)
{
  AnsiString Size,Unit=" B";
  double dSize=0;
  if(Length/K>1)
  {
    if(Length/M>1)
    {
      if(Length/G>1)
      {
        dSize=Length/G;
        Unit=" G";
      }
      else
      {
        dSize=Length/M;
        Unit=" M";
      }
    }
    else
    {
      dSize=Length/K;
      Unit=" K";
    }
  }
  else
    dSize=Length;
  Size=Sysutils::FloatToStr(dSize);
  int nPos=Size.Pos(".");
  if(nPos>0)
  {
    int nLen=Size.SubString(0,nPos-1).Length();
    Size=Size.FloatToStrF(dSize,ffFixed,nLen+1,1);
  }
  return Size+Unit;
}
//---------------------------------------------------------------------------
void __fastcall TMainFrm::D_StartTaskClick(TObject *Sender)
{
 TListItem *Item;
 CTask* pTask=NULL;
 int nTaskId;
 Item=DownLoadView->Selected;

 if(!Item) return;
 nTaskId=Item->ImageIndex;

 if(!TaskPool.GetTask(nTaskId,pTask)) return;
 pTask->Start();
}
//---------------------------------------------------------------------------

void __fastcall TMainFrm::D_StopTaskClick(TObject *Sender)
{
  TListItem *Item;
  CTask* pTask=NULL;
  int nTaskId;
  Item=DownLoadView->Selected;

  if(!Item) return;
  nTaskId=Item->ImageIndex;

  if(!TaskPool.GetTask(nTaskId,pTask)) return;
  pTask->Stop(0);
}
//---------------------------------------------------------------------------


void __fastcall TMainFrm::DownLoadViewMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
  TPoint Point;
  Point.x=X;
  Point.y=Y;
  Point=DownLoadView->ClientToScreen(Point);
  TListItem *Item=DownLoadView->GetItemAt(X,Y);
  if(mbRight==Button)
  {
    if(DownLoadView->SelCount>1)
       SetDownLoadPopup(false,false,false,true,true,true);
    else if(DownLoadView->SelCount==1)
    {
      if(!Item) return;
      if(Item->StateIndex==0)
        SetDownLoadPopup(false,true,true,false,true,true);
      else
        SetDownLoadPopup(true,false,true,true,false,true);
    }
    else
    {
      if(DownLoadView->Items->Count>0)
        SetDownLoadPopup(false,false,false,true,true,true);
      else
        SetDownLoadPopup(false,false,false,false,false,false);
    }
    DownLoadPopupMenu->Popup(Point.x,Point.y);
  }
  else if(mbLeft==Button)
  {
    if(!Item)
      SetToolBar(false,false,false,false,false);
    else
    {
      if(Item->StateIndex==0)
        SetToolBar(false,true,true,false,true);
      else
        SetToolBar(true,false,true,false,true);
    }
  }
  if(Item)
    ShowTaskInfo(Item->ImageIndex);
}
//---------------------------------------------------------------------------

void __fastcall TMainFrm::CompleteViewMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
  TPoint Point;
  Point.x=X;
  Point.y=Y;
  Point=CompleteView->ClientToScreen(Point);
  TListItem *Item=CompleteView->GetItemAt(X,Y);
  if(mbRight==Button)
  {
    if(CompleteView->SelCount>1)
      SetCompletePopup(false,false,true,true);
    else if(CompleteView->SelCount==1)
    {
      if(!Item) return;
      SetCompletePopup(true,true,true,true);
    }
    else
      SetCompletePopup(false,false,true,true);
    CompletePopupMenu->Popup(Point.x,Point.y);
  }
  else if(mbLeft==Button)
  {
    if(!Item)
      SetToolBar(false,false,false,false,false);
    else
      SetToolBar(false,false,true,true,true);
  }
  if(Item)
    ShowTaskInfo(Item->ImageIndex);
}
//---------------------------------------------------------------------------

void __fastcall TMainFrm::D_DelTaskClick(TObject *Sender)
{
  TListItem *Item;
  CTask* pTask=NULL;
  int nTaskId;
  Item=DownLoadView->Selected;

  if(!Item) return;
  nTaskId=Item->ImageIndex;

  if(!TaskPool.GetTask(nTaskId,pTask)) return;
  pTask->Delete();
}
//---------------------------------------------------------------------------

void __fastcall TMainFrm::C_DelTaskClick(TObject *Sender)
{

  CMetaData MetaData;
  TListItem *Item;
  int nTaskId;
  Item=CompleteView->Selected;

  if(!Item) return;
  nTaskId=Item->ImageIndex;

  if(MetaFile.Load(nTaskId,&MetaData))
  {
    MetaData.State=2;
    MetaFile.UpDate(&MetaData);
  }

  int nIndex=0;
  if(!SerachTask(nTaskId,nIndex,CompleteView)) return;

  Item=RecycleView->Items->Add();
  Item->ImageIndex=CompleteView->Items->Item[nIndex]->ImageIndex;
  Item->StateIndex=2;
  Item->SubItems->Add(CompleteView->Items->Item[nIndex]->SubItems->Strings[0]);
  Item->SubItems->Add(CompleteView->Items->Item[nIndex]->SubItems->Strings[1]);

  CompleteView->Items->Item[nIndex]->Delete();

}
//---------------------------------------------------------------------------



void TMainFrm::InitView(TListView* ListView, int nType)
{
  //TODO: Add your source code here
  CMetaList List;
  CMetaList::iterator iter;
  
  if(!MetaFile.Load(List,nType))return;

  for(iter=List.begin();iter!=List.end();iter++)
  {
    TListItem* Item;
    Item=ListView->Items->Add();
    if(nType==1)
      Item->StateIndex=3;
    else
      Item->StateIndex=2;
    Item->ImageIndex=(*iter)->Index;
    Item->SubItems->Add(GetFileName((*iter)->FileName));            //文件名
    Item->SubItems->Add(Size((*iter)->FileSize));//文件大小
    delete  (*iter);
  }
}
void __fastcall TMainFrm::FormCreate(TObject *Sender)
{
  InitView(CompleteView,1);
  InitView(RecycleView,2);
  SetProp(this->Handle,APP_NAME,this->Handle);//唯一实例的标记
  LeftSplitter->Color=RGB(240,241,243);
  vSplitter->Color=RGB(240,241,243);
  MainStatusBar->Color=RGB(240,241,243);
//  AnsiString path="";
  TRegistry *reg=new TRegistry(KEY_READ);
  try
  {
    reg->RootKey=HKEY_CURRENT_USER;
    reg->OpenKey("Software\\Ligle",false);
    bmppath=reg->ReadString("Ligle");
  }
  catch(...)
  {
    bmppath="C:\\";
  }
  delete reg;

  ZeroMemory(&icon,sizeof(NOTIFYICONDATA));
  icon.cbSize=sizeof(NOTIFYICONDATA);
  icon.hWnd=this->Handle;
  icon.hIcon=Application->Icon->Handle;
  icon.uID=10000;
  icon.uFlags=NIF_ICON|NIF_MESSAGE|NIF_TIP;
  icon.uCallbackMessage=WM_NOTIFYICON;
  strcpy(icon.szTip,"骊狗下载");

  Shell_NotifyIcon(NIM_ADD,&icon); 
}
//---------------------------------------------------------------------------

void __fastcall TMainFrm::ScanTreeMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
  TTreeNode *rNode;
  rNode = ScanTree->GetNodeAt(X, Y);

  if((Button == mbLeft)&&(rNode))
  {
    if((rNode->ImageIndex) == 5)  //已下载
    {
      CompletePanel->Visible=true;
      CompleteView->Visible=true;

      DownLoadPanel->Visible=false;
      DownLoadView->Visible=false;

      RecyclePanel->Visible=false;
      RecycleView->Visible=false;
//      SetToolBar(false,false,false,false,false);
    }
    else  if((rNode->ImageIndex) == 6) //垃圾箱
    {
      RecyclePanel->Visible=true;
      RecycleView->Visible=true;
      
      CompletePanel->Visible=false;
      CompleteView->Visible=false;

      DownLoadPanel->Visible=false;
      DownLoadView->Visible=false;
//      SetToolBar(false,false,false,false,false);
    }
    else if((rNode->ImageIndex) == 4) //正在下载
    {
      DownLoadPanel->Visible=true;
      DownLoadView->Visible=true;

      CompletePanel->Visible=false;
      CompleteView->Visible=false;

      RecyclePanel->Visible=false;
      RecycleView->Visible=false;
    }
    SetToolBar(false,false,false,false,false);
  }
  if(InfoBox->Items->Count>0)
    InfoBox->Clear();
}
//---------------------------------------------------------------------------
//描述：自定义消息相应的处理函数
//      新建任务和重复任务处理函数
//---------------------------------------------------------------------------
void __fastcall TMainFrm::CNewTask(TMessage& msg)
{
  /*
  char *ch=(char*)msg.LParam;
  char c[500];
  memset(c,0,500);
  strcpy(c,ch);
  ShowMessage(ch);
  string str=ch;
  ShowMessage(str.c_str());
  string Ref,Url;
  int n;
  n=str.find(" ",0);
  if(n>0)
  {
    Url=str.substr(0,n);
    Ref=str.substr(n+1,str.length()-n+1);
    TNewTaskDlg *NewTaskDlg=new TNewTaskDlg(MainFrm);
    NewTaskDlg->m_Ref->Text=Ref.c_str();
    NewTaskDlg->m_Url->Text=Url.c_str();
    NewTaskDlg->ShowModal();
    SetForegroundWindow(this->Handle);
    delete NewTaskDlg;
  }
  delete ch;
  */
  TClipboard* clip=Clipboard();
  string str=clip->AsText.c_str();
  string Ref,Url;
  int n;
  n=str.find(" ",0);
  if(n>0)
  {
    Url=str.substr(0,n);
    Ref=str.substr(n+1,str.length()-n+1);
    TNewTaskDlg *NewTaskDlg=new TNewTaskDlg(MainFrm);
    NewTaskDlg->m_Ref->Text=Ref.c_str();
    NewTaskDlg->m_Url->Text=Url.c_str();
    NewTaskDlg->ShowModal();
    SetForegroundWindow(this->Handle);
    delete NewTaskDlg;
  }
}
void __fastcall TMainFrm::CMulitpTask(TMessage& msg)
{
//  SetForegroundWindow(this->Handle);
/*
  if(TrayShow)
  {
    TrayIcon1->Restore();
    TrayShow=false;
  }  */
  OKBottomDlg->ShowModal();
  SetForegroundWindow(OKBottomDlg->Handle);
}
//---------------------------------------------------------------------------
void __fastcall TMainFrm::M_NewTaskClick(TObject *Sender)
{
  TNewTaskDlg *NewTaskDlg=new TNewTaskDlg(MainFrm);
  NewTaskDlg->ShowModal();
  delete NewTaskDlg;
}
//---------------------------------------------------------------------------

void __fastcall TMainFrm::M_ToolBarClick(TObject *Sender)
{
  M_ToolBar->Checked=!M_ToolBar->Checked;
  MainToolBar->Visible=!MainToolBar->Visible;  
}
//---------------------------------------------------------------------------


void __fastcall TMainFrm::M_StatusClick(TObject *Sender)
{
  M_Status->Checked=!M_Status->Checked;
  MainStatusBar->Visible=!MainStatusBar->Visible;
}
//---------------------------------------------------------------------------

void __fastcall TMainFrm::Tool_NewClick(TObject *Sender)
{
  TNewTaskDlg *NewTaskDlg=new TNewTaskDlg(MainFrm);
  NewTaskDlg->ShowModal();
  delete NewTaskDlg;
}
//---------------------------------------------------------------------------

void __fastcall TMainFrm::D_SelectAllClick(TObject *Sender)
{
  DownLoadView->SelectAll();  
}
//---------------------------------------------------------------------------


void __fastcall TMainFrm::C_SelectAllClick(TObject *Sender)
{
  CompleteView->SelectAll();  
}
//---------------------------------------------------------------------------

void __fastcall TMainFrm::D_StartAllClick(TObject *Sender)
{
  CTask* pTask=NULL;
  for(int i=0;i<DownLoadView->Items->Count;i++)
  {
    int nTaskId=DownLoadView->Items->Item[i]->ImageIndex;
    if(DownLoadView->Items->Item[i]->StateIndex==0)
      continue;
    if(!TaskPool.GetTask(nTaskId,pTask))
      continue;
    else
      pTask->Start();
  }
}
//---------------------------------------------------------------------------

void __fastcall TMainFrm::D_StopAllClick(TObject *Sender)
{
  CTask* pTask=NULL;
  for(int i=0;i<DownLoadView->Items->Count;i++)
  {
    int nTaskId=DownLoadView->Items->Item[i]->ImageIndex;
    if(DownLoadView->Items->Item[i]->StateIndex==1)
      continue;
    if(!TaskPool.GetTask(nTaskId,pTask))
      continue;
    else
      pTask->Stop(0);
  }
}
//---------------------------------------------------------------------------

void __fastcall TMainFrm::D_DelAllClick(TObject *Sender)
{
  CTask* pTask=NULL;
  for(int i=DownLoadView->Items->Count-1;i>=0;i--)
  {
    int nTaskId=DownLoadView->Items->Item[i]->ImageIndex;
    if(TaskPool.GetTask(nTaskId,pTask))
      pTask->Delete();
  }
}
//---------------------------------------------------------------------------

void __fastcall TMainFrm::C_DelAllClick(TObject *Sender)
{
  CMetaData MetaData;
  TListItem* Item;
  for(int i=CompleteView->Items->Count-1;i>=0;i--)
  {
    int nTaskId=CompleteView->Items->Item[i]->ImageIndex;
    if(MetaFile.Load(nTaskId,&MetaData))
    {
      MetaData.State=2;
      MetaFile.UpDate(&MetaData);
    }


    Item=RecycleView->Items->Add();
    Item->ImageIndex=CompleteView->Items->Item[i]->ImageIndex;//任务ID
    Item->StateIndex=2;
    Item->SubItems->Add(CompleteView->Items->Item[i]->SubItems->Strings[0]);
    Item->SubItems->Add(CompleteView->Items->Item[i]->SubItems->Strings[1]);

    CompleteView->Items->Delete(i);
  }
}
//---------------------------------------------------------------------------



void __fastcall TMainFrm::RecycleViewMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{

  TPoint Point;
  Point.x=X;
  Point.y=Y;
  Point=RecycleView->ClientToScreen(Point);
  TListItem *Item=RecycleView->GetItemAt(X,Y);
  if(mbRight==Button)
  {
    if(RecycleView->SelCount>1)
    {
      R_DelTask->Enabled=false;
      R_DelAll->Enabled=true;
      R_Open->Enabled=false;
    }
    else if(RecycleView->SelCount==1)
    {
      if(!Item) return;
      R_DelTask->Enabled=true;
      R_DelAll->Enabled=true;
      R_Open->Enabled=true;
    }
    else
    {
      R_DelTask->Enabled=false;
      R_DelAll->Enabled=false;
      R_Open->Enabled=false;
    }
    RecyclePopupMenu->Popup(Point.x,Point.y);
  }
  else if(mbLeft==Button)
  {
    if(!Item)
      SetToolBar(false,false,false,false,false);
    else
      SetToolBar(false,false,true,true,true);
  }
  if(Item)
    ShowTaskInfo(Item->ImageIndex);
}
//---------------------------------------------------------------------------

void __fastcall TMainFrm::R_DelTaskClick(TObject *Sender)
{
  TListItem *Item;
  int nTaskId,nIndex=0;
  Item=RecycleView->Selected;

  if(!Item) return;
  nTaskId=Item->ImageIndex;

  if(!SerachTask(nTaskId,nIndex,RecycleView)) return;
  AlertDlg->CheckBox1->Checked=false;
  if(AlertDlg->ShowModal()==mrCancel) return;

  bool bIsDel=AlertDlg->IsDelete;
  MetaFile.Delete(nTaskId);
  RecycleView->Items->Delete(nIndex);

  if(!bIsDel) return;
  
  CMetaData* Meta=new CMetaData;
  MetaFile.Load(nTaskId,Meta);
  AnsiString strPath=Meta->FileName;
  delete Meta;
  DeleteFile(strPath);
  strPath=strPath+".blc";
  DeleteFile(strPath);
}
//---------------------------------------------------------------------------

void __fastcall TMainFrm::R_DelAllClick(TObject *Sender)
{
  AlertDlg->CheckBox1->Checked=false;
  if(AlertDlg->ShowModal()==mrCancel) return;
  bool bIsDel=AlertDlg->IsDelete;
  int nTaskId;
  while(RecycleView->Items->Count>0)
  {
    nTaskId=RecycleView->Items->Item[RecycleView->Items->Count-1]->ImageIndex;
    MetaFile.Delete(nTaskId);
    RecycleView->Items->Item[RecycleView->Items->Count-1]->Delete();
    if(bIsDel)
    {
       CMetaData* Meta=new CMetaData;
       MetaFile.Load(nTaskId,Meta);
       AnsiString strPath=Meta->FileName;
       delete Meta;
       DeleteFile(strPath);
       strPath=strPath+".blc";
       DeleteFile(strPath);
    }
  }
}
//---------------------------------------------------------------------------



AnsiString TMainFrm::GetFileName(AnsiString FullName)
{
  //TODO: Add your source code here
  AnsiString strResult=FullName;
  string strFileName=FullName.c_str();
  int nPos=strFileName.find_last_of("\\",strFileName.length());
  if(nPos<0)
    return strResult;
  strFileName=strFileName.substr(nPos+1,strFileName.length());
  return strResult=strFileName.c_str();
}

void __fastcall TMainFrm::DownLoadViewDblClick(TObject *Sender)
{
  CTask* pTask=NULL;
  int nTaskId=0;
//取得鼠标位置并转化为客户区坐标
  TPoint Point;
  GetCursorPos(&Point);
  Point=DownLoadView->ScreenToClient(Point);
  TListItem* Item=DownLoadView->GetItemAt(Point.x,Point.y);
  if(!Item) return;
  nTaskId=Item->ImageIndex;
  if(!TaskPool.GetTask(nTaskId,pTask))  return;
  switch(Item->StateIndex)
  {
  case 0:  //开始
    pTask->Stop(0);
    break;
  case 1: //暂停
    pTask->Start();
    break;
  default:break;
  }
}
//---------------------------------------------------------------------------



void __fastcall TMainFrm::CompleteViewDblClick(TObject *Sender)
{
  //取得鼠标位置并转化为客户区坐标
  TPoint Point;
  GetCursorPos(&Point);
  Point=CompleteView->ScreenToClient(Point);
  TListItem* Item=CompleteView->GetItemAt(Point.x,Point.y);
  if(!Item) return;
  int nTaskId=Item->ImageIndex;
  CMetaData* Meta=new CMetaData;
  MetaFile.Load(nTaskId,Meta);
  ShellExecute(this->Handle,"open",Meta->FileName,NULL,"",SW_SHOW);
  delete Meta;
}
//---------------------------------------------------------------------------

void __fastcall TMainFrm::C_OpenClick(TObject *Sender)
{
  TListItem *Item;
  int nTaskId;
  Item=CompleteView->Selected;

  if(!Item) return;
  nTaskId=Item->ImageIndex;
  CMetaData* Meta=new CMetaData;
  MetaFile.Load(nTaskId,Meta);
  ShellExecute(this->Handle,"open",Meta->FileName,NULL,"",SW_SHOW);
  delete Meta;
}
//---------------------------------------------------------------------------

void __fastcall TMainFrm::T_NewTaskClick(TObject *Sender)
{
  TNewTaskDlg *NewTaskDlg=new TNewTaskDlg(MainFrm);
  NewTaskDlg->ShowModal();
  delete NewTaskDlg;  
}
//---------------------------------------------------------------------------


void TMainFrm::SetDownLoadPopup(bool m1, bool m2, bool m3, bool m4, bool m5, bool m6)
{
  //TODO: Add your source code here
  D_StartTask->Enabled=m1;
  D_StopTask->Enabled=m2;
  D_DelTask->Enabled=m3;
  D_StartAll->Enabled=m4;
  D_StopAll->Enabled=m5;
  D_DelAll->Enabled=m6;
}
void TMainFrm::SetCompletePopup(bool m1, bool m2, bool m3, bool m4)
{
  C_Open->Enabled=m1;
  C_DelTask->Enabled=m2;
  C_SelectAll->Enabled=m3;
  C_DelAll->Enabled=m4;
}
void TMainFrm::SetToolBar(bool t1, bool t2, bool t3, bool t4, bool t5)
{
  //TODO: Add your source code here
  T_StartTask->Enabled=t1;
  T_StopTask->Enabled=t2;
  T_DelTask->Enabled=t3;
  T_Open->Enabled=t4;
  T_Dir->Enabled=t5;
}
void __fastcall TMainFrm::T_StartTaskClick(TObject *Sender)
{
  TListItem *Item;
  CTask* pTask=NULL;
  int nTaskId;
  Item=DownLoadView->Selected;

  if(!Item) return;
  nTaskId=Item->ImageIndex;

  if(!TaskPool.GetTask(nTaskId,pTask)) return;
  pTask->Start();

  T_StartTask->Enabled=false;
  T_StopTask->Enabled=true;
}
//---------------------------------------------------------------------------

void __fastcall TMainFrm::T_StopTaskClick(TObject *Sender)
{
  TListItem *Item;
  CTask* pTask=NULL;
  int nTaskId;
  Item=DownLoadView->Selected;

  if(!Item) return;
  nTaskId=Item->ImageIndex;

  if(!TaskPool.GetTask(nTaskId,pTask)) return;
  pTask->Stop(0);

  T_StartTask->Enabled=true;
  T_StopTask->Enabled=false;
}
//---------------------------------------------------------------------------

void __fastcall TMainFrm::T_DelTaskClick(TObject *Sender)
{
  if(DownLoadPanel->Visible)
    D_DelTask->Click();
  else if(CompletePanel->Visible)
    C_DelTask->Click();
  else
    R_DelTask->Click();
  T_DelTask->Enabled=false;
}
//---------------------------------------------------------------------------

void __fastcall TMainFrm::T_OpenClick(TObject *Sender)
{
  if(CompletePanel->Visible)
    C_Open->Click();
  else
    R_Open->Click();
}
//---------------------------------------------------------------------------

void __fastcall TMainFrm::R_OpenClick(TObject *Sender)
{
  TListItem *Item;
  int nTaskId;
  Item=RecycleView->Selected;

  if(!Item) return;
  nTaskId=Item->ImageIndex;
  CMetaData* Meta=new CMetaData;
  MetaFile.Load(nTaskId,Meta);
  ShellExecute(this->Handle,"open",Meta->FileName,NULL,"",SW_SHOW);
  delete Meta;
}
//---------------------------------------------------------------------------

void __fastcall TMainFrm::T_DirClick(TObject *Sender)
{
  TListItem *Item;
  int nTaskId;
  if(CompletePanel->Visible)
  {
    Item=CompleteView->Selected;
    if(!Item) return;
    nTaskId=Item->ImageIndex;
  }
  else if(RecyclePanel->Visible)
  {
    Item=RecycleView->Selected;
    if(!Item) return;
    nTaskId=Item->ImageIndex;
  }
  else
  {
    Item=DownLoadView->Selected;
    if(!Item) return;
    nTaskId=Item->ImageIndex;
  }
  CMetaData* Meta=new CMetaData;
  MetaFile.Load(nTaskId,Meta);
  string strPath=Meta->FileName;
  int nPos=strPath.find_last_of("\\",strPath.length());
  if(nPos>0)
    strPath=strPath.substr(0,nPos+1);
  ShellExecute(this->Handle,"open",strPath.c_str(),NULL,"",SW_SHOW);
  delete Meta;
}
//---------------------------------------------------------------------------


AnsiString TMainFrm::GetReMainTime(__int64 Down, __int64 Size,int Speed)
{
  //TODO: Add your source code here
  int Hour=0,Min=0,Sec=0;
  AnsiString strResult="";
  if(!Speed) return strResult;
  int ReMainTime=(Size-Down)/Speed;
  while(ReMainTime>0)
  {
    if(ReMainTime/3600>0)
    {
      Hour=ReMainTime/3600;
      ReMainTime=ReMainTime%3600;
    }
    else if(ReMainTime/60>0)
    {
      Min=ReMainTime/60;
      ReMainTime=ReMainTime%60;
    }
    else
    {
      Sec=ReMainTime;
      ReMainTime=0;
    }
  }
   return strResult=Sysutils::IntToStr(Hour)+" : "+Sysutils::IntToStr(Min)+" : "+Sysutils::IntToStr(Sec);
}
void __fastcall TMainFrm::M_ExitClick(TObject *Sender)
{
  Shell_NotifyIcon(NIM_DELETE,&icon);
  Application->Terminate();
}
//---------------------------------------------------------------------------


void __fastcall TMainFrm::MainToolBarCustomDraw(TToolBar *Sender,
      const TRect &ARect, bool &DefaultDraw)
{
  TRect Rect;
  Graphics::TBitmap *bitmap;
  bitmap=new Graphics::TBitmap;
  bitmap->LoadFromFile(bmppath+"toolbar_back.bmp");
//  ::BitBlt(ToolBar1->Canvas->Handle,0,0,ToolBar1->Width,ToolBar1->Height,bitmap->Canvas->Handle,0,0,SRCCOPY);
  int i=0;
  while(i<MainToolBar->Width)
  {
    MainToolBar->Canvas->Draw(i++,0,bitmap);
  }
  delete bitmap;
}
//---------------------------------------------------------------------------

void __fastcall TMainFrm::MenuBarCustomDraw(TToolBar *Sender,
      const TRect &ARect, bool &DefaultDraw)
{
  TRect Rect;
  Graphics::TBitmap *bitmap;
  bitmap=new Graphics::TBitmap;
  bitmap->LoadFromFile(bmppath+"tasklist_head_bg.bmp");
//  ::BitBlt(ToolBar1->Canvas->Handle,0,0,ToolBar1->Width,ToolBar1->Height,bitmap->Canvas->Handle,0,0,SRCCOPY);
  int i=0;
  while(i<MenuBar->Width)
  {
    MenuBar->Canvas->Draw(i++,0,bitmap);
  }
  delete bitmap;
}
//---------------------------------------------------------------------------


void TMainFrm::ShowTaskInfo(int TaskId)
{
  //TODO: Add your source code here
  AnsiString FileName,Pos,Size,Url,SavePath;
  CMetaData* Meta=new CMetaData;
  if(MetaFile.Load(TaskId,Meta))
  {
    SavePath=Meta->FileName;
    FileName=ExtractFileName(SavePath);
    Pos=SavePath.Delete(SavePath.Length()-FileName.Length(),SavePath.Length());
    Url=Meta->Url;
    Size=(AnsiString)Meta->FileSize;
  }
  InfoBox->Clear();
  InfoBox->Items->Add("文件名:                            "+FileName);
  InfoBox->Items->Add("文件大小:                           "+Size);
  InfoBox->Items->Add("位置：                             "+Pos);
  InfoBox->Items->Add("URL：                              "+Url);
  delete Meta;
}
void __fastcall TMainFrm::N5Click(TObject *Sender)
{

  if(TrayShow)
  {
    Application->Restore();
//    TrayIcon1->Restore();
    ShowWindow(this->Handle,SW_NORMAL);
    TrayShow=false;
  }
  else
  {
    Application->Minimize();
//    TrayIcon1->Minimize();
    ShowWindow(this->Handle,SW_HIDE);
    TrayShow=true;
  }
}
//---------------------------------------------------------------------------

void __fastcall TMainFrm::N6Click(TObject *Sender)
{
  Shell_NotifyIcon(NIM_DELETE,&icon);
  Application->Terminate();  
}
//---------------------------------------------------------------------------

void __fastcall TMainFrm::FormCloseQuery(TObject *Sender, bool &CanClose)
{
  Application->Minimize();
  ShowWindow(this->Handle,SW_HIDE);
  CanClose=CanExit;
}
//---------------------------------------------------------------------------


void   __fastcall   TMainFrm::WndProc(Messages::TMessage   &Message)   
{     //   在Form1的头中定义   
                if     (Message.Msg==WM_QUERYENDSESSION   ||   
                                                Message.Msg==WM_ENDSESSION)   
                        CanExit=true;                         //   允许   
                TForm::WndProc(Message);   
}
void __fastcall TMainFrm::FormDestroy(TObject *Sender)
{
//  Shell_NotifyIcon(NIM_DELETE,&icon);
  RemoveProp(this->Handle,APP_NAME);
}
//---------------------------------------------------------------------------
void __fastcall TMainFrm::NotifyIcon(TMessage &msg)
{
//  SetForegroundWindow(Handle);
  if(msg.LParam==WM_RBUTTONDOWN)
  {
    TPoint Point;
    GetCursorPos(&Point);
    TrayMenu->Popup(Point.x,Point.y);
  }
  else if(msg.LParam==WM_LBUTTONDBLCLK)
  {
    Application->Restore();
//    TrayIcon1->Restore();
    ShowWindow(this->Handle,SW_NORMAL);
  }
}

void TMainFrm::ReDownTask(int TaskId,TListView *View)
{
  if(ReDownDlg->ShowModal()==mrCancel)
    return;
  CMetaData *meta=new CMetaData;
  MetaFile.Load(TaskId,meta);
  //删除文件
  AnsiString path=meta->FileName;
  DeleteFile(path);
  //删除任务
  MetaFile.Delete(TaskId);
  //删除列表对应项
  int nIndex=0;
  if(SerachTask(TaskId,nIndex,View))
    View->Items->Delete(nIndex);
  //重新建立任务
  string url,ref,filename;
  url=meta->Url;
  ref=meta->Ref;
  filename=meta->FileName;
  TaskPool.AddTask(url,ref,filename);
  delete meta;
}
void __fastcall TMainFrm::C_ReDownClick(TObject *Sender)
{
  TListItem *Item=CompleteView->Selected;
  if(!Item)
    return;
  int nTaskId=Item->ImageIndex;
  ReDownTask(nTaskId,CompleteView);
}
//---------------------------------------------------------------------------

void __fastcall TMainFrm::R_ReDownClick(TObject *Sender)
{
  TListItem *Item=RecycleView->Selected;
  if(!Item)
    return;
  int nTaskId=Item->ImageIndex;
  ReDownTask(nTaskId,RecycleView);
}
//---------------------------------------------------------------------------


