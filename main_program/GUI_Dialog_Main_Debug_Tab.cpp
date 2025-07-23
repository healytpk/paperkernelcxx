#include <cassert>                                             // assert
#include <cstddef>                                             // size_t
#include <cstdint>                                             // uint_fast64_t
#include <functional>                                          // function
#include <map>                                                 // map
#include <type_traits>                                         // is_signed
#include <utility>                                             // pair
#include <wx/webview.h>                                        // wxWebView
#ifdef wxUSE_WEBVIEW_CHROMIUM
#    include <wx/webview_chromium.h>                           // wxWebViewChromium
#endif
#include "wxApp.hpp"                                           // wxApp
#include "names.hpp"                                           // g_primary_names_unsorted, g_alternative_names_unsorted
#include "debug.hpp"                                           // GetResidentMemory, GetThreadCount, GetChildProcessCount
#include "GUI_Dialog_Main.hpp"                                 // Dialog_Main
#include "stringcraft.hpp"                                     // wxstring_view

void Dialog_Main::ShowDebugTab(void)
{
    assert( wxIsMainThread() );
    if ( this->already_showing_debug_tab ) return;
    this->m_notebook1->AddPage( this->panelDebug, wxS("Debug") );
    this->already_showing_debug_tab = true;
    this->m_notebook1->SetSelection(4u);  // Select the Debug tab
}
void Dialog_Main::listAuthors_OnListItemRightClick(wxListEvent &event)
{
    if ( wxS("Edward Catmur (1982 \u002D 2024)") != event.GetText() ) return;
    this->ShowDebugTab();
}

void Dialog_Main::m_notebook1_OnNotebookPageChanged(wxNotebookEvent &event)
{
    switch ( event.GetSelection() )
    {
    case 4u:
        wxCommandEvent dummy;
        this->btnDebug_Refresh_OnButtonClick(dummy);
        break;
    }
}

void Dialog_Main::btnDebug_Refresh_OnButtonClick(wxCommandEvent&)
{
    assert(wxIsMainThread());
    static bool already_done = false;
    if ( false == already_done )
    {
        already_done = true;
        this->txtDebug_Info->SetFont(  wxFont(12, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL)  );
    }

    wxString s;

    auto print_size = [&s]<typename T>(wxStringCharType const *const name, unsigned const len = 14u)
    {
        s << wxS("sizeof(");
        for ( unsigned i = 0u; i < (len - wxstring_view(name).size()); ++i ) s << wxS(" ");
        s << name << wxS(") == ") << sizeof(T) << wxS(" bytes == ") << (sizeof(T) * CHAR_BIT) << wxS("-Bit\n");
    };

#define PRINT_TYPE(type)          print_size.operator()<type>( wxS( #type ) )
#define PRINT_TYPE_LEN(type, len) print_size.operator()<type>( wxS( #type ), len )

    constexpr wxStringCharType endl[] = wxS("\n");

    s << wxS("Total resident memory in use by this process: ") << GetResidentMemory() / 1024u / 1024u << wxS(" MiB") << endl;
    s << wxS("This process has ") << GetThreadCount() << wxS(" threads") << endl;
    s << wxS("This process has ") << GetChildProcessCount() << wxS(" child processes") << endl << endl;

    s << wxS("Local HTTP server:") << endl
      << wxS("    IsListening: ") << this->local_http_server.IsListening() << endl
      << wxS("    IsUsingIPv6: ") << this->local_http_server.IsUsingIPv6() << endl
      << wxS("    Listening on TCP Port ") << this->local_http_server.GetListeningPort() << endl
      << wxS("    Death warrant: ") << this->local_http_server.GetDeathWarrant() << endl
      << wxS("    IsAcceptorThreadJoinable: ") << this->local_http_server.IsAcceptorThreadJoinable() << endl << endl;

    s << wxS("Compression for embedded archive: ");
#ifdef PAPERKERNEL_INDIVIDUAL_COMPRESSION
    s << wxS("Individual files compressed in an\n")
      << wxS("uncompressed archive (all_cxx_papers_individual_zst.tar).\n");
#else
    s << wxS("Uncompressed files in a compressed\n")
      << wxS("archive (all_cxx_papers.tar.zst).\n");
#endif
    s << endl;

    s << wxS("wxUSE_WEBVIEW             == ") << (int)wxUSE_WEBVIEW             << endl;
#ifdef wxUSE_WEBVIEW_CHROMIUM
    s << wxS("wxUSE_WEBVIEW_CHROMIUM    == ") << (int)wxUSE_WEBVIEW_CHROMIUM    << endl;
#else
    s << wxS("wxUSE_WEBVIEW_CHROMIUM    == undefined\n");
#endif
    s << wxS("wxUSE_WEBVIEW_IE          == ") << (int)wxUSE_WEBVIEW_IE          << endl;
    s << wxS("wxUSE_WEBVIEW_EDGE        == ") << (int)wxUSE_WEBVIEW_EDGE        << endl;
    s << wxS("wxUSE_WEBVIEW_EDGE_STATIC == ") << (int)wxUSE_WEBVIEW_EDGE_STATIC << endl;
    s << wxS("wxUSE_WEBVIEW_WEBKIT      == ") << (int)wxUSE_WEBVIEW_WEBKIT      << endl;
    s << wxS("wxUSE_WEBVIEW_WEBKIT2     == ") << (int)wxUSE_WEBVIEW_WEBKIT2     << endl;
    s << wxS("IsBackendAvailable(wxWebViewBackendIE      ) == ") << (int)wxWebView::IsBackendAvailable(wxWebViewBackendIE      ) << endl;
    s << wxS("IsBackendAvailable(wxWebViewBackendEdge    ) == ") << (int)wxWebView::IsBackendAvailable(wxWebViewBackendEdge    ) << endl;
    s << wxS("IsBackendAvailable(wxWebViewBackendWebKit  ) == ") << (int)wxWebView::IsBackendAvailable(wxWebViewBackendWebKit  ) << endl;
#if defined(wxUSE_WEBVIEW) && defined(wxUSE_WEBVIEW_CHROMIUM)
#   if wxUSE_WEBVIEW && wxUSE_WEBVIEW_CHROMIUM
        s << wxS("IsBackendAvailable(wxWebViewBackendChromium) == ") << (int)wxWebView::IsBackendAvailable(wxWebViewBackendChromium) << endl;
#   else
        s << wxS("IsBackendAvailable(wxWebViewBackendChromium) == undefined\n");
#   endif
#else
    s << wxS("IsBackendAvailable(wxWebViewBackendChromium) == undefined\n");
#endif

    s << endl;

    s << wxS("CHAR_BIT == ") << CHAR_BIT << endl << endl;
    PRINT_TYPE_LEN(wxStringCharType, 16u);
    PRINT_TYPE_LEN(char, 16u);
    PRINT_TYPE_LEN(wchar_t, 16u);
    s << wxS("is_same_v<wxStringCharType,  char  >  == ") << (std::is_same_v<wxStringCharType,  char  > ? wxS("true") : wxS("false")) << endl;
    s << wxS("is_same_v<wxStringCharType, wchar_t>  == ") << (std::is_same_v<wxStringCharType, wchar_t> ? wxS("true") : wxS("false")) << endl;
    s << wxS("is_signed_v<          char  >  == ") << (std::is_signed_v<          char  > ? wxS("true") : wxS("false")) << endl;
    s << wxS("is_signed_v<         wchar_t>  == ") << (std::is_signed_v<         wchar_t> ? wxS("true") : wxS("false")) << endl;
    s << wxS("is_signed_v<wxStringCharType>  == ") << (std::is_signed_v<wxStringCharType> ? wxS("true") : wxS("false")) << endl << endl;

    PRINT_TYPE(bool);
    PRINT_TYPE(char);
    PRINT_TYPE(short);
    PRINT_TYPE(int);
    PRINT_TYPE(long);
    PRINT_TYPE(long long);
    using std::uint_least32_t, std::uint_least64_t, std::uint_fast32_t, std::uint_fast64_t;
    PRINT_TYPE(uint_least32_t);
    PRINT_TYPE(uint_fast32_t);
    PRINT_TYPE(uint_least64_t);
    PRINT_TYPE(uint_fast64_t);
    PRINT_TYPE(Hash_t);

    wxGetApp().SafeYield(nullptr, false);
    this->txtDebug_Info->SetValue(s);
    wxGetApp().SafeYield(nullptr, false);
    this->txtDebug_Info->Refresh();
    wxGetApp().SafeYield(nullptr, false);
    this->txtDebug_Info->Update();
    wxGetApp().SafeYield(nullptr, false);
}

void Dialog_Main::panelDebug_Notebook_OnPageChanged(wxNotebookEvent &event)
{
    assert( wxIsMainThread() );

    switch ( event.GetSelection() )
    {
    case 1u:
      {
        static bool already_shown_1 = false;
        if ( already_shown_1 ) break;
        already_shown_1 = true;
        this->panelDebug_panelNames_listctrlMappings->InsertColumn(0, wxS("Alternative"));
        this->panelDebug_panelNames_listctrlMappings->InsertColumn(1, wxS("Primary"));
        this->panelDebug_panelNames_listctrlMappings->InsertItem  (0, wxS("Thomas Patrick Kevin Healy III Esquire Twice Removed ZZZ"));
        this->panelDebug_panelNames_listctrlMappings->SetItem     (0, 1, wxS("Thomas Patrick Kevin Healy III"));
        this->panelDebug_panelNames_listctrlMappings->SetColumnWidth(0, wxLIST_AUTOSIZE);
        this->panelDebug_panelNames_listctrlMappings->SetColumnWidth(1, wxLIST_AUTOSIZE);
        this->panelDebug_panelNames_listctrlMappings->DeleteItem(0);
        for ( auto const &e : g_primary_names_unsorted )
        {
            this->panelDebug_panelNames_listPrimary->Append( std::get<1u>(e) );
        }
        long index = -1;
        for ( auto const &e : g_alternative_names_unsorted )
        {
            ++index;
            this->panelDebug_panelNames_listctrlMappings->InsertItem(index, std::get<1u>(e));
            auto const it = std::ranges::lower_bound( g_primary_names, std::get<2u>(e), {}, [](auto const &arg) { return std::get<0u>(arg); } );
            if ( (it == std::end(g_primary_names)) || (std::get<0u>(*it) != std::get<2u>(e)) ) continue;
            this->panelDebug_panelNames_listctrlMappings->SetItem(index, 1, std::get<1u>(*it));
        }
      }
      break;
    case 2u:
      {
        static bool already_shown_2 = false;
        if ( already_shown_2 ) break;
        already_shown_2 = true;
        this->panelDebug_panelHash_txtOutputHash->SetFont(  wxFont(12, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL)  );
      }
      break;
    }
}

void Dialog_Main::panelDebug_panelNames_listPrimary_OnListBox(wxCommandEvent &event)
{
    this->panelDebug_panelNames_listAlternative->Clear();

    int const signed_n = event.GetSelection();
    if ( signed_n < 0 ) return;
    unsigned const n = static_cast<unsigned>(signed_n);

    if ( n >= ( std::cend(g_primary_names_unsorted) - std::cbegin(g_primary_names_unsorted) ) ) return;

    auto const &mypair = g_primary_names_unsorted[n];

    Hash_t const hp = std::get<0u>(mypair);     // hash of primary
    for ( auto const &e : g_alternative_names_unsorted )
    {
        Hash_t const hap = std::get<2u>(e);     // hash of alternative's primary
        if ( hp != hap ) continue;
        this->panelDebug_panelNames_listAlternative->Append( std::get<1u>(e) );
    }
}

void CheckForNameClashes( wxString &retval, std::function<void(void)> const &callback, long unsigned const interval, std::string_view const extra_name )
{
    std::map< Hash_t, std::pair<wxString, wxString> > clashes;

    std::size_t const count_primary     = std::cend(g_primary_names_unsorted    ) - std::cbegin(g_primary_names_unsorted    );
    std::size_t const count_alternative = std::cend(g_alternative_names_unsorted) - std::cbegin(g_alternative_names_unsorted);

    auto Item = [extra_name](std::size_t i) -> std::pair< Hash_t, wxString >
        {
            if ( i < count_primary     ) return { std::get<0u>(g_primary_names_unsorted    [i]), std::get<1u>(g_primary_names_unsorted    [i]) };
            i -= count_primary;
            if ( i < count_alternative ) return { std::get<0u>(g_alternative_names_unsorted[i]), std::get<1u>(g_alternative_names_unsorted[i]) };
            return { Hash(extra_name), wxString(std::string(extra_name)) };  // REVISIT FIX -- Must convert escaped Unicode sequences to characters
        };

    std::size_t const total_count = count_primary + count_alternative + 1u;

    long unsigned counter = 0u;
    for ( std::size_t i = 0u; i < total_count; ++i )
    {
        for ( std::size_t j = i+1u; j < total_count; ++j )
        {
            if ( 0u == (++counter % interval) ) callback();
            if ( Item(i).first != Item(j).first ) continue;  // if the hashes are different, no clash
            // If control reaches here, we have a clash!
            clashes.insert({ Item(i).first, { Item(i).second, Item(j).second } });
        }
    }

    for ( auto const &e : clashes )
    {
        if ( e.second.first == e.second.second )
        {
            retval += wxS("Duplicate: '") + e.second.first << wxS("'\n");
        }
        else
        {
            retval += wxS("Collision: '") + e.second.first + wxS("' and '") + e.second.second + wxS("'\n");
        }
    }

    if ( clashes.empty() ) retval += wxS(" -- no hash collisions detected --\n");

    retval += wxS("Operation completed.\n");
}

static std::string extra_name_to_hash("the null string");

void Dialog_Main::panelDebug_panelHash_txtInputHash_OnText(wxCommandEvent&)
{
    wxString const wxstr = this->panelDebug_panelHash_txtInputHash->GetValue();
    wxstring const &s = wxString_inner(wxstr);
    std::string &x = extra_name_to_hash;
    x.clear();
    x.reserve(s.size());
    bool invalid = false;
    for ( std::size_t i = 0u; i < s.size(); ++i )
    {
        wxStringCharType const c = s[i];

        if constexpr ( std::is_signed_v<wxStringCharType> )
        {
            if ( (c < 0) || (c > 127) ) { invalid = true; break; }
        }
        else
        {
            if ( c > 127u ) { invalid = true; break; }
        }

        x += c;
    }

    if ( invalid )
    {
        x = "the null string";
        this->panelDebug_panelHash_txtOutputHash->SetValue( wxS("--- invalid ---") );
        return;
    }

    Hash_t const h = Hash(x);
    this->panelDebug_panelHash_txtOutputHash->SetValue( wxString::Format("%016llx", (long long unsigned)h ) );
}

void Dialog_Main::panelDebug_panelHash_btnCheckForCollisions_OnButtonClick(wxCommandEvent&)
{
    wxString s = wxS("Checking for hash collisions in the authors' names:\n");

    std::size_t len = s.size();

    bool already_done_once = false;

    auto Refresh = [this,&s,&len,&already_done_once](void)
      {
        bool refresh = false;
        if ( false == already_done_once ) refresh = true;
        already_done_once = true;
        std::size_t const new_len = s.size();
        if ( new_len > len ) refresh = true;
        len = new_len;
        if ( false == refresh ) return;
        wxGetApp().SafeYield(nullptr, false);
        this->panelDebug_panelHash_txtSummary->SetValue(s);
        wxGetApp().SafeYield(nullptr, false);
        this->panelDebug_panelHash_txtSummary->Refresh();
        wxGetApp().SafeYield(nullptr, false);
        this->panelDebug_panelHash_txtSummary->Update();
        wxGetApp().SafeYield(nullptr, false);
      };

    Refresh();

    CheckForNameClashes( s, Refresh, 800u, extra_name_to_hash );

    Refresh();

    unsigned counter = 0u;
    s += wxS("Checking for alternatives present in primaries:\n");
    Refresh();
    bool alarm = false;
    for ( auto const &e : g_alternative_names )
    {
        if ( 0u == (++counter % 800u) ) Refresh();
        auto const hash_alternative = std::get<0u>(e);

        auto const it = std::ranges::lower_bound( g_primary_names, hash_alternative, {}, [](auto const &arg) { return std::get<0u>(arg); } );
        if ( (it != std::cend(g_primary_names)) && (hash_alternative == std::get<0u>(*it)) )
        {
            alarm = true;
            s << wxS("!!! Alternative hash found in primaries array: ") << std::get<1u>(e) << wxS(" --- ") << std::get<1u>(*it) << wxS("\n");
        }
    }
    if ( false == alarm ) s += wxS(" -- no alternative hashes detected in primaries array --\n");

    s += wxS("Making sure all alternatives' primaries are in the primary array:\n");
    Refresh();
    alarm = false;
    for ( auto const &e : g_alternative_names )
    {
        if ( 0u == (++counter % 800u) ) Refresh();
      //auto const hash_alternative = std::get<0u>(e);
        auto const hash_primary     = std::get<2u>(e);

        auto const it = std::ranges::lower_bound( g_primary_names, hash_primary, {}, [](auto const &arg) { return std::get<0u>(arg); } );
        if ( (it == std::cend(g_primary_names)) || (hash_primary != std::get<0u>(*it)) )
        {
            alarm = true;
            s << wxS("!!! Alternative's primary hash not found in primaries array: ") << std::get<1u>(e) << wxS("\n");
        }
    }
    if ( false == alarm ) s += wxS(" -- all alternatives' primaries found in the primary array --\n");

    s += wxS("Making sure primary array is sorted by hashes without duplicates:\n");
    Refresh();
    alarm = false;
    for ( std::size_t i = 0u; i < (g_primary_names.size() - 1u); ++i )
    {
        if ( 0u == (++counter % 800u) ) Refresh();
        if ( g_primary_names[i].first < g_primary_names[i+1u].first ) continue;
        s << wxS("!!! Unsorted or duplicate in primary's array: '") << g_primary_names[i].second << wxS("' and '") << g_primary_names[i+1u].second << wxS("'\n");
    }
    if ( false == alarm ) s += wxS(" -- the primary array is sorted by hash without duplicates --\n");

    s += wxS("Making sure alternative array is sorted by hashes without duplicates:\n");
    Refresh();
    alarm = false;
    for ( std::size_t i = 0u; i < (g_alternative_names.size() - 1u); ++i )
    {
        if ( 0u == (++counter % 800u) ) Refresh();
        if ( std::get<0u>(g_alternative_names[i]) < std::get<0u>(g_alternative_names[i+1u]) ) continue;
        s << wxS("!!! Unsorted or duplicate in alternative's array: '") << std::get<1u>(g_alternative_names[i]) << wxS("' and '") << std::get<1u>(g_alternative_names[i+1u]) << wxS("'\n");
    }
    if ( false == alarm ) s += wxS(" -- the alternative array is sorted by hash without duplicates --\n");

    s += wxS("+++ FIN +++\n");
    Refresh();
}

