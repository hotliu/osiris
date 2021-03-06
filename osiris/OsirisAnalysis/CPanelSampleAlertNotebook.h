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
*  FileName: CPanelSampleAlertNotebook.h
*  Author:   Douglas Hoffman
*
*/
#ifndef __C_PANEL_SAMPLE_ALERT_NOTEBOOK_H__
#define __C_PANEL_SAMPLE_ALERT_NOTEBOOK_H__

#include <wx/datetime.h>
#include <wx/splitter.h>
#include <wx/notebook.h>
#include <wx/panel.h>
#include <wx/textctrl.h>
#include "CGridAlerts.h"
#include "COARfile.h"
#include "CHistoryTime.h"
#include "ISetDateTime.h"
#include "CPanelSampleAlertDetails.h"

class CPanelUserID;

#ifndef __WINDOW_ARRAY_INDEX__
#define __WINDOW_ARRAY_INDEX__
typedef enum
{
  SA_NDX_DIR,
  SA_NDX_SAMPLE,
  SA_NDX_ILS,
  SA_NDX_CHANNEL,
  SA_WINDOW_COUNT
} ALERT_WINDOW_TYPE; // used in arrays below
#endif

class CPanelSampleAlertNotebook : public wxPanel, public ISetDateTime
{
public:
  CPanelSampleAlertNotebook(
    const COARfile *pFile,
    const COARsample *pSample, 
    wxWindow *parent,
    wxWindowID id = wxID_ANY,
    const map<int,wxString> *pmapChannelNames = NULL,
    bool bSplitHorizontal = true,
    bool bReadOnly = false,
    int nSelect = SA_NDX_SAMPLE,
    bool bShowOne = false
    );
  virtual ~CPanelSampleAlertNotebook();
  virtual bool TransferDataToWindow();
  virtual bool Validate();
  bool IsModified(int n);
  bool IsModified();
  bool IsNotesModified(int n);

  void Select(int n)
  {
    // select notebook page, Sample, ILS, or Channel
    m_pNotebook->ChangeSelection((size_t)n);
  }
  CPanelSampleAlertDetails **GetSplitterWindows()
  {
    return &m_pSplitter[0];
  }
  CPanelSampleAlertDetails *GetSplitterWindow(int n)
  {
    return m_pSplitter[n];
  }
  wxNotebook *GetNotebookWindow()
  {
    return m_pNotebook;
  }
  virtual bool SetDateTime(const wxDateTime *pTime);
  const COARmessages &GetDirMessages()
  {
    return m_MsgDir;
  }
  const COARmessages &GetSampleMessages()
  {
    return m_MsgSample;
  }
  const COARmessages &GetILSMessages()
  {
    return m_MsgILS;
  }
  const COARmessages &GetChannelMessages()
  {
    return m_MsgChannel;
  }
/*
  const wxString &GetNotes(int i)
  {
    _TransferNotes(i);
    return m_sText[i];
  }
  const wxString &GetSampleNotes()
  {
    return GetNotes(SA_NDX_SAMPLE);
  }
  const wxString &GetILSNotes()
  {
    return GetNotes(SA_NDX_ILS);
  }
  const wxString &GetChannelNotes()
  {
    return GetNotes(SA_NDX_CHANNEL);
  }
*/

  wxString GetNewNotes(int n)
  {
    wxString sRtn;
    CPanelSampleAlertDetails *psd = m_pSplitter[n];

    if(psd != NULL)
    {
      sRtn = psd->GetNewNotesValue();
    }
    return sRtn;
  }
  wxString GetNewSampleNotes()
  {
    return GetNewNotes(SA_NDX_SAMPLE);
  }
  wxString GetNewILSNotes()
  {
    return GetNewNotes(SA_NDX_ILS);
  }
  wxString GetNewChannelNotes()
  {
    return GetNewNotes(SA_NDX_CHANNEL);
  }
  wxString GetNewDirNotes()
  {
    return GetNewNotes(SA_NDX_DIR);
  }

  //  bool IsNotesEmpty(int n);
  bool IsAlertsModified(int n);

  bool IsIndexUsed(int n)
  {
    return (m_pSplitter[n] != NULL);
  }
  void SetPanelUserID(CPanelUserID *p)
  {
    m_pPanelUser = p;
  }

private:
//  void _TransferNotes(int i);
  bool _ProcessEvent();
  const COARnotes *_GetNotes(int n, const wxDateTime *pdt = NULL);
  wxString _GetReviewAcceptance(int n, const wxDateTime *pdt = NULL);
  const wxString &_GetNotesText(int n, const wxDateTime *pdt = NULL)
  {
    return COARnotes::GetText(_GetNotes(n,pdt));
  }

  CHistoryTime m_HistTime;
  COARmessages m_MsgDir;
  COARmessages m_MsgSample;
  COARmessages m_MsgILS;
  COARmessages m_MsgChannel;
//  wxString m_sText[SA_WINDOW_COUNT];

  const COARsample *m_pSample;
  const COARfile *m_pFile;

  wxNotebook *m_pNotebook;
  CPanelUserID *m_pPanelUser;
  CPanelSampleAlertDetails *m_pSplitter[SA_WINDOW_COUNT];
  bool m_bReadOnly;


  static const wxString g_sLabelDirNotices;
  static const wxString g_sLabelSampleNotices;
  static const wxString g_sLabelILSNotices;
  static const wxString g_sLabelChannelNotices;
  static const wxString * const g_psLabels[SA_WINDOW_COUNT];
  static const wxString * const g_psNotesLabels[SA_WINDOW_COUNT];
public:
  static const wxString g_sLabelDir;
  static const wxString g_sLabelSample;
  static const wxString g_sLabelILS;
  static const wxString g_sLabelChannel;

  static const wxString &GetLabel(int ndx)
  {
    const wxString *psRtn = g_psLabels[ndx];
    return *psRtn;
  }
public:
  void OnCellChange(wxGridEvent &e);
  void OnNotesChange(wxCommandEvent &e);
  DECLARE_EVENT_TABLE();
};


#endif
