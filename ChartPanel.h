#pragma once

#include <wx/wx.h>
#include <wx/datetime.h>
#include <vector>

// Struktura do przechowywania danych pomiarowych dla wykresu
struct MeasurementData {
    wxDateTime date;
    double value;
    bool hasValue;
};

class ChartPanel : public wxPanel {
public:
    ChartPanel(wxWindow* parent);

    // Ustawienie danych wykresu
    void SetData(const std::vector<MeasurementData>& data, const wxString& paramName);

    // Ustawienie tytu³u wykresu
    void SetTitle(const wxString& title);

    // Dostep do danych
    const std::vector<MeasurementData>& GetChartData() const { return chartData; }

private:
    std::vector<MeasurementData> chartData;
    
    wxString title;
    wxString parameter;

    // Event paintowania
    void OnPaint(wxPaintEvent& event);
    void OnSize(wxSizeEvent& event);

    // Metody pomocnicze do rysowania
    void DrawAxes(wxDC& dc, const wxRect& rect);
    void DrawData(wxDC& dc, const wxRect& rect);
    void DrawLabels(wxDC& dc, const wxRect& rect);
};
