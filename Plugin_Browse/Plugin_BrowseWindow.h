#ifndef _PLUGIN_BROWSEWINDOW_H_
#define _PLUGIN_BROWSEWINDOW_H_

#include <wx/textctrl.h>

#include <wxGuiPluginWindowBase.h>

#define ID_PLUGIN_BROWSEWINDOW 10000
#define ID_SAMPLE_TEXTCTRL 10001
#define ID_SEND_EVENT_BUTTON 10002
#define SYMBOL_PLUGIN_BROWSEWINDOW_STYLE wxTAB_TRAVERSAL
#define SYMBOL_PLUGIN_BROWSEWINDOW_TITLE _("Plugin_BrowseWindow")
#define SYMBOL_PLUGIN_BROWSEWINDOW_IDNAME ID_PLUGIN_BROWSEWINDOW
#define SYMBOL_PLUGIN_BROWSEWINDOW_SIZE wxSize(400, 300)
#define SYMBOL_PLUGIN_BROWSEWINDOW_POSITION wxDefaultPosition

class Plugin_BrowseWindow : public wxGuiPluginWindowBase {

    DECLARE_DYNAMIC_CLASS( Plugin_BrowseWindow )
    DECLARE_EVENT_TABLE()

    wxTextCtrl *m_SamppleTextCtrl;

public:

    Plugin_BrowseWindow();
    Plugin_BrowseWindow(wxGuiPluginBase * plugin, wxWindow* parent, wxWindowID id = SYMBOL_PLUGIN_BROWSEWINDOW_IDNAME, const wxPoint& pos = SYMBOL_PLUGIN_BROWSEWINDOW_POSITION, const wxSize& size = SYMBOL_PLUGIN_BROWSEWINDOW_SIZE, long style = SYMBOL_PLUGIN_BROWSEWINDOW_STYLE );

    bool Create(wxGuiPluginBase * plugin, wxWindow* parent, wxWindowID id = SYMBOL_PLUGIN_BROWSEWINDOW_IDNAME, const wxPoint& pos = SYMBOL_PLUGIN_BROWSEWINDOW_POSITION, const wxSize& size = SYMBOL_PLUGIN_BROWSEWINDOW_SIZE, long style = SYMBOL_PLUGIN_BROWSEWINDOW_STYLE );

    ~Plugin_BrowseWindow();

    void Init();

    void CreateControls();

    void OnSENDEVENTBUTTONClick( wxCommandEvent& event );

    wxBitmap GetBitmapResource( const wxString& name );

    wxIcon GetIconResource( const wxString& name );

    static bool ShowToolTips();
};

#endif
