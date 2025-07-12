#include "view_portal.hpp"
#include <cstdio>                    // fprintf, stderr
#include "local_http_server.hpp"

using std::uint16_t;

static LocalHttpServer *g_p_local_http_server = nullptr;

wxWindow *ViewPortal_Create(wxWindow *const parent, LocalHttpServer &server) noexcept
{
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
        uint16_t const port = g_p_local_http_server->GetListeningPort();
        assert( (0u != port) && (-1 != port) );
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
        std::fprintf(  stderr, "ViewPortal_Set exception thrown: %s\n", e.what()  );
    }
}
