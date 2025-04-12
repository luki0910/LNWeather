#include "Utils.h"
#include <wx/wx.h>

// Callback do zapisywania odpowiedzi
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t totalSize = size * nmemb;
    output->append((char*)contents, totalSize);
    return totalSize;
}

// Funkcja pomocnicza do konwersji string daty na wxDateTime
namespace Utils {
    wxDateTime parseDateTime(const std::string& dateStr) {
        wxDateTime dt;
        // Format daty API: "2024-03-29 12:00:00"
        wxString wxDateStr = wxString::FromUTF8(dateStr.c_str());
        dt.ParseFormat(wxDateStr, "%Y-%m-%d %H:%M:%S");

        // Dodaj sprawdzenie poprawnoœci
        if (!dt.IsValid()) {
            dt = wxDateTime::Now();
        }
        return dt;
    }
}