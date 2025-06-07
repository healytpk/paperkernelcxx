#ifndef H__dynamoMain
#define H__dynamoMain

#include "AUTO_GENERATED_wxformbuilder.h"

class wxDataViewCtrl;
class wxDataViewEvent;
class wxDataViewItem;
class wxSplitterWindow;

template<unsigned column_count>
class wxDataViewTreeStoreWithColumns;

class Dialog_Main : public Dialog_Main__Auto_Base_Class {
protected:
    wxSplitterWindow *splitter = nullptr;
    wxDataViewCtrl *treeAllPapers = nullptr;
    wxDataViewTreeStoreWithColumns<3u> *treeStore = nullptr;
    wxWindow *view_portal = nullptr;
    void OnClose(wxCloseEvent&) override;
    void btnLoadModel_OnButtonClick(wxCommandEvent&) override;
    void btnUnloadModel_OnButtonClick(wxCommandEvent&) override;
    void btnLoadPapers_OnButtonClick(wxCommandEvent&) override;
    void btnXapianLoadPapers_OnButtonClick(wxCommandEvent&) override;
    void btnXapianUnloadPapers_OnButtonClick(wxCommandEvent&) override;
    void btnXapianSearch_OnButtonClick(wxCommandEvent&) override;
    void listXapianResults_OnListItemActivated(wxListEvent&) override;
    void btnWhittleDownPapers_OnButtonClick(wxCommandEvent&) override;
    void PaperTree_OnSelChanged(wxDataViewEvent&);
    wxString GetPaperTreeItemText(wxDataViewItem) const;
    wxString GetPaperTreeItemLastChildText(wxDataViewItem) const;
    void OnTool_Common(int this_tool, int other_tool, wxWindow *this_window);
    void OnTool_ShowPaperTree(wxCommandEvent&) override;
    void OnTool_ShowViewPortal(wxCommandEvent&) override;
    void OnViewPortalLoaded(wxEvent&) noexcept;
    bool is_viewportal_loaded = false;
public:
    Dialog_Main(wxWindow *parent);
};

extern Dialog_Main *g_p_dlgmain;

#endif
