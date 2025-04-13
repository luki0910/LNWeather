/**
 * @file Utils.h
 * @brief Narz�dzia pomocnicze u�ywane w aplikacji.
 */

#pragma once

#include <string>
#include <wx/wx.h>
#include <wx/datetime.h>

 /**
  * @brief Funkcja callback do odbierania danych HTTP poprzez libcurl.
  *
  * @param contents Wska�nik do odebranych danych
  * @param size Rozmiar pojedynczego elementu
  * @param nmemb Liczba element�w
  * @param output Wska�nik do bufora wyj�ciowego
  * @return Ca�kowity rozmiar odebranych danych
  */
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output);

/**
 * @namespace Utils
 * @brief Przestrze� nazw zawieraj�ca funkcje narz�dziowe.
 */
namespace Utils {
    /**
     * @brief Parsuje dat� i czas z formatu API do wxDateTime.
     *
     * @param dateStr String zawieraj�cy dat� w formacie "YYYY-MM-DD HH:MM:SS"
     * @return wxDateTime Obiekt daty i czasu
     */
    wxDateTime parseDateTime(const std::string& dateStr);
}