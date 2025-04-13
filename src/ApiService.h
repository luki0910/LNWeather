/**
 * @file ApiService.h
 * @brief Us³uga do komunikacji z zewnêtrznym API.
 */

 #pragma once
 #include <string>
 
  /**
   * @class ApiService
   * @brief Klasa obs³uguj¹ca komunikacjê z zewnêtrznymi API.
   *
   * Zapewnia metody do pobierania danych z zewnêtrznych Ÿróde³
   * oraz sprawdzania dostêpnoœci sieci.
   */
 class ApiService {
 public:
     /**
      * @brief Pobiera dane z okreœlonego URL.
      *
      * @param url Adres URL do pobrania danych
      * @return std::string OdpowiedŸ z API lub pusty string w przypadku b³êdu
      */
     static std::string fetchDataFromAPI(const std::string& url);
 
     /**
      * @brief Sprawdza czy jest dostêp do internetu.
      *
      * @return true Jeœli po³¹czenie z internetem jest dostêpne
      * @return false Jeœli brak po³¹czenia z internetem
      */
     static bool isNetworkAvailable();
 };