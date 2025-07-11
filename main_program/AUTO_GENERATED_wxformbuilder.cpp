///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version 4.2.1-0-g80c4cb6)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "AUTO_GENERATED_wxformbuilder.h"

#include "../pictures/left_pane.ico.h"
#include "../pictures/right_pane.ico.h"

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

	m_toolBar1 = new wxToolBar( panelBrowse, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_DEFAULT_STYLE|wxTB_NO_TOOLTIPS );
	m_toolBar1->SetToolBitmapSize( wxSize( 18,18 ) );
	m_toolBar1->SetMargins( wxSize( 36,36 ) );
	toolShowPaperTree = m_toolBar1->AddTool( wxID_ANY, wxT("Show List of Papers"), left_pane_ico_to_wx_bitmap(), wxNullBitmap, wxITEM_CHECK, wxT("Show List of Papers"), wxEmptyString, NULL );

	toolShowViewPortal = m_toolBar1->AddTool( wxID_ANY, wxT("Show Paper"), right_pane_ico_to_wx_bitmap(), wxNullBitmap, wxITEM_CHECK, wxT("Show Paper"), wxEmptyString, NULL );

	m_toolBar1->Realize();

	bSizer8->Add( m_toolBar1, 0, wxEXPAND, 5 );

	bSizerForPanelBrowse = new wxBoxSizer( wxHORIZONTAL );


	bSizer8->Add( bSizerForPanelBrowse, 1, wxEXPAND, 5 );


	panelBrowse->SetSizer( bSizer8 );
	panelBrowse->Layout();
	bSizer8->Fit( panelBrowse );
	m_notebook1->AddPage( panelBrowse, wxT("Browse All Papers"), true );
	panelAuthors = new wxPanel( m_notebook1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer12;
	bSizer12 = new wxBoxSizer( wxVERTICAL );

	m_splitter2 = new wxSplitterWindow( panelAuthors, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_3D );
	m_splitter2->Connect( wxEVT_IDLE, wxIdleEventHandler( Dialog_Main__Auto_Base_Class::m_splitter2OnIdle ), NULL, this );

	m_panel5 = new wxPanel( m_splitter2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer15;
	bSizer15 = new wxBoxSizer( wxVERTICAL );

	listAuthors = new wxListCtrl( m_panel5, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT );
	bSizer15->Add( listAuthors, 1, wxALL|wxEXPAND, 5 );


	m_panel5->SetSizer( bSizer15 );
	m_panel5->Layout();
	bSizer15->Fit( m_panel5 );
	m_panel6 = new wxPanel( m_splitter2, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer16;
	bSizer16 = new wxBoxSizer( wxVERTICAL );

	treeAuthorPapers = new wxDataViewCtrl( m_panel6, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer16->Add( treeAuthorPapers, 1, wxALL|wxEXPAND, 5 );


	m_panel6->SetSizer( bSizer16 );
	m_panel6->Layout();
	bSizer16->Fit( m_panel6 );
	m_splitter2->SplitVertically( m_panel5, m_panel6, 0 );
	bSizer12->Add( m_splitter2, 1, wxEXPAND, 5 );


	panelAuthors->SetSizer( bSizer12 );
	panelAuthors->Layout();
	bSizer12->Fit( panelAuthors );
	m_notebook1->AddPage( panelAuthors, wxT("Authors"), false );
	panelXapian = new wxPanel( m_notebook1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer61;
	bSizer61 = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bSizer41;
	bSizer41 = new wxBoxSizer( wxHORIZONTAL );

	btnXapianLoadPapers = new wxButton( panelXapian, wxID_ANY, wxT("Load Papers"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer41->Add( btnXapianLoadPapers, 0, wxALL, 5 );

	btnXapianUnloadPapers = new wxButton( panelXapian, wxID_ANY, wxT("Unload Papers"), wxDefaultPosition, wxDefaultSize, 0 );
	btnXapianUnloadPapers->Enable( false );

	bSizer41->Add( btnXapianUnloadPapers, 0, wxALL, 5 );


	bSizer61->Add( bSizer41, 0, wxALIGN_CENTER_HORIZONTAL, 5 );

	wxBoxSizer* bSizer11;
	bSizer11 = new wxBoxSizer( wxVERTICAL );

	m_staticText4 = new wxStaticText( panelXapian, wxID_ANY, wxT("Search for:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText4->Wrap( -1 );
	bSizer11->Add( m_staticText4, 0, wxALL, 5 );

	txtXapianSearchFor = new wxTextCtrl( panelXapian, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer11->Add( txtXapianSearchFor, 0, wxALL|wxEXPAND, 5 );

	btnXapianSearch = new wxButton( panelXapian, wxID_ANY, wxT("Search"), wxDefaultPosition, wxDefaultSize, 0 );
	btnXapianSearch->Enable( false );

	bSizer11->Add( btnXapianSearch, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );


	bSizer11->Add( 0, 0, 0, 0, 5 );

	m_staticText5 = new wxStaticText( panelXapian, wxID_ANY, wxT("Results:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText5->Wrap( -1 );
	bSizer11->Add( m_staticText5, 0, wxALL, 5 );

	listXapianResults = new wxListCtrl( panelXapian, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT|wxLC_SINGLE_SEL );
	bSizer11->Add( listXapianResults, 1, wxALL|wxEXPAND, 5 );


	bSizer61->Add( bSizer11, 1, wxEXPAND, 5 );


	panelXapian->SetSizer( bSizer61 );
	panelXapian->Layout();
	bSizer61->Fit( panelXapian );
	m_notebook1->AddPage( panelXapian, wxT("Semantic Search"), false );
	panelAI = new wxPanel( m_notebook1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxHORIZONTAL );

	AI_btnLoadModel = new wxButton( panelAI, wxID_ANY, wxT("Load Model"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer6->Add( AI_btnLoadModel, 0, wxALL, 5 );

	AI_btnUnloadModel = new wxButton( panelAI, wxID_ANY, wxT("Unload Model"), wxDefaultPosition, wxDefaultSize, 0 );
	AI_btnUnloadModel->Enable( false );

	bSizer6->Add( AI_btnUnloadModel, 0, wxALL, 5 );


	bSizer3->Add( bSizer6, 0, wxALIGN_CENTER_HORIZONTAL, 5 );

	m_staticline1 = new wxStaticLine( panelAI, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer3->Add( m_staticline1, 0, wxEXPAND | wxALL, 5 );

	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxHORIZONTAL );

	AI_btnLoadPapers = new wxButton( panelAI, wxID_ANY, wxT("Load Papers"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer4->Add( AI_btnLoadPapers, 0, wxALL, 5 );

	AI_btnUnloadPapers = new wxButton( panelAI, wxID_ANY, wxT("Unload Papers"), wxDefaultPosition, wxDefaultSize, 0 );
	AI_btnUnloadPapers->Enable( false );

	bSizer4->Add( AI_btnUnloadPapers, 0, wxALL, 5 );


	bSizer3->Add( bSizer4, 0, wxALIGN_CENTER_HORIZONTAL, 5 );

	m_staticline2 = new wxStaticLine( panelAI, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer3->Add( m_staticline2, 0, wxEXPAND | wxALL, 5 );

	m_staticText1 = new wxStaticText( panelAI, wxID_ANY, wxT("Question to whittle down list of papers (must be a Yes/No question):"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	bSizer3->Add( m_staticText1, 0, wxALL, 5 );

	AI_txtBooleanQuestion = new wxTextCtrl( panelAI, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( -1,100 ), wxTE_MULTILINE );
	bSizer3->Add( AI_txtBooleanQuestion, 0, wxALL|wxEXPAND, 5 );

	AI_btnWhittleDownPapers = new wxButton( panelAI, wxID_ANY, wxT("Next"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer3->Add( AI_btnWhittleDownPapers, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );

	AI_lcPapers = new wxListCtrl( panelAI, wxID_ANY, wxDefaultPosition, wxSize( -1,200 ), wxLC_REPORT );
	bSizer3->Add( AI_lcPapers, 0, wxALL|wxEXPAND, 5 );

	m_staticText2 = new wxStaticText( panelAI, wxID_ANY, wxT("More detailed question to ask for each of the above papers:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	bSizer3->Add( m_staticText2, 0, wxALL, 5 );

	AI_txtDetailedQuestion = new wxTextCtrl( panelAI, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( -1,80 ), wxTE_MULTILINE );
	bSizer3->Add( AI_txtDetailedQuestion, 0, wxALL|wxEXPAND, 5 );

	AI_btnAnalysePapers = new wxButton( panelAI, wxID_ANY, wxT("Ask"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer3->Add( AI_btnAnalysePapers, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );

	m_staticText3 = new wxStaticText( panelAI, wxID_ANY, wxT("Responses:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText3->Wrap( -1 );
	bSizer3->Add( m_staticText3, 0, wxALL, 5 );

	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxHORIZONTAL );

	AI_listPapers = new wxListBox( panelAI, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 );
	bSizer2->Add( AI_listPapers, 1, wxALL|wxEXPAND, 5 );

	AI_txtDetailedAnswer = new wxTextCtrl( panelAI, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE );
	bSizer2->Add( AI_txtDetailedAnswer, 1, wxALL|wxEXPAND, 5 );


	bSizer3->Add( bSizer2, 1, wxEXPAND, 5 );


	panelAI->SetSizer( bSizer3 );
	panelAI->Layout();
	bSizer3->Fit( panelAI );
	m_notebook1->AddPage( panelAI, wxT("Artificial Intelligence"), false );
	panelDebug = new wxPanel( m_notebook1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer151;
	bSizer151 = new wxBoxSizer( wxVERTICAL );

	panelDebug_Notebook = new wxNotebook( panelDebug, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	panelDebug_panelInfo = new wxPanel( panelDebug_Notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer161;
	bSizer161 = new wxBoxSizer( wxVERTICAL );

	labelDebug_Info = new wxStaticText( panelDebug_panelInfo, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	labelDebug_Info->Wrap( -1 );
	bSizer161->Add( labelDebug_Info, 1, wxALL|wxEXPAND, 5 );

	btnDebug_Refresh = new wxButton( panelDebug_panelInfo, wxID_ANY, wxT("Refresh"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer161->Add( btnDebug_Refresh, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );


	panelDebug_panelInfo->SetSizer( bSizer161 );
	panelDebug_panelInfo->Layout();
	bSizer161->Fit( panelDebug_panelInfo );
	panelDebug_Notebook->AddPage( panelDebug_panelInfo, wxT("Information"), true );
	panelDebug_panelNames = new wxPanel( panelDebug_Notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer17;
	bSizer17 = new wxBoxSizer( wxVERTICAL );

	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer1->AddGrowableCol( 0 );
	fgSizer1->AddGrowableCol( 1 );
	fgSizer1->AddGrowableRow( 1 );
	fgSizer1->SetFlexibleDirection( wxVERTICAL );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_ALL );

	m_staticText7 = new wxStaticText( panelDebug_panelNames, wxID_ANY, wxT("Primary Name:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText7->Wrap( -1 );
	fgSizer1->Add( m_staticText7, 0, wxALL, 5 );

	m_staticText8 = new wxStaticText( panelDebug_panelNames, wxID_ANY, wxT("All alternative forms for selected primary name:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText8->Wrap( -1 );
	fgSizer1->Add( m_staticText8, 0, wxALL, 5 );

	panelDebug_panelNames_listPrimary = new wxListBox( panelDebug_panelNames, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 );
	fgSizer1->Add( panelDebug_panelNames_listPrimary, 1, wxALL|wxEXPAND, 5 );

	panelDebug_panelNames_listAlternative = new wxListBox( panelDebug_panelNames, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 );
	fgSizer1->Add( panelDebug_panelNames_listAlternative, 1, wxALL|wxEXPAND, 5 );


	bSizer17->Add( fgSizer1, 1, wxEXPAND, 5 );

	m_staticText9 = new wxStaticText( panelDebug_panelNames, wxID_ANY, wxT("Mappings of alternatives to primaries:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText9->Wrap( -1 );
	bSizer17->Add( m_staticText9, 0, wxALL, 5 );

	panelDebug_panelNames_listctrlMappings = new wxListCtrl( panelDebug_panelNames, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT|wxLC_SINGLE_SEL );
	bSizer17->Add( panelDebug_panelNames_listctrlMappings, 1, wxALL|wxEXPAND, 5 );


	panelDebug_panelNames->SetSizer( bSizer17 );
	panelDebug_panelNames->Layout();
	bSizer17->Fit( panelDebug_panelNames );
	panelDebug_Notebook->AddPage( panelDebug_panelNames, wxT("Names"), false );
	panelDebug_panelHash = new wxPanel( panelDebug_Notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer18;
	bSizer18 = new wxBoxSizer( wxVERTICAL );

	m_staticText10 = new wxStaticText( panelDebug_panelHash, wxID_ANY, wxT("Enter string to hash:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText10->Wrap( -1 );
	bSizer18->Add( m_staticText10, 0, wxALL, 5 );

	panelDebug_panelHash_txtInputHash = new wxTextCtrl( panelDebug_panelHash, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer18->Add( panelDebug_panelHash_txtInputHash, 0, wxALL|wxEXPAND, 5 );

	m_staticText11 = new wxStaticText( panelDebug_panelHash, wxID_ANY, wxT("(non-ASCII characters must be escaped as Unicode,\n  e.g. 'Tomás' should be  'Tom\\u00e1s')"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText11->Wrap( -1 );
	bSizer18->Add( m_staticText11, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );

	wxBoxSizer* bSizer19;
	bSizer19 = new wxBoxSizer( wxHORIZONTAL );

	m_staticText12 = new wxStaticText( panelDebug_panelHash, wxID_ANY, wxT("Computed 64-Bit hash digest:    0x"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText12->Wrap( -1 );
	bSizer19->Add( m_staticText12, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	panelDebug_panelHash_txtOutputHash = new wxTextCtrl( panelDebug_panelHash, wxID_ANY, wxT("0000000000000000"), wxDefaultPosition, wxSize( 200,-1 ), wxTE_READONLY );
	bSizer19->Add( panelDebug_panelHash_txtOutputHash, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );


	bSizer18->Add( bSizer19, 0, wxEXPAND, 5 );

	panelDebug_panelHash_btnCheckForCollisions = new wxButton( panelDebug_panelHash, wxID_ANY, wxT("Check for Collisions"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer18->Add( panelDebug_panelHash_btnCheckForCollisions, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );

	panelDebug_panelHash_txtSummary = new wxTextCtrl( panelDebug_panelHash, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY );
	bSizer18->Add( panelDebug_panelHash_txtSummary, 1, wxALL|wxEXPAND, 5 );


	panelDebug_panelHash->SetSizer( bSizer18 );
	panelDebug_panelHash->Layout();
	bSizer18->Fit( panelDebug_panelHash );
	panelDebug_Notebook->AddPage( panelDebug_panelHash, wxT("Hash"), false );

	bSizer151->Add( panelDebug_Notebook, 1, wxEXPAND | wxALL, 5 );


	panelDebug->SetSizer( bSizer151 );
	panelDebug->Layout();
	bSizer151->Fit( panelDebug );
	m_notebook1->AddPage( panelDebug, wxT("Debug"), false );

	bSizer1->Add( m_notebook1, 1, wxEXPAND | wxALL, 5 );


	this->SetSizer( bSizer1 );
	this->Layout();

	this->Centre( wxBOTH );

	// Connect Events
	this->Connect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( Dialog_Main__Auto_Base_Class::OnClose ) );
	m_notebook1->Connect( wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED, wxNotebookEventHandler( Dialog_Main__Auto_Base_Class::m_notebook1_OnNotebookPageChanged ), NULL, this );
	this->Connect( toolShowPaperTree->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( Dialog_Main__Auto_Base_Class::OnTool_ShowPaperTree ) );
	this->Connect( toolShowViewPortal->GetId(), wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler( Dialog_Main__Auto_Base_Class::OnTool_ShowViewPortal ) );
	listAuthors->Connect( wxEVT_COMMAND_LIST_ITEM_RIGHT_CLICK, wxListEventHandler( Dialog_Main__Auto_Base_Class::listAuthors_OnListItemRightClick ), NULL, this );
	listAuthors->Connect( wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler( Dialog_Main__Auto_Base_Class::listAuthors_OnListItemSelected ), NULL, this );
	treeAuthorPapers->Connect( wxEVT_COMMAND_DATAVIEW_ITEM_ACTIVATED, wxDataViewEventHandler( Dialog_Main__Auto_Base_Class::treeAuthorPapers_OnDataViewCtrlItemActivated ), NULL, this );
	btnXapianLoadPapers->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( Dialog_Main__Auto_Base_Class::btnXapianLoadPapers_OnButtonClick ), NULL, this );
	btnXapianUnloadPapers->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( Dialog_Main__Auto_Base_Class::btnXapianUnloadPapers_OnButtonClick ), NULL, this );
	btnXapianSearch->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( Dialog_Main__Auto_Base_Class::btnXapianSearch_OnButtonClick ), NULL, this );
	listXapianResults->Connect( wxEVT_COMMAND_LIST_ITEM_ACTIVATED, wxListEventHandler( Dialog_Main__Auto_Base_Class::listXapianResults_OnListItemActivated ), NULL, this );
	AI_btnLoadModel->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( Dialog_Main__Auto_Base_Class::AI_btnLoadModel_OnButtonClick ), NULL, this );
	AI_btnUnloadModel->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( Dialog_Main__Auto_Base_Class::AI_btnUnloadModel_OnButtonClick ), NULL, this );
	AI_btnLoadPapers->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( Dialog_Main__Auto_Base_Class::AI_btnLoadPapers_OnButtonClick ), NULL, this );
	AI_btnUnloadPapers->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( Dialog_Main__Auto_Base_Class::AI_btnUnloadPapers_OnButtonClick ), NULL, this );
	AI_btnWhittleDownPapers->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( Dialog_Main__Auto_Base_Class::AI_btnWhittleDownPapers_OnButtonClick ), NULL, this );
	AI_btnAnalysePapers->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( Dialog_Main__Auto_Base_Class::AI_btnAnalysePapers_OnButtonClick ), NULL, this );
	panelDebug_Notebook->Connect( wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED, wxNotebookEventHandler( Dialog_Main__Auto_Base_Class::panelDebug_Notebook_OnPageChanged ), NULL, this );
	btnDebug_Refresh->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( Dialog_Main__Auto_Base_Class::btnDebug_Refresh_OnButtonClick ), NULL, this );
	panelDebug_panelNames_listPrimary->Connect( wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( Dialog_Main__Auto_Base_Class::panelDebug_panelNames_listPrimary_OnListBox ), NULL, this );
	panelDebug_panelHash_txtInputHash->Connect( wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( Dialog_Main__Auto_Base_Class::panelDebug_panelHash_txtInputHash_OnText ), NULL, this );
	panelDebug_panelHash_btnCheckForCollisions->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( Dialog_Main__Auto_Base_Class::panelDebug_panelHash_btnCheckForCollisions_OnButtonClick ), NULL, this );
}

Dialog_Main__Auto_Base_Class::~Dialog_Main__Auto_Base_Class()
{
}
