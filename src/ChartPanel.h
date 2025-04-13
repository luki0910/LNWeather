/**
 * @file ChartPanel.h
 * @brief Panel wykresu do wizualizacji danych pomiarowych.
 */

#pragma once

#include <wx/wx.h>
#include <wx/datetime.h>
#include <vector>

 /**
  * @struct MeasurementData
  * @brief Struktura do przechowywania danych pomiarowych dla wykresu.
  */
struct MeasurementData {
    wxDateTime date;   /**< Data i czas pomiaru */
    double value;      /**< Wartoœæ pomiaru */
    bool hasValue;     /**< Flaga okreœlaj¹ca czy wartoœæ jest dostêpna */
};

/**
 * @class ChartPanel
 * @brief Panel wyœwietlaj¹cy wykres danych pomiarowych.
 *
 * Wyœwietla dane pomiarowe w postaci interaktywnego wykresu
 * z mo¿liwoœci¹ zmiany tytu³u i danych.
 */
class ChartPanel : public wxPanel {
public:
    /**
     * @brief Konstruktor panelu wykresu.
     *
     * @param parent Rodzic panelu
     */
    ChartPanel(wxWindow* parent);

    /**
     * @brief Ustawia dane do wyœwietlenia na wykresie.
     *
     * @param data Wektor danych pomiarowych
     * @param paramName Nazwa parametru
     */
    void SetData(const std::vector<MeasurementData>& data, const wxString& paramName);

    /**
     * @brief Ustawia tytu³ wykresu.
     *
     * @param title Nowy tytu³ wykresu
     */
    void SetTitle(const wxString& title);

    /**
     * @brief Zwraca referencjê do aktualnych danych wykresu.
     *
     * @return const std::vector<MeasurementData>& Referencja do wektora danych
     */
    const std::vector<MeasurementData>& GetChartData() const { return chartData; }

private:
    std::vector<MeasurementData> chartData;  /**< Dane pomiarowe */

    wxString title;     /**< Tytu³ wykresu */
    wxString parameter; /**< Nazwa parametru */

    /**
     * @brief Obs³uga zdarzenia rysowania.
     *
     * @param event Zdarzenie rysowania
     */
    void OnPaint(wxPaintEvent& event);

    /**
     * @brief Obs³uga zdarzenia zmiany rozmiaru.
     *
     * @param event Zdarzenie zmiany rozmiaru
     */
    void OnSize(wxSizeEvent& event);

    /**
     * @brief Rysuje osie wykresu.
     *
     * @param dc Kontekst urz¹dzenia
     * @param rect Prostok¹t obszaru wykresu
     */
    void DrawAxes(wxDC& dc, const wxRect& rect);

    /**
     * @brief Rysuje dane na wykresie.
     *
     * @param dc Kontekst urz¹dzenia
     * @param rect Prostok¹t obszaru wykresu
     */
    void DrawData(wxDC& dc, const wxRect& rect);

    /**
     * @brief Rysuje etykiety na wykresie.
     *
     * @param dc Kontekst urz¹dzenia
     * @param rect Prostok¹t obszaru wykresu
     */
    void DrawLabels(wxDC& dc, const wxRect& rect);
};