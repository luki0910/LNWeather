/**
 * @file DatabaseService.h
 * @brief Us³uga zarz¹dzaj¹ca lokaln¹ baz¹ danych.
 */

#pragma once

#include <string>
#include <vector>
#include <wx/wx.h>
#include <json.hpp>
#include "Utils.h"

 /**
  * @class DatabaseService
  * @brief Klasa obs³uguj¹ca lokaln¹ "bazê danych" (pliki JSON).
  *
  * Zapewnia funkcje do zapisywania, odczytywania i zarz¹dzania
  * danymi przechowywanymi lokalnie w plikach JSON.
  */
class DatabaseService {
public:
    /**
     * @brief Konstruktor us³ugi bazy danych.
     *
     * Inicjalizuje œcie¿kê do folderu bazy danych i tworzy
     * strukturê katalogów.
     */
    DatabaseService();

    /**
     * @brief Inicjalizuje strukturê bazy danych.
     *
     * Tworzy folder dla bazy danych, jeœli nie istnieje.
     *
     * @return true Jeœli inicjalizacja siê powiod³a
     * @return false Jeœli wyst¹pi³ b³¹d
     */
    bool initializeDatabase();

    /**
     * @brief Zapisuje dane stacji do bazy.
     *
     * @param jsonData Dane w formacie JSON
     * @return true Jeœli zapis siê powiód³
     * @return false Jeœli wyst¹pi³ b³¹d
     */
    bool saveStationsData(const std::string& jsonData);

    /**
     * @brief Zapisuje dane czujników dla stacji.
     *
     * @param stationId ID stacji
     * @param jsonData Dane w formacie JSON
     * @return true Jeœli zapis siê powiód³
     * @return false Jeœli wyst¹pi³ b³¹d
     */
    bool saveSensorsData(int stationId, const std::string& jsonData);

    /**
     * @brief Zapisuje dane pomiarowe dla czujnika.
     *
     * @param sensorId ID czujnika
     * @param jsonData Dane w formacie JSON
     * @return true Jeœli zapis siê powiód³
     * @return false Jeœli wyst¹pi³ b³¹d
     */
    bool saveMeasurementData(int sensorId, const std::string& jsonData);

    /**
     * @brief Zapisuje indeks jakoœci powietrza dla stacji.
     *
     * @param stationId ID stacji
     * @param jsonData Dane w formacie JSON
     * @return true Jeœli zapis siê powiód³
     * @return false Jeœli wyst¹pi³ b³¹d
     */
    bool saveAirQualityIndex(int stationId, const std::string& jsonData);

    /**
     * @brief Wczytuje dane stacji z bazy.
     *
     * @return std::string Dane w formacie JSON
     */
    std::string loadStationsData();

    /**
     * @brief Wczytuje dane czujników dla stacji.
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
     * @brief Wczytuje indeks jakoœci powietrza dla stacji.
     *
     * @param stationId ID stacji
     * @return std::string Dane w formacie JSON
     */
    std::string loadAirQualityIndex(int stationId);

    /**
     * @brief Sprawdza czy dane stacji istniej¹ w bazie.
     *
     * @return true Jeœli dane istniej¹
     * @return false Jeœli dane nie istniej¹
     */
    bool hasStationsData();

    /**
     * @brief Sprawdza czy dane czujników dla stacji istniej¹ w bazie.
     *
     * @param stationId ID stacji
     * @return true Jeœli dane istniej¹
     * @return false Jeœli dane nie istniej¹
     */
    bool hasSensorsData(int stationId);

    /**
     * @brief Sprawdza czy dane pomiarowe dla czujnika istniej¹ w bazie.
     *
     * @param sensorId ID czujnika
     * @return true Jeœli dane istniej¹
     * @return false Jeœli dane nie istniej¹
     */
    bool hasMeasurementData(int sensorId);

    /**
     * @brief Sprawdza czy indeks jakoœci powietrza dla stacji istnieje w bazie.
     *
     * @param stationId ID stacji
     * @return true Jeœli dane istniej¹
     * @return false Jeœli dane nie istniej¹
     */
    bool hasAirQualityIndex(int stationId);

    /**
     * @brief Pobiera czas ostatniej aktualizacji pliku.
     *
     * @param filePath Œcie¿ka do pliku
     * @return wxDateTime Czas ostatniej modyfikacji
     */
    wxDateTime getLastUpdateTime(const std::string& filePath);

    /**
     * @brief Zwraca œcie¿kê pliku z danymi stacji.
     *
     * @return std::string Œcie¿ka do pliku
     */
    std::string getStationsFilePath();

    /**
     * @brief Zwraca œcie¿kê pliku z danymi czujników dla stacji.
     *
     * @param stationId ID stacji
     * @return std::string Œcie¿ka do pliku
     */
    std::string getSensorsFilePath(int stationId);

    /**
     * @brief Zwraca œcie¿kê pliku z danymi pomiarowymi dla czujnika.
     *
     * @param sensorId ID czujnika
     * @return std::string Œcie¿ka do pliku
     */
    std::string getMeasurementFilePath(int sensorId);

    /**
     * @brief Zwraca œcie¿kê pliku z indeksem jakoœci powietrza dla stacji.
     *
     * @param stationId ID stacji
     * @return std::string Œcie¿ka do pliku
     */
    std::string getAirQualityFilePath(int stationId);

private:
    std::string dbFolder;  /**< Œcie¿ka do folderu bazy danych */

    /**
     * @brief Zapisuje dane do pliku.
     *
     * @param filePath Œcie¿ka do pliku
     * @param data Dane do zapisania
     * @return true Jeœli zapis siê powiód³
     * @return false Jeœli wyst¹pi³ b³¹d
     */
    bool saveToFile(const std::string& filePath, const std::string& data);

    /**
     * @brief Odczytuje dane z pliku.
     *
     * @param filePath Œcie¿ka do pliku
     * @return std::string Odczytane dane
     */
    std::string loadFromFile(const std::string& filePath);
};