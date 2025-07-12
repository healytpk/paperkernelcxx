#pragma once

#include <wx/string.h>                          // wxString
#include <wx/window.h>                          // wxWindow
#include <wx/webview.h>                         // wxWebView
#include "local_http_server.hpp"                // LocalHttpServer

wxWindow *ViewPortal_Create(wxWindow *parent, LocalHttpServer &server) noexcept;

void ViewPortal_Set(wxWindow*, wxString const &paper_name) noexcept;

template<class T>
void ViewPortal_BindFinishedLoading( wxWindow *const pw, void (T::*const mfp)(wxEvent&), T *const pdlg ) noexcept(false)
{
    pw->Bind(wxEVT_WEBVIEW_LOADED, mfp, pdlg);
}
