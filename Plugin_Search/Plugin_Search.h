#pragma once

#include <wxGuiPluginBase.h>

class Plugin_Search : public wxGuiPluginBase {
	DECLARE_DYNAMIC_CLASS(Plugin_Search)
protected:
	wxEvtHandler *m_Handler;
public:
	virtual bool ShouldInsertSpacers(void) const override { return true; }
	Plugin_Search() noexcept = default;
	Plugin_Search(wxEvtHandler *handler);
	virtual wxString GetName() const override;
	virtual wxString GetId() const override;
	virtual wxWindow *CreatePanel(wxWindow *parent) override;
	virtual wxEvtHandler *GetEventHandler() override { return m_Handler; }
	virtual void SetEventHandler(wxEvtHandler *const handler) override { m_Handler = handler; }
};
