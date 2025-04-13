/**
 * @file DatabaseService.h
 * @brief Us�uga zarz�dzaj�ca lokaln� baz� danych.
 */

#pragma once

#include <string>
#include <vector>
#include <wx/wx.h>
#include <json.hpp>
#include "Utils.h"

 /**
  * @class DatabaseService
  * @brief Klasa obs�uguj�ca lokaln� "baz� danych" (pliki JSON).
  *
  * Zapewnia funkcje do zapisywania, odczytywania i zarz�dzania
  * danymi przechowywanymi lokalnie w plikach JSON.
  */
class DatabaseService {
public:
    /**
     * @brief Konstruktor us�ugi bazy danych.
     *
     * Inicjalizuje �cie�k� do folderu bazy danych i tworzy
     * struktur� katalog�w.
     */
    DatabaseService();

    /**
     * @brief Inicjalizuje struktur� bazy danych.
     *
     * Tworzy folder dla bazy danych, je�li nie istnieje.
     *
     * @return true Je�li inicjalizacja si� powiod�a
     * @return false Je�li wyst�pi� b��d
     */
    bool initializeDatabase();

    /**
     * @brief Zapisuje dane stacji do bazy.
     *
     * @param jsonData Dane w formacie JSON
     * @return true Je�li zapis si� powi�d�
     * @return false Je�li wyst�pi� b��d
     */
    bool saveStationsData(const std::string& jsonData);

    /**
     * @brief Zapisuje dane czujnik�w dla stacji.
     *
     * @param stationId ID stacji
     * @param jsonData Dane w formacie JSON
     * @return true Je�li zapis si� powi�d�
     * @return false Je�li wyst�pi� b��d
     */
    bool saveSensorsData(int stationId, const std::string& jsonData);

    /**
     * @brief Zapisuje dane pomiarowe dla czujnika.
     *
     * @param sensorId ID czujnika
     * @param jsonData Dane w formacie JSON
     * @return true Je�li zapis si� powi�d�
     * @return false Je�li wyst�pi� b��d
     */
    bool saveMeasurementData(int sensorId, const std::string& jsonData);

    /**
     * @brief Zapisuje indeks jako�ci powietrza dla stacji.
     *
     * @param stationId ID stacji
     * @param jsonData Dane w formacie JSON
     * @return true Je�li zapis si� powi�d�
     * @return false Je�li wyst�pi� b��d
     */
    bool saveAirQualityIndex(int stationId, const std::string& jsonData);

    /**
     * @brief Wczytuje dane stacji z bazy.
     *
     * @return std::string Dane w formacie JSON
     */
    std::string loadStationsData();

    /**
     * @brief Wczytuje dane czujnik�w dla stacji.
     *
     * @param stationId ID stacji
     * @return std::string Dane w formacie JSON
     */
    std::string loadSensorsData(int stationId);

    /**
     * @brief Wczytuje dane pomiarowe dla czujnika.
     *
     * @param sensorId ID czujnika
     * @return std::string Dane w formacie JSON
     */
    std::string loadMeasurementData(int sensorId);

    /**
     * @brief Wczytuje indeks jako�ci powietrza dla stacji.
     *
     * @param stationId ID stacji
     * @return std::string Dane w formacie JSON
     */
    std::string loadAirQualityIndex(int stationId);

    /**
     * @brief Sprawdza czy dane stacji istniej� w bazie.
     *
     * @return true Je�li dane istniej�
     * @return false Je�li dane nie istniej�
     */
    bool hasStationsData();

    /**
     * @brief Sprawdza czy dane czujnik�w dla stacji istniej� w bazie.
     *
     * @param stationId ID stacji
     * @return true Je�li dane istniej�
     * @return false Je�li dane nie istniej�
     */
    bool hasSensorsData(int stationId);

    /**
     * @brief Sprawdza czy dane pomiarowe dla czujnika istniej� w bazie.
     *
     * @param sensorId ID czujnika
     * @return true Je�li dane istniej�
     * @return false Je�li dane nie istniej�
     */
    bool hasMeasurementData(int sensorId);

    /**
     * @brief Sprawdza czy indeks jako�ci powietrza dla stacji istnieje w bazie.
     *
     * @param stationId ID stacji
     * @return true Je�li dane istniej�
     * @return false Je�li dane nie istniej�
     */
    bool hasAirQualityIndex(int stationId);

    /**
     * @brief Pobiera czas ostatniej aktualizacji pliku.
     *
     * @param filePath �cie�ka do pliku
     * @return wxDateTime Czas ostatniej modyfikacji
     */
    wxDateTime getLastUpdateTime(const std::string& filePath);

    /**
     * @brief Zwraca �cie�k� pliku z danymi stacji.
     *
     * @return std::string �cie�ka do pliku
     */
    std::string getStationsFilePath();

    /**
     * @brief Zwraca �cie�k� pliku z danymi czujnik�w dla stacji.
     *
     * @param stationId ID stacji
     * @return std::string �cie�ka do pliku
     */
    std::string getSensorsFilePath(int stationId);

    /**
     * @brief Zwraca �cie�k� pliku z danymi pomiarowymi dla czujnika.
     *
     * @param sensorId ID czujnika
     * @return std::string �cie�ka do pliku
     */
    std::string getMeasurementFilePath(int sensorId);

    /**
     * @brief Zwraca �cie�k� pliku z indeksem jako�ci powietrza dla stacji.
     *
     * @param stationId ID stacji
     * @return std::string �cie�ka do pliku
     */
    std::string getAirQualityFilePath(int stationId);

private:
    std::string dbFolder;  /**< �cie�ka do folderu bazy danych */

    /**
     * @brief Zapisuje dane do pliku.
     *
     * @param filePath �cie�ka do pliku
     * @param data Dane do zapisania
     * @return true Je�li zapis si� powi�d�
     * @return false Je�li wyst�pi� b��d
     */
    bool saveToFile(const std::string& filePath, const std::string& data);

    /**
     * @brief Odczytuje dane z pliku.
     *
     * @param filePath �cie�ka do pliku
     * @return std::string Odczytane dane
     */
    std::string loadFromFile(const std::string& filePath);
};