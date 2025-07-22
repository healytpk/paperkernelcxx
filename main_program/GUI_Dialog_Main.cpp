#include "GUI_Dialog_Main.hpp"
#include <cassert>                                   // assert
#include <climits>                                   // CHAR_BIT
#include <cstddef>                                   // size_t
#include <cstdint>                                   // uintptr_t
#include <cstring>                                   // strcmp, strstr
#include <array>                                     // array
#include <map>                                       // map
#include <memory>                                    // addressof
#include <random>                                    // ------------------------------ remove this ---- just for testing
#include <string>                                    // string
#include <string_view>                               // string_view
#include <vector>                                    // vector
#include <thread>                                    // jthread
#include <type_traits>                               // is_same, remove_cvref
#include <utility>                                   // move
#include <wx/accel.h>                                // wxAcceleratorEntry, wxAcceleratorTable
#include <wx/event.h>                                // wxEVT_MENU and event binding
#include <wx/filedlg.h>                              // wxFileDialog
#include <wx/msgdlg.h>                               // wxMessageBox
#include <wx/dataview.h>                             // wxDataViewCtrl
#include <wx/splitter.h>                             // wxSplitterWindow
#include <wx/wfstream.h>                             // wxFFileOutputStream (not wxFileOutputStream)
#include "wxApp.hpp"
#include "GUI_Dialog_About.hpp"
#include "GUI_Dialog_Waiting.hpp"
#include "ai.hpp"
#include "embedded_archive.hpp"
#include "paperman.hpp"
#include "semantic.hpp"
#include "tree_author.hpp"
#include "tree_paper.hpp"
#include "view_portal.hpp"
#include "Auto.h"

#include "AUTO_GENERATED_names.hpp"

using std::string, std::string_view;

Dialog_Main *g_p_dlgmain = nullptr;

AImanager g_aimanager;
PaperManager g_paperman("./paperfiles/papers/");
SemanticSearcher g_seman;

IMPLEMENT_APP(App_CxxPapers);  // This creates the "main" function

wxDataViewItem EncodeStringAsTreeItem(wxstring_view const wsv) noexcept
{
    static_assert(  sizeof(wxDataViewItem) ==  sizeof(void*) );
    static_assert( alignof(wxDataViewItem) == alignof(void*) );

    assert( (wsv.size() >= 5u) && (wsv.size() <= 8u) );

    if constexpr ( 8u == sizeof(void*) )
    {
        wxDataViewItem dvi{nullptr};  // all bits zero
        char *const q = static_cast<char*>(static_cast<void*>(std::addressof(dvi)));
        for ( unsigned i = 0u; i < wsv.size(); ++i ) q[i] = (char)wsv[i];
        if ( ('p' == q[0]) && ('\0' == q[5]) )
        {
            q[5] = 'r';
            q[6] = 'X';    // e.g. p3288rX -- X means the latest revision
        }
        return dvi;
    }
    else if constexpr ( 4u == sizeof(void*) )
    {
        return {};
    }
    else
    {
        static_assert( (8u == sizeof(void*)) || (4u == sizeof(void*)) );
    }
}

template<unsigned column_count>
class wxDataViewTreeStoreWithColumns : public wxDataViewModel {
protected:
    using ArrCols_t = std::array<wxString, column_count>;

    struct Node {
        wxDataViewItem parent, prior_sibling, last_child;
        ArrCols_t column_values;
    };

    std::map< wxDataViewItem, Node > m_data;

public:
    bool AlreadyExists(wxDataViewItem const arg) const
    {
        return m_data.end() != m_data.find(arg);
    }

    wxDataViewItem FindItem(wxString const &s) const
    {
        for ( auto const &e : this->m_data )
        {
            if ( s == e.second.column_values[0] ) return e.first;
        }
        return wxDataViewItem{};
    }

    void Reset(void)
    {
        this->m_data.clear();
        // The root node is a null wxDataViewItem
        this->m_data.emplace(wxDataViewItem{}, Node{ wxDataViewItem{}, wxDataViewItem{}, wxDataViewItem{}, ArrCols_t{} });
    }

    wxDataViewTreeStoreWithColumns(void)
    {
        // The root node is a null wxDataViewItem
        m_data.emplace( wxDataViewItem{}, Node{ wxDataViewItem{}, wxDataViewItem{}, wxDataViewItem{}, ArrCols_t{} } );
    }
    bool HasContainerColumns(wxDataViewItem const &) const override { return true; }
    wxDataViewItem GetParent(wxDataViewItem const &item) const override
    {
        auto const it = m_data.find(item);
        assert( m_data.end() != it );
        return it->second.parent;
    }
    bool IsContainer(wxDataViewItem const &item) const override
    {
        auto const it = m_data.find(item);
        assert( m_data.end() != it );
        return it->second.last_child.IsOk();
    }
    unsigned GetChildren(wxDataViewItem const &item, wxDataViewItemArray &children) const override
    {
        unsigned counter = 0u;

        auto it = m_data.find(item);
        assert( m_data.end() != it );
        wxDataViewItem x = it->second.last_child;
        while ( x.IsOk() )
        {
            children.Add(x);
            ++counter;
            it = m_data.find(x);
            assert( m_data.end() != it );
            x = it->second.prior_sibling;
        }

        return counter;
    }
    wxDataViewItem GetLastChild(wxDataViewItem const item) const
    {
        auto const it = m_data.find(item);
        assert( m_data.end() != it );
        return it->second.last_child;
    }
    void GetValue(wxVariant &value, wxDataViewItem const &item, unsigned const arg) const override
    {
        value.Clear();
        if ( arg >= column_count ) return;
        auto const it = m_data.find(item);
        assert( m_data.end() != it );
        value = it->second.column_values[arg];
    }
    bool SetValue(wxVariant const &value, wxDataViewItem const &item, unsigned const arg) override
    {
        if ( arg >= column_count ) return false;
        auto const it = m_data.find(item);
        assert( m_data.end() != it );
        it->second.column_values[arg] = value.GetString();
        return true;
    }
    wxDataViewItem AppendItemWithColumns(wxDataViewItem const &parent, ArrCols_t &&columns)
    {
        wxDataViewItem const dvi = EncodeStringAsTreeItem( wxString_inner(columns[0]) );
        auto it = m_data.find(parent);  // It's okay to find the null node (i.e. the root node)
        assert( m_data.end() != it );
        wxDataViewItem const prior_sibling = it->second.last_child;
        it->second.last_child = dvi;
        if ( columns[0].size() > 5u ) columns[0].erase(0u,5u);
        m_data.emplace( dvi, Node{ parent, prior_sibling, wxDataViewItem{}, std::move(columns) } );
        //std::cout << " +++++++++++++++  about to add item to map\n";
        return dvi;
    }
};

void Dialog_Main::OnViewPortalLoaded(wxEvent&) noexcept
{
    this->is_viewportal_loaded = true;
}

void Dialog_Main::PresentPaperInViewPortal(Paper const paper)
{
    assert( nullptr != this->view_portal );
    this->is_viewportal_loaded = false;
    //::ViewPortal_Set(this->view_portal, "<html><body><h1>Loading. . .</h1></body></html>");
    //do wxGetApp().SafeYield(nullptr, false); while ( false == this->is_viewportal_loaded );   -- This locks up and freezes
    this->is_viewportal_loaded = false;
    this->view_portal->Refresh();
    this->view_portal->Update();
    wxGetApp().SafeYield(nullptr, false);
    this->view_portal_manager.Set( paper.GetPaper() );
}

Paper DecodeTreeItem(wxDataViewItem arg) noexcept
{
    static_assert(  sizeof(wxDataViewItem) ==  sizeof(void*) );
    static_assert( alignof(wxDataViewItem) == alignof(void*) );

    if constexpr ( 8u == sizeof(void*) )
    {
        char const *const p = static_cast<char const*>(static_cast<void const*>(std::addressof(arg)));
        string_view sv(p, p + sizeof(void*));
        while ( sv.ends_with('\0') ) sv.remove_suffix(1u);
        if ( 'n' == sv[0] ) return Paper(sv);
        if ( ('p' == sv[0]) && ('r' == sv[5]) && ('X' != sv[6]) ) return Paper(sv);
        if ( ('p' == sv[0]) && ('r' == sv[5]) && ('X' == sv[6]) )
        {
            char *const p6 = const_cast<char*>(  &sv.back()  );
            assert( 'X' == *p6 );
            *p6 = '0';
            return GetPaperLatestRev( Paper(sv) );
        }
        std::abort();
    }
    else if constexpr ( 4u == sizeof(void*) )
    {
        return {};
    }
    else
    {
        static_assert( (8u == sizeof(void*)) || (4u == sizeof(void*)) );
        return {}; // suppress compiler warning
    }
}

Paper Dialog_Main::GetPaperFromDataViewEvent(wxDataViewEvent &event)
{
    return DecodeTreeItem( event.GetItem() );
}

void Dialog_Main::PaperTree_OnSelChanged(wxDataViewEvent &event)
{
    std::cout << "Entered OnSelChanged --------------------\n";
    this->PresentPaperInViewPortal( this->GetPaperFromDataViewEvent(event) );
}

void Dialog_Main::treeAuthorPapers_OnDataViewCtrlItemActivated(wxDataViewEvent &event)
{
    Paper const paper = this->GetPaperFromDataViewEvent(event);
    if ( false == this->SelectPaperInPaperTree(paper) ) return;
    this->m_notebook1->SetSelection(0u);
    wxGetApp().SafeYield(nullptr, false);
    this->PresentPaperInViewPortal(paper);
}

void Dialog_Main::listXapianResults_OnListItemActivated(wxListEvent &event)
{
    int const index = event.GetSelection();  // index of clicked item
    if ( (index < 0) || (index >= this->listXapianResults->GetItemCount()) ) return;
    wxString const itemText = this->listXapianResults->GetItemText(index,0);  // text of clicked item
    if ( itemText.IsEmpty() ) return;

    Paper const paper( itemText.ToStdString() );  // will throw if entry in listbox is dodgy
    if ( false == this->SelectPaperInPaperTree(paper) ) return;
    this->m_notebook1->SetSelection(0u);
    wxGetApp().SafeYield(nullptr, false);
    this->PresentPaperInViewPortal(paper);
}

inline constexpr wxStringCharType const *PrimaryNameFromHash(Hash_t const h)
{
    auto const it = std::ranges::lower_bound( g_primary_names, h, {}, [](auto const &e) { return std::get<0u>(e); } );
    if ( (it != std::end(g_primary_names)) && (std::get<0u>(*it) == h) ) return std::get<1u>(*it);
    return nullptr;
}

Dialog_Main::Dialog_Main(wxWindow *const parent) : Dialog_Main__Auto_Base_Class(parent)
{
    std::cout << "HTTP Server listening port == " << (unsigned)this->local_http_server.GetListeningPort() << std::endl;

    // =========================== Set up the Ctrl + Alt + D hotkey ========================
    wxAcceleratorEntry entries[2u];
    entries[0].Set(wxACCEL_CTRL | wxACCEL_SHIFT, (int)'D', wxID_HIGHEST + 1);
    entries[1].Set(wxACCEL_CTRL | wxACCEL_SHIFT, (int)'A', wxID_HIGHEST + 2);
    wxAcceleratorTable accel(2, entries);
    this->SetAcceleratorTable(accel);
    this->Bind(wxEVT_MENU, [this](wxCommandEvent&)
      {
        this->ShowDebugTab();
      }, wxID_HIGHEST + 1);
    this->Bind(wxEVT_MENU, [this](wxCommandEvent&)
      {
        try { (new Dialog_About(this))->ShowModal(); }
        catch(...){}
      }, wxID_HIGHEST + 2);
    // =====================================================================================

    this->m_notebook1->RemovePage(4u);    // Remove the Debug tab

    this->listAuthors->InsertColumn(0, wxS("Name"));
    this->listAuthors->InsertColumn(1, wxS("Authored"));
    this->listAuthors->InsertItem(0, wxS("Thomas Patrick Kevin Healy III Esquire ZZZ"));
    this->listAuthors->SetItem(0, 1, wxS("AuthoredZZZ"));
    this->listAuthors->SetColumnWidth(0, wxLIST_AUTOSIZE);
    this->listAuthors->SetColumnWidth(1, wxLIST_AUTOSIZE);
    this->listAuthors->DeleteItem(0);

    this->listXapianResults->InsertColumn(0, wxS("Paper"));
    this->listXapianResults->InsertColumn(1, wxS("Title"));
    this->listXapianResults->InsertColumn(2, wxS("Author"));
    this->listXapianResults->InsertItem(0, wxS("P1234R56ZZZ"));
    this->listXapianResults->SetItem(0,1, wxS("This is the title of the paper and it's a little long, what do you thinkZZZ"));
    this->listXapianResults->SetItem(0,2, wxS("Thomas PK Healy, Thomas PK Healy, Thomas PK Healy, Thomas PK Healy, Thomas PK Healy"));
    this->listXapianResults->SetColumnWidth(0,wxLIST_AUTOSIZE);
    this->listXapianResults->SetColumnWidth(1,wxLIST_AUTOSIZE);
    this->listXapianResults->SetColumnWidth(2,wxLIST_AUTOSIZE);
    this->listXapianResults->DeleteItem(0);

    this->m_toolBar1->ToggleTool(this->toolShowPaperTree ->GetId(), true);
    this->m_toolBar1->ToggleTool(this->toolShowViewPortal->GetId(), true);

    // ================ Data Storage for wxDataViewCtrl ================
    this->treeStore = new std::remove_reference_t<decltype(*treeStore)>;
    assert( 1u == this->treeStore->GetRefCount() );

    for ( auto const &e : g_map_papers )
    {
        //if ( e.paper.IsTerminator() ) break;  // -- Not needed because not null terminated FIX FIX FIX REVISIT REVISIT REVISIT FIX FIX FIX

        using std::get;

        assert( 0u != e.paper.num );

        wxString title_of_last_revision;
        wxString author_of_last_revision;
        if ( 'p' == e.paper.letter )
        {
            assert( nullptr != e.prevs );  // we need at least one revision
            assert( PaperRevInfo_t::terminator != e.prevs->rev );
            unsigned const latest_revision = e.latest_rev;
            for ( PaperRevInfo_t const *prev = e.prevs; PaperRevInfo_t::terminator != prev->rev; ++prev )
            {
                if ( latest_revision != prev->rev ) continue;
                title_of_last_revision  = prev->title;
                for ( Hash_t const *ph = prev->hashes_authors; 0u != *ph; ++ph )
                {
                    wxStringCharType const *str = PrimaryNameFromHash(*ph);
                    if ( nullptr == str )
                    {
                        std::cerr << "Failed to get primary name for hash: 0x" << std::setfill('0') << std::setw(16u) << std::hex << *ph << std::endl;
                        std::cerr << std::dec;
                        str = wxS("<unknown>");
                    }
                    author_of_last_revision << str;
                    if ( 0u != ph[1] ) author_of_last_revision << wxS(", ");
                }
            }
        }
        else
        {
            Paper ppr(e.paper);
             title_of_last_revision = ppr.GetTitle ();
            author_of_last_revision = ppr.GetAuthor();
        }

        if (  title_of_last_revision.empty() ) std::abort();    // REVISIT --- FIX --- revisit --- fix
        if ( author_of_last_revision.empty() ) std::abort();    // REVISIT --- FIX --- revisit --- fix

        wxString str = e.paper.PaperNameWithoutRevisionWx();

        wxDataViewItem const item_papernum =
          treeStore->AppendItemWithColumns(
            {},
            { str, title_of_last_revision, author_of_last_revision }
          );

        if ( 'p' == e.paper.letter )
        {
            for ( PaperRevInfo_t const *p = e.prevs; PaperRevInfo_t::terminator != p->rev; ++p )
            {
                static wxString const up_arrows = wxS("^ ^ ^");
                wxString const  title = ( p->title == title_of_last_revision ) ? up_arrows : wxString(p->title);
                wxString const author = ( (wxString() << p->hashes_authors[0]) == author_of_last_revision ) ? up_arrows : (wxString() << p->hashes_authors[0]);

                wxDataViewItem const item_rev =
                  this->treeStore->AppendItemWithColumns(
                    item_papernum,
                    { std::move(str) + wxS("r") << p->rev, title, author }
                  );

                (void)item_rev;
              //this->treeAllPapers->Collapse(tii_rev);
            }
        }

        //this->treeAllPapers->Expand(item_papernum);
    }

    this->splitter = new wxSplitterWindow(this->panelBrowse, wxID_ANY);
    assert( nullptr != this->splitter );

    // ================ Create the wxDataViewCtrl widget ===============
    this->treeAllPapers = new std::remove_reference_t<decltype(*this->treeAllPapers)>(this->splitter, wxID_ANY);
    assert( nullptr != this->treeAllPapers );
    wxDataViewColumn *const pcol = this->treeAllPapers->AppendTextColumn(wxS("Paper") , 0);
    this->treeAllPapers->AppendTextColumn(wxS("Title") , 1, wxDATAVIEW_CELL_INERT, 200);
    this->treeAllPapers->AppendTextColumn(wxS("Author") , 2);
    this->treeAllPapers->SetExpanderColumn(pcol);
    this->treeAllPapers->AssociateModel(this->treeStore);
    assert( 2u == this->treeStore->GetRefCount() );
    this->treeStore->DecRef();
    assert( 1u == this->treeStore->GetRefCount() );
    this->treeAllPapers->Bind(wxEVT_DATAVIEW_SELECTION_CHANGED, &Dialog_Main::PaperTree_OnSelChanged, this);
    // =================================================================

    // ====================== View Portal ==============================
    this->view_portal = this->view_portal_manager.Create(this->splitter, this->local_http_server);
    assert( nullptr != this->view_portal );
    this->view_portal_manager.BindFinishedLoading( &Dialog_Main::OnViewPortalLoaded, this );
    // =================================================================

    // ====================== wxListCtrl for authors ===================
    size_t i = _Max;
    for ( std::pair< Hash_t, Paper const * > const &e : g_map_authors )
    {
        ++i;
        wxString str_name( HashToDirectString(e.first) );
        this->listAuthors->InsertItem(i, str_name);
        Paper const *const pbegin = e.second;
        assert( nullptr != pbegin );
        Paper const *      p      = pbegin;
        assert( nullptr != p );
        while ( false == p->IsTerminator() ) ++p;
        this->listAuthors->SetItem(i, 1, wxString() << (p - pbegin) );
        this->listAuthors->SetItemPtrData(i, reinterpret_cast<std::uintptr_t>(const_cast<void*>(static_cast<void const*>(&e))) );
    }

    //this->listAuthors->SortItems(compare, 0);
    // =================================================================

    this->bSizerForPanelBrowse->Add(this->splitter, 1, wxEXPAND, 0);
    this->splitter->SplitVertically( this->treeAllPapers, this->view_portal, FromDIP(250) );
    this->panelBrowse->Layout();

    this->authorPaperStore = new std::remove_reference_t<decltype(*this->authorPaperStore)>;
    wxDataViewColumn *const pcolAuthor = this->treeAuthorPapers->AppendTextColumn(wxS("Paper") , 0);
    this->treeAuthorPapers->AppendTextColumn(wxS("Title") , 1, wxDATAVIEW_CELL_INERT, 200);
    this->treeAuthorPapers->SetExpanderColumn(pcolAuthor);

    this->Layout();
}

void Dialog_Main::OnClose(wxCloseEvent&)
{
    this->Destroy();
}

void Dialog_Main::AI_btnLoadModel_OnButtonClick(wxCommandEvent&)
{
    Dialog_Waiting &dlg = *new Dialog_Waiting(nullptr, wxS("Loading the artificial intelligence model. . ."));
    dlg.m_gauge->SetRange(100u);
    dlg.m_gauge->Hide();

    std::atomic_bool is_loaded{false};

    std::jthread mythread([&dlg,&is_loaded]
      {
          g_aimanager.Init();
          is_loaded = true;
          dlg.CallAfter( &Dialog_Waiting::CallAfter_Destroy );
      });

    dlg.ShowModal();

    this->AI_btnLoadModel  ->Enable( ! is_loaded );
    this->AI_btnUnloadModel->Enable(   is_loaded );
}

void Dialog_Main::AI_btnUnloadModel_OnButtonClick(wxCommandEvent&)
{
    g_aimanager.Reset();
    this->AI_btnLoadModel->Enable(true);
    this->AI_btnUnloadModel->Enable(false);
}

void Dialog_Main::AI_btnLoadPapers_OnButtonClick(wxCommandEvent&)
{
    Dialog_Waiting &dlg = *new Dialog_Waiting(nullptr, wxS("Loading the thousands of C++ papers. . ."));
    dlg.m_gauge->SetRange(100u);
    dlg.m_gauge->Hide();

    std::atomic_bool is_loaded{false};

    std::jthread mythread([&dlg,&is_loaded]
      {
          try
          {
              g_paperman.LoadAllTokensFromAllPapers();
              is_loaded = true;
          }
          catch(...) {}

          dlg.CallAfter( &Dialog_Waiting::CallAfter_Destroy );
      });

    dlg.ShowModal();

    this->AI_btnLoadPapers  ->Enable( ! is_loaded );
    this->AI_btnUnloadPapers->Enable(   is_loaded );
}

void Dialog_Main::AI_btnUnloadPapers_OnButtonClick(wxCommandEvent&)
{
    // Unload all the papers
}

void Dialog_Main::AI_btnWhittleDownPapers_OnButtonClick(wxCommandEvent&)
{
    Dialog_Waiting &dlg = *new Dialog_Waiting(nullptr, wxS("Whittling down the list of papers. . ."));
    dlg.m_gauge->SetRange( g_paperman.size() );
    //dlg.m_gauge->Hide();

    std::jthread mythread([&dlg]
      {
          try
          {
              for ( unsigned i = 0u; i < g_paperman.size(); ++i )
              {
                  auto const [paper, ptokens] = g_paperman.GetPaper(i);
                  size_t const token_count = g_paperman.TokenCount(i);
                  g_aimanager.NewContext(token_count);
                  g_aimanager.LoadInPaper(ptokens);
                  dlg.CallAfter( &Dialog_Waiting::CallAfter_Increment );
              }
          }
          catch(...) {}

          dlg.CallAfter( &Dialog_Waiting::CallAfter_Destroy );
      });

    dlg.ShowModal();
}

void Dialog_Main::btnXapianUnloadPapers_OnButtonClick(wxCommandEvent&)
{
    wxMessageBox(wxS("I haven't coded this yet"), wxS("paperkernelcxx"), wxOK|wxCENTRE, this);
}

void Dialog_Main::btnXapianLoadPapers_OnButtonClick(wxCommandEvent&)
{
    Dialog_Waiting &dlg = *new Dialog_Waiting(nullptr, wxS("Loading Papers. . ."));
    dlg.m_gauge->SetRange(100);
    //dlg.m_gauge->Hide();

    std::atomic_bool is_loaded{false};

    std::jthread mythread([this,&dlg,&is_loaded]
      {
          try
          {
              g_seman.Init( [&dlg](unsigned const n, unsigned const total)
                {
                    dlg.CallAfter( &Dialog_Waiting::CallAfter_SetProgress, n, total );
                });

              is_loaded = true;
          }
          catch(std::exception const &e)
          {
              std::cout << "what = " << e.what() << std::endl;
          }
          catch(char const *const what)
          {
              std::cout << "what = " << what << std::endl;
          }
          catch(...) { std::cout << "===== exception\n"; }

          dlg.CallAfter( &Dialog_Waiting::CallAfter_Destroy );
      });

    dlg.ShowModal();

    if ( false == is_loaded ) wxMessageBox(wxS("Failed to load Xapian database file"), wxS("Paper Kernel C++"), wxICON_ERROR|wxCENTRE, this);

    this->btnXapianLoadPapers  ->Enable( ! is_loaded );
    this->btnXapianUnloadPapers->Enable(   is_loaded );
    this->btnXapianSearch      ->Enable(   is_loaded );
}

wxString Dialog_Main::GetPaperTreeItemText(wxDataViewCtrl *const pdvc, wxDataViewItem const selected_item) const
{
    if ( false == selected_item.IsOk() ) return {};
    wxVariant myvar;
    pdvc->GetModel()->GetValue(myvar, selected_item, 0u);
    return myvar.GetString();
}

wxString Dialog_Main::GetPaperTreeItemLastChildText(wxDataViewCtrl *const pdv, wxDataViewItem const selected_item) const
{
    wxDataViewModel *const pdvm = pdv->GetModel();
    wxDataViewItem child{};
    auto *const pdvm2 = dynamic_cast<  wxDataViewTreeStoreWithColumns<2u> *  >(pdvm);
    if ( pdvm2 ) child = pdvm2->GetLastChild(selected_item);
    else
    {
        auto *const pdvm3 = dynamic_cast<  wxDataViewTreeStoreWithColumns<3u> *  >(pdvm);
        if ( pdvm3 ) child = pdvm3->GetLastChild(selected_item);
    }
    if ( wxDataViewItem{} == child ) return {};
    return this->GetPaperTreeItemText(pdv, child);
}

void Dialog_Main::OnTool_Common(int const this_tool, int const other_tool, wxWindow *const this_window)
{
    bool const show = this->m_toolBar1->GetToolState(this_tool);
    if ( show )
    {
        this_window->Show();
        this->splitter->SplitVertically( this->treeAllPapers, this->view_portal, FromDIP(250) );
    }
    else
    {
        bool const other_is_shown = this->m_toolBar1->GetToolState(other_tool);
        if ( other_is_shown )
        {
            this_window->Hide();
            this->splitter->Unsplit(this_window);
        }
        else
        {
            this->m_toolBar1->ToggleTool( this_tool, true );
            return;
        }
    }

    this->splitter->Layout();
}

void Dialog_Main::OnTool_ShowPaperTree(wxCommandEvent&)
{
    std::cout << "OnTool_ShowPaperTree\n";

    auto const this_one  = this->toolShowPaperTree ->GetId(),
               other_one = this->toolShowViewPortal->GetId();

    this->OnTool_Common( this_one, other_one, this->treeAllPapers );
}

void Dialog_Main::OnTool_ShowViewPortal(wxCommandEvent&)
{
    std::cout << "OnTool_ShowViewPortal\n";

    auto const this_one  = this->toolShowViewPortal->GetId(),
               other_one = this->toolShowPaperTree ->GetId();

    this->OnTool_Common( this_one, other_one, this->view_portal );
}

void Dialog_Main::btnXapianSearch_OnButtonClick(wxCommandEvent&)
{
    this->listXapianResults->DeleteAllItems();
    wxGetApp().SafeYield(nullptr, false);

    auto const mylambda = [this](std::string_view const sv) -> void
     {
        Paper paper(sv);
        this->listXapianResults->InsertItem(0, paper.c_str());
        this->listXapianResults->SetItem(0,1,paper.GetTitle ());
        this->listXapianResults->SetItem(0,2,paper.GetAuthor());
     };

    g_seman.Search(this->txtXapianSearchFor->GetValue().ToStdString(), mylambda);
}

bool Dialog_Main::SelectPaperInPaperTree(Paper const paper_selected)
{
    wxDataViewItem const item = EncodeStringAsTreeItem(paper_selected.wx_str());
    assert( this->treeStore->AlreadyExists(item) );
    this->treeAllPapers->EnsureVisible(item); // scroll to item
    this->treeAllPapers->Select(item);
    return true;
}

void Dialog_Main::listAuthors_OnListItemSelected(wxListEvent &event)
{
    assert( 1u == this->authorPaperStore->GetRefCount() );
    this->authorPaperStore->IncRef();
    assert( 2u == this->authorPaperStore->GetRefCount() );
    this->treeAuthorPapers->AssociateModel(nullptr);
    // Next line is required if model was never associated
    if ( 2u == this->authorPaperStore->GetRefCount() ) this->authorPaperStore->DecRef();
    assert( 1u == this->authorPaperStore->GetRefCount() );
    this->authorPaperStore->Reset();

    typedef std::remove_reference_t< decltype( *std::cbegin(g_map_authors) ) > MapPairType;
    //std::pair< Hash_t, Paper const * >

    static_assert( sizeof(wxUIntPtr) >= sizeof(void*) );
    std::uintptr_t const addr_as_int = static_cast<std::uintptr_t>(event.GetData());
    assert( 0u != addr_as_int );
    MapPairType const *const mypair = static_cast<MapPairType*>( reinterpret_cast<void*>(addr_as_int) );

    for ( Paper const *p = mypair->second; false == p->IsTerminator(); ++p )
    {
        wxString paper_str = p->PaperNameWithoutRevisionWx();
        wxDataViewItem const item_papernum = EncodeStringAsTreeItem( wxString_inner(paper_str) );
        if ( false == this->authorPaperStore->AlreadyExists(item_papernum) )
        {
            wxDataViewItem const temp = this->authorPaperStore->AppendItemWithColumns(
                {},
                { paper_str, p->GetTitle() }
            );

            assert( temp == item_papernum );  // just a sanity check
          //this->authorPaperStore->ItemAdded(wxDataViewItem{}, item_papernum);   --  not associated
        }

        if ( 'p' != p->letter ) continue;

        wxDataViewItem const item_rev =
            this->authorPaperStore->AppendItemWithColumns(
            item_papernum,
            { std::move(paper_str) + wxS("r") << p->rev }
        );
        (void)item_rev;
      //this->authorPaperStore->ItemAdded(item_papernum, item_rev);  --  not associated
    }

    assert( 1u == this->authorPaperStore->GetRefCount() );
    this->treeAuthorPapers->AssociateModel( this->authorPaperStore );
    assert( 2u == this->authorPaperStore->GetRefCount() );
    this->authorPaperStore->DecRef();
    assert( 1u == this->authorPaperStore->GetRefCount() );
}

void Dialog_Main::OnTool_DownloadIndividualPaper(wxCommandEvent&)
{
    try
    {
        wxString const wXs = this->view_portal_manager.GetCurrentPaper();
        wxstring const &wxs = wxString_inner(wXs);
        if ( wxs.empty() ) throw std::runtime_error("String for current paper is empty");
        std::string s( wxs.cbegin(), wxs.cend() );  // just in case wxStringCharType is wchar_t
        s += '.';
        string extension;
        string const file_binary_contents = ArchiveGetFile(s, extension, true);
        if ( extension.empty() || file_binary_contents.empty() ) throw std::runtime_error("Failed to retrieve contents of paper file");

        wxString filename = wxString(s) + extension;

        wxFileDialog saveFileDialog(
            this,
            wxS("Save paper file ") + filename,
            wxEmptyString,
            filename,
            wxString(extension) + wxS(" files (*.") + extension + wxS(")|*.") + extension,
            wxFD_SAVE|wxFD_OVERWRITE_PROMPT);

        if ( wxID_CANCEL == saveFileDialog.ShowModal() ) return;  // the user changed their mind

        // save the current contents in the file;
        // this can be done with e.g. wxWidgets output streams:
        wxFFileOutputStream out(  saveFileDialog.GetPath(), wxS("wb")  );   // binary mode
        if ( false == out.IsOk() || false == out.WriteAll( file_binary_contents.data(), file_binary_contents.size() ) )
        {
            throw std::runtime_error("Failed to save file");
        }
    }
    catch(std::exception const &e)
    {
        wxMessageBox( wxString(e.what()), wxS("Error"), wxOK|wxCENTRE|wxICON_ERROR, this );
    }
}
