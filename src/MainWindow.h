/**
 * @file MainWindow.h
 * @brief G³ówne okno aplikacji do monitorowania jakoœci powietrza.
 */

#pragma once
#include <wx/wx.h>
#include <wx/choice.h>
#include <wx/progdlg.h>
#include <wx/datectrl.h>  
#include <wx/dateevt.h>   
#include <wx/notebook.h>
#include <vector>
#include <utility>
#include "ChartPanel.h"
#include "DatabaseService.h"

 /**
  * @class MainWindow
  * @brief Klasa implementuj¹ca g³ówne okno aplikacji.
  *
  * Zarz¹dza interfejsem u¿ytkownika, pobieraniem i wyœwietlaniem danych
  * z API jakoœci powietrza oraz lokalnej bazy danych.
  */
class MainWindow : public wxFrame {
public:
    /**
     * @brief Konstruktor g³ównego okna.
     *
     * Inicjalizuje interfejs aplikacji i ³aduje dane stacji.
     */
    MainWindow();

private:

    /**
     * @brief Lista rozwijana do wyboru stacji pomiarowej.
     */
    wxChoice* stationChoice;

    /**
     * @brief Lista rozwijana do wyboru czujnika.
     */
    wxChoice* sensorChoice;

    /**
     * @brief Pole tekstowe do wyœwietlania danych pomiarowych.
     */
    wxTextCtrl* measurementText;

    /**
     * @brief Czcionka u¿ywana w interfejsie.
     */
    wxFont font;

    /**
     * @brief Etykieta wyœwietlaj¹ca indeks jakoœci powietrza.
     */
    wxStaticText* airQualityIndex;

    /**
     * @brief Przycisk do zapisania bie¿¹cego pomiaru do bazy danych.
     */
    wxButton* saveCurrentToDbButton;

    /**
     * @brief Przycisk do zapisania wszystkich danych do bazy danych.
     */
    wxButton* saveAllToDbButton;

    /**
     * @brief Etykieta wyœwietlaj¹ca status bazy danych.
     */
    wxStaticText* dbStatusText;

    //Analiza danych
    /**
     * @brief Przycisk do pokazania analizy danych.
     */
    wxButton* showAnalysisButton;

    /**
     * @brief Pole tekstowe do wyœwietlania analizy danych.
     */
    wxTextCtrl* analysisText;

    /**
     * @brief Kontrolka z zak³adkami dla ró¿nych widoków danych.
     */
    wxNotebook* dataNotebook;

    //Chart Stuff
    /**
     * @brief Panel wykresu do wizualizacji danych pomiarowych.
     */
    ChartPanel* chartPanel;

    /**
     * @brief Kontrolka do wyboru daty pocz¹tkowej zakresu.
     */
    wxDatePickerCtrl* startDatePicker;

    /**
     * @brief Kontrolka do wyboru daty koñcowej zakresu.
     */
    wxDatePickerCtrl* endDatePicker;

    /**
     * @brief Przycisk do zastosowania wybranego zakresu dat.
     */
    wxButton* applyDateRangeButton;

    // Data Storage
    /**
     * @brief Wektor przechowuj¹cy pary (ID, nazwa) dostêpnych stacji.
     */
    std::vector<std::pair<int, std::string>> stations;

    /**
     * @brief Wektor przechowuj¹cy pary (ID, nazwa) dostêpnych czujników.
     */
    std::vector<std::pair<int, std::string>> sensors;

    /**
     * @brief Us³uga do obs³ugi lokalnej bazy danych.
     */
    DatabaseService dbService;


    /**
     * @brief £aduje listê dostêpnych stacji pomiarowych.
     */
    void loadStations();

    /**
     * @brief £aduje czujniki dla wybranej stacji.
     *
     * @param stationId ID stacji, dla której nale¿y za³adowaæ czujniki
     */
    void loadSensorsForStation(int stationId);

    /**
     * @brief £aduje indeks jakoœci powietrza dla stacji.
     *
     * @param stationId ID stacji, dla której nale¿y za³adowaæ indeks jakoœci powietrza
     */
    void loadAirQualityIndex(int stationId);

    /**
     * @brief Konwertuje string daty na obiekt wxDateTime.
     *
     * @param dateStr String zawieraj¹cy datê w odpowiednim formacie
     * @return wxDateTime Skonwertowany obiekt daty i czasu
     */
    wxDateTime parseDateTime(const std::string& dateStr);


    /**
     * @brief Obs³uga zdarzenia wyboru stacji.
     *
     * @param event Zdarzenie wyboru
     */
    void OnStationSelection(wxCommandEvent& event);

    /**
     * @brief Obs³uga zdarzenia wyboru czujnika.
     *
     * @param event Zdarzenie wyboru
     */
    void OnSensorSelection(wxCommandEvent& event);

    /**
     * @brief Obs³uga zdarzenia zapisania bie¿¹cego pomiaru do bazy danych.
     *
     * @param event Zdarzenie przycisku
     */
    void OnSaveCurrentToDatabase(wxCommandEvent& event);

    /**
     * @brief Obs³uga zdarzenia zapisania wszystkich danych do bazy danych.
     *
     * @param event Zdarzenie przycisku
     */
    void OnSaveAllToDatabase(wxCommandEvent& event);

    /**
     * @brief Obs³uga zdarzenia zastosowania zakresu dat dla wykresu.
     *
     * @param event Zdarzenie przycisku
     */
    void OnApplyDateRange(wxCommandEvent& event);

    // Metody
    /**
     * @brief Próbuje za³adowaæ stacje z API.
     *
     * @return true Jeœli za³adowanie siê powiod³o
     * @return false Jeœli wyst¹pi³ b³¹d
     */
    bool tryLoadStationsFromAPI();

    /**
     * @brief Próbuje za³adowaæ stacje z lokalnej bazy danych.
     *
     * @return true Jeœli za³adowanie siê powiod³o
     * @return false Jeœli wyst¹pi³ b³¹d
     */
    bool tryLoadStationsFromDatabase();

    /**
     * @brief Próbuje za³adowaæ czujniki dla stacji z API.
     *
     * @param stationId ID stacji
     * @return true Jeœli za³adowanie siê powiod³o
     * @return false Jeœli wyst¹pi³ b³¹d
     */
    bool tryLoadSensorsFromAPI(int stationId);

    /**
     * @brief Próbuje za³adowaæ czujniki dla stacji z lokalnej bazy danych.
     *
     * @param stationId ID stacji
     * @return true Jeœli za³adowanie siê powiod³o
     * @return false Jeœli wyst¹pi³ b³¹d
     */
    bool tryLoadSensorsFromDatabase(int stationId);

    /**
     * @brief Próbuje za³adowaæ pomiary z API.
     *
     * @param sensorId ID czujnika
     * @return true Jeœli za³adowanie siê powiod³o
     * @return false Jeœli wyst¹pi³ b³¹d
     */
    bool tryLoadMeasurementsFromAPI(int sensorId);

    /**
     * @brief Próbuje za³adowaæ pomiary z lokalnej bazy danych.
     *
     * @param sensorId ID czujnika
     * @return true Jeœli za³adowanie siê powiod³o
     * @return false Jeœli wyst¹pi³ b³¹d
     */
    bool tryLoadMeasurementsFromDatabase(int sensorId);

    /**
     * @brief Próbuje za³adowaæ indeks jakoœci powietrza z API.
     *
     * @param stationId ID stacji
     * @return true Jeœli za³adowanie siê powiod³o
     * @return false Jeœli wyst¹pi³ b³¹d
     */
    bool tryLoadAirQualityFromAPI(int stationId);

    /**
     * @brief Próbuje za³adowaæ indeks jakoœci powietrza z lokalnej bazy danych.
     *
     * @param stationId ID stacji
     * @return true Jeœli za³adowanie siê powiod³o
     * @return false Jeœli wyst¹pi³ b³¹d
     */
    bool tryLoadAirQualityFromDatabase(int stationId);

    /**
     * @brief Próbuje za³adowaæ pomiary z okreœlonego zakresu dat.
     *
     * @param sensorId ID czujnika
     * @return true Jeœli za³adowanie siê powiod³o
     * @return false Jeœli wyst¹pi³ b³¹d
     */
    bool tryLoadMeasurementsWithDateRange(int sensorId);

    /**
     * @brief Próbuje za³adowaæ pomiary z okreœlonego zakresu dat z lokalnej bazy danych.
     *
     * @param sensorId ID czujnika
     * @return true Jeœli za³adowanie siê powiod³o
     * @return false Jeœli wyst¹pi³ b³¹d
     */
    bool tryLoadMeasurementsWithDateRangeFromDatabase(int sensorId);


    /**
     * @brief Obs³uga zdarzenia pokazania analizy danych.
     *
     * @param event Zdarzenie przycisku
     */
    void OnShowAnalysis(wxCommandEvent& event);

    /**
     * @brief Wykonuje analizê danych pomiarowych.
     *
     * Analizuje dane, obliczaj¹c statystyki i przygotowuj¹c raport.
     *
     * @param data Wektor danych pomiarowych do analizy
     * @param paramName Nazwa parametru, dla którego wykonywana jest analiza
     */
    void PerformDataAnalysis(const std::vector<MeasurementData>& data, const wxString& paramName);


    /**
     * @brief Aktualizuje wyœwietlany status bazy danych.
     */
    void updateDatabaseStatus();
};