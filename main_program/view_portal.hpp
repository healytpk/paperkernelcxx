#pragma once

#include <wx/string.h>                          // wxString
#include <wx/webview.h>                         // wxWebView, wxEVT_WEBVIEW_LOADED

class wxWindow;
class LocalHttpServer;

class ViewPortalManager {
protected:
    wxString str_current_paper;
    LocalHttpServer *phttp = nullptr;
    wxWebView *w = nullptr;
public:
    wxString GetCurrentPaper(void) const noexcept { return this->str_current_paper; }
    wxWindow *Create(wxWindow *parent, LocalHttpServer &server) noexcept;
    void Set(wxString const &paper_name) noexcept;
    void SetHtml(wxString const &html, wxString const &baseUrl) noexcept;

    template<class T>
    void BindFinishedLoading( void (T::*const mfp)(wxEvent&), T *const parent ) noexcept(false)
    {
        assert( nullptr != this->w );
        this->w->Bind(wxEVT_WEBVIEW_LOADED, mfp, parent);
    }
};
