#include "view_portal.hpp"
#include <cstdio>                    // fprintf, stderr
#include "local_http_server.hpp"

#ifdef __WXGTK__
#   include <stdlib.h>               // setenv
#endif

using std::uint16_t;

static LocalHttpServer *g_p_local_http_server = nullptr;

wxWindow *ViewPortal_Create(wxWindow *const parent, LocalHttpServer &server) noexcept
{
#ifdef __WXGTK__
    setenv("WEBKIT_DISABLE_COMPOSITING_MODE", "1", 1);
#endif

    g_p_local_http_server = &server;

try
{
#if defined(_WIN32) && defined(wxUSE_WEBVIEW_EDGE)
#   if wxUSE_WEBVIEW_EDGE
      if ( wxWebView::IsBackendAvailable(wxWebViewBackendEdge) )
      {
        return wxWebView::New(parent, wxID_ANY, wxWebViewDefaultURLStr, wxDefaultPosition, wxDefaultSize, wxWebViewBackendEdge);
      }
#   endif
#endif

    return wxWebView::New(parent, wxID_ANY);
} catch(...){}

    return nullptr;
}

void ViewPortal_Set(wxWindow *const arg, wxString const &paper_name) noexcept
{
    try
    {
        assert( nullptr != g_p_local_http_server );
        if ( false == g_p_local_http_server->IsListening() )
        {
            std::fprintf(stderr, "Local HTTP server is not listening\n");
            return;
        }
        uint16_t const port = g_p_local_http_server->StartAccepting();
        assert( (0u != port) && (0xFFFF != port) );
        wxString url;
        if ( g_p_local_http_server->IsUsingIPv6() ) url = wxS("http://[::1]:");
        /************************************/ else url = wxS("http://127.0.0.1:");
        url << port << wxS("/") << paper_name;
        wxWebView *const pwv = dynamic_cast<wxWebView*>(arg);
        assert( nullptr != pwv );
        pwv->LoadURL(url);
    }
    catch (std::exception const &e)
    {
        std::fprintf( stderr, "ViewPortal_Set exception thrown: %s\n", e.what() );
    }
}
