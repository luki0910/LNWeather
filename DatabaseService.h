#pragma once

#include <string>
#include <vector>
#include <wx/wx.h>
#include <json.hpp>
#include "Utils.h"

// Klasa obs³uguj¹ca lokaln¹ "bazê danych" (pliki JSON)
class DatabaseService {
public:
    DatabaseService();

    // Tworzenie folderu dla bazy danych
    bool initializeDatabase();

    // Zapisywanie danych do lokalnej bazy
    bool saveStationsData(const std::string& jsonData);
    bool saveSensorsData(int stationId, const std::string& jsonData);
    bool saveMeasurementData(int sensorId, const std::string& jsonData);
    bool saveAirQualityIndex(int stationId, const std::string& jsonData);

    // Pobieranie danych z lokalnej bazy
    std::string loadStationsData();
    std::string loadSensorsData(int stationId);
    std::string loadMeasurementData(int sensorId);
    std::string loadAirQualityIndex(int stationId);

    // Sprawdzanie czy dane istniej¹ w lokalnej bazie
    bool hasStationsData();
    bool hasSensorsData(int stationId);
    bool hasMeasurementData(int sensorId);
    bool hasAirQualityIndex(int stationId);

    // Pobieranie czasu ostatniej aktualizacji danych
    wxDateTime getLastUpdateTime(const std::string& filePath);

    // Metody zwracaj¹ce œcie¿ki plików (przeniesione z private do public)
    std::string getStationsFilePath();
    std::string getSensorsFilePath(int stationId);
    std::string getMeasurementFilePath(int sensorId);
    std::string getAirQualityFilePath(int stationId);

private:
    std::string dbFolder;

    // Zapisywanie do pliku
    bool saveToFile(const std::string& filePath, const std::string& data);
    // Odczytywanie z pliku
    std::string loadFromFile(const std::string& filePath);
};
