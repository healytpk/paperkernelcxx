///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version 4.2.1-0-g80c4cb6)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#pragma once

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/intl.h>
#include <wx/string.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/gdicmn.h>
#include <wx/toolbar.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/button.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/listctrl.h>
#include <wx/statline.h>
#include <wx/listbox.h>
#include <wx/notebook.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// Class Dialog_Main__Auto_Base_Class
///////////////////////////////////////////////////////////////////////////////
class Dialog_Main__Auto_Base_Class : public wxDialog
{
	private:

	protected:
		wxToolBar* m_toolBar1;
		wxToolBarToolBase* toolShowPaperTree;
		wxToolBarToolBase* toolShowViewPortal;
		wxNotebook* m_notebook1;
		wxPanel* panelBrowse;
		wxBoxSizer* bSizerForPanelBrowse;
		wxPanel* panelXapian;
		wxButton* btnXapianLoadPapers;
		wxButton* btnXapianUnloadPapers;
		wxStaticText* m_staticText4;
		wxTextCtrl* txtXapianSearchFor;
		wxButton* btnXapianSearch;
		wxStaticText* m_staticText5;
		wxListCtrl* listXapianResults;
		wxPanel* panelAI;
		wxButton* btnLoadModel;
		wxButton* btnUnloadModel;
		wxStaticLine* m_staticline1;
		wxButton* btnLoadPapers;
		wxButton* btnUnloadPapers;
		wxStaticLine* m_staticline2;
		wxStaticText* m_staticText1;
		wxTextCtrl* txtBooleanQuestion;
		wxButton* btnWhittleDownPapers;
		wxListCtrl* lcPapers;
		wxStaticText* m_staticText2;
		wxTextCtrl* txtDetailedQuestion;
		wxButton* btnAnalysePapers;
		wxStaticText* m_staticText3;
		wxListBox* listPapers;
		wxTextCtrl* txtDetailedAnswer;

		// Virtual event handlers, override them in your derived class
		virtual void OnClose( wxCloseEvent& event ) { event.Skip(); }
		virtual void OnTool_ShowPaperTree( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnTool_ShowViewPortal( wxCommandEvent& event ) { event.Skip(); }
		virtual void btnXapianLoadPapers_OnButtonClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void btnXapianUnloadPapers_OnButtonClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void btnXapianSearch_OnButtonClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void listXapianResults_OnListItemActivated( wxListEvent& event ) { event.Skip(); }
		virtual void btnLoadModel_OnButtonClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void btnUnloadModel_OnButtonClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void btnLoadPapers_OnButtonClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void btnWhittleDownPapers_OnButtonClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void btnAnalysePapers_OnButtonClick( wxCommandEvent& event ) { event.Skip(); }


	public:

		Dialog_Main__Auto_Base_Class( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Paper Kernel C++"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 815,794 ), long style = wxCAPTION|wxCLOSE_BOX|wxDEFAULT_DIALOG_STYLE|wxMAXIMIZE_BOX|wxMINIMIZE_BOX|wxRESIZE_BORDER );

		~Dialog_Main__Auto_Base_Class();

};

