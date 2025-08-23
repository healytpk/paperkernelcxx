#pragma once
#include <string>                               // string
#include <wx/string.h>                          // wxString
#include <wx/webview.h>                         // wxWebView, wxEVT_WEBVIEW_LOADED

class wxWindow;
class LocalHttpServer;

class ViewPortalManager {
protected:
    wxString str_current_paper;
    LocalHttpServer *phttp = nullptr;
    wxWebView *w = nullptr;
protected:
    void Load(wxString const &, bool spawn_new_thread = true) noexcept(false);
    void Set_Common(void) noexcept(false);
public:
    wxString GetCurrentPaper(void) const noexcept { return this->str_current_paper; }
    wxWindow *Create(wxWindow *parent, LocalHttpServer &server) noexcept;
    void Set(wxString const &paper_name) noexcept;
    void SetHtml(std::string const &html, bool spawn_new_thread = true) noexcept;

    template<class T>
    void BindFinishedLoading( void (T::*const mfp)(wxEvent&), T *const parent ) noexcept(false)
    {
        assert( nullptr != this->w );
        this->w->Bind(wxEVT_WEBVIEW_LOADED, mfp, parent);
    }
};
