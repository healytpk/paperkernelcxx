#ifndef _PLUGIN_SEARCHWINDOW_H_
#define _PLUGIN_SEARCHWINDOW_H_

#include <wx/textctrl.h>

#include <wxGuiPluginWindowBase.h>

#define ID_PLUGIN_SEARCHWINDOW 10000
#define ID_GUI_PLUGIN2_MESSAGE_TEXTCTRL 10001
#define SYMBOL_PLUGIN_SEARCHWINDOW_STYLE wxTAB_TRAVERSAL
#define SYMBOL_PLUGIN_SEARCHWINDOW_TITLE _("Plugin_SearchWindow")
#define SYMBOL_PLUGIN_SEARCHWINDOW_IDNAME ID_PLUGIN_SEARCHWINDOW
#define SYMBOL_PLUGIN_SEARCHWINDOW_SIZE wxSize(400, 300)
#define SYMBOL_PLUGIN_SEARCHWINDOW_POSITION wxDefaultPosition

class Plugin_SearchWindow: public wxGuiPluginWindowBase {

    DECLARE_DYNAMIC_CLASS( Plugin_SearchWindow )
    DECLARE_EVENT_TABLE()

    wxTextCtrl* m_MessageTextCtrl;

public:

    Plugin_SearchWindow();
    Plugin_SearchWindow(wxGuiPluginBase * plugin, wxWindow* parent, wxWindowID id = SYMBOL_PLUGIN_SEARCHWINDOW_IDNAME, const wxPoint& pos = SYMBOL_PLUGIN_SEARCHWINDOW_POSITION, const wxSize& size = SYMBOL_PLUGIN_SEARCHWINDOW_SIZE, long style = SYMBOL_PLUGIN_SEARCHWINDOW_STYLE );

    bool Create(wxGuiPluginBase * plugin, wxWindow* parent, wxWindowID id = SYMBOL_PLUGIN_SEARCHWINDOW_IDNAME, const wxPoint& pos = SYMBOL_PLUGIN_SEARCHWINDOW_POSITION, const wxSize& size = SYMBOL_PLUGIN_SEARCHWINDOW_SIZE, long style = SYMBOL_PLUGIN_SEARCHWINDOW_STYLE );

    ~Plugin_SearchWindow();

    void Init();

    void CreateControls();

    wxBitmap GetBitmapResource( const wxString& name );

    wxIcon GetIconResource( const wxString& name );

    void OnDestroy( wxWindowDestroyEvent& event );
    void OnInteropMessageReceived(wxCommandEvent & event);

    static bool ShowToolTips();
};

#endif
