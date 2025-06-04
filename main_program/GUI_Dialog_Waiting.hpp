#ifndef __GUI_Dialog_Waiting__
#define __GUI_Dialog_Waiting__

#include <cassert>     // assert
#include <cstdlib>     // abort
#include <stop_token>  // stop_source
#include <chrono>      // steady_clock::now

#include "AUTO_GENERATED_wxformbuilder.h"

#include "GUI_Dialog_Main.hpp"  // MsgBox
#include "wx/thread.h"          // wxThread::IsMain
#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/combobox.h>
#include <wx/button.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/statline.h>
#include <wx/spinctrl.h>
#include <wx/textctrl.h>
#include <wx/panel.h>
#include <wx/checkbox.h>
#include <wx/gauge.h>
#include <wx/radiobut.h>
#include <wx/gbsizer.h>
#include <wx/notebook.h>
#include <wx/listctrl.h>
#include <wx/treectrl.h>
#include <wx/dialog.h>
#include <wx/listbox.h>
#include <wx/hyperlink.h>

class Dialog_Waiting_BASE : public wxDialog {
protected:
    wxStaticText* m_txt_message;
    wxStaticText* m_txt_under;
    wxButton* m_buttonCancel;

    // Virtual event handlers, override them in your derived class
    virtual void OnClick_Cancel( wxCommandEvent& event ) { event.Skip(); }

public:
    wxGauge* m_gauge;

    Dialog_Waiting_BASE( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 435,240 ), long style = 0 )
      : wxDialog( parent, id, title, pos, size, style )
    {
        this->SetSizeHints( wxDefaultSize, wxDefaultSize );

        wxBoxSizer* bSizer17;
        bSizer17 = new wxBoxSizer( wxVERTICAL );

        wxBoxSizer* bSizer18;
        bSizer18 = new wxBoxSizer( wxVERTICAL );

        bSizer18->Add( 0, 0, 1, 0, 5 );

        m_txt_message = new wxStaticText( this, wxID_ANY, wxT("message. . ."), wxDefaultPosition, wxDefaultSize, 0 );
        m_txt_message->Wrap( -1 );
        bSizer18->Add( m_txt_message, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );

        m_gauge = new wxGauge( this, wxID_ANY, 100, wxDefaultPosition, wxSize( 300,-1 ), wxGA_HORIZONTAL|wxGA_SMOOTH );
        m_gauge->SetValue( 0 );
        bSizer18->Add( m_gauge, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );

        m_txt_under = new wxStaticText( this, wxID_ANY, wxT("please wait"), wxDefaultPosition, wxDefaultSize, 0 );
        m_txt_under->Wrap( -1 );
        bSizer18->Add( m_txt_under, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );

        m_buttonCancel = new wxButton( this, wxID_ANY, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
        bSizer18->Add( m_buttonCancel, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );

        bSizer18->Add( 0, 0, 1, 0, 5 );

        bSizer17->Add( bSizer18, 1, wxEXPAND, 5 );

        this->SetSizer( bSizer17 );
        this->Layout();

        this->Centre( wxBOTH );

        m_buttonCancel->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( Dialog_Waiting_BASE::OnClick_Cancel ), NULL, this );
    }
};

class Dialog_Waiting : public Dialog_Waiting_BASE {
public:
    bool const is_cancel_fatal;
    std::stop_source member_stop_source;
    decltype( std::chrono::steady_clock::now() ) timestamp_from_beginning, timestamp_since_screen_update;
    unsigned displayed_seconds_on_screen;

    Dialog_Waiting(wxWindow *const parent, wxString const &arg_txt_msg, wxString const &arg_txt_under = "please wait", bool const arg_can_cancel = true, bool const arg_is_cancel_fatal = false)
        : Dialog_Waiting_BASE(parent), is_cancel_fatal(arg_is_cancel_fatal), timestamp_from_beginning(), timestamp_since_screen_update(), displayed_seconds_on_screen(0u)
    {
        (void)arg_txt_msg;
        if ( false == arg_can_cancel ) this->m_buttonCancel->Hide();

        this->m_txt_message->SetLabel(arg_txt_msg  );
        this->m_txt_under  ->SetLabel(arg_txt_under);
    }

    void OnClick_Cancel(wxCommandEvent&) override
    {
        member_stop_source.request_stop();

        if ( false == this->is_cancel_fatal ) return;

        std::this_thread::sleep_for( std::chrono::milliseconds(200u) );

        if ( wxYES != wxMessageBox("Do you wish to kill the program?", "BC43 SDKtester",
                                   wxYES_NO|wxNO_DEFAULT|wxCENTRE|wxICON_ERROR, this) ) return;

        wxWindow *w = this;

        do
        {
            w->Hide();
            w = w->GetParent();
        }
        while ( nullptr != w );

        std::abort();
    }

    void CallAfter_Destroy(void)
    {
        assert( wxThread::IsMain() );  // This method should only be invoked from the GUI Thread
        this->Destroy();
    }

    void CallAfter_SetRange(size_t const arg)
    {
        assert( wxThread::IsMain() );  // This method should only be invoked from the GUI Thread
        this->m_gauge->SetRange(arg);
    }

    void CallAfter_Increment(void)
    {
        assert( wxThread::IsMain() );  // This method should only be invoked from the GUI Thread

        if ( 0u == this->m_gauge->GetValue() ) this->timestamp_from_beginning = std::chrono::steady_clock::now();

        this->m_gauge->SetValue( this->m_gauge->GetValue() + 1u );

        //long double const already_gone = (long double)(this->m_gauge->GetValue()) / this->m_gauge->GetRange(),
        //                     remaining = (long double)(this->m_gauge->GetRange() - this->m_gauge->GetValue()) / this->m_gauge->GetRange();

        auto millis = std::chrono::duration_cast<std::chrono::milliseconds>( std::chrono::steady_clock::now() - this->timestamp_from_beginning ).count();

        if ( millis < 1000u ) return;  // just show "please wait" for 1 second

        millis /= this->m_gauge->GetValue();
        millis *= this->m_gauge->GetRange() - this->m_gauge->GetValue();

        // 'millis' is now the amount of milliseconds that will elapse until operation is finished
        if ( (millis % 1000u) < 900u ) return;

        unsigned const seconds_on_screen = (millis+500u) / 1000u;

        if ( seconds_on_screen == displayed_seconds_on_screen ) return;

        auto millis_since_screen_update = std::chrono::duration_cast<std::chrono::milliseconds>( std::chrono::steady_clock::now() - this->timestamp_since_screen_update ).count();

        // Don't allow a sudden bounce from bigger to smaller to bigger
        if ( (seconds_on_screen > displayed_seconds_on_screen) && (millis_since_screen_update < 200u) ) return;

        this->m_txt_under->SetLabel( std::to_string(seconds_on_screen) + " seconds remaining" );

        this->Layout();

        this->displayed_seconds_on_screen = seconds_on_screen;
    }

    void CallAfter_SetProgress(unsigned const n, unsigned const total)
    {
        assert( wxThread::IsMain() );  // This method should only be invoked from the GUI Thread
        this->m_gauge->SetRange(total);
        this->m_gauge->SetValue(n);
    }
};

#endif // __GUI_Dialog_Waiting__
