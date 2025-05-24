#ifndef H__dynamoMain
#define H__dynamoMain

#include "AUTO_GENERATED_wxformbuilder.h"
#include <wx/webview.h>

class Dialog_Main : public Dialog_Main__Auto_Base_Class {
protected:
    wxWebView *webView = nullptr;
    void OnClose( wxCloseEvent &event ) override;
public:
	Dialog_Main( wxWindow *parent );
    void btnLoadModel_OnButtonClick(wxCommandEvent&) override;
    void btnUnloadModel_OnButtonClick(wxCommandEvent&) override;
    void btnLoadPapers_OnButtonClick(wxCommandEvent&) override;
    void btnXapianLoadPapers_OnButtonClick(wxCommandEvent&) override;
    void btnWhittleDownPapers_OnButtonClick(wxCommandEvent&) override;
    void PaperTree_OnSelChanged(wxTreeEvent&) override;
};

extern Dialog_Main *g_p_dlgmain;

#endif
