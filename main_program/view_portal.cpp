#include "view_portal.hpp"
#include <cstdio>                    // fprintf, stderr
#include <chrono>                    // steady_clock, seconds
#ifdef __WXGTK__
#   include <stdlib.h>               // setenv
#endif
#include <wx/window.h>               // wxWindow
#include "wxApp.hpp"                 // wxGetApp
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
        uint16_t const port = this->phttp->GetListeningPort();
        if ( (0u == port) || (_Max == port) )
        {
            std::fputs("Local HTTP server is listening on an invalid TCP port number\n", stderr);
            return;
        }
        wxString url;
        if ( this->phttp->IsUsingIPv6() ) url = wxS("http://[::1]:");
        /**************************/ else url = wxS("http://127.0.0.1:");
        url << port << wxS("/") << paper_name;
        this->phttp->DiscardAllPendingConnections();
        assert( nullptr != this->w );
        this->w->LoadURL(url);

        auto const timestamp = std::chrono::steady_clock::now();
        for (; /* ever */ ;)
        {
            wxGetApp().SafeYield(nullptr, false);
            if ( (std::chrono::steady_clock::now() - timestamp) > std::chrono::seconds(10u) ) break;
            if ( this->phttp->TryServeWebpage() ) break;
        }
    }
    catch (std::exception const &e)
    {
        std::fprintf( stderr, "ViewPortal_Set exception thrown: %s\n", e.what() );
    }
}
