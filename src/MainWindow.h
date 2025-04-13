/**
 * @file MainWindow.h
 * @brief G��wne okno aplikacji do monitorowania jako�ci powietrza.
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
  * @brief Klasa implementuj�ca g��wne okno aplikacji.
  *
  * Zarz�dza interfejsem u�ytkownika, pobieraniem i wy�wietlaniem danych
  * z API jako�ci powietrza oraz lokalnej bazy danych.
  */
class MainWindow : public wxFrame {
public:
    /**
     * @brief Konstruktor g��wnego okna.
     *
     * Inicjalizuje interfejs aplikacji i �aduje dane stacji.
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
     * @brief Pole tekstowe do wy�wietlania danych pomiarowych.
     */
    wxTextCtrl* measurementText;

    /**
     * @brief Czcionka u�ywana w interfejsie.
     */
    wxFont font;

    /**
     * @brief Etykieta wy�wietlaj�ca indeks jako�ci powietrza.
     */
    wxStaticText* airQualityIndex;

    /**
     * @brief Przycisk do zapisania bie��cego pomiaru do bazy danych.
     */
    wxButton* saveCurrentToDbButton;

    /**
     * @brief Przycisk do zapisania wszystkich danych do bazy danych.
     */
    wxButton* saveAllToDbButton;

    /**
     * @brief Etykieta wy�wietlaj�ca status bazy danych.
     */
    wxStaticText* dbStatusText;

    //Analiza danych
    /**
     * @brief Przycisk do pokazania analizy danych.
     */
    wxButton* showAnalysisButton;

    /**
     * @brief Pole tekstowe do wy�wietlania analizy danych.
     */
    wxTextCtrl* analysisText;

    /**
     * @brief Kontrolka z zak�adkami dla r�nych widok�w danych.
     */
    wxNotebook* dataNotebook;

    //Chart Stuff
    /**
     * @brief Panel wykresu do wizualizacji danych pomiarowych.
     */
    ChartPanel* chartPanel;

    /**
     * @brief Kontrolka do wyboru daty pocz�tkowej zakresu.
     */
    wxDatePickerCtrl* startDatePicker;

    /**
     * @brief Kontrolka do wyboru daty ko�cowej zakresu.
     */
    wxDatePickerCtrl* endDatePicker;

    /**
     * @brief Przycisk do zastosowania wybranego zakresu dat.
     */
    wxButton* applyDateRangeButton;

    // Data Storage
    /**
     * @brief Wektor przechowuj�cy pary (ID, nazwa) dost�pnych stacji.
     */
    std::vector<std::pair<int, std::string>> stations;

    /**
     * @brief Wektor przechowuj�cy pary (ID, nazwa) dost�pnych czujnik�w.
     */
    std::vector<std::pair<int, std::string>> sensors;

    /**
     * @brief Us�uga do obs�ugi lokalnej bazy danych.
     */
    DatabaseService dbService;


    /**
     * @brief �aduje list� dost�pnych stacji pomiarowych.
     */
    void loadStations();

    /**
     * @brief �aduje czujniki dla wybranej stacji.
     *
     * @param stationId ID stacji, dla kt�rej nale�y za�adowa� czujniki
     */
    void loadSensorsForStation(int stationId);

    /**
     * @brief �aduje indeks jako�ci powietrza dla stacji.
     *
     * @param stationId ID stacji, dla kt�rej nale�y za�adowa� indeks jako�ci powietrza
     */
    void loadAirQualityIndex(int stationId);

    /**
     * @brief Konwertuje string daty na obiekt wxDateTime.
     *
     * @param dateStr String zawieraj�cy dat� w odpowiednim formacie
     * @return wxDateTime Skonwertowany obiekt daty i czasu
     */
    wxDateTime parseDateTime(const std::string& dateStr);


    /**
     * @brief Obs�uga zdarzenia wyboru stacji.
     *
     * @param event Zdarzenie wyboru
     */
    void OnStationSelection(wxCommandEvent& event);

    /**
     * @brief Obs�uga zdarzenia wyboru czujnika.
     *
     * @param event Zdarzenie wyboru
     */
    void OnSensorSelection(wxCommandEvent& event);

    /**
     * @brief Obs�uga zdarzenia zapisania bie��cego pomiaru do bazy danych.
     *
     * @param event Zdarzenie przycisku
     */
    void OnSaveCurrentToDatabase(wxCommandEvent& event);

    /**
     * @brief Obs�uga zdarzenia zapisania wszystkich danych do bazy danych.
     *
     * @param event Zdarzenie przycisku
     */
    void OnSaveAllToDatabase(wxCommandEvent& event);

    /**
     * @brief Obs�uga zdarzenia zastosowania zakresu dat dla wykresu.
     *
     * @param event Zdarzenie przycisku
     */
    void OnApplyDateRange(wxCommandEvent& event);

    // Metody
    /**
     * @brief Pr�buje za�adowa� stacje z API.
     *
     * @return true Je�li za�adowanie si� powiod�o
     * @return false Je�li wyst�pi� b��d
     */
    bool tryLoadStationsFromAPI();

    /**
     * @brief Pr�buje za�adowa� stacje z lokalnej bazy danych.
     *
     * @return true Je�li za�adowanie si� powiod�o
     * @return false Je�li wyst�pi� b��d
     */
    bool tryLoadStationsFromDatabase();

    /**
     * @brief Pr�buje za�adowa� czujniki dla stacji z API.
     *
     * @param stationId ID stacji
     * @return true Je�li za�adowanie si� powiod�o
     * @return false Je�li wyst�pi� b��d
     */
    bool tryLoadSensorsFromAPI(int stationId);

    /**
     * @brief Pr�buje za�adowa� czujniki dla stacji z lokalnej bazy danych.
     *
     * @param stationId ID stacji
     * @return true Je�li za�adowanie si� powiod�o
     * @return false Je�li wyst�pi� b��d
     */
    bool tryLoadSensorsFromDatabase(int stationId);

    /**
     * @brief Pr�buje za�adowa� pomiary z API.
     *
     * @param sensorId ID czujnika
     * @return true Je�li za�adowanie si� powiod�o
     * @return false Je�li wyst�pi� b��d
     */
    bool tryLoadMeasurementsFromAPI(int sensorId);

    /**
     * @brief Pr�buje za�adowa� pomiary z lokalnej bazy danych.
     *
     * @param sensorId ID czujnika
     * @return true Je�li za�adowanie si� powiod�o
     * @return false Je�li wyst�pi� b��d
     */
    bool tryLoadMeasurementsFromDatabase(int sensorId);

    /**
     * @brief Pr�buje za�adowa� indeks jako�ci powietrza z API.
     *
     * @param stationId ID stacji
     * @return true Je�li za�adowanie si� powiod�o
     * @return false Je�li wyst�pi� b��d
     */
    bool tryLoadAirQualityFromAPI(int stationId);

    /**
     * @brief Pr�buje za�adowa� indeks jako�ci powietrza z lokalnej bazy danych.
     *
     * @param stationId ID stacji
     * @return true Je�li za�adowanie si� powiod�o
     * @return false Je�li wyst�pi� b��d
     */
    bool tryLoadAirQualityFromDatabase(int stationId);

    /**
     * @brief Pr�buje za�adowa� pomiary z okre�lonego zakresu dat.
     *
     * @param sensorId ID czujnika
     * @return true Je�li za�adowanie si� powiod�o
     * @return false Je�li wyst�pi� b��d
     */
    bool tryLoadMeasurementsWithDateRange(int sensorId);

    /**
     * @brief Pr�buje za�adowa� pomiary z okre�lonego zakresu dat z lokalnej bazy danych.
     *
     * @param sensorId ID czujnika
     * @return true Je�li za�adowanie si� powiod�o
     * @return false Je�li wyst�pi� b��d
     */
    bool tryLoadMeasurementsWithDateRangeFromDatabase(int sensorId);


    /**
     * @brief Obs�uga zdarzenia pokazania analizy danych.
     *
     * @param event Zdarzenie przycisku
     */
    void OnShowAnalysis(wxCommandEvent& event);

    /**
     * @brief Wykonuje analiz� danych pomiarowych.
     *
     * Analizuje dane, obliczaj�c statystyki i przygotowuj�c raport.
     *
     * @param data Wektor danych pomiarowych do analizy
     * @param paramName Nazwa parametru, dla kt�rego wykonywana jest analiza
     */
    void PerformDataAnalysis(const std::vector<MeasurementData>& data, const wxString& paramName);


    /**
     * @brief Aktualizuje wy�wietlany status bazy danych.
     */
    void updateDatabaseStatus();
};