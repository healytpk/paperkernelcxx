#pragma once

#include <wx/string.h>                          // wxString
#include <wx/window.h>                          // wxWindow
#include <wx/webview.h>                         // wxWebView

wxWindow *CreateViewPortal(wxWindow *parent) noexcept(false);

void SetViewPortal(wxWindow*, wxString const &) noexcept(false);

template<class T>
void ViewPortal_BindFinishedLoading( wxWindow *const pw, void (T::*const mfp)(wxEvent&), T *const pdlg ) noexcept(false)
{
    pw->Bind(wxEVT_WEBVIEW_LOADED, mfp, pdlg);
}
