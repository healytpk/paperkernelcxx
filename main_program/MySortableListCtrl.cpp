#include "MySortableListCtrl.hpp"

#include <cstdint>  // intptr_t
#include <map>      // map

#include "reservations_multithreaded.hpp"  // Reservable<>

static Reservable< std::map<wxListCtrl*,MySortableListCtrl*> > g_ctrls;

MySortableListCtrl::MySortableListCtrl(wxListCtrl *const arg) : sm_tmpinfo(), plist(arg)
{
    assert( nullptr != arg );

    {
        auto ctrls = g_ctrls.Reserve();
        assert( nullptr == (*ctrls)[arg] );
        (*ctrls)[arg] = this;
    }

    plist->Bind( wxEVT_LIST_COL_CLICK, MySortableListCtrl::OnColClick );
}

wxString MySortableListCtrl::GetDataByColumn(long const index, unsigned const column)
{
    wxListItem item;                // the item whose text we want
    item.SetId(index);              // set the index
    item.SetColumn(column);         // set the column
    item.SetMask(wxLIST_MASK_TEXT); // enable GetText()
    plist->GetItem(item);           // get the item
    return item.GetText();          // get and return its text
}

int wxCALLBACK MySortableListCtrl::Compare_MSLC(wxIntPtr item1, wxIntPtr item2, wxIntPtr sortData)  // static member function
{
#if 1
    // The C++11 Standard affirms the use of reinterpret_cast with intptr_t
    MySortInfo const *const ptmpinfo = static_cast<MySortInfo const *>(reinterpret_cast<void const *>(static_cast<std::intptr_t>(sortData)));
    assert( nullptr != ptmpinfo );
    MySortInfo const &tmpinfo = *ptmpinfo;
    assert( nullptr != tmpinfo.p );
    MySortableListCtrl &mslc = *(tmpinfo.p);
    assert( nullptr != mslc.plist );
    wxListCtrl &list = *(mslc.plist);

    int const multiplier = tmpinfo.is_ascending ? ((0u==tmpinfo.n_col) ? -1 : +1) : ((0u==tmpinfo.n_col) ? +1 : -1);

    long const index1 = list.FindItem(-1, item1),  // gets index of the first item
               index2 = list.FindItem(-1, item2);  // gets index of the second item

    if ( wxNOT_FOUND == index1 ) return 0;  /* REVISIT FIX - why is this sometimes happening? */
    if ( wxNOT_FOUND == index2 ) return 0;

    wxString const string1 = mslc.GetDataByColumn(index1, tmpinfo.n_col),  // gets int/text of 1st item
                   string2 = mslc.GetDataByColumn(index2, tmpinfo.n_col);  // gets int/text of 2nd item

    if ( 0u != tmpinfo.n_col )  // All but the 1st column are integers
    {
        long num1 = 0, num2 = 0;
        if ( false == string1.ToLong(&num1) ) return +1;
        if ( false == string2.ToLong(&num2) ) return -1;
        if ( num1 == num2 ) return 0;
        if ( num1  < num2 ) return multiplier * -1;
        if ( num1  > num2 ) return multiplier * +1;
    }

    int const result_of_comparison = string1.Cmp(string2);

    if ( 0 == result_of_comparison ) return 0;

    if ( string1.IsEmpty() ) return +1;
    if ( string2.IsEmpty() ) return -1;

    if ( result_of_comparison < 0 ) return multiplier * -1;
    /************************/ else return multiplier * +1; /* if ( result_of_comparison >= 0 ) */

#else
    // invert the order
    if (item1 < item2)
        return 1;
    if (item1 > item2)
        return -1;

    return 0;
#endif
}

void MySortableListCtrl::OnColClick(wxListEvent &evt)  // static member function
{
    int const n_signed_col = evt.GetColumn();

    if ( n_signed_col < 0 ) return;

    wxObject           *const pobj  = evt.GetEventObject();
    wxListCtrl         *const plc   = dynamic_cast<wxListCtrl*>(pobj); assert( nullptr != plc  );
    MySortableListCtrl *const pmslc = g_ctrls->operator[](plc);        assert( nullptr != pmslc );

    pmslc->sm_tmpinfo.p = pmslc;
    pmslc->sm_tmpinfo.n_col = static_cast<unsigned>(n_signed_col);
    pmslc->sm_tmpinfo.is_ascending = false;

    // The C++11 Standard affirms the use of reinterpret_cast with intptr_t
    plc->SortItems(Compare_MSLC,
      static_cast<wxIntPtr>(reinterpret_cast<std::intptr_t>(&pmslc->sm_tmpinfo)));

    //wxFAIL;
}

MySortableListCtrl::~MySortableListCtrl(void) noexcept
{
    try
    {
        assert( this == g_ctrls->operator[](this->plist) );
        g_ctrls->erase(this->plist);
    }
    catch(...) {}
}
