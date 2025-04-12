#pragma once
#include <wx/datetime.h>

// Pomocnicza struktura do przechowywania danych pomiarowych
struct MeasurementData {
    wxDateTime date;
    double value;
    bool hasValue;
};
