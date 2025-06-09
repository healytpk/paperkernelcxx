///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version 4.2.1-0-g80c4cb6)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "AUTO_GENERATED_wxformbuilder.h"

#include "../pictures/circle.xpm"
#include "../pictures/triangle.xpm"

///////////////////////////////////////////////////////////////////////////

Dialog_Main__Auto_Base_Class::Dialog_Main__Auto_Base_Class( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bSizer111;
	bSizer111 = new wxBoxSizer( wxVERTICAL );


	bSizer1->Add( bSizer111, 1, wxEXPAND, 5 );

	m_notebook1 = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	panelBrowse = new wxPanel( m_notebook1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer8;
	bSizer8 = new wxBoxSizer( wxVERTICAL );

	m_toolBar1 = new wxToolBar( panelBrowse, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_HORIZONTAL );
	toolShowPaperTree = m_toolBar1->AddTool( wxID_ANY, _("Show List of Papers"), wxBitmap( circle_xpm ), wxNullBitmap, wxITEM_CHECK, _("Show List of Papers"), wxEmptyString, NULL );

	toolShowViewPortal = m_toolBar1->AddTool( wxID_ANY, _("Show Paper"), wxBitmap( triangle_xpm ), wxNullBitmap, wxITEM_CHECK, _("Show Paper"), wxEmptyString, NULL );

	m_toolBar1->Realize();

	bSizer8->Add( m_toolBar1, 0, wxEXPAND, 5 );

	bSizerForPanelBrowse = new wxBoxSizer( wxHORIZONTAL );


	bSizer8->Add( bSizerForPanelBrowse, 1, wxEXPAND, 5 );


	panelBrowse->SetSizer( bSizer8 );
	panelBrowse->Layout();
	bSizer8->Fit( panelBrowse );
	m_notebook1->AddPage( panelBrowse, _("Browse All Papers"), true );
	panelAuthors = new wxPanel( m_notebook1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer12;
	bSizer12 = new wxBoxSizer( wxVERTICAL );

	listAuthors = new wxListCtrl( panelAuthors, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT|wxLC_SINGLE_SEL );
	bSizer12->Add( listAuthors, 1, wxALL, 5 );


	panelAuthors->SetSizer( bSizer12 );
	panelAuthors->Layout();
	bSizer12->Fit( panelAuthors );
	m_notebook1->AddPage( panelAuthors, _("Authors"), false );
	panelXapian = new wxPanel( m_notebook1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer61;
	bSizer61 = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bSizer41;
	bSizer41 = new wxBoxSizer( wxHORIZONTAL );

	btnXapianLoadPapers = new wxButton( panelXapian, wxID_ANY, _("Load Papers"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer41->Add( btnXapianLoadPapers, 0, wxALL, 5 );

	btnXapianUnloadPapers = new wxButton( panelXapian, wxID_ANY, _("Unload Papers"), wxDefaultPosition, wxDefaultSize, 0 );
	btnXapianUnloadPapers->Enable( false );

	bSizer41->Add( btnXapianUnloadPapers, 0, wxALL, 5 );


	bSizer61->Add( bSizer41, 0, wxALIGN_CENTER_HORIZONTAL, 5 );

	wxBoxSizer* bSizer11;
	bSizer11 = new wxBoxSizer( wxVERTICAL );

	m_staticText4 = new wxStaticText( panelXapian, wxID_ANY, _("Search for:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText4->Wrap( -1 );
	bSizer11->Add( m_staticText4, 0, wxALL, 5 );

	txtXapianSearchFor = new wxTextCtrl( panelXapian, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer11->Add( txtXapianSearchFor, 0, wxALL|wxEXPAND, 5 );

	btnXapianSearch = new wxButton( panelXapian, wxID_ANY, _("Search"), wxDefaultPosition, wxDefaultSize, 0 );
	btnXapianSearch->Enable( false );

	bSizer11->Add( btnXapianSearch, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );


	bSizer11->Add( 0, 0, 0, 0, 5 );

	m_staticText5 = new wxStaticText( panelXapian, wxID_ANY, _("Results:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText5->Wrap( -1 );
	bSizer11->Add( m_staticText5, 0, wxALL, 5 );

	listXapianResults = new wxListCtrl( panelXapian, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT|wxLC_SINGLE_SEL );
	bSizer11->Add( listXapianResults, 1, wxALL|wxEXPAND, 5 );


	bSizer61->Add( bSizer11, 1, wxEXPAND, 5 );


	panelXapian->SetSizer( bSizer61 );
	panelXapian->Layout();
	bSizer61->Fit( panelXapian );
	m_notebook1->AddPage( panelXapian, _("Xapian"), false );
	panelAI = new wxPanel( m_notebook1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxHORIZONTAL );

	btnLoadModel = new wxButton( panelAI, wxID_ANY, _("Load Model"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer6->Add( btnLoadModel, 0, wxALL, 5 );

	btnUnloadModel = new wxButton( panelAI, wxID_ANY, _("Unload Model"), wxDefaultPosition, wxDefaultSize, 0 );
	btnUnloadModel->Enable( false );

	bSizer6->Add( btnUnloadModel, 0, wxALL, 5 );


	bSizer3->Add( bSizer6, 0, wxALIGN_CENTER_HORIZONTAL, 5 );

	m_staticline1 = new wxStaticLine( panelAI, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer3->Add( m_staticline1, 0, wxEXPAND | wxALL, 5 );

	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxHORIZONTAL );

	btnLoadPapers = new wxButton( panelAI, wxID_ANY, _("Load Papers"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer4->Add( btnLoadPapers, 0, wxALL, 5 );

	btnUnloadPapers = new wxButton( panelAI, wxID_ANY, _("Unload Papers"), wxDefaultPosition, wxDefaultSize, 0 );
	btnUnloadPapers->Enable( false );

	bSizer4->Add( btnUnloadPapers, 0, wxALL, 5 );


	bSizer3->Add( bSizer4, 0, wxALIGN_CENTER_HORIZONTAL, 5 );

	m_staticline2 = new wxStaticLine( panelAI, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer3->Add( m_staticline2, 0, wxEXPAND | wxALL, 5 );

	m_staticText1 = new wxStaticText( panelAI, wxID_ANY, _("Question to whittle down list of papers (must be a Yes/No question):"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	bSizer3->Add( m_staticText1, 0, wxALL, 5 );

	txtBooleanQuestion = new wxTextCtrl( panelAI, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( -1,100 ), wxTE_MULTILINE );
	bSizer3->Add( txtBooleanQuestion, 0, wxALL|wxEXPAND, 5 );

	btnWhittleDownPapers = new wxButton( panelAI, wxID_ANY, _("Next"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer3->Add( btnWhittleDownPapers, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );

	lcPapers = new wxListCtrl( panelAI, wxID_ANY, wxDefaultPosition, wxSize( -1,200 ), wxLC_REPORT );
	bSizer3->Add( lcPapers, 0, wxALL|wxEXPAND, 5 );

	m_staticText2 = new wxStaticText( panelAI, wxID_ANY, _("More detailed question to ask for each of the above papers:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	bSizer3->Add( m_staticText2, 0, wxALL, 5 );

	txtDetailedQuestion = new wxTextCtrl( panelAI, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( -1,80 ), wxTE_MULTILINE );
	bSizer3->Add( txtDetailedQuestion, 0, wxALL|wxEXPAND, 5 );

	btnAnalysePapers = new wxButton( panelAI, wxID_ANY, _("Ask"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer3->Add( btnAnalysePapers, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );

	m_staticText3 = new wxStaticText( panelAI, wxID_ANY, _("Responses:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3->Wrap( -1 );
	bSizer3->Add( m_staticText3, 0, wxALL, 5 );

	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxHORIZONTAL );

	listPapers = new wxListBox( panelAI, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 );
	bSizer2->Add( listPapers, 1, wxALL|wxEXPAND, 5 );

	txtDetailedAnswer = new wxTextCtrl( panelAI, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE );
	bSizer2->Add( txtDetailedAnswer, 1, wxALL|wxEXPAND, 5 );


	bSizer3->Add( bSizer2, 1, wxEXPAND, 5 );


	panelAI->SetSizer( bSizer3 );
	panelAI->Layout();
	bSizer3->Fit( panelAI );
	m_notebook1->AddPage( panelAI, _("AI"), false );

	bSizer1->Add( m_notebook1, 1, wxEXPAND | wxALL, 5 );


	this->SetSizer( bSizer1 );
	this->Layout();

	this->Centre( wxBOTH );

	// Connect Events
	this->Connect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( Dialog_Main__Auto_Base_Class::OnClose ) );
	this->Connect( toolShowPaperTree->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( Dialog_Main__Auto_Base_Class::OnTool_ShowPaperTree ) );
	this->Connect( toolShowViewPortal->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( Dialog_Main__Auto_Base_Class::OnTool_ShowViewPortal ) );
	listAuthors->Connect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( Dialog_Main__Auto_Base_Class::listAuthors_OnListItemActivated ), NULL, this );
	btnXapianLoadPapers->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( Dialog_Main__Auto_Base_Class::btnXapianLoadPapers_OnButtonClick ), NULL, this );
	btnXapianUnloadPapers->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( Dialog_Main__Auto_Base_Class::btnXapianUnloadPapers_OnButtonClick ), NULL, this );
	btnXapianSearch->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( Dialog_Main__Auto_Base_Class::btnXapianSearch_OnButtonClick ), NULL, this );
	listXapianResults->Connect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( Dialog_Main__Auto_Base_Class::listXapianResults_OnListItemActivated ), NULL, this );
	btnLoadModel->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( Dialog_Main__Auto_Base_Class::btnLoadModel_OnButtonClick ), NULL, this );
	btnUnloadModel->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( Dialog_Main__Auto_Base_Class::btnUnloadModel_OnButtonClick ), NULL, this );
	btnLoadPapers->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( Dialog_Main__Auto_Base_Class::btnLoadPapers_OnButtonClick ), NULL, this );
	btnWhittleDownPapers->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( Dialog_Main__Auto_Base_Class::btnWhittleDownPapers_OnButtonClick ), NULL, this );
	btnAnalysePapers->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( Dialog_Main__Auto_Base_Class::btnAnalysePapers_OnButtonClick ), NULL, this );
}

Dialog_Main__Auto_Base_Class::~Dialog_Main__Auto_Base_Class()
{
}
