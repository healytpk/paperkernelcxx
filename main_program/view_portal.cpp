#include "view_portal.hpp"
#include <cstdio>                    // fprintf, stderr
#ifdef __WXGTK__
#   include <stdlib.h>               // setenv
#endif
#include <wx/window.h>               // wxWindow
#include "local_http_server.hpp"     // LocalHttpServer
#include "_Max.hpp"

using std::uint16_t;

wxWindow *ViewPortal::Create(wxWindow *const parent, LocalHttpServer &server) noexcept
{
    assert( nullptr == this->w );

#ifdef __WXGTK__
    setenv("WEBKIT_DISABLE_COMPOSITING_MODE", "1", 1);
#endif

    this->phttp = &server;

try
{
#if defined(_WIN32) && defined(wxUSE_WEBVIEW_EDGE)
#   if wxUSE_WEBVIEW_EDGE
      if ( wxWebView::IsBackendAvailable(wxWebViewBackendEdge) )
      {
        this->w = wxWebView::New(parent, wxID_ANY, wxWebViewDefaultURLStr, wxDefaultPosition, wxDefaultSize, wxWebViewBackendEdge);
      }
#   endif
#endif
}
catch(...)
{
    this->w = nullptr;
}

try
{
    if ( nullptr == this->w ) this->w = wxWebView::New(parent, wxID_ANY);
}
catch(...)
{
    this->w = nullptr;
}

    return this->w;
}

void ViewPortal::Set(wxString const &paper_name) noexcept
{
    this->str_current_paper = paper_name;

    try
    {
        assert( nullptr != this->phttp );
        if ( false == this->phttp->IsListening() )
        {
            std::fputs("Local HTTP server is not listening\n", stderr);
            return;
        }
        uint16_t const port = this->phttp->StartAccepting();
        if ( (0u == port) || (_Max == port) )
        {
            std::fputs("Local HTTP server is not listening on invalid TCP port number\n", stderr);
            return;
        }
        wxString url;
        if ( this->phttp->IsUsingIPv6() ) url = wxS("http://[::1]:");
        /**************************/ else url = wxS("http://127.0.0.1:");
        url << port << wxS("/") << paper_name;
        assert( nullptr != this->w );
        this->w->LoadURL(url);
    }
    catch (std::exception const &e)
    {
        std::fprintf( stderr, "ViewPortal_Set exception thrown: %s\n", e.what() );
    }
}
