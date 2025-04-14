/**
 * @file DatabaseService.cpp
 * @brief Implementacja us�ugi bazy danych.
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
  * Inicjalizuje folder bazy danych i tworzy struktur� katalog�w.
  */
DatabaseService::DatabaseService() {
    // Utw�rz folder dla bazy danych w katalogu programu
    dbFolder = "LocalDB";
    initializeDatabase();
}

/**
 * @brief Inicjalizuje struktur� bazy danych.
 *
 * Sprawdza czy folder bazy istnieje, je�li nie - tworzy go.
 *
 * @return true Je�li folder istnieje lub zosta� utworzony
 * @return false Je�li utworzenie folderu nie powiod�o si�
 */
bool DatabaseService::initializeDatabase() {
    // Sprawd� czy folder istnieje, je�li nie - utw�rz go
    if (!wxDirExists(dbFolder)) {
        return wxMkdir(dbFolder);
    }
    return true;
}

/**
 * @brief Zapisuje dane stacji do pliku.
 *
 * @param jsonData Dane w formacie JSON
 * @return true Je�li zapis si� powi�d�
 * @return false Je�li wyst�pi� b��d
 */
bool DatabaseService::saveStationsData(const std::string& jsonData) {
    return saveToFile(getStationsFilePath(), jsonData);
}

/**
 * @brief Zapisuje dane czujnik�w dla stacji.
 *
 * @param stationId ID stacji
 * @param jsonData Dane w formacie JSON
 * @return true Je�li zapis si� powi�d�
 * @return false Je�li wyst�pi� b��d
 */
bool DatabaseService::saveSensorsData(int stationId, const std::string& jsonData) {
    return saveToFile(getSensorsFilePath(stationId), jsonData);
}

/**
 * @brief Zapisuje dane pomiarowe dla czujnika.
 *
 * @param sensorId ID czujnika
 * @param jsonData Dane w formacie JSON
 * @return true Je�li zapis si� powi�d�
 * @return false Je�li wyst�pi� b��d
 */
bool DatabaseService::saveMeasurementData(int sensorId, const std::string& jsonData) {
    return saveToFile(getMeasurementFilePath(sensorId), jsonData);
}

/**
 * @brief Zapisuje indeks jako�ci powietrza dla stacji.
 *
 * @param stationId ID stacji
 * @param jsonData Dane w formacie JSON
 * @return true Je�li zapis si� powi�d�
 * @return false Je�li wyst�pi� b��d
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
 * @brief Wczytuje dane czujnik�w dla stacji.
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
 * @brief Wczytuje indeks jako�ci powietrza dla stacji.
 *
 * @param stationId ID stacji
 * @return std::string Dane w formacie JSON lub pusty string gdy brak danych
 */
std::string DatabaseService::loadAirQualityIndex(int stationId) {
    return loadFromFile(getAirQualityFilePath(stationId));
}

/**
 * @brief Sprawdza czy dane stacji istniej� w bazie.
 *
 * @return true Je�li plik z danymi istnieje
 * @return false Je�li plik z danymi nie istnieje
 */
bool DatabaseService::hasStationsData() {
    return wxFileExists(getStationsFilePath());
}

/**
 * @brief Sprawdza czy dane czujnik�w dla stacji istniej� w bazie.
 *
 * @param stationId ID stacji
 * @return true Je�li plik z danymi istnieje
 * @return false Je�li plik z danymi nie istnieje
 */
bool DatabaseService::hasSensorsData(int stationId) {
    return wxFileExists(getSensorsFilePath(stationId));
}

/**
 * @brief Sprawdza czy dane pomiarowe dla czujnika istniej� w bazie.
 *
 * @param sensorId ID czujnika
 * @return true Je�li plik z danymi istnieje
 * @return false Je�li plik z danymi nie istnieje
 */
bool DatabaseService::hasMeasurementData(int sensorId) {
    return wxFileExists(getMeasurementFilePath(sensorId));
}

/**
 * @brief Sprawdza czy indeks jako�ci powietrza dla stacji istnieje w bazie.
 *
 * @param stationId ID stacji
 * @return true Je�li plik z danymi istnieje
 * @return false Je�li plik z danymi nie istnieje
 */
bool DatabaseService::hasAirQualityIndex(int stationId) {
    return wxFileExists(getAirQualityFilePath(stationId));
}

/**
 * @brief Pobiera czas ostatniej aktualizacji pliku.
 *
 * @param filePath �cie�ka do pliku
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
 * @brief Zwraca �cie�k� pliku z danymi stacji.
 *
 * @return std::string �cie�ka do pliku
 */
std::string DatabaseService::getStationsFilePath() {
    return dbFolder + "/stations.json";
}

/**
 * @brief Zwraca �cie�k� pliku z danymi czujnik�w dla stacji.
 *
 * @param stationId ID stacji
 * @return std::string �cie�ka do pliku
 */
std::string DatabaseService::getSensorsFilePath(int stationId) {
    return dbFolder + "/sensors_" + std::to_string(stationId) + ".json";
}

/**
 * @brief Zwraca �cie�k� pliku z danymi pomiarowymi dla czujnika.
 *
 * @param sensorId ID czujnika
 * @return std::string �cie�ka do pliku
 */
std::string DatabaseService::getMeasurementFilePath(int sensorId) {
    return dbFolder + "/measurements_" + std::to_string(sensorId) + ".json";
}

/**
 * @brief Zwraca �cie�k� pliku z indeksem jako�ci powietrza dla stacji.
 *
 * @param stationId ID stacji
 * @return std::string �cie�ka do pliku
 */
std::string DatabaseService::getAirQualityFilePath(int stationId) {
    return dbFolder + "/airquality_" + std::to_string(stationId) + ".json";
}

/**
 * @brief Zapisuje dane do pliku.
 *
 * @param filePath �cie�ka do pliku
 * @param data Dane do zapisania
 * @return true Je�li zapis si� powi�d�
 * @return false Je�li wyst�pi� b��d
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
 * @param filePath �cie�ka do pliku
 * @return std::string Odczytane dane lub pusty string gdy b��d
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