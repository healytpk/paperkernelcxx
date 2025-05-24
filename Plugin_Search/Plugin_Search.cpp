#include "Plugin_Search.h"
#include "Plugin_SearchWindow.h"

IMPLEMENT_DYNAMIC_CLASS(Plugin_Search, wxObject)

Plugin_Search::Plugin_Search(wxEvtHandler *const handler) : m_Handler(handler)
{
}

wxString Plugin_Search::GetName() const
{
	return _("GUI Plugin 2");
}

wxString Plugin_Search::GetId() const
{
	return wxT("{1B226C84-6436-4092-9AB8-B2B0D6731EBE}");
}

wxWindow *Plugin_Search::CreatePanel(wxWindow *const parent)
{
	return new Plugin_SearchWindow(this, parent);
}
