#include "view_portal.hpp"

#include <wx/webview.h>                     // wxWebView

wxWindow *CreateViewPortal(wxWindow *const parent) noexcept(false)
{
    return wxWebView::New(parent, wxID_ANY);
}

void SetViewPortal(wxWindow *const arg, wxString const &data) noexcept(false)
{
    wxWebView *const p = dynamic_cast<wxWebView*>(arg);
    if ( nullptr == p ) return;
    p->SetPage(data, wxEmptyString);
}
