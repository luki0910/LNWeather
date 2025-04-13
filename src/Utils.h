/**
 * @file Utils.h
 * @brief Narzêdzia pomocnicze u¿ywane w aplikacji.
 */

#pragma once

#include <string>
#include <wx/wx.h>
#include <wx/datetime.h>

 /**
  * @brief Funkcja callback do odbierania danych HTTP poprzez libcurl.
  *
  * @param contents WskaŸnik do odebranych danych
  * @param size Rozmiar pojedynczego elementu
  * @param nmemb Liczba elementów
  * @param output WskaŸnik do bufora wyjœciowego
  * @return Ca³kowity rozmiar odebranych danych
  */
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output);

/**
 * @namespace Utils
 * @brief Przestrzeñ nazw zawieraj¹ca funkcje narzêdziowe.
 */
namespace Utils {
    /**
     * @brief Parsuje datê i czas z formatu API do wxDateTime.
     *
     * @param dateStr String zawieraj¹cy datê w formacie "YYYY-MM-DD HH:MM:SS"
     * @return wxDateTime Obiekt daty i czasu
     */
    wxDateTime parseDateTime(const std::string& dateStr);
}