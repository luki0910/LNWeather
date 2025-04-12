#include "DatabaseService.h"
#include <wx/dir.h>
#include <wx/file.h>
#include <wx/filefn.h>
#include <wx/datetime.h>
#include <wx/filename.h>  
#include <fstream>
#include <iostream>

DatabaseService::DatabaseService() {
    // Utwórz folder dla bazy danych w katalogu programu
    dbFolder = "LocalDB";
    initializeDatabase();
}

bool DatabaseService::initializeDatabase() {
    // SprawdŸ czy folder istnieje, jeœli nie - utwórz go
    if (!wxDirExists(dbFolder)) {
        return wxMkdir(dbFolder);
    }
    return true;
}

bool DatabaseService::saveStationsData(const std::string& jsonData) {
    return saveToFile(getStationsFilePath(), jsonData);
}

bool DatabaseService::saveSensorsData(int stationId, const std::string& jsonData) {
    return saveToFile(getSensorsFilePath(stationId), jsonData);
}

bool DatabaseService::saveMeasurementData(int sensorId, const std::string& jsonData) {
    return saveToFile(getMeasurementFilePath(sensorId), jsonData);
}

bool DatabaseService::saveAirQualityIndex(int stationId, const std::string& jsonData) {
    return saveToFile(getAirQualityFilePath(stationId), jsonData);
}

std::string DatabaseService::loadStationsData() {
    return loadFromFile(getStationsFilePath());
}

std::string DatabaseService::loadSensorsData(int stationId) {
    return loadFromFile(getSensorsFilePath(stationId));
}

std::string DatabaseService::loadMeasurementData(int sensorId) {
    return loadFromFile(getMeasurementFilePath(sensorId));
}

std::string DatabaseService::loadAirQualityIndex(int stationId) {
    return loadFromFile(getAirQualityFilePath(stationId));
}

bool DatabaseService::hasStationsData() {
    return wxFileExists(getStationsFilePath());
}

bool DatabaseService::hasSensorsData(int stationId) {
    return wxFileExists(getSensorsFilePath(stationId));
}

bool DatabaseService::hasMeasurementData(int sensorId) {
    return wxFileExists(getMeasurementFilePath(sensorId));
}

bool DatabaseService::hasAirQualityIndex(int stationId) {
    return wxFileExists(getAirQualityFilePath(stationId));
}

wxDateTime DatabaseService::getLastUpdateTime(const std::string& filePath) {
    wxDateTime lastModTime;
    if (wxFileExists(filePath)) {
        wxFileName fileName(filePath);
        bool success = fileName.GetTimes(NULL, &lastModTime, NULL);
        if (!success || !lastModTime.IsValid()) {
            lastModTime = wxDateTime::Now(); // U¿yj aktualnej daty jako zabezpieczenie
        }
    }
    else {
        lastModTime = wxDateTime::Now(); // U¿yj aktualnej daty jako zabezpieczenie
    }
    return lastModTime;
}

std::string DatabaseService::getStationsFilePath() {
    return dbFolder + "/stations.json";
}

std::string DatabaseService::getSensorsFilePath(int stationId) {
    return dbFolder + "/sensors_" + std::to_string(stationId) + ".json";
}

std::string DatabaseService::getMeasurementFilePath(int sensorId) {
    return dbFolder + "/measurements_" + std::to_string(sensorId) + ".json";
}

std::string DatabaseService::getAirQualityFilePath(int stationId) {
    return dbFolder + "/airquality_" + std::to_string(stationId) + ".json";
}

bool DatabaseService::saveToFile(const std::string& filePath, const std::string& data) {
    std::ofstream file(filePath);
    if (!file.is_open()) {
        return false;
    }
    file << data;
    return !file.bad();
}

std::string DatabaseService::loadFromFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        return "";
    }
    std::string content((std::istreambuf_iterator<char>(file)),
        std::istreambuf_iterator<char>());
    return content;
}