#pragma once

#include <wxGuiPluginBase.h>

class Plugin_Browse : public wxGuiPluginBase {
	DECLARE_DYNAMIC_CLASS(Plugin_Browse)
protected:
	wxEvtHandler *m_Handler;
public:
	virtual bool ShouldInsertSpacers(void) const override { return true; }
	Plugin_Browse() noexcept = default;
	Plugin_Browse(wxEvtHandler *handler);
	virtual wxString GetName() const override;
	virtual wxString GetId() const override;
	virtual wxWindow *CreatePanel(wxWindow *parent) override;
	virtual wxEvtHandler *GetEventHandler() override { return m_Handler; }
	virtual void SetEventHandler(wxEvtHandler *const handler) override { m_Handler = handler; }
};
