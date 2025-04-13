/**
 * @file main.cpp
 * @brief Punkt wej�ciowy aplikacji.
 */

#include <wx/wx.h>
#include "MainWindow.h"

 /**
  * @class App
  * @brief G��wna klasa aplikacji wxWidgets.
  *
  * Inicjalizuje i konfiguruje g��wne okno aplikacji.
  */
class App : public wxApp {
public:
    /**
     * @brief Metoda inicjalizacji aplikacji.
     *
     * Tworzy g��wne okno aplikacji i ustawia jego ikon�.
     *
     * @return true Je�li inicjalizacja si� powiod�a
     * @return false Je�li wyst�pi� b��d
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