#include "view_portal.hpp"
#include <cstdio>                    // fprintf, stderr
#include <chrono>                    // steady_clock, seconds
#include <new>                       // 'placement new'
#include <thread>                    // thread
#ifdef __WXGTK__
#   include <stdlib.h>               // setenv
#endif
#include <wx/window.h>               // wxWindow
#include "wxApp.hpp"                 // wxGetApp
#include "local_http_server.hpp"     // LocalHttpServer
#include "_Max.hpp"
#include "Auto.h"

using std::uint16_t;

wxWindow *ViewPortalManager::Create(wxWindow *const parent, LocalHttpServer &server) noexcept
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

void ViewPortalManager::Load(wxString const &arg, bool const spawn_new_thread) noexcept(false)
{
    wxString url;
    if ( this->phttp->IsUsingIPv6() ) url = wxS("http://[::1]:");
    /**************************/ else url = wxS("http://127.0.0.1:");
    url << this->phttp->GetListeningPort() << wxS("/");

    this->w->LoadURL( url << arg );

    void (*yield)(void) = nullptr;

    if ( spawn_new_thread )
    {
        yield = +[](){ std::this_thread::sleep_for( std::chrono::milliseconds(20u) ); };
    }
    else
    {
        yield = +[](){ wxGetApp().SafeYield(nullptr, false); };
    }

    auto const Do = [this,yield]()
    {
        auto const timestamp = std::chrono::steady_clock::now();
        for (; /* ever */ ;)
        {
            yield();
            if ( (std::chrono::steady_clock::now() - timestamp) > std::chrono::seconds(10u) )
            {
                std::fputs("Timeout when waiting for incoming HTTP connection.\n", stderr);
                break;
            }
            if ( this->phttp->TryServeWebpage() ) break;
        }
    };

    if ( spawn_new_thread )
    {
        std::thread(Do).detach();
    }
    else
    {
        Do();
    }
}

void ViewPortalManager::Set_Common(void) noexcept(false)
{
    std::fprintf(stderr, "Entered ViewPortalManager::Set_Common\n");
    Auto( std::fprintf(stderr, "Left ViewPortalManager::Set_Common\n") );

    assert( nullptr != this->phttp );
    if ( false == this->phttp->IsListening() )
    {
        std::fputs("Local HTTP server is not listening\n", stderr);
        throw -1;
    }
    uint16_t const port = this->phttp->GetListeningPort();
    if ( (0u == port) || (_Max == port) )
    {
        std::fputs("Local HTTP server is listening on an invalid TCP port number\n", stderr);
        throw -1;
    }

    this->phttp->DiscardAllPendingConnections();
    assert( nullptr != this->w );
    this->w->Stop();
}

void ViewPortalManager::Set(wxString const &paper_name) noexcept
{
    std::fprintf(stderr, "Entered ViewPortalManager::Set, paper_name = '%s'\n", static_cast<char const*>(paper_name.c_str()));
    Auto( std::fprintf(stderr, "Left ViewPortalManager::Set, paper_name = '%s'\n", static_cast<char const*>(paper_name.c_str())) );

    try
    {
        this->Set_Common();
#ifndef PAPERKERNEL_EMBED_ARCHIVE
        static bool already_done = false;
        if ( false == already_done )
        {
            already_done = true;
            this->Load( "loading_all" );
            for ( unsigned i = 0u; i < 16u; ++i ) wxGetApp().SafeYield(nullptr, false);
        }
#endif
        this->Load(paper_name);
    }
    catch (std::exception const &e)
    {
        std::fprintf( stderr, "ViewPortal_Set exception thrown: %s\n", e.what() );
    }
}

#if 1
void ViewPortalManager::SetHtml(std::string const &html, bool const spawn_new_thread) noexcept
{
    std::fprintf(stderr, "Entered ViewPortalManager::SetHtml\n");
    Auto( std::fprintf(stderr, "Left ViewPortalManager::SetHtml\n") );

    try
    {
        this->phttp->html_str = html;
        this->Set_Common();
        this->Load("html", spawn_new_thread);
    }
    catch (std::exception const &e)
    {
        std::fprintf( stderr, "ViewPortal_Set exception thrown: %s\n", e.what() );
    }
}
#else
void ViewPortalManager::SetHtml(std::string const &html, bool const spawn_new_thread) noexcept
{
    std::fprintf(stderr, "Entered ViewPortalManager::SetHtml\n");
    Auto( std::fprintf(stderr, "Left ViewPortalManager::SetHtml\n") );

    try
    {
        this->w->SetPage(html,"index.htm");
    }
    catch(...){}
}
#endif
