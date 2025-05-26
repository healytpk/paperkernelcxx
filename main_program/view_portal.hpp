#pragma once

#include <wx/string.h>                          // wxString
#include <wx/window.h>                          // wxWindow

wxWindow *CreateViewPortal(wxWindow *parent) noexcept(false);

void SetViewPortal(wxWindow*, wxString const &) noexcept(false);
