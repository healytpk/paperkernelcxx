#ifndef H__dynamoMain
#define H__dynamoMain

#include <wx/dataview.h>                          // wxDataViewCtrl
#include "AUTO_GENERATED_wxformbuilder.h"

template<unsigned column_count>
class wxDataViewTreeStoreWithColumns;

class Dialog_Main : public Dialog_Main__Auto_Base_Class {
protected:
    wxDataViewCtrl *treeAllPapers = nullptr;
    wxDataViewTreeStoreWithColumns<3u> *treeStore = nullptr;
    wxWindow *view_portal = nullptr;
    void OnClose( wxCloseEvent &event ) override;
public:
	Dialog_Main( wxWindow *parent );
    void btnLoadModel_OnButtonClick(wxCommandEvent&) override;
    void btnUnloadModel_OnButtonClick(wxCommandEvent&) override;
    void btnLoadPapers_OnButtonClick(wxCommandEvent&) override;
    void btnXapianLoadPapers_OnButtonClick(wxCommandEvent&) override;
    void btnWhittleDownPapers_OnButtonClick(wxCommandEvent&) override;

    void PaperTree_OnSelChanged(wxDataViewEvent &event);
    wxString GetPaperTreeItemText(wxDataViewItem) const;
    wxString GetPaperTreeItemLastChildText(wxDataViewItem) const;
};

extern Dialog_Main *g_p_dlgmain;

#endif
