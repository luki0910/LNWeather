/**
 * @file main.cpp
 * @brief Punkt wejœciowy aplikacji.
 */

#include <wx/wx.h>
#include "MainWindow.h"

 /**
  * @class App
  * @brief G³ówna klasa aplikacji wxWidgets.
  *
  * Inicjalizuje i konfiguruje g³ówne okno aplikacji.
  */
class App : public wxApp {
public:
    /**
     * @brief Metoda inicjalizacji aplikacji.
     *
     * Tworzy g³ówne okno aplikacji i ustawia jego ikonê.
     *
     * @return true Jeœli inicjalizacja siê powiod³a
     * @return false Jeœli wyst¹pi³ b³¹d
     */
    virtual bool OnInit() {
        MainWindow* window = new MainWindow();
        wxIcon icon;
        icon.LoadFile("C:/Users/luki0/LNWeather/src/logo.ico", wxBITMAP_TYPE_ICO);
        window->SetIcon(icon);
        window->Show();
        return true;
    }
};

wxIMPLEMENT_APP(App);