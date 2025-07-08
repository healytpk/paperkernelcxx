#ifndef H__dynamoMain
#define H__dynamoMain

#include "AUTO_GENERATED_wxformbuilder.h"
#include "MySortableListCtrl.hpp"

class wxDataViewCtrl;
class wxDataViewEvent;
class wxDataViewItem;
class wxSplitterWindow;
class Paper;

template<unsigned column_count>
class wxDataViewTreeStoreWithColumns;

class Dialog_Main : public Dialog_Main__Auto_Base_Class {
protected:
    MySortableListCtrl adapter_over_listAuthors{this->listAuthors};
    wxSplitterWindow *splitter = nullptr;
    wxDataViewCtrl *treeAllPapers = nullptr;
    wxDataViewTreeStoreWithColumns<3u> *treeStore = nullptr;
    wxDataViewTreeStoreWithColumns<2u> *authorPaperStore = nullptr;
    wxWindow *view_portal = nullptr;
    void OnClose(wxCloseEvent&) override;
    void AI_btnLoadModel_OnButtonClick(wxCommandEvent&) override;
    void AI_btnUnloadModel_OnButtonClick(wxCommandEvent&) override;
    void AI_btnLoadPapers_OnButtonClick(wxCommandEvent&) override;
    void AI_btnUnloadPapers_OnButtonClick(wxCommandEvent&) override;
    void btnDebug_Refresh_OnButtonClick(wxCommandEvent&) override;
    void btnXapianLoadPapers_OnButtonClick(wxCommandEvent&) override;
    void btnXapianUnloadPapers_OnButtonClick(wxCommandEvent&) override;
    void btnXapianSearch_OnButtonClick(wxCommandEvent&) override;
    void listAuthors_OnListItemSelected(wxListEvent&) override;
    void listAuthors_OnListItemRightClick(wxListEvent&) override;
    void listXapianResults_OnListItemActivated(wxListEvent&) override;
    void m_notebook1_OnNotebookPageChanged(wxNotebookEvent&) override;
    void AI_btnWhittleDownPapers_OnButtonClick(wxCommandEvent&) override;
    void PaperTree_OnSelChanged(wxDataViewEvent&);
    wxString GetPaperTreeItemText(wxDataViewCtrl*,wxDataViewItem) const;
    wxString GetPaperTreeItemLastChildText(wxDataViewCtrl*,wxDataViewItem) const;
    void OnTool_Common(int this_tool, int other_tool, wxWindow *this_window);
    void OnTool_ShowPaperTree(wxCommandEvent&) override;
    void OnTool_ShowViewPortal(wxCommandEvent&) override;
    void OnViewPortalLoaded(wxEvent&) noexcept;
    void PresentPaperInViewPortal(Paper);
    Paper GetPaperFromDataViewEvent(wxDataViewEvent&);
    void treeAuthorPapers_OnDataViewCtrlItemActivated(wxDataViewEvent&) override;
    bool SelectPaperInPaperTree(Paper);
    bool is_viewportal_loaded = false;
    bool already_showing_debug_tab = false;
public:
    Dialog_Main(wxWindow *parent);
    ~Dialog_Main(void) noexcept
    {
        if ( false == this->already_showing_debug_tab ) this->panelDebug->Destroy();
    }
};

extern Dialog_Main *g_p_dlgmain;

#endif
