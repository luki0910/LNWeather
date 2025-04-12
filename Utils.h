#pragma once

#include <string>
#include <wx/wx.h>
#include <wx/datetime.h>

size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output);

namespace Utils {
    // Parsowanie daty i czasu z formatu API do wxDateTime
    wxDateTime parseDateTime(const std::string& dateStr);
}