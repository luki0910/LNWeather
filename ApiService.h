#pragma once
#include <string>

class ApiService {
public:
    // Funkcja pobierania danych z API
    static std::string fetchDataFromAPI(const std::string& url);

    // Sprawdza czy jest dost�p do internetu
    static bool isNetworkAvailable();
};
