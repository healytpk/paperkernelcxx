#include "Plugin_Browse.h"
#include "Plugin_BrowseWindow.h"

IMPLEMENT_DYNAMIC_CLASS(Plugin_Browse, wxObject)

Plugin_Browse::Plugin_Browse(wxEvtHandler *const handler) : m_Handler(handler)
{
}

wxString Plugin_Browse::GetName() const
{
	return _("GUI Plugin 1");
}

wxString Plugin_Browse::GetId() const
{
	return wxT("{4E97DF66-5FBB-4719-AF17-76C1C82D3FE1}");
}

wxWindow *Plugin_Browse::CreatePanel(wxWindow *const parent)
{
	return new Plugin_BrowseWindow(this, parent);
}
