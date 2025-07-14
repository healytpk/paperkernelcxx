#ifndef __GUI_Dialog_About__
#define __GUI_Dialog_About__

#include "AUTO_GENERATED_wxformbuilder.h"

class Dialog_About : public Dialog_About__Auto_Base_Class {
public:
	Dialog_About(wxWindow *const parent)
	  : Dialog_About__Auto_Base_Class(parent)
    {
#ifdef __WXGTK__
        wxSize s = this->GetSize();
        s.x = (int)((s.x * 1.1L) + 1.0L);
        s.y = (int)((s.y * 1.1L) + 1.0L);
        this->SetSize(s);
        this->Layout();
#endif
    }

	void dlgAbout_OnClose(wxCloseEvent&) override
	{
		this->Destroy();
	}

	void dlgAbout_btnClose_OnButtonClick(wxCommandEvent&) override
	{
		this->Destroy();
	}
};

#endif // __GUI_Dialog_About__
