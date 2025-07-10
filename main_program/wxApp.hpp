#pragma once

#include <wx/app.h>                                  // wxApp
#include <wx/image.h>                                // wxImage::AddHandler, wxICOHandler
#include <wx/window.h>                               // wxWindow
#include <wx/thread.h>                               // wxThread::SetConCurrency
#include "GUI_Dialog_Main.hpp"                       // Dialog_Main

class App_CxxPapers : public wxApp {
public:

    bool OnInit(void) override
    {
        wxThread::SetConcurrency(2u);

        wxImage::AddHandler(new wxICOHandler);

        RecreateGUI();

        return true;
    }

    int OnExit(void) override
    {
        return 0;
    }

    void RecreateGUI(void)
    {
        //wxMessageBox( wxT("Recreating GUI") );

        wxWindow *const topwindow = this->GetTopWindow();

        if ( topwindow )
        {
            this->SetTopWindow(nullptr);
            topwindow->Destroy();
        }

        g_p_dlgmain = new Dialog_Main(nullptr);

        g_p_dlgmain->Show();   /* Just let this throw if it fails */

#if 0
        GtkWidget *const gtkWidget = static_cast<GtkWidget*>(   g_p_dlgmain->GetHandle()   );
        if ( gtkWidget )
        {
            GdkWindow *const gdk_window = gtk_widget_get_window(gtkWidget);
            if ( gdk_window )
            {
                gdk_window_set_decorations(gdk_window, static_cast<GdkWMDecoration>(GDK_DECOR_BORDER | GDK_DECOR_TITLE | GDK_DECOR_MENU | GDK_DECOR_MINIMIZE | GDK_DECOR_MAXIMIZE));
            }
        }
#endif
    }
};

DECLARE_APP(App_CxxPapers);
