#include "wx/wx.h"
#include "Plugin_SearchWindow.h"
#include <wxGuiPluginBase.h>
#include "host_interaction.hpp"

IMPLEMENT_DYNAMIC_CLASS( Plugin_SearchWindow, wxGuiPluginWindowBase )

BEGIN_EVENT_TABLE( Plugin_SearchWindow, wxGuiPluginWindowBase )
    EVT_WINDOW_DESTROY( Plugin_SearchWindow::OnDestroy )
END_EVENT_TABLE()

Plugin_SearchWindow::Plugin_SearchWindow()
{
    Init();
}

Plugin_SearchWindow::Plugin_SearchWindow( wxGuiPluginBase * plugin, wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
{
    Init();
    Create(plugin, parent, id, pos, size, style);
}

bool Plugin_SearchWindow::Create( wxGuiPluginBase * plugin, wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
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

Plugin_SearchWindow::~Plugin_SearchWindow()
{

}

void Plugin_SearchWindow::Init()
{
    m_MessageTextCtrl = nullptr;
}

void Plugin_SearchWindow::CreateControls()
{
    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    this->SetSizer(itemBoxSizer2);

    wxStaticText* itemStaticText3 = new wxStaticText( this, wxID_STATIC, _("This text box receives messages from GUI Plugin 1:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer2->Add(itemStaticText3, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP, 5);

    m_MessageTextCtrl = new wxTextCtrl( this, ID_GUI_PLUGIN2_MESSAGE_TEXTCTRL, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
    itemBoxSizer2->Add(m_MessageTextCtrl, 0, wxGROW|wxALL, 5);

    // Connect events and objects
    this->Connect(ID_PLUGIN_SEARCHWINDOW, wxEVT_DESTROY, wxWindowDestroyEventHandler(Plugin_SearchWindow::OnDestroy), NULL, this);

    GetPlugin()->GetEventHandler()->Bind( hostapi->wxEVT_GUI_PLUGIN_INTEROP, wxCommandEventHandler(Plugin_SearchWindow::OnInteropMessageReceived), this );
}

bool Plugin_SearchWindow::ShowToolTips()
{
    return true;
}

wxBitmap Plugin_SearchWindow::GetBitmapResource( const wxString& /*name*/ )
{
    return wxNullBitmap;
}

wxIcon Plugin_SearchWindow::GetIconResource( const wxString& /*name*/ )
{
    return wxNullIcon;
}

void Plugin_SearchWindow::OnInteropMessageReceived(wxCommandEvent & event)
{
    m_MessageTextCtrl->SetValue(event.GetString());
}

void Plugin_SearchWindow::OnDestroy( wxWindowDestroyEvent& event )
{
    GetPlugin()->GetEventHandler()->Unbind( hostapi->wxEVT_GUI_PLUGIN_INTEROP, wxCommandEventHandler(Plugin_SearchWindow::OnInteropMessageReceived), this );
    event.Skip();
}
