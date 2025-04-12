#include <wx/wx.h>
#include "MainWindow.h"

class App : public wxApp {
public:
    virtual bool OnInit() {
        MainWindow* window = new MainWindow();
        window->Show();
        return true;
    }
};

wxIMPLEMENT_APP(App);