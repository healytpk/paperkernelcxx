#include "GUI_Dialog_Main.hpp"
#include <cassert>                                   // assert
#include <climits>                                   // CHAR_BIT
#include <cstddef>                                   // size_t
#include <cstdint>                                   // uintptr_t
#include <cstring>                                   // strcmp, strstr
#include <array>                                     // array
#include <map>                                       // map
#include <random>                                    // ------------------------------ remove this ---- just for testing
#include <string>                                    // string
#include <string_view>                               // string_view
#include <vector>                                    // vector
#include <thread>                                    // jthread
#include <type_traits>                               // is_same, remove_cvref
#include <utility>                                   // move
#include <wx/app.h>                                  // wxApp
#include <wx/image.h>                                // wxImage::AddHandler, wxICOHandler
#include <wx/msgdlg.h>                               // wxMessageBox
#include <wx/dataview.h>                             // wxDataViewCtrl
#include <wx/splitter.h>                             // wxSplitterWindow
#include "GUI_Dialog_Waiting.hpp"
#include "ai.hpp"
#include "debug.hpp"
#include "embedded_archive.hpp"
#include "paperman.hpp"
#include "semantic.hpp"
#include "tree_author.hpp"
#include "tree_paper.hpp"
#include "view_portal.hpp"
#include "Auto.h"

using std::string;

Dialog_Main *g_p_dlgmain = nullptr;

AImanager g_aimanager;
PaperManager g_paperman("./paperfiles/papers/");
SemanticSearcher g_seman;

class App_CxxPapers : public wxApp {
public:

    bool OnInit(void) override
    {
        wxThread::SetConcurrency(2u);

        wxImage::AddHandler(new wxICOHandler);

        RecreateGUI();

        return true;
    }

    int OnExit(void) override
    {
        return 0;
    }

    void RecreateGUI(void)
    {
        //wxMessageBox( wxT("Recreating GUI") );

        wxWindow *const topwindow = this->GetTopWindow();

        if ( topwindow )
        {
            this->SetTopWindow(nullptr);
            topwindow->Destroy();
        }

        g_p_dlgmain = new Dialog_Main(nullptr);

        g_p_dlgmain->Show();   /* Just let this throw if it fails */

#if 0
        GtkWidget *const gtkWidget = static_cast<GtkWidget*>(   g_p_dlgmain->GetHandle()   );
        if ( gtkWidget )
        {
            GdkWindow *const gdk_window = gtk_widget_get_window(gtkWidget);
            if ( gdk_window )
            {
                gdk_window_set_decorations(gdk_window, static_cast<GdkWMDecoration>(GDK_DECOR_BORDER | GDK_DECOR_TITLE | GDK_DECOR_MENU | GDK_DECOR_MINIMIZE | GDK_DECOR_MAXIMIZE));
            }
        }
#endif
    }
};

//DECLARE_APP(App_CxxPapers);  -  Not needed

IMPLEMENT_APP(App_CxxPapers);  // This creates the "main" function

inline char const *PaperString(unsigned const num)
{
    static thread_local char s[] = "pxxxx";

    s[1] = '0' + num / 1000u % 10u;
    s[2] = '0' + num /  100u % 10u;
    s[3] = '0' + num /   10u % 10u;
    s[4] = '0' + num /    1u % 10u;

    return s;
}

template<unsigned column_count>
class wxDataViewTreeStoreWithColumns : public wxDataViewModel {
protected:
    using ArrCols_t = std::array<wxString, column_count>;

    struct Node {
        wxDataViewItem parent, prior_sibling, last_child;
        ArrCols_t column_values;
    };

    std::uintptr_t current_id = 0u;
    std::map< wxDataViewItem, Node > m_data;

public:

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
        this->current_id = 0u;
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
        // This is where we increment the 'current_id' so that each
        // item in the tree has a unique ID. Note that the root node
        // is zero (or null).
        wxDataViewItem const dvi = wxDataViewItem( reinterpret_cast<void*>(++this->current_id) );  // constructor is 'explicit'

        auto it = m_data.find(parent);  // It's okay to find the null node (i.e. the root node)
        assert( m_data.end() != it );
        wxDataViewItem const prior_sibling = it->second.last_child;
        it->second.last_child = dvi;

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
    ::ViewPortal_Set(this->view_portal, "<html><body><h1>Loading. . .</h1></body></html>");
    //do wxGetApp().SafeYield(nullptr, false); while ( false == this->is_viewportal_loaded );   -- This locks up and freezes
    this->is_viewportal_loaded = false;
    this->view_portal->Refresh();
    this->view_portal->Update();
    wxGetApp().SafeYield(nullptr, false);

    string const htmlPath = paper.c_str() + string(".html");
    std::cout << "htmlPath = " << htmlPath << " --------------------\n";
    std::cout << "About to call 'GetFile'\n";
    string html = ArchiveGetFile( htmlPath.c_str() );
    std::cout << "Returned from 'GetFile'\n";

    std::cout << "Length of string returned from GetFile: " << html.size() << std::endl;

    std::cout << "(when searching for '" << htmlPath << "')\n";

    if ( html.empty() )
    {
        html = "<html><body><h1>Error</h1>"
               "<p>Failed to display the paper in the view portal.</p></body></html>";
    }

    ::ViewPortal_Set( this->view_portal, html );
}

Paper Dialog_Main::GetPaperFromDataViewEvent(wxDataViewEvent &event)
{
    wxDataViewItem const selectedItem = event.GetItem();
    if ( wxDataViewItem{} == selectedItem ) return {};
    wxDataViewCtrl *const pdv = dynamic_cast<wxDataViewCtrl*>( event.GetEventObject() );
    if ( nullptr == pdv ) return {};
    wxString itemText = this->GetPaperTreeItemText(pdv, selectedItem);
    wxString paper_str;
    if ( itemText.StartsWith('p') )
    {
        wxString lastChildText = this->GetPaperTreeItemLastChildText(pdv, selectedItem);
        paper_str = std::move(itemText) + std::move(lastChildText);
    }
    else
    {
        wxDataViewModel *const pdvm = pdv->GetModel();
        wxDataViewItem parent{};
        auto *const pdvm2 = dynamic_cast<  wxDataViewTreeStoreWithColumns<2u> *  >(pdvm);
        if ( pdvm2 ) parent = pdvm2->GetParent(selectedItem);
        else
        {
            auto *const pdvm3 = dynamic_cast<  wxDataViewTreeStoreWithColumns<3u> *  >(pdvm);
            if ( pdvm3 ) parent = pdvm3->GetParent(selectedItem);
        }

        if ( wxDataViewItem{} == parent ) return {};
        wxString parentText = this->GetPaperTreeItemText(pdv, parent);
        paper_str = std::move(parentText) + std::move(itemText);
    }

    return Paper( paper_str.ToStdString() );
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

Dialog_Main::Dialog_Main(wxWindow *const parent) : Dialog_Main__Auto_Base_Class(parent)
{
    this->listAuthors->InsertColumn(0, "Name");
    this->listAuthors->InsertColumn(1, "Authored");
    this->listAuthors->InsertColumn(2, "Acknowledged");
    this->listAuthors->InsertItem(0, "Thomas Patrick HealyZZZ");
    this->listAuthors->SetItem(0, 1, "AuthoredZZZ");
    this->listAuthors->SetItem(0, 2, "AcknowledgedZZZ");
    this->listAuthors->SetColumnWidth(0, wxLIST_AUTOSIZE);
    this->listAuthors->SetColumnWidth(1, wxLIST_AUTOSIZE);
    this->listAuthors->SetColumnWidth(2, wxLIST_AUTOSIZE);
    this->listAuthors->DeleteItem(0);

    this->listXapianResults->InsertColumn(0, "Paper" );
    this->listXapianResults->InsertColumn(1, "Title" );
    this->listXapianResults->InsertColumn(2, "Author");
    this->listXapianResults->InsertItem(0, "P1234R56ZZZ");
    this->listXapianResults->SetItem(0,1, "This is the title of the paper and it's a little long, what do you thinkZZZ" );
    this->listXapianResults->SetItem(0,2, "Thomas PK Healy, Thomas PK Healy, Thomas PK Healy, Thomas PK Healy, Thomas PK Healy" );
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
        using std::get;

        auto const papernum = e.paper.num;
        assert( 0u != papernum );

        assert( nullptr != e.prevs );  // we need at least one revision
        assert( PaperRevInfo_t::terminator != e.prevs->rev );

        PaperRevInfo_t const *last_rev = e.prevs;
        while ( PaperRevInfo_t::terminator != last_rev[1].rev ) ++last_rev;

        wxString const  title_of_last_revision = last_rev->title;
        wxString const author_of_last_revision = wxString() << last_rev->hashes_authors[0];

        wxDataViewItem const item_papernum =
          treeStore->AppendItemWithColumns(
            {},
            { PaperString(papernum), title_of_last_revision, author_of_last_revision }
          );

        for ( PaperRevInfo_t const *p = e.prevs; PaperRevInfo_t::terminator != p->rev; ++p )
        {
            static wxString const up_arrows = wxS("^ ^ ^");
            wxString const  title = ( p->title == title_of_last_revision ) ? up_arrows : wxString(p->title);
            wxString const author = ( (wxString() << p->hashes_authors[0]) == author_of_last_revision ) ? up_arrows : (wxString() << p->hashes_authors[0]);

            wxDataViewItem const item_rev =
              this->treeStore->AppendItemWithColumns(
                item_papernum,
                { wxString("r") << p->rev, title, author }
              );

            (void)item_rev;
          //this->treeAllPapers->Collapse(tii_rev);
        }

        //this->treeAllPapers->Expand(item_papernum);
    }

    this->splitter = new wxSplitterWindow(this->panelBrowse, wxID_ANY);
    assert( nullptr != this->splitter );

    // ================ Create the wxDataViewCtrl widget ===============
    this->treeAllPapers = new std::remove_reference_t<decltype(*this->treeAllPapers)>(this->splitter, wxID_ANY);
    assert( nullptr != this->treeAllPapers );
    wxDataViewColumn *const pcol = this->treeAllPapers->AppendTextColumn("Paper" , 0);
    this->treeAllPapers->AppendTextColumn("Title" , 1, wxDATAVIEW_CELL_INERT, 200);
    this->treeAllPapers->AppendTextColumn("Author", 2);
    this->treeAllPapers->SetExpanderColumn(pcol);
    this->treeAllPapers->AssociateModel(this->treeStore);
    assert( 2u == this->treeStore->GetRefCount() );
    this->treeStore->DecRef();
    assert( 1u == this->treeStore->GetRefCount() );
    this->treeAllPapers->Bind(wxEVT_DATAVIEW_SELECTION_CHANGED, &Dialog_Main::PaperTree_OnSelChanged, this);
    // =================================================================

    // ====================== View Portal ==============================
    this->view_portal = ::ViewPortal_Create(this->splitter);
    assert( nullptr != this->view_portal );
    ::ViewPortal_BindFinishedLoading( this->view_portal, &Dialog_Main::OnViewPortalLoaded, this );
    // =================================================================

    // ====================== wxListCtrl for authors ===================
    constexpr auto number =
      +[](void) -> int
      {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        static std::discrete_distribution<> dist({5, 4, 3, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1});
        return dist(gen) + 1; // Adjusting index to match range 1-20
      };

    size_t i = -1;
    for ( std::pair< std::uint_fast64_t, std::pair< wchar_t const*, Paper const * > > const &e : g_map_authors )
    {
        ++i;
        this->listAuthors->InsertItem (i, e.second.first);
        Paper const *const pbegin = e.second.second;
        Paper const *      p      = pbegin;
        while ( false == p->IsTerminator() ) ++p;
        this->listAuthors->SetItem(i, 1, wxString() << (p - pbegin) );
        this->listAuthors->SetItem(i, 2, wxString() << number()     );
        this->listAuthors->SetItemPtrData(i, reinterpret_cast<std::uintptr_t>(const_cast<void*>(static_cast<void const*>(&e))) );
    }

    //this->listAuthors->SortItems(compare, 0);
    // =================================================================

    this->bSizerForPanelBrowse->Add(this->splitter, 1, wxEXPAND, 0);
    this->splitter->SplitVertically( this->treeAllPapers, this->view_portal, FromDIP(250) );
    this->panelBrowse->Layout();

    this->authorPaperStore = new std::remove_reference_t<decltype(*this->authorPaperStore)>;
    wxDataViewColumn *const pcolAuthor = this->treeAuthorPapers->AppendTextColumn("Paper" , 0);
    this->treeAuthorPapers->AppendTextColumn("Title" , 1, wxDATAVIEW_CELL_INERT, 200);
    this->treeAuthorPapers->SetExpanderColumn(pcolAuthor);

    this->Layout();
}

void Dialog_Main::OnClose(wxCloseEvent&)
{
    this->Destroy();
}

void Dialog_Main::AI_btnLoadModel_OnButtonClick(wxCommandEvent&)
{
    Dialog_Waiting &dlg = *new Dialog_Waiting(nullptr, "Loading the artificial intelligence model. . .");
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
    Dialog_Waiting &dlg = *new Dialog_Waiting(nullptr, "Loading the thousands of C++ papers. . .");
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
    Dialog_Waiting &dlg = *new Dialog_Waiting(nullptr, "Whittling down the list of papers. . .");
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
    wxMessageBox("I haven't coded this yet", "paperkernelcxx", wxOK|wxCENTRE, this);
}

void Dialog_Main::btnXapianLoadPapers_OnButtonClick(wxCommandEvent&)
{
    Dialog_Waiting &dlg = *new Dialog_Waiting(nullptr, "Loading Papers. . .");
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

    if ( false == is_loaded ) wxMessageBox("Failed to load Xapian database file", "Paper Kernel C++", wxICON_ERROR|wxCENTRE, this);

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
    wxDataViewModel const *const model = this->treeAllPapers->GetModel();
    wxDataViewItemArray children;
    model->GetChildren( wxDataViewItem{}, children );

    for ( wxDataViewItem const item : children )
    {
        wxVariant value;
        model->GetValue(value, item, 0u);
        wxString wxs = value.GetString();
        if ( wxs.empty() ) continue;  // revisit fix -- this should be fatal because of corruption
        if ( paper_selected.IsRelatedTo( wxs.ToStdString() + "r0" ) )  // revisit fix -- this is horrible
        {
            children.Clear();
            model->GetChildren(item, children);
            for ( wxDataViewItem const item2 : children )
            {
                value.Clear();
                model->GetValue(value, item2, 0u);
                wxs = value.GetString();
                if ( wxs.empty() ) continue;  // revisit fix -- this should be fatal because of corruption
                if ( wxs == (wxString("r") << paper_selected.rev) )
                {
                    this->treeAllPapers->EnsureVisible(item ); // scroll to item
                    this->treeAllPapers->EnsureVisible(item2); // scroll to item
                    this->treeAllPapers->Select(item2);
                    return true;
                }
            }
            break;
        }
    }
    return false;
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
    //std::pair< std::uint_fast64_t, std::pair< wchar_t const*, Paper const * > >

    static_assert( sizeof(wxUIntPtr) >= sizeof(void*) );
    std::uintptr_t const addr_as_int = static_cast<std::uintptr_t>(event.GetData());
    assert( 0u != addr_as_int );
    MapPairType const *const mypair = static_cast<MapPairType*>( reinterpret_cast<void*>(addr_as_int) );

    for ( Paper const *p = mypair->second.second; false == p->IsTerminator(); ++p )
    {
        wxString const paper_str = wxString::Format("p%04u", p->num);

        wxDataViewItem item_papernum = this->authorPaperStore->FindItem(paper_str);

        if ( wxDataViewItem{} == item_papernum )
        {
            item_papernum = this->authorPaperStore->AppendItemWithColumns(
                {},
                { paper_str, p->GetTitle() }
            );
          //this->authorPaperStore->ItemAdded(wxDataViewItem{}, item_papernum);   --  not associated
        }

        wxDataViewItem const item_rev =
            this->authorPaperStore->AppendItemWithColumns(
            item_papernum,
            { wxString("r") << p->rev }
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

void Dialog_Main::btnDebug_Refresh_OnButtonClick(wxCommandEvent&)
{
    assert( wxIsMainThread() );
    static bool already_done = false;
    if ( false == already_done )
    {
        already_done = true;
        this->labelDebug_Info->SetFont( wxFont(12, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL) );
    }

    wxString s;

    auto print_size = [&s]<typename T>(wxStringCharType const *const name, unsigned const len = 14u)
      {
        s << wxS("sizeof(");
        for ( unsigned i = 0u; i < (len - wxstring_view(name).size()); ++i ) s << " ";
        s << name << wxS(") == ") << sizeof(T) << wxS(" bytes == ") << (sizeof(T) * CHAR_BIT) << wxS("-Bit\n");
      };

    #define PRINT_TYPE(type) print_size.operator()<type>( wxS( #type ) )
    #define PRINT_TYPE_LEN(type, len) print_size.operator()<type>( wxS( #type ), len )

    constexpr wxStringCharType endl[] = wxS("\n");

    s << wxS("Total resident memory in use by this process: ") << GetResidentMemory() / 1024u / 1024u << wxS(" MiB") << endl;
    s << wxS("This process has ") << GetThreadCount() << wxS(" threads") << endl;
    s << wxS("This process has ") << GetChildProcessCount() << wxS(" child processes") << endl << endl;

    s << wxS("CHAR_BIT == ") << CHAR_BIT << endl << endl;
    PRINT_TYPE_LEN(wxStringCharType, 16u);
    PRINT_TYPE_LEN(char, 16u);
    PRINT_TYPE_LEN(wchar_t, 16u);
    s << wxS("is_same_v<wxStringCharType,  char  >  == ") << (std::is_same_v<wxStringCharType,  char  > ? wxS("true") : wxS("false")) << endl;
    s << wxS("is_same_v<wxStringCharType, wchar_t>  == ") << (std::is_same_v<wxStringCharType, wchar_t> ? wxS("true") : wxS("false")) << endl;
    s << wxS("is_signed_v<            char>  == ") << (std::is_signed_v<            char> ? wxS("true") : wxS("false")) << endl;
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

    wxGetApp().SafeYield(nullptr, false);
    this->labelDebug_Info->SetLabel(s);
    wxGetApp().SafeYield(nullptr, false);
    this->labelDebug_Info->Refresh();
    wxGetApp().SafeYield(nullptr, false);
    this->labelDebug_Info->Update();
    wxGetApp().SafeYield(nullptr, false);
}
