#include <cassert>
#include "GUI_Dialog_Main.hpp"
#include "AUTO_GENERATED_names.hpp"

void Dialog_Main::panelDebug_Notebook_OnPageChanged(wxNotebookEvent &event)
{
    assert( wxIsMainThread() );

    switch ( event.GetSelection() )
    {
    case 1u:
        static bool already_shown_1 = false;
        if ( already_shown_1 ) break;
        already_shown_1 = true;
        this->panelDebug_panelNames_listctrlMappings->InsertColumn(0, wxS("Alternative"));
        this->panelDebug_panelNames_listctrlMappings->InsertColumn(1, wxS("Primary"));
        this->panelDebug_panelNames_listctrlMappings->InsertItem(0, wxS("Thomas Patrick Kevin Healy III Esquire Twice Removed ZZZ"));
        this->panelDebug_panelNames_listctrlMappings->SetItem(0, 1, wxS("Thomas Patrick Kevin Healy III"));
        this->panelDebug_panelNames_listctrlMappings->SetColumnWidth(0, wxLIST_AUTOSIZE);
        this->panelDebug_panelNames_listctrlMappings->SetColumnWidth(1, wxLIST_AUTOSIZE);
        this->panelDebug_panelNames_listctrlMappings->DeleteItem(0);
        for ( auto const &e : g_primary_names_unsorted )
        {
            this->panelDebug_panelNames_listPrimary->Append( std::get<1u>(e) );
        }
        long index = -1;
        for ( auto const &e : g_alternative_names_unsorted )
        {
            ++index;
            this->panelDebug_panelNames_listctrlMappings->InsertItem(index, std::get<1u>(e));
            auto const it = std::ranges::lower_bound( g_primary_names, std::get<2u>(e), {}, [](auto const &arg) { return std::get<0u>(arg); } );
            if ( (it == std::end(g_primary_names)) || (std::get<0u>(*it) != std::get<2u>(e)) ) continue;
            this->panelDebug_panelNames_listctrlMappings->SetItem(index, 1, std::get<1u>(*it));
        }
    }
}

void Dialog_Main::panelDebug_panelNames_listPrimary_OnListBox(wxCommandEvent &event)
{
    this->panelDebug_panelNames_listAlternative->Clear();

    int const signed_n = event.GetSelection();
    if ( signed_n < 0 ) return;
    unsigned const n = static_cast<unsigned>(signed_n);

    if ( n >= ( std::cend(g_primary_names_unsorted) - std::cbegin(g_primary_names_unsorted) ) ) return;

    auto const &mypair = g_primary_names_unsorted[n];

    std::uint_fast64_t const hp = std::get<0u>(mypair);     // hash of primary
    for ( auto const &e : g_alternative_names_unsorted )
    {
        std::uint_fast64_t const hap = std::get<2u>(e);     // hash of alternative's primary
        if ( hp != hap ) continue;
        this->panelDebug_panelNames_listAlternative->Append( std::get<1u>(e) );
    }
}
