#include "GUI_Dialog_Main.hpp"
#include <cassert>                                   // assert
#include <cstddef>                                   // size_t
#include <cstdint>                                   // uintptr_t
#include <cstring>                                   // strcmp, strstr
#include <array>                                     // array
#include <map>                                       // map
#include <string>                                    // string
#include <vector>                                    // vector
#include <thread>                                    // jthread
#include <wx/app.h>                                  // wxApp
#include <wx/msgdlg.h>                               // wxMessageBox
#include <wx/dataview.h>                             // wxDataViewCtrl
#include "GUI_Dialog_Waiting.hpp"
#include "ai.hpp"
#include "embedded_archive.hpp"
#include "paperman.hpp"
#include "papertree.hpp"
#include "semantic.hpp"
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

void Dialog_Main::PaperTree_OnSelChanged(wxDataViewEvent &event)
{
    std::cout << "Entered OnSelChanged --------------------\n";
    wxDataViewItem const selectedItem = event.GetItem();

    wxString const itemText = this->GetPaperTreeItemText(selectedItem);
    wxString htmlPath;
    if ( itemText.StartsWith('p') )
    {
        wxString const lastChildText = this->GetPaperTreeItemLastChildText(selectedItem);
        htmlPath = itemText + lastChildText + ".html";
    }
    else
    {
        wxDataViewItem const parent = this->treeStore->GetParent(selectedItem);
        wxString const parentText = this->GetPaperTreeItemText(parent);
        htmlPath = parentText + itemText + ".html";
    }
    std::cout << "htmlPath = " << htmlPath.ToStdString() << " --------------------\n";
    std::cout << "About to call 'GetFile'\n";
    string html = ArchiveGetFile( htmlPath.ToStdString().c_str() );
    std::cout << "Returned from 'GetFile'\n";

    std::cout << "Length of string returned from GetFile: " << html.size() << std::endl;

    std::cout << "(when searching for '" << htmlPath << "')\n";

    if ( html.empty() )
    {
        html = "<html><body><h1>Hello!</h1>"
               "<p>This is an example of loading HTML using SetPage().</p></body></html>";
    }

    assert( nullptr != this->view_portal );
    ::SetViewPortal( this->view_portal, html );
}

Dialog_Main::Dialog_Main(wxWindow *const parent) : Dialog_Main__Auto_Base_Class(parent)
{
    // ================ Data Storage for wxDataViewCtrl ================
    this->treeStore = new std::remove_reference_t<decltype(*treeStore)>;

    for ( auto const &e : g_map_papers )
    {
        using std::get;

        auto &papernum = e.first;
        auto &set_revs = e.second;

        std::tuple< unsigned, char const*, char const* > const &last_rev = e.second.back();

        char const *const  title_of_last_revision = get<1u>(last_rev),
                   *const author_of_last_revision = get<2u>(last_rev);

        wxDataViewItem const item_papernum =
          treeStore->AppendItemWithColumns(
            {},
            { PaperString(papernum), title_of_last_revision, author_of_last_revision }
          );

        for ( auto const &rev : set_revs )
        {
            char const *const  title = (0 == strcmp(get<1u>(rev),  title_of_last_revision)) ? "^ ^ ^" : get<1u>(rev),
                       *const author = (0 == strcmp(get<2u>(rev), author_of_last_revision)) ? "^ ^ ^" : get<2u>(rev);

            wxDataViewItem const item_rev =
              this->treeStore->AppendItemWithColumns(
                item_papernum,
                { "r" + wxString(std::to_string(get<0u>(rev))), title, author }
              );

            (void)item_rev;
          //this->treeAllPapers->Collapse(tii_rev);
        }

        //this->treeAllPapers->Expand(item_papernum);
    }

    // ================ Create the wxDataViewCtrl widget ===============
    this->treeAllPapers = new std::remove_reference_t<decltype(*this->treeAllPapers)>(
        this->panelBrowse, wxID_ANY, wxDefaultPosition, wxSize(150, -1) );
    assert( nullptr != this->treeAllPapers );
    this->bSizerForPanelBrowse->Insert( 0u, this->treeAllPapers, 0, wxALL|wxEXPAND, 5 );
    wxDataViewColumn *const pcol = this->treeAllPapers->AppendTextColumn("Paper" , 0);
    this->treeAllPapers->AppendTextColumn("Title" , 1, wxDATAVIEW_CELL_INERT, 200);
    this->treeAllPapers->AppendTextColumn("Author", 2);
    this->treeAllPapers->SetExpanderColumn(pcol);
    this->treeAllPapers->AssociateModel(this->treeStore);
    this->treeAllPapers->Bind(wxEVT_DATAVIEW_SELECTION_CHANGED, &Dialog_Main::PaperTree_OnSelChanged, this);
    // =================================================================

    // ====================== View Portal ==============================
    this->view_portal = ::CreateViewPortal(this->panelBrowse);
    assert( nullptr != this->view_portal );
    this->bSizerForPanelBrowse->Add( this->view_portal, 1, wxALL|wxEXPAND, 5 );
    // =================================================================

    this->panelBrowse->Layout();
    this->Layout();
}

void Dialog_Main::OnClose(wxCloseEvent&)
{
    this->Destroy();
}

void Dialog_Main::btnLoadModel_OnButtonClick(wxCommandEvent&)
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

    this->btnLoadModel  ->Enable( ! is_loaded );
    this->btnUnloadModel->Enable(   is_loaded );
}

void Dialog_Main::btnLoadPapers_OnButtonClick(wxCommandEvent&)
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

    this->btnLoadPapers  ->Enable( ! is_loaded );
    this->btnUnloadPapers->Enable(   is_loaded );
}

void Dialog_Main::btnUnloadModel_OnButtonClick(wxCommandEvent&)
{
    g_aimanager.Reset();
    this->btnLoadModel  ->Enable( true  );
    this->btnUnloadModel->Enable( false );
}

void Dialog_Main::btnWhittleDownPapers_OnButtonClick(wxCommandEvent&)
{
    Dialog_Waiting &dlg = *new Dialog_Waiting(nullptr, "Whittling down the list of papers. . .");
    dlg.m_gauge->SetRange( g_paperman.size() );
    //dlg.m_gauge->Hide();

    std::atomic_bool is_loaded{false};

    std::jthread mythread([&dlg,&is_loaded]
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
              is_loaded = true;
          }
          catch(...) {}

          dlg.CallAfter( &Dialog_Waiting::CallAfter_Destroy );
      });

    dlg.ShowModal();

    this->btnLoadPapers  ->Enable( ! is_loaded );
    this->btnUnloadPapers->Enable(   is_loaded );
}

void Dialog_Main::btnXapianLoadPapers_OnButtonClick(wxCommandEvent&)
{
    Dialog_Waiting &dlg = *new Dialog_Waiting(nullptr, "Loading Papers. . .");
    dlg.m_gauge->SetRange(100);
    //dlg.m_gauge->Hide();

    std::atomic_bool is_loaded{false};

    std::jthread mythread([&dlg,&is_loaded]
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

    this->btnXapianLoadPapers  ->Enable( ! is_loaded );
    //this->btnUnloadPapers->Enable(   is_loaded );
}

wxString Dialog_Main::GetPaperTreeItemText(wxDataViewItem const selected_item) const
{
    if ( false == selected_item.IsOk() ) return {};
    wxVariant myvar;
    this->treeStore->GetValue(myvar, selected_item, 0u);
    return myvar.GetString();
}

wxString Dialog_Main::GetPaperTreeItemLastChildText(wxDataViewItem const selected_item) const
{
    wxDataViewItem const lastChild = this->treeStore->GetLastChild(selected_item);
    return this->GetPaperTreeItemText(lastChild);
}

void Dialog_Main::lineBrowseDivider_OnMotion(wxMouseEvent &event)
{
/*
    std::cout << "lineBrowseDivider_OnMotion\n";
    this->lineBrowseDivider->SetCursor(wxCURSOR_SIZEWE); // Horizontal resize cursor
    if ( false == this->m_isResizing ) return;
    std::cout << "--------------------- stretching ------------------------\n";
*/
}

void Dialog_Main::lineBrowseDivider_OnLeftDown(wxMouseEvent &event)
{
    std::cout << "lineBrowseDivider_OnLeftDown\n";
    assert(   false == this->m_isResizing     );
    assert( INT_MAX == this->m_dragStartPoint );
    this->m_isResizing = true;
    this->m_dragStartPoint = event.GetPosition().x;
}

void Dialog_Main::lineBrowseDivider_OnLeftUp(wxMouseEvent &event)
{
    std::cout << "lineBrowseDivider_OnLeftUp\n";
    if ( false == m_isResizing ) return;
    this->m_isResizing     = false;
    Auto(this->m_dragStartPoint = INT_MAX);
    std::cout << " --------- About to Resize -----------\n";
    int const delta = event.GetPosition().x - this->m_dragStartPoint;  // might be negative
    std::cout << "delta = " << delta << std::endl;
    if ( delta < INT_MIN/2 || delta > INT_MAX/2 ) return;
    int const current_width = this->treeAllPapers->GetSize().x;
    std::cout << "current width = " << current_width << std::endl;
    if ( current_width < INT_MIN/2 || current_width > INT_MAX/2 ) return;
    int const new_width = current_width + delta;
    std::cout << "new width = " << new_width << std::endl;
    this->treeAllPapers->SetSize(new_width, -1);
    wxSize  const sV = this->view_portal->GetSize();
    wxPoint const pV = this->view_portal->GetPosition();
    this->view_portal->SetPosition( wxPoint(pV.x + delta, pV.y) );
    this->view_portal->SetSize( sV.x - delta, sV.y );
    wxPoint const sL = this->panelBrowse_panelLineBrowseDivider->GetPosition();
    this->panelBrowse_panelLineBrowseDivider->SetPosition( wxPoint(sL.x + delta, sL.y) );
    this->InvalidateBestSize();
    //this->SendSizeEvent();
    //this->bSizerForPanelBrowse->Layout();
    //this->panelBrowse->Layout();
}

void Dialog_Main::lineBrowseDivider_OnLeaveWindow(wxMouseEvent &event)
{
    std::cout << "lineBrowseDivider_OnLeaveWindow\n";
/*
    if ( m_isResizing ) return;
    this->lineBrowseDivider->GetParent()->SetCursor(wxCURSOR_ARROW); // Reset cursor when mouse leaves
*/
}

void Dialog_Main::lineBrowseDivider_OnEnterWindow(wxMouseEvent &event)
{
    std::cout << "lineBrowseDivider_OnEnterWindow\n";
    if ( m_isResizing ) return;
    this->lineBrowseDivider->GetParent()->SetCursor(wxCURSOR_SIZEWE); // Reset cursor when mouse leaves
}

