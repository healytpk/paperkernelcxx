#include "wx/wx.h"
#include "Plugin_BrowseWindow.h"
#include <wxGuiPluginBase.h>
#include "host_interaction.hpp"

IMPLEMENT_DYNAMIC_CLASS( Plugin_BrowseWindow, wxGuiPluginWindowBase )

BEGIN_EVENT_TABLE( Plugin_BrowseWindow, wxGuiPluginWindowBase )
    EVT_BUTTON( ID_SEND_EVENT_BUTTON, Plugin_BrowseWindow::OnSENDEVENTBUTTONClick )
END_EVENT_TABLE()

Plugin_BrowseWindow::Plugin_BrowseWindow()
{
    Init();
}

Plugin_BrowseWindow::Plugin_BrowseWindow( wxGuiPluginBase * plugin, 
											   wxWindow* parent, wxWindowID id, 
											   const wxPoint& pos, const wxSize& size, 
											   long style )
{
    Init();
    Create(plugin, parent, id, pos, size, style);
}

bool Plugin_BrowseWindow::Create(wxGuiPluginBase * plugin, 
									wxWindow* parent, wxWindowID id, 
									const wxPoint& pos, const wxSize& size, 
									long style )
{
    wxGuiPluginWindowBase::Create(plugin, parent, id, pos, size, style );

    CreateControls();
    if (GetSizer())
    {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
    return true;
}

Plugin_BrowseWindow::~Plugin_BrowseWindow()
{

}

void Plugin_BrowseWindow::Init()
{
    m_SamppleTextCtrl = nullptr;
}

void Plugin_BrowseWindow::CreateControls()
{    
    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(itemBoxSizer2);

    wxStaticText* itemStaticText3 = new wxStaticText( this, wxID_STATIC, _("Enter some text here:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer2->Add(itemStaticText3, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP, 5);

    m_SamppleTextCtrl = new wxTextCtrl( this, ID_SAMPLE_TEXTCTRL, _("Hello, GUI Plugin 2!"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer2->Add(m_SamppleTextCtrl, 0, wxGROW|wxLEFT|wxRIGHT|wxTOP, 5);

    wxButton* itemButton5 = new wxButton( this, ID_SEND_EVENT_BUTTON, _("Send event"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer2->Add(itemButton5, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);
}

bool Plugin_BrowseWindow::ShowToolTips()
{
    return true;
}

wxBitmap Plugin_BrowseWindow::GetBitmapResource( const wxString& /*name*/ )
{
    return wxNullBitmap;
}

wxIcon Plugin_BrowseWindow::GetIconResource( const wxString& /*name*/ )
{
    return wxNullIcon;
}

void Plugin_BrowseWindow::OnSENDEVENTBUTTONClick( wxCommandEvent& event )
{
	wxCommandEvent e( hostapi->wxEVT_GUI_PLUGIN_INTEROP );
	e.SetString(m_SamppleTextCtrl->GetValue());
	GetPlugin()->GetEventHandler()->AddPendingEvent(e);
    event.Skip();
}
