#include "view_portal.hpp"

wxWindow *ViewPortal_Create(wxWindow *const parent) noexcept(false)
{
#ifdef __WXMSW__
    //return wxWebView::New(parent, wxID_ANY, wxWebViewDefaultURLStr, wxDefaultPosition, wxDefaultSize, wxWebViewBackendEdge);
#endif

    return wxWebView::New(parent, wxID_ANY);
}

void ViewPortal_Set(wxWindow *const arg, wxString const &data) noexcept(false)
{
    wxWebView *const p = dynamic_cast<wxWebView*>(arg);
    if ( nullptr == p ) return;
    p->SetPage(data, wxEmptyString);
}
