#pragma once

#include <wx/listctrl.h>

class MySortableListCtrl {

protected:

    static int wxCALLBACK Compare_MSLC(wxIntPtr item1, wxIntPtr item2, wxIntPtr sortData);
    static void OnColClick(wxListEvent&);

    struct MySortInfo {
        MySortableListCtrl *p;
        unsigned n_col;
        bool is_ascending;
    };

    MySortInfo sm_tmpinfo;

    wxString GetDataByColumn(long index, unsigned column);

    wxListCtrl *const plist;

public:

    explicit MySortableListCtrl(wxListCtrl*);

    virtual ~MySortableListCtrl(void) noexcept;
};
