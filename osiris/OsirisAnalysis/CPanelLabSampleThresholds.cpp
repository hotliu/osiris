/*
* ===========================================================================
*
*                            PUBLIC DOMAIN NOTICE
*               National Center for Biotechnology Information
*
*  This software/database is a "United States Government Work" under the
*  terms of the United States Copyright Act.  It was written as part of
*  the author's official duties as a United States Government employee and
*  thus cannot be copyrighted.  This software/database is freely available
*  to the public for use. The National Library of Medicine and the U.S.
*  Government have not placed any restriction on its use or reproduction.
*
*  Although all reasonable efforts have been taken to ensure the accuracy
*  and reliability of the software and data, the NLM and the U.S.
*  Government do not and cannot warrant the performance or results that
*  may be obtained by using this software or data. The NLM and the U.S.
*  Government disclaim all warranties, express or implied, including
*  warranties of performance, merchantability or fitness for any particular
*  purpose.
*
*  Please cite the author in any work or product based on this material.
*
* ===========================================================================
*
*  FileName: CPanelLabSampleThresholds.cpp
*  Author:   Douglas Hoffman
*
*/
#include "mainApp.h"
#include "CPanelLabSettings.h"
#include <wx/stattext.h>
#include <wx/sizer.h>

//********************************************************************
//
//    CPanelLabSampleThresholds - CGridRFULimits, CGridSampleLimits
//

//***************************                     CGridSampleLimits

CGridSampleLimits::CGridSampleLimits(wxWindow *parent, wxWindowID id) :
  _CGridEdit(parent,id), 
  m_pValidatePct(NULL), 
  m_pData(NULL),
  m_pMessageBook(NULL)
  //,m_bReadOnly(false)
{
  wxString PEAKS("peaks");
  CreateGrid(ROW_MESSAGE_BOOK_OFFSET,2);
  EnableDragColSize(false);
  EnableDragRowSize(false);

  EnableScrolling(false,true);

  SetRowLabelValue(ROW_PULLUP,"Max. No. of pullups peaks per sample");
  SetCellValue(ROW_PULLUP,1,PEAKS);

  SetRowLabelValue(ROW_STUTTER,"Max. No. of stutter peaks per sample");
  SetCellValue(ROW_STUTTER,1,PEAKS);

  SetRowLabelValue(ROW_ADENYLATION,"Max. No. of adenylation peaks per sample");
  SetCellValue(ROW_ADENYLATION,1,PEAKS);

  SetRowLabelValue(ROW_OFF_LADDER,"Max. off-ladder alleles per sample:");
  SetCellValue(ROW_OFF_LADDER,1,PEAKS);
  
  SetRowLabelValue(ROW_RESIDUAL,"Max. residual for allele (<0.5 bp):");
  SetCellValue(ROW_RESIDUAL,1,"Sample/Ladder BP alignment");
  SetCellEditor(ROW_RESIDUAL,0,new wxGridCellFloatEditor(-1,4));
  SetDefaultCellValidator(new nwxGridCellUIntRangeValidator(0,1000000,true));
  SetCellValidator(
    new nwxGridCellDoubleRangeValidator(0.0,0.5,true),
    ROW_RESIDUAL,0);
  
  SetRowLabelValue(ROW_EXCESSIVE_RESIDUAL,"Max. No. of peaks with excessive residual:");
  SetCellValue(ROW_EXCESSIVE_RESIDUAL,1,PEAKS);

  SetRowLabelValue(ROW_RFU_INCOMPLETE_SAMPLE,"Incomplete profile threshold for Reamp More/Reamp Less:");
  SetCellValue(ROW_RFU_INCOMPLETE_SAMPLE,1,"RFU");

  SetRowLabelValue(ROW_MIN_BPS_ARTIFACTS,"Ignore artifacts smaller than:");
  SetCellValue(ROW_MIN_BPS_ARTIFACTS,1,"bps");
  m_clrBackground = GetLabelBackgroundColour();
  wxFont fnLabel = GetLabelFont();
  for(int nRow = 0; nRow < ROW_MESSAGE_BOOK_OFFSET; nRow++)
  {
    SetCellBackgroundColour(nRow,1,m_clrBackground);
    SetReadOnly(nRow,1,true);
    SetCellFont(nRow,1,fnLabel);
    SetCellAlignment(nRow,0,wxALIGN_RIGHT, wxALIGN_CENTRE);
  }
  SetDefaultEditor(new wxGridCellFloatEditor(1,0));
  SetColLabelValue(0,"Value");
  SetColLabelValue(1,"Units");
  SetRowLabelAlignment(wxALIGN_LEFT, wxALIGN_CENTRE);
  SetColLabelAlignment(wxALIGN_CENTRE, wxALIGN_CENTRE);
}

void CGridSampleLimits::SetData(
  CLabThresholds *pData, const CXMLmessageBook *pMessageBook)
{
  m_pData = pData;
  m_pMessageBook = pMessageBook;
  set<wxString> setMsgThresholds; // message names for thresholds already used
  size_t nCount = 
    ((m_pMessageBook != NULL) && m_pMessageBook->IsOK())
    ? m_pMessageBook->GetMessageCount()
    : 0;
  if(nCount)
  {
    const wxString sEmpty;
    const wxString sUnitPCT("%");
    const CXMLmessageBookSM *pmsgSM;
    size_t nCountKeep = 0;
    bool bPct;

    // the following 5 will not use the message book
    setMsgThresholds.insert(CLabThresholds::g_psmNumberOfPullUpsInSample);
    setMsgThresholds.insert(CLabThresholds::g_psmNumberOfStuttersInSample);
    setMsgThresholds.insert(CLabThresholds::g_psmNumberOfAdenylationsInSample);
    setMsgThresholds.insert(CLabThresholds::g_psmNumberOfOffLadderAllelesInSample);
    setMsgThresholds.insert(CLabThresholds::g_psmNumberOfExcessiveResidualsInSample);

    m_vpMsgBookSM.clear();
    m_vpMsgBookSM.reserve(nCount);
    for(size_t i = 0; i < nCount; i++)
    {
      pmsgSM = m_pMessageBook->GetMessage(i);
      if(setMsgThresholds.find(pmsgSM->GetName()) == setMsgThresholds.end())
      {
        m_vpMsgBookSM.push_back(pmsgSM);
        setMsgThresholds.insert(pmsgSM->GetName());
        if(!pmsgSM->EmptyDescriptor())
        {
          nCountKeep++;
        }
      }
    }
    SetMessageBookRows((int)nCountKeep);
    if(nCountKeep)
    {
      wxFont fn = GetLabelFont();
      vector<const CXMLmessageBookSM *>::iterator itrSM;
      int nRow = ROW_MESSAGE_BOOK_OFFSET;
      for(itrSM = m_vpMsgBookSM.begin();
        itrSM != m_vpMsgBookSM.end();
        ++itrSM)
      {
        pmsgSM = *itrSM;
        if(!pmsgSM->EmptyDescriptor())
        {
          bPct = pmsgSM->IsPercent();
          if(bPct)
          {
            SetCellValidator(GetPctValidator(),nRow,0);
          }
          SetRowLabelValue(nRow,pmsgSM->GetDescriptor());
          SetCellBackgroundColour(nRow,1,m_clrBackground);
          SetCellFont(nRow,1,fn);
          SetCellValue(nRow,1,bPct ? sUnitPCT : sEmpty);
          SetReadOnly(nRow,1,true);
          if(pmsgSM->IsPreset())
          {
            SetCellAlignment(nRow,0,wxALIGN_RIGHT,wxALIGN_CENTRE);
          }
          nRow++;
        }
      }
    }
  }
  SetCellValue(0,0,"00000000");
  SetMargins(0,0);
  nwxGrid::UpdateLabelSizes(this);
  AutoSize();
}
void CGridSampleLimits::SetMessageBookRows(int n)
{
//  int nOld = GetNumberRows();
  int nNew = n + ROW_MESSAGE_BOOK_OFFSET;
  int nRow;
  nwxGrid::SetRowCount(this,ROW_MESSAGE_BOOK_OFFSET);// bug workaround
  nwxGrid::SetRowCount(this,nNew);
  for(nRow = ROW_MESSAGE_BOOK_OFFSET; nRow < nNew; nRow++)
  {
    //SetReadOnly(nRow,0,m_bReadOnly);
    SetReadOnly(nRow,1,true);
    SetCellAlignment(nRow,0,wxALIGN_RIGHT, wxALIGN_CENTRE);
  }
}
bool CGridSampleLimits::TransferDataFromWindow()
{
  bool bRtn = (m_pData != NULL) && (m_pMessageBook != NULL);
  if(bRtn)
  {
    wxString s;
    double d;
    int n;
    bool bCreateIfNeeded;
    s = GetCellValue(ROW_PULLUP,0);
    n = CPanelLabSettings::Str2Int(s);
    m_pData->SetMaxNumberOfPullupsPerSample(n);

    s = GetCellValue(ROW_STUTTER,0);
    n = CPanelLabSettings::Str2Int(s);
    m_pData->SetMaxNumberOfStutterPeaksPerSample(n);

    s = GetCellValue(ROW_ADENYLATION,0);
    n = CPanelLabSettings::Str2Int(s);
    m_pData->SetMaxNumberOfAdenylationsPerSample(n);

    s = GetCellValue(ROW_OFF_LADDER,0);
    n = CPanelLabSettings::Str2Int(s);
    m_pData->SetMaxNumberOfOLAllelesPerSample(n);

    s = GetCellValue(ROW_RESIDUAL,0);
    d = CPanelLabSettings::Str2Double(s);
    m_pData->SetMaxResidualForAlleleCall(d);

    s = GetCellValue(ROW_EXCESSIVE_RESIDUAL,0);
    n = CPanelLabSettings::Str2Int(s);
    m_pData->SetMaxExcessiveResidual(n);

    s = GetCellValue(ROW_RFU_INCOMPLETE_SAMPLE,0);
    n = CPanelLabSettings::Str2Int(s);
    m_pData->SetMaxRFUForIncompleteSample(n);

    s = GetCellValue(ROW_MIN_BPS_ARTIFACTS,0);
    n = CPanelLabSettings::Str2Int(s);
    m_pData->SetMinBPSForArtifacts(n);


    // message book parameters

    int nRow = ROW_MESSAGE_BOOK_OFFSET;
    vector<const CXMLmessageBookSM *>::iterator itrMsg;
    for(itrMsg = m_vpMsgBookSM.begin();
      itrMsg != m_vpMsgBookSM.end();
      ++itrMsg)
    {
      if(!(*itrMsg)->EmptyDescriptor())
      {
        if((*itrMsg)->IsPreset())
        {
          n = GetBoolValue(nRow,0) ? 1 : 0;
        }
        else
        {
          s = GetCellValue(nRow,0);
          n = CPanelLabSettings::Str2Int(s,-1);
        }
        bCreateIfNeeded = (n != (*itrMsg)->GetThreshold());
        m_pData->SetValue((*itrMsg)->GetName(),n,bCreateIfNeeded);
        ++nRow;
      }

      // need validation
    }
  }
  return bRtn;
}
bool CGridSampleLimits::TransferDataToWindow()
{
  bool bRtn = (m_pData != NULL) && (m_pMessageBook != NULL);
  if(bRtn)
  {
    SetCellValue(ROW_PULLUP,0,CPanelLabSettings::Number2Str(
      m_pData->GetMaxNumberOfPullupsPerSample()));
    SetCellValue(ROW_STUTTER,0,CPanelLabSettings::Number2Str(
      m_pData->GetMaxNumberOfStutterPeaksPerSample()));
    SetCellValue(ROW_ADENYLATION,0,CPanelLabSettings::Number2Str(
      m_pData->GetMaxNumberOfAdenylationsPerSample()));
    SetCellValue(ROW_OFF_LADDER,0,CPanelLabSettings::Number2Str(
      m_pData->GetMaxNumberOfOLAllelesPerSample()));
    SetCellValue(ROW_RESIDUAL,0,CPanelLabSettings::Number2Str(
      m_pData->GetMaxResidualForAlleleCall()));
    SetCellValue(ROW_EXCESSIVE_RESIDUAL,0,CPanelLabSettings::Number2Str(
      m_pData->GetMaxExcessiveResidual()));

    SetCellValue(ROW_RFU_INCOMPLETE_SAMPLE,0,CPanelLabSettings::Number2Str(
      m_pData->GetMaxRFUForIncompleteSample()));
    SetCellValue(ROW_MIN_BPS_ARTIFACTS,0,CPanelLabSettings::Number2Str(
      m_pData->GetMinBPSForArtifacts()));

    //  message book stuff
    int nRow = ROW_MESSAGE_BOOK_OFFSET;
    int nValue;
//    int nRowHeight = GetRowHeight(0);
    vector<const CXMLmessageBookSM *>::iterator itrMsg = m_vpMsgBookSM.begin();
    for(itrMsg = m_vpMsgBookSM.begin();
      itrMsg != m_vpMsgBookSM.end();
      ++itrMsg)
    {
      if(!(*itrMsg)->EmptyDescriptor())
      {
        nValue = m_pData->GetValue((*itrMsg)->GetName(),(*itrMsg)->GetThreshold());
        if((*itrMsg)->IsPreset())
        {
          SetBoolCell(nRow,0);
          SetBoolValue(nRow,0,!!nValue);
  //        SetRowHeight(nRow,nRowHeight);
        }
        else
        {
          SetCellValue(nRow,0,
            CPanelLabSettings::Number2Str(nValue));
        }
        ++nRow;
      }
    }
  }
  return bRtn;
}
/*
void CGridSampleLimits::SetAllReadOnly(bool bReadOnly)
{
  int n = GetNumberRows();
  int nRow;
  m_bReadOnly = bReadOnly;
  for(nRow = 0; nRow < n; nRow++)
  {
    SetReadOnly(nRow,0,bReadOnly);
  }
}
*/


//********************************************************************
//
//    CPanelLabSampleThresholds
//
CPanelLabSampleThresholds::CPanelLabSampleThresholds(
  wxWindow *parent, wxWindowID id) :
    nwxPanel(parent,id),
    m_pData(NULL)
{
  wxBoxSizer *pSizer = new wxBoxSizer(wxVERTICAL);  
#if 0  
  wxStaticText *pTextSample = new wxStaticText(
    this,wxID_ANY,"Sample Limits");
  mainApp::SetBoldFont(pTextSample);
  pSizer->Add(pTextSample,0,
    wxALIGN_LEFT | (wxALL ^ wxBOTTOM), ID_BORDER);
#endif
  m_pGridSample = new CGridSampleLimits(this,wxID_ANY);
  pSizer->Add(m_pGridSample,1,wxEXPAND | wxALL,ID_BORDER);
  SetSizer(pSizer);
}

bool CPanelLabSampleThresholds::TransferDataFromWindow()
{
  bool bRtn = (m_pData != NULL);
  if(!m_pGridSample->TransferDataFromWindow())
  {
    bRtn = false;
  }
  return bRtn;
}
bool CPanelLabSampleThresholds::TransferDataToWindow()
{
  bool bRtn = (m_pData != NULL);
  if(!m_pGridSample->TransferDataToWindow())
  {
    bRtn = false;
  }
  return bRtn;
}
