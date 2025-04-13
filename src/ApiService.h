/**
 * @file ApiService.h
 * @brief Us�uga do komunikacji z zewn�trznym API.
 */

 #pragma once
 #include <string>
 
  /**
   * @class ApiService
   * @brief Klasa obs�uguj�ca komunikacj� z zewn�trznymi API.
   *
   * Zapewnia metody do pobierania danych z zewn�trznych �r�de�
   * oraz sprawdzania dost�pno�ci sieci.
   */
 class ApiService {
 public:
     /**
      * @brief Pobiera dane z okre�lonego URL.
      *
      * @param url Adres URL do pobrania danych
      * @return std::string Odpowied� z API lub pusty string w przypadku b��du
      */
     static std::string fetchDataFromAPI(const std::string& url);
 
     /**
      * @brief Sprawdza czy jest dost�p do internetu.
      *
      * @return true Je�li po��czenie z internetem jest dost�pne
      * @return false Je�li brak po��czenia z internetem
      */
     static bool isNetworkAvailable();
 };