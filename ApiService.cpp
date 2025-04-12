#include "ApiService.h"
#include "Utils.h"
#include <curl/curl.h>
#include <wx/wx.h>
#include <fstream>

std::string ApiService::fetchDataFromAPI(const std::string& url) {
    // Najpierw sprawd� czy jest dost�p do internetu
    if (!isNetworkAvailable()) {
        // Je�li nie ma dost�pu do internetu, zwr�� pust� odpowied�
        return "";
    }

    CURL* curl = curl_easy_init();
    std::string response;
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5L); // Timeout po 5 sekundach
        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            // Zapisz b��d do logu zamiast wy�wietla� komunikat
            std::ofstream logFile("curl_error.log", std::ios::app);
            logFile << "B��d cURL: " << curl_easy_strerror(res) << " dla URL: " << url << std::endl;
            logFile.close();
        }
        curl_easy_cleanup(curl);
    }
    // Zapisz odpowied� do pliku debugowego
    std::ofstream debugFile("response_debug.json");
    debugFile << response;
    debugFile.close();
    return response;
}

bool ApiService::isNetworkAvailable() {
    CURL* curl = curl_easy_init();
    bool result = false;

    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, "https://api.gios.gov.pl");
        curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 3L); // Kr�tki timeout 3 sekundy

        CURLcode res = curl_easy_perform(curl);
        result = (res == CURLE_OK);
        curl_easy_cleanup(curl);
    }

    return result;
}