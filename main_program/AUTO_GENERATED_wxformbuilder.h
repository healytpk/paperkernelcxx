///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version 4.2.1-0-g80c4cb6)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#pragma once

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/sizer.h>
#include <wx/gdicmn.h>
#include <wx/string.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/toolbar.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/panel.h>
#include <wx/listctrl.h>
#include <wx/dataview.h>
#include <wx/splitter.h>
#include <wx/button.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/statline.h>
#include <wx/listbox.h>
#include <wx/notebook.h>
#include <wx/frame.h>
#include <wx/statbmp.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// Class Dialog_Main__Auto_Base_Class
///////////////////////////////////////////////////////////////////////////////
class Dialog_Main__Auto_Base_Class : public wxFrame
{
	private:

	protected:
		wxNotebook* m_notebook1;
		wxPanel* panelBrowse;
		wxToolBar* m_toolBar1;
		wxToolBarToolBase* toolShowPaperTree;
		wxToolBarToolBase* toolShowViewPortal;
		wxToolBarToolBase* toolDownloadIndividualPaper;
		wxBoxSizer* bSizerForPanelBrowse;
		wxPanel* panelAuthors;
		wxSplitterWindow* m_splitter2;
		wxPanel* m_panel5;
		wxListCtrl* listAuthors;
		wxPanel* m_panel6;
		wxDataViewCtrl* treeAuthorPapers;
		wxPanel* panelXapian;
		wxButton* btnXapianLoadPapers;
		wxButton* btnXapianUnloadPapers;
		wxStaticText* m_staticText4;
		wxTextCtrl* txtXapianSearchFor;
		wxButton* btnXapianSearch;
		wxStaticText* m_staticText5;
		wxListCtrl* listXapianResults;
		wxPanel* panelAI;
		wxBoxSizer* bSizerForPanelAI;
		wxButton* AI_btnLoadModel;
		wxButton* AI_btnUnloadModel;
		wxStaticLine* m_staticline1;
		wxButton* AI_btnLoadPapers;
		wxButton* AI_btnUnloadPapers;
		wxStaticLine* m_staticline2;
		wxStaticText* m_staticText1;
		wxTextCtrl* AI_txtBooleanQuestion;
		wxButton* AI_btnWhittleDownPapers;
		wxListCtrl* AI_lcPapers;
		wxStaticText* m_staticText2;
		wxTextCtrl* AI_txtDetailedQuestion;
		wxButton* AI_btnAnalysePapers;
		wxStaticText* m_staticText3;
		wxListBox* AI_listPapers;
		wxTextCtrl* AI_txtDetailedAnswer;
		wxPanel* panelDebug;
		wxNotebook* panelDebug_Notebook;
		wxPanel* panelDebug_panelInfo;
		wxTextCtrl* txtDebug_Info;
		wxButton* btnDebug_Refresh;
		wxPanel* panelDebug_panelNames;
		wxStaticText* m_staticText7;
		wxStaticText* m_staticText8;
		wxListBox* panelDebug_panelNames_listPrimary;
		wxListBox* panelDebug_panelNames_listAlternative;
		wxStaticText* m_staticText9;
		wxListCtrl* panelDebug_panelNames_listctrlMappings;
		wxPanel* panelDebug_panelHash;
		wxStaticText* m_staticText10;
		wxTextCtrl* panelDebug_panelHash_txtInputHash;
		wxStaticText* m_staticText11;
		wxStaticText* m_staticText12;
		wxTextCtrl* panelDebug_panelHash_txtOutputHash;
		wxButton* panelDebug_panelHash_btnCheckForCollisions;
		wxTextCtrl* panelDebug_panelHash_txtSummary;

		// Virtual event handlers, override them in your derived class
		virtual void OnClose( wxCloseEvent& event ) { event.Skip(); }
		virtual void m_notebook1_OnNotebookPageChanged( wxNotebookEvent& event ) { event.Skip(); }
		virtual void OnTool_ShowPaperTree( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnTool_ShowViewPortal( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnTool_DownloadIndividualPaper( wxCommandEvent& event ) { event.Skip(); }
		virtual void listAuthors_OnListItemRightClick( wxListEvent& event ) { event.Skip(); }
		virtual void listAuthors_OnListItemSelected( wxListEvent& event ) { event.Skip(); }
		virtual void treeAuthorPapers_OnDataViewCtrlItemActivated( wxDataViewEvent& event ) { event.Skip(); }
		virtual void btnXapianLoadPapers_OnButtonClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void btnXapianUnloadPapers_OnButtonClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void btnXapianSearch_OnButtonClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void listXapianResults_OnListItemActivated( wxListEvent& event ) { event.Skip(); }
		virtual void AI_btnLoadModel_OnButtonClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void AI_btnUnloadModel_OnButtonClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void AI_btnLoadPapers_OnButtonClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void AI_btnUnloadPapers_OnButtonClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void AI_btnWhittleDownPapers_OnButtonClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void AI_btnAnalysePapers_OnButtonClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void panelDebug_Notebook_OnPageChanged( wxNotebookEvent& event ) { event.Skip(); }
		virtual void btnDebug_Refresh_OnButtonClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void panelDebug_panelNames_listPrimary_OnListBox( wxCommandEvent& event ) { event.Skip(); }
		virtual void panelDebug_panelHash_txtInputHash_OnText( wxCommandEvent& event ) { event.Skip(); }
		virtual void panelDebug_panelHash_btnCheckForCollisions_OnButtonClick( wxCommandEvent& event ) { event.Skip(); }


	public:

		Dialog_Main__Auto_Base_Class( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Paper Kernel C++"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 815,794 ), long style = wxDEFAULT_FRAME_STYLE );

		~Dialog_Main__Auto_Base_Class();

		void m_splitter2OnIdle( wxIdleEvent& )
		{
			m_splitter2->SetSashPosition( 0 );
			m_splitter2->Disconnect( wxEVT_IDLE, wxIdleEventHandler( Dialog_Main__Auto_Base_Class::m_splitter2OnIdle ), NULL, this );
		}

};

///////////////////////////////////////////////////////////////////////////////
/// Class Dialog_About__Auto_Base_Class
///////////////////////////////////////////////////////////////////////////////
class Dialog_About__Auto_Base_Class : public wxDialog
{
	private:

	protected:
		wxStaticBitmap* m_bitmap3;
		wxStaticText* m_staticText13;
		wxStaticText* m_staticText14;
		wxStaticText* m_staticText15;
		wxStaticText* m_staticText26;
		wxStaticText* m_staticText16;
		wxButton* dlgAbout_btnClose;

		// Virtual event handlers, override them in your derived class
		virtual void dlgAbout_OnClose( wxCloseEvent& event ) { event.Skip(); }
		virtual void dlgAbout_btnClose_OnButtonClick( wxCommandEvent& event ) { event.Skip(); }


	public:

		Dialog_About__Auto_Base_Class( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("About Paper Kernel C++"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 580,369 ), long style = wxDEFAULT_DIALOG_STYLE );

		~Dialog_About__Auto_Base_Class();

};

