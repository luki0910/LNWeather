/**
 * @file Utils.cpp
 * @brief Implementacja narz�dzi pomocniczych.
 */

#include "Utils.h"
#include <wx/wx.h>

 /**
  * @brief Funkcja callback do zapisywania odpowiedzi HTTP.
  *
  * U�ywana jako callback w libcurl do odbierania danych HTTP.
  *
  * @param contents Wska�nik do odebranych danych
  * @param size Rozmiar pojedynczego elementu
  * @param nmemb Liczba element�w
  * @param output Wska�nik do bufora wyj�ciowego (std::string)
  * @return Ca�kowity rozmiar odebranych danych w bajtach
  */
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t totalSize = size * nmemb;
    output->append((char*)contents, totalSize);
    return totalSize;
}

namespace Utils {
    /**
     * @brief Konwertuje string daty na obiekt wxDateTime.
     *
     * @param dateStr String zawieraj�cy dat� w formacie "YYYY-MM-DD HH:MM:SS"
     * @return wxDateTime Skonwertowany obiekt daty i czasu
     */
    wxDateTime parseDateTime(const std::string& dateStr) {
        wxDateTime dt;
        wxString wxDateStr = wxString::FromUTF8(dateStr.c_str());
        dt.ParseFormat(wxDateStr, "%Y-%m-%d %H:%M:%S");

        //Sprawdzenie poprawno�ci daty
        if (!dt.IsValid()) {
            dt = wxDateTime::Now();
        }
        return dt;
    }
}