#pragma once
#include <wx/wx.h>
#include <wx/choice.h>
#include <wx/progdlg.h>
#include <wx/datectrl.h>  // dla wxDatePickerCtrl
#include <wx/dateevt.h>   // dla obs³ugi eventów daty
#include <wx/notebook.h>
#include <vector>
#include <utility>
#include "ChartPanel.h"
#include "DatabaseService.h"

class MainWindow : public wxFrame {
public:
    MainWindow();

private:
    // UI Components
    wxChoice* stationChoice;
    wxChoice* sensorChoice;
    wxTextCtrl* measurementText;
    wxFont font;
    wxStaticText* airQualityIndex;
    wxButton* saveCurrentToDbButton;
    wxButton* saveAllToDbButton;
    wxStaticText* dbStatusText;

    //Analiza danych
    wxButton* showAnalysisButton;
    wxTextCtrl* analysisText;
    wxNotebook* dataNotebook;  // Kontrolka z zak³adkami

    //Chart Stuff
    ChartPanel* chartPanel;
    wxDatePickerCtrl* startDatePicker;
    wxDatePickerCtrl* endDatePicker;
    wxButton* applyDateRangeButton;

    // Data Storage
    std::vector<std::pair<int, std::string>> stations;
    std::vector<std::pair<int, std::string>> sensors;
    DatabaseService dbService;

    // Helper methods
    void loadStations();
    void loadSensorsForStation(int stationId);
    void loadAirQualityIndex(int stationId);
    wxDateTime parseDateTime(const std::string& dateStr);

    // Event handlers
    void OnStationSelection(wxCommandEvent& event);
    void OnSensorSelection(wxCommandEvent& event);
    void OnSaveCurrentToDatabase(wxCommandEvent& event);
    void OnSaveAllToDatabase(wxCommandEvent& event);
    void OnApplyDateRange(wxCommandEvent& event);

    // Metody
    bool tryLoadStationsFromAPI();
    bool tryLoadStationsFromDatabase();
    bool tryLoadSensorsFromAPI(int stationId);
    bool tryLoadSensorsFromDatabase(int stationId);
    bool tryLoadMeasurementsFromAPI(int sensorId);
    bool tryLoadMeasurementsFromDatabase(int sensorId);
    bool tryLoadAirQualityFromAPI(int stationId);
    bool tryLoadAirQualityFromDatabase(int stationId);
    bool tryLoadMeasurementsWithDateRange(int sensorId);
    bool tryLoadMeasurementsWithDateRangeFromDatabase(int sensorId);

    //Metody do analizy
    void OnShowAnalysis(wxCommandEvent& event);
    void PerformDataAnalysis(const std::vector<MeasurementData>& data, const wxString& paramName);
    

    // Status methods
    void updateDatabaseStatus();
};