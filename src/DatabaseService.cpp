/**
 * @file DatabaseService.cpp
 * @brief Implementacja us³ugi bazy danych.
 */

#include "DatabaseService.h"
#include <wx/dir.h>
#include <wx/file.h>
#include <wx/filefn.h>
#include <wx/datetime.h>
#include <wx/filename.h>  
#include <fstream>
#include <iostream>

 /**
  * @brief Konstruktor klasy DatabaseService.
  *
  * Inicjalizuje folder bazy danych i tworzy strukturê katalogów.
  */
DatabaseService::DatabaseService() {
    // Utwórz folder dla bazy danych w katalogu programu
    dbFolder = "LocalDB";
    initializeDatabase();
}

/**
 * @brief Inicjalizuje strukturê bazy danych.
 *
 * Sprawdza czy folder bazy istnieje, jeœli nie - tworzy go.
 *
 * @return true Jeœli folder istnieje lub zosta³ utworzony
 * @return false Jeœli utworzenie folderu nie powiod³o siê
 */
bool DatabaseService::initializeDatabase() {
    // SprawdŸ czy folder istnieje, jeœli nie - utwórz go
    if (!wxDirExists(dbFolder)) {
        return wxMkdir(dbFolder);
    }
    return true;
}

/**
 * @brief Zapisuje dane stacji do pliku.
 *
 * @param jsonData Dane w formacie JSON
 * @return true Jeœli zapis siê powiód³
 * @return false Jeœli wyst¹pi³ b³¹d
 */
bool DatabaseService::saveStationsData(const std::string& jsonData) {
    return saveToFile(getStationsFilePath(), jsonData);
}

/**
 * @brief Zapisuje dane czujników dla stacji.
 *
 * @param stationId ID stacji
 * @param jsonData Dane w formacie JSON
 * @return true Jeœli zapis siê powiód³
 * @return false Jeœli wyst¹pi³ b³¹d
 */
bool DatabaseService::saveSensorsData(int stationId, const std::string& jsonData) {
    return saveToFile(getSensorsFilePath(stationId), jsonData);
}

/**
 * @brief Zapisuje dane pomiarowe dla czujnika.
 *
 * @param sensorId ID czujnika
 * @param jsonData Dane w formacie JSON
 * @return true Jeœli zapis siê powiód³
 * @return false Jeœli wyst¹pi³ b³¹d
 */
bool DatabaseService::saveMeasurementData(int sensorId, const std::string& jsonData) {
    return saveToFile(getMeasurementFilePath(sensorId), jsonData);
}

/**
 * @brief Zapisuje indeks jakoœci powietrza dla stacji.
 *
 * @param stationId ID stacji
 * @param jsonData Dane w formacie JSON
 * @return true Jeœli zapis siê powiód³
 * @return false Jeœli wyst¹pi³ b³¹d
 */
bool DatabaseService::saveAirQualityIndex(int stationId, const std::string& jsonData) {
    return saveToFile(getAirQualityFilePath(stationId), jsonData);
}

/**
 * @brief Wczytuje dane stacji z bazy.
 *
 * @return std::string Dane w formacie JSON lub pusty string gdy brak danych
 */
std::string DatabaseService::loadStationsData() {
    return loadFromFile(getStationsFilePath());
}

/**
 * @brief Wczytuje dane czujników dla stacji.
 *
 * @param stationId ID stacji
 * @return std::string Dane w formacie JSON lub pusty string gdy brak danych
 */
std::string DatabaseService::loadSensorsData(int stationId) {
    return loadFromFile(getSensorsFilePath(stationId));
}

/**
 * @brief Wczytuje dane pomiarowe dla czujnika.
 *
 * @param sensorId ID czujnika
 * @return std::string Dane w formacie JSON lub pusty string gdy brak danych
 */
std::string DatabaseService::loadMeasurementData(int sensorId) {
    return loadFromFile(getMeasurementFilePath(sensorId));
}

/**
 * @brief Wczytuje indeks jakoœci powietrza dla stacji.
 *
 * @param stationId ID stacji
 * @return std::string Dane w formacie JSON lub pusty string gdy brak danych
 */
std::string DatabaseService::loadAirQualityIndex(int stationId) {
    return loadFromFile(getAirQualityFilePath(stationId));
}

/**
 * @brief Sprawdza czy dane stacji istniej¹ w bazie.
 *
 * @return true Jeœli plik z danymi istnieje
 * @return false Jeœli plik z danymi nie istnieje
 */
bool DatabaseService::hasStationsData() {
    return wxFileExists(getStationsFilePath());
}

/**
 * @brief Sprawdza czy dane czujników dla stacji istniej¹ w bazie.
 *
 * @param stationId ID stacji
 * @return true Jeœli plik z danymi istnieje
 * @return false Jeœli plik z danymi nie istnieje
 */
bool DatabaseService::hasSensorsData(int stationId) {
    return wxFileExists(getSensorsFilePath(stationId));
}

/**
 * @brief Sprawdza czy dane pomiarowe dla czujnika istniej¹ w bazie.
 *
 * @param sensorId ID czujnika
 * @return true Jeœli plik z danymi istnieje
 * @return false Jeœli plik z danymi nie istnieje
 */
bool DatabaseService::hasMeasurementData(int sensorId) {
    return wxFileExists(getMeasurementFilePath(sensorId));
}

/**
 * @brief Sprawdza czy indeks jakoœci powietrza dla stacji istnieje w bazie.
 *
 * @param stationId ID stacji
 * @return true Jeœli plik z danymi istnieje
 * @return false Jeœli plik z danymi nie istnieje
 */
bool DatabaseService::hasAirQualityIndex(int stationId) {
    return wxFileExists(getAirQualityFilePath(stationId));
}

/**
 * @brief Pobiera czas ostatniej aktualizacji pliku.
 *
 * @param filePath Œcie¿ka do pliku
 * @return wxDateTime Czas ostatniej modyfikacji
 */
wxDateTime DatabaseService::getLastUpdateTime(const std::string& filePath) {
    wxDateTime lastModTime;
    if (wxFileExists(filePath)) {
        wxFileName fileName(filePath);
        bool success = fileName.GetTimes(NULL, &lastModTime, NULL);
        if (!success || !lastModTime.IsValid()) {
            lastModTime = wxDateTime::Now(); // Aktualna data jako zabezpieczenie
        }
    }
    else {
        lastModTime = wxDateTime::Now(); 
    }
    return lastModTime;
}

/**
 * @brief Zwraca œcie¿kê pliku z danymi stacji.
 *
 * @return std::string Œcie¿ka do pliku
 */
std::string DatabaseService::getStationsFilePath() {
    return dbFolder + "/stations.json";
}

/**
 * @brief Zwraca œcie¿kê pliku z danymi czujników dla stacji.
 *
 * @param stationId ID stacji
 * @return std::string Œcie¿ka do pliku
 */
std::string DatabaseService::getSensorsFilePath(int stationId) {
    return dbFolder + "/sensors_" + std::to_string(stationId) + ".json";
}

/**
 * @brief Zwraca œcie¿kê pliku z danymi pomiarowymi dla czujnika.
 *
 * @param sensorId ID czujnika
 * @return std::string Œcie¿ka do pliku
 */
std::string DatabaseService::getMeasurementFilePath(int sensorId) {
    return dbFolder + "/measurements_" + std::to_string(sensorId) + ".json";
}

/**
 * @brief Zwraca œcie¿kê pliku z indeksem jakoœci powietrza dla stacji.
 *
 * @param stationId ID stacji
 * @return std::string Œcie¿ka do pliku
 */
std::string DatabaseService::getAirQualityFilePath(int stationId) {
    return dbFolder + "/airquality_" + std::to_string(stationId) + ".json";
}

/**
 * @brief Zapisuje dane do pliku.
 *
 * @param filePath Œcie¿ka do pliku
 * @param data Dane do zapisania
 * @return true Jeœli zapis siê powiód³
 * @return false Jeœli wyst¹pi³ b³¹d
 */
bool DatabaseService::saveToFile(const std::string& filePath, const std::string& data) {
    std::ofstream file(filePath);
    if (!file.is_open()) {
        return false;
    }
    file << data;
    return !file.bad();
}

/**
 * @brief Odczytuje dane z pliku.
 *
 * @param filePath Œcie¿ka do pliku
 * @return std::string Odczytane dane lub pusty string gdy b³¹d
 */
std::string DatabaseService::loadFromFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        return "";
    }
    std::string content((std::istreambuf_iterator<char>(file)),
        std::istreambuf_iterator<char>());
    return content;
}