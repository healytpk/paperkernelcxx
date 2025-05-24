#ifndef _SAMPLEGUIPLUGINWINDOW1_H_
#define _SAMPLEGUIPLUGINWINDOW1_H_

#include <wx/textctrl.h>

#include <wxGuiPluginWindowBase.h>

#define ID_SAMPLEGUIPLUGINWINDOW1 10000
#define ID_SAMPLE_TEXTCTRL 10001
#define ID_SEND_EVENT_BUTTON 10002
#define SYMBOL_SAMPLEGUIPLUGINWINDOW1_STYLE wxTAB_TRAVERSAL
#define SYMBOL_SAMPLEGUIPLUGINWINDOW1_TITLE _("Plugin_BrowseWindow")
#define SYMBOL_SAMPLEGUIPLUGINWINDOW1_IDNAME ID_SAMPLEGUIPLUGINWINDOW1
#define SYMBOL_SAMPLEGUIPLUGINWINDOW1_SIZE wxSize(400, 300)
#define SYMBOL_SAMPLEGUIPLUGINWINDOW1_POSITION wxDefaultPosition

class Plugin_BrowseWindow : public wxGuiPluginWindowBase {

    DECLARE_DYNAMIC_CLASS( Plugin_BrowseWindow )
    DECLARE_EVENT_TABLE()

    wxTextCtrl *m_SamppleTextCtrl;

public:

    Plugin_BrowseWindow();
    Plugin_BrowseWindow(wxGuiPluginBase * plugin, wxWindow* parent, wxWindowID id = SYMBOL_SAMPLEGUIPLUGINWINDOW1_IDNAME, const wxPoint& pos = SYMBOL_SAMPLEGUIPLUGINWINDOW1_POSITION, const wxSize& size = SYMBOL_SAMPLEGUIPLUGINWINDOW1_SIZE, long style = SYMBOL_SAMPLEGUIPLUGINWINDOW1_STYLE );

    bool Create(wxGuiPluginBase * plugin, wxWindow* parent, wxWindowID id = SYMBOL_SAMPLEGUIPLUGINWINDOW1_IDNAME, const wxPoint& pos = SYMBOL_SAMPLEGUIPLUGINWINDOW1_POSITION, const wxSize& size = SYMBOL_SAMPLEGUIPLUGINWINDOW1_SIZE, long style = SYMBOL_SAMPLEGUIPLUGINWINDOW1_STYLE );

    ~Plugin_BrowseWindow();

    void Init();

    void CreateControls();

    void OnSENDEVENTBUTTONClick( wxCommandEvent& event );

    wxBitmap GetBitmapResource( const wxString& name );

    wxIcon GetIconResource( const wxString& name );

    static bool ShowToolTips();
};

#endif
