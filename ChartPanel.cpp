#include "ChartPanel.h"
#include <wx/dcbuffer.h>
#include <algorithm>

ChartPanel::ChartPanel(wxWindow* parent)
    : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize),
    title("Wykres pomiar�w"), parameter("") {

    // W��cz buforowanie do p�ynnego rysowania
    SetBackgroundStyle(wxBG_STYLE_PAINT);

    // Pod��cz event paintowania
    Bind(wxEVT_PAINT, &ChartPanel::OnPaint, this);
    Bind(wxEVT_SIZE, &ChartPanel::OnSize, this);
}

void ChartPanel::SetData(const std::vector<MeasurementData>& data, const wxString& paramName) {
    chartData = data;
    parameter = paramName;
    Refresh(); // Od�wie� panel aby pokaza� nowe dane
}

void ChartPanel::SetTitle(const wxString& newTitle) {
    title = newTitle;
    Refresh();
}

void ChartPanel::OnPaint(wxPaintEvent& event) {
    wxAutoBufferedPaintDC dc(this);
    dc.Clear();

    // Pobierz rozmiar panelu
    wxSize size = GetClientSize();

    // Zwi�ksz lewy margines, aby pomie�ci� etykiety osi Y
    int leftMargin = 60;  // Wi�cej miejsca na etykiety osi Y
    int rightMargin = 20;
    int topMargin = 40;   // Miejsce na tytu�
    int bottomMargin = 30; // Wi�cej miejsca na etykiety osi X

    wxRect chartRect(leftMargin, topMargin,
        size.x - leftMargin - rightMargin,
        size.y - topMargin - bottomMargin);

    // Rysuj tytu�
    dc.SetFont(wxFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
    dc.DrawText(title, 10, 10);

    // Rysuj osie i dane tylko je�li mamy dane
    if (!chartData.empty()) {
        DrawAxes(dc, chartRect);
        DrawData(dc, chartRect);
        DrawLabels(dc, chartRect);
    }
    else {
        // Je�li nie ma danych, wy�wietl informacj�
        dc.SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
        dc.DrawText("Brak danych do wy�wietlenia", chartRect.x + 20, chartRect.y + chartRect.height / 2);
    }
}
void ChartPanel::OnSize(wxSizeEvent& event) {
    Refresh();  // Przerysuj wykres po zmianie rozmiaru
    event.Skip();
}

void ChartPanel::DrawAxes(wxDC& dc, const wxRect& rect) {
    // Rysuj osie X i Y
    dc.SetPen(wxPen(*wxBLACK, 1));

    // O� Y
    dc.DrawLine(rect.x, rect.y, rect.x, rect.y + rect.height);

    // O� X
    dc.DrawLine(rect.x, rect.y + rect.height, rect.x + rect.width, rect.y + rect.height);

    // Podzia�ki na osi Y (5 podzia�ek)
    int steps = 5;
    for (int i = 0; i <= steps; i++) {
        int y = rect.y + rect.height - (i * rect.height / steps);
        dc.DrawLine(rect.x - 5, y, rect.x, y);
    }
}

void ChartPanel::DrawData(wxDC& dc, const wxRect& rect) {
    if (chartData.empty()) return;

    // Znajd� minimaln� i maksymaln� warto��
    double minValue = std::numeric_limits<double>::max();
    double maxValue = std::numeric_limits<double>::lowest();

    for (const auto& point : chartData) {
        if (point.hasValue) {
            minValue = std::min(minValue, point.value);
            maxValue = std::max(maxValue, point.value);
        }
    }

    // Dodaj margines do zakresu warto�ci
    double range = maxValue - minValue;
    if (range < 0.1) range = 0.1; // Zapobiegaj dzieleniu przez zero

    minValue -= range * 0.1;
    maxValue += range * 0.1;

    // Przelicz wsp�rz�dne na piksele
    int dataCount = chartData.size();
    if (dataCount <= 1) return;

    // Szeroko�� segmentu na osi X
    double segmentWidth = static_cast<double>(rect.width) / (dataCount - 1);

    // Przygotuj tablic� na punkty z wa�nymi warto�ciami
    std::vector<wxPoint> validPoints;
    std::vector<size_t> validIndices;

    for (size_t i = 0; i < chartData.size(); i++) {
        if (chartData[i].hasValue) {
            double x = rect.x + i * segmentWidth;
            double normalizedValue = (chartData[i].value - minValue) / (maxValue - minValue);
            double y = rect.y + rect.height - (normalizedValue * rect.height);

            validPoints.push_back(wxPoint(x, y));
            validIndices.push_back(i);
        }
    }

    // Rysuj linie mi�dzy punktami
    if (validPoints.size() > 1) {
        dc.SetPen(wxPen(*wxBLUE, 2));
        dc.DrawLines(validPoints.size(), &validPoints[0]);
    }

    // Rysuj punkty
    for (const auto& point : validPoints) {
        dc.SetBrush(*wxBLUE_BRUSH);
        dc.DrawCircle(point.x, point.y, 3);
    }
}

void ChartPanel::DrawLabels(wxDC& dc, const wxRect& rect) {
    if (chartData.empty()) return;

    // Znajd� minimaln� i maksymaln� warto��
    double minValue = std::numeric_limits<double>::max();
    double maxValue = std::numeric_limits<double>::lowest();

    for (const auto& point : chartData) {
        if (point.hasValue) {
            minValue = std::min(minValue, point.value);
            maxValue = std::max(maxValue, point.value);
        }
    }

    // Dodaj margines do zakresu warto�ci
    double range = maxValue - minValue;
    if (range < 0.1) range = 0.1;

    minValue -= range * 0.1;
    maxValue += range * 0.1;

    // Rysuj etykiety osi Y
    dc.SetFont(wxFont(8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

    int steps = 5;
    for (int i = 0; i <= steps; i++) {
        double value = minValue + (maxValue - minValue) * i / steps;
        wxString label = wxString::Format("%.2f", value);
        int y = rect.y + rect.height - (i * rect.height / steps);

        // Wycentruj etykiet� wzgl�dem linii podzia�ki
        wxSize textSize = dc.GetTextExtent(label);
        dc.DrawText(label, rect.x - textSize.GetWidth() - 5, y - textSize.GetHeight() / 2);
    }

    // Rysuj etykiety osi X (wybrane daty)
    int dataCount = chartData.size();
    if (dataCount <= 1) return;

    double segmentWidth = static_cast<double>(rect.width) / (dataCount - 1);

    // Oblicz ile etykiet mo�emy pokaza� bez nak�adania si�
    wxSize sampleTextSize = dc.GetTextExtent("00:00");
    int minLabelSpacing = sampleTextSize.GetWidth() + 5; // minimalna odleg�o�� mi�dzy etykietami
    int maxLabels = std::max(2, static_cast<int>(rect.width / minLabelSpacing));
    int labelStep = std::max(1, static_cast<int>(ceil(static_cast<double>(dataCount) / maxLabels)));

    for (size_t i = 0; i < chartData.size(); i += labelStep) {
        if (!chartData[i].date.IsValid()) continue;

        wxString label = chartData[i].date.Format("%H:%M");
        double x = rect.x + i * segmentWidth;

        // Wycentruj etykiet� pod punktem danych
        wxSize textSize = dc.GetTextExtent(label);
        dc.DrawText(label, x - textSize.GetWidth() / 2, rect.y + rect.height + 5);
    }

    // Dodaj etykiet� ostatniego punktu je�li nie zosta�a ju� dodana
    if (dataCount > 1 && (dataCount - 1) % labelStep != 0) {
        if (chartData[dataCount - 1].date.IsValid()) {
            wxString label = chartData[dataCount - 1].date.Format("%H:%M");
            double x = rect.x + (dataCount - 1) * segmentWidth;

            wxSize textSize = dc.GetTextExtent(label);
            dc.DrawText(label, x - textSize.GetWidth() / 2, rect.y + rect.height + 5);
        }
    }
}