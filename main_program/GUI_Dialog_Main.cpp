#include "GUI_Dialog_Main.hpp"
#include <cassert>                                   // assert
#include <cstddef>                                   // size_t
#include <cstring>                                   // strcmp, strstr
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

#include <cstddef>
#include <array>
#include <map>

template<unsigned column_count>
class wxDataViewTreeStoreWithColumns : public wxDataViewModel {
private:
    using ArrCols_t = std::array<wxString, column_count>;

    struct Node {
        wxDataViewItem parent, last_child;
        ArrCols_t column_values;
    };

    std::map< wxDataViewItem, Node > m_data;

public:
    //unsigned GetColumnCount(void) const override { return column_count; }
    //wxString GetColumnType(unsigned const column) const override { return "string"; }
    wxDataViewItem GetParent(wxDataViewItem const &item) const override
    {
        auto const it = m_data.find(item);
        if ( m_data.end() == it ) return {};
        return it->second.parent;
    }
    bool IsContainer(wxDataViewItem const &item) const override
    {
        auto const it = m_data.find(item);
        if ( m_data.end() == it ) return {};
        return it->second.last_child.IsOk();
    }
    unsigned GetChildren(wxDataViewItem const &item, wxDataViewItemArray &children) const override
    {
        unsigned counter = 0u;
        for ( auto const &e : m_data )
        {
            if ( e.second.parent != item ) continue;
            children.Add(e.first);
            ++counter;
        }
        return counter;
    }
    wxDataViewItem GetLastChild(wxDataViewItem const item) const
    {
        auto const it = m_data.find(item);
        if ( m_data.end() == it ) return {};
        return it->second.last_child;
    }
    void GetValue(wxVariant &value, wxDataViewItem const &item, unsigned const arg) const override
    {
        value.Clear();
        if ( arg >= column_count ) return;
        auto const it = m_data.find(item);
        if ( m_data.end() == it ) return;
        value = it->second.column_values[arg];
    }
    bool SetValue(wxVariant const &value, wxDataViewItem const &item, unsigned const arg) override
    {
        if ( arg >= column_count ) return false;
        auto const it = m_data.find(item);
        if ( m_data.end() == it ) return false;
        it->second.column_values[arg] = value.GetString();
        return true;
    }
    wxDataViewItem AppendItemWithColumns(wxDataViewItem const &parent, ArrCols_t &&columns)
    {
        static long long unsigned current_id = 1u;
        wxDataViewItem const dvi = (wxDataViewItem)(void*)++current_id;

        if ( parent.IsOk() ) m_data[parent].last_child = dvi;
        m_data[dvi] = Node{ parent, wxDataViewItem{}, std::move(columns) };
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
        html = "<html><body><h1>Hello, wxHtmlWindow!</h1>"
               "<p>This is an example of loading HTML using SetPage().</p></body></html>";
    }

    assert( nullptr != this->view_portal );
    SetViewPortal( this->view_portal, html );
}

Dialog_Main::Dialog_Main(wxWindow *const parent) : Dialog_Main__Auto_Base_Class(parent)
{
    // ====================== View Portal ==============================
    this->view_portal = CreateViewPortal(this->panelBrowse);
    assert( nullptr != this->view_portal );
    this->bSizerForViewPortal->Add( this->view_portal, 1, wxALL|wxEXPAND, 5 );
    this->Layout();
    // =================================================================

    // ================ Data Storage for wxDataViewCtrl ================
    this->treeStore = new std::remove_reference_t<decltype(*treeStore)>;

    for ( auto &e : g_map_papers )
    {
        auto &papernum = e.first;
        auto &set_revs = e.second;

        wxDataViewItem const item_papernum = treeStore->AppendItemWithColumns( {}, { PaperString(papernum), "Title", "Author" } );

#if 1
        for ( unsigned const rev : set_revs )
        {
            wxDataViewItem const item_rev = treeStore->AppendItemWithColumns( item_papernum   , { "r" + wxString(std::to_string(rev)), "Title", "Author" } );
          //wxDataViewItem const item_rev = treeStore->AppendItemWithColumns( wxDataViewItem{}, { "r" + wxString(std::to_string(rev)), "Title", "Author" } );
          //this->treeAllPapers->Collapse(tii_rev);
        }
#endif

        //this->treeAllPapers->Expand(item_papernum);
    }

    // ================ Create the wxDataViewCtrl widget ===============
    this->treeAllPapers = new std::remove_reference_t<decltype(*this->treeAllPapers)>(this->panelBrowse, wxID_ANY);
    assert( nullptr != this->treeAllPapers );
    this->bSizerForPaperTree->Add( this->treeAllPapers, 1, wxALL|wxEXPAND, 5 );
    wxDataViewColumn *const pcol = this->treeAllPapers->AppendTextColumn("Paper" , 0);
    this->treeAllPapers->AppendTextColumn("Title" , 1);
    this->treeAllPapers->AppendTextColumn("Author", 2);
    this->treeAllPapers->SetExpanderColumn(pcol);
    this->treeAllPapers->AssociateModel(treeStore);
    this->Layout();
    this->Bind(wxEVT_DATAVIEW_SELECTION_CHANGED, &Dialog_Main::PaperTree_OnSelChanged, this);
    // =================================================================

}

void Dialog_Main::OnClose(wxCloseEvent& event)
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
