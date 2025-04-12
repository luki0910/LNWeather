#include "MainWindow.h"
#include "ApiService.h"
#include "Utils.h"
#include <json.hpp>
#include <fstream>
#include <algorithm>

using json = nlohmann::json;

MainWindow::MainWindow() : wxFrame(NULL, wxID_ANY, "Dane pomiarowe - GIOŚ", wxDefaultPosition, wxSize(1000, 700)) {
    wxPanel* mainPanel = new wxPanel(this, wxID_ANY);
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* dataSizer = new wxBoxSizer(wxHORIZONTAL);

    //Panel kontrolny (lewy)
    wxBoxSizer* controlSizer = new wxBoxSizer(wxVERTICAL);

    //Elementy panelu (lewego)
    stationChoice = new wxChoice(mainPanel, wxID_ANY);
    sensorChoice = new wxChoice(mainPanel, wxID_ANY);
    measurementText = new wxTextCtrl(mainPanel, wxID_ANY, "Dane pomiarowe", wxDefaultPosition, wxSize(300, 400), wxTE_MULTILINE | wxTE_READONLY | wxTE_RICH2);
    airQualityIndex = new wxStaticText(mainPanel, wxID_ANY, "Indeks jakości powietrza", wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE_HORIZONTAL);
    saveCurrentToDbButton = new wxButton(mainPanel, wxID_ANY, "Zapisz dane bieżącej stacji do bazy lokalnej");
    saveAllToDbButton = new wxButton(mainPanel, wxID_ANY, "Zapisz dane wszystkich stacji do bazy lokalnej");
    dbStatusText = new wxStaticText(mainPanel, wxID_ANY, "Status bazy: Nieznany", wxDefaultPosition, wxDefaultSize);

    //Elementy do wykresu
    startDatePicker = new wxDatePickerCtrl(mainPanel, wxID_ANY);
    endDatePicker = new wxDatePickerCtrl(mainPanel, wxID_ANY, wxDefaultDateTime, wxDefaultPosition, wxDefaultSize, wxDP_DEFAULT | wxDP_SHOWCENTURY);
    applyDateRangeButton = new wxButton(mainPanel, wxID_ANY, "Zastosuj zakres dat");

    //Wybór stacji
    controlSizer->Add(new wxStaticText(mainPanel, wxID_ANY, "Wybierz stację"), 0, wxALL, 5);
    controlSizer->Add(stationChoice, 0, wxEXPAND | wxALL, 5);

    //Wybór sensora
    controlSizer->Add(new wxStaticText(mainPanel, wxID_ANY, "Wybierz czujnik"), 0, wxALL, 5);
    controlSizer->Add(sensorChoice, 0, wxEXPAND | wxALL, 5);

    //Pozostałe
    controlSizer->Add(airQualityIndex, 0, wxALL, 10);
    controlSizer->Add(measurementText, 1, wxALL | wxEXPAND, 10);
    controlSizer->Add(saveCurrentToDbButton, 0, wxALL | wxEXPAND, 10);
    controlSizer->Add(saveAllToDbButton, 0, wxALL | wxEXPAND, 10);
    controlSizer->Add(dbStatusText, 0, wxALL, 10);
    
    //Do wykresu
    wxBoxSizer* dateRangeSizer = new wxBoxSizer(wxHORIZONTAL);
    dateRangeSizer->Add(new wxStaticText(mainPanel, wxID_ANY, "Od:"), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    dateRangeSizer->Add(startDatePicker, 0, wxRIGHT, 10);
    dateRangeSizer->Add(new wxStaticText(mainPanel, wxID_ANY, "Do:"), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    dateRangeSizer->Add(endDatePicker, 0, wxRIGHT, 10);
    dateRangeSizer->Add(applyDateRangeButton, 0);
    controlSizer->Add(dateRangeSizer, 0, wxALL, 10);

    // Inicjalizacja czcionki
    font = wxFont(9, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
    measurementText->SetFont(font);

    // Panel wykresu (prawy)
    chartPanel = new ChartPanel(mainPanel);

    // Dodaj panele do głównego sizer'a
    dataSizer->Add(controlSizer, 1, wxEXPAND | wxALL, 10);
    dataSizer->Add(chartPanel, 2, wxEXPAND | wxALL, 10);

    mainSizer->Add(dataSizer, 1, wxEXPAND);
    mainPanel->SetSizer(mainSizer);

    // Przypisz eventy
    stationChoice->Bind(wxEVT_CHOICE, &MainWindow::OnStationSelection, this);
    sensorChoice->Bind(wxEVT_CHOICE, &MainWindow::OnSensorSelection, this);
    saveCurrentToDbButton->Bind(wxEVT_BUTTON, &MainWindow::OnSaveCurrentToDatabase, this);
    saveAllToDbButton->Bind(wxEVT_BUTTON, &MainWindow::OnSaveAllToDatabase, this);
    applyDateRangeButton->Bind(wxEVT_BUTTON, &MainWindow::OnApplyDateRange, this);

    // Inicjalizacja bazy danych
    dbService.initializeDatabase();
    updateDatabaseStatus();

    // Załaduj stacje
    loadStations();

    // Kontrolki wykresu
    wxDateTime today = wxDateTime::Now();
    wxDateTime weekAgo = today;
    weekAgo.Subtract(wxDateSpan(0, 0, 1, 0)); // tydzień wstecz
    startDatePicker->SetValue(weekAgo);
    endDatePicker->SetValue(today);
}

bool compareStationsByName(const std::pair<int, std::string>& a, const std::pair<int, std::string>& b) {
    return a.second < b.second;
}

void MainWindow::updateDatabaseStatus() {
    if (dbService.hasStationsData()) {
        wxDateTime lastUpdate = dbService.getLastUpdateTime(dbService.getStationsFilePath());
        if (lastUpdate.IsValid()) {
            dbStatusText->SetLabel(wxString::Format("Status bazy: Aktywna (ostatnia aktualizacja: %s)",
                lastUpdate.FormatDate()));
            dbStatusText->SetForegroundColour(*wxGREEN);
        }
        else {
            dbStatusText->SetLabel("Status bazy: Aktywna (brak informacji o dacie aktualizacji)");
            dbStatusText->SetForegroundColour(*wxGREEN);
        }
    }
    else {
        dbStatusText->SetLabel("Status bazy: Brak danych lokalnych");
        dbStatusText->SetForegroundColour(*wxRED);
    }
}

void MainWindow::loadStations() {
    bool apiAvailable = ApiService::isNetworkAvailable();
    bool hasLocalData = dbService.hasStationsData();

    // Jeśli jest dostęp do internetu, najpierw próbujemy z API
    if (apiAvailable) {
        if (tryLoadStationsFromAPI()) {
            return; // Sukces - dane pobrane z API
        }
    }

    // Jeśli nie udało się pobrać z API lub nie ma internetu, próbujemy z bazy
    if (hasLocalData) {
        if (tryLoadStationsFromDatabase()) {
            return; // Sukces - dane pobrane z lokalnej bazy
        }
    }

    // Jeśli dotarliśmy tutaj, nie udało się pobrać danych ani z API, ani z bazy
    wxString errorMsg;
    if (apiAvailable) {
        errorMsg = "Nie można załadować stacji z API ani z lokalnej bazy danych.";
    }
    else {
        errorMsg = "Nie można załadować stacji z lokalnej bazy danych (brak połączenia z internetem).";
    }

    wxMessageBox(errorMsg, "Błąd", wxOK | wxICON_ERROR);
}

void MainWindow::loadSensorsForStation(int stationId) {
    sensorChoice->Clear();
    sensors.clear();

    bool apiAvailable = ApiService::isNetworkAvailable();
    bool hasLocalData = dbService.hasSensorsData(stationId);

    // Jeśli jest dostęp do internetu, najpierw próbujemy z API
    if (apiAvailable) {
        if (tryLoadSensorsFromAPI(stationId)) {
            // Sukces - dane pobrane z API
        }
        else if (hasLocalData) {
            // Jeśli API nie zadziałało, próbujemy z bazy
            tryLoadSensorsFromDatabase(stationId);
        }
        else {
            wxMessageBox(wxString::Format("Nie można załadować czujników dla stacji %d ani z API, ani z lokalnej bazy danych.",
                stationId), "Błąd", wxOK | wxICON_ERROR);
        }
    }
    else {
        // Brak internetu, próbujemy tylko z bazy
        if (!tryLoadSensorsFromDatabase(stationId)) {
            wxMessageBox(wxString::Format("Nie można załadować czujników dla stacji %d z lokalnej bazy danych (brak połączenia z internetem).",
                stationId), "Błąd", wxOK | wxICON_ERROR);
        }
    }
}

void MainWindow::loadAirQualityIndex(int stationId) {
    // Sprawdź czy istnieją dane w lokalnej bazie
    bool hasLocalData = dbService.hasAirQualityIndex(stationId);

    // Jeśli jest połączenie internetowe, najpierw próbujemy z API
    if (ApiService::isNetworkAvailable()) {
        if (tryLoadAirQualityFromAPI(stationId)) {
            return; // Sukces - dane pobrane z API
        }
    }

    // Jeśli nie udało się pobrać z API lub nie ma internetu, próbujemy z bazy
    if (hasLocalData) {
        if (tryLoadAirQualityFromDatabase(stationId)) {
            return; // Sukces - dane pobrane z lokalnej bazy
        }
    }

    // Jeśli dotarliśmy tutaj, nie udało się pobrać danych ani z API, ani z bazy
    wxString errorMsg;
    if (ApiService::isNetworkAvailable()) {
        errorMsg = wxString::Format("Nie można załadować indeksu jakości powietrza dla stacji %d ani z API, ani z lokalnej bazy danych.", stationId);
    }
    else {
        errorMsg = wxString::Format("Nie można załadować indeksu jakości powietrza dla stacji %d z lokalnej bazy danych (brak połączenia z internetem).", stationId);
    }

    // Wyświetl błąd tylko jeśli nie ma danych w bazie
    if (!hasLocalData) {
        wxMessageBox(errorMsg, "Błąd", wxOK | wxICON_ERROR);
    }

    airQualityIndex->SetLabel("Indeks jakości powietrza: Brak danych");
    airQualityIndex->SetForegroundColour(*wxBLACK);
}

void MainWindow::OnStationSelection(wxCommandEvent& event) {
    int selectedIdx = stationChoice->GetSelection();
    if (selectedIdx == wxNOT_FOUND) return;

    int stationId = stations[selectedIdx].first;

    // Ładowanie czujników i indeksu jakości powietrza
    loadSensorsForStation(stationId);
    loadAirQualityIndex(stationId);

    // Zresetuj wykres
    chartPanel->SetData(std::vector<MeasurementData>(), "");
}

bool MainWindow::tryLoadStationsFromAPI() {
    std::string response = ApiService::fetchDataFromAPI("https://api.gios.gov.pl/pjp-api/rest/station/findAll");
    if (response.empty()) {
        return false;
    }

    try {
        json stationsData = json::parse(response);

        if (!stationsData.is_array()) {
            return false;
        }

        stations.clear();
        stationChoice->Clear();

        // Dodaj stacje do wektora
        for (const auto& station : stationsData) {
            if (station.contains("id") && station.contains("stationName")) {
                int id = station["id"];
                std::string name = station["stationName"];
                stations.push_back({ id, name });
            }
        }

        // Sortuj alfabetycznie według nazwy
        std::sort(stations.begin(), stations.end(), compareStationsByName);

        // Dodaj posortowane stacje do kontrolki wyboru
        for (const auto& station : stations) {
            stationChoice->Append(wxString::FromUTF8(station.second.c_str()));
        }

        // Zapisz do bazy danych
        dbService.saveStationsData(response);
        updateDatabaseStatus();
        return true;
    }
    catch (json::exception& e) {
        wxMessageBox(wxString::Format("Błąd parsowania stacji: %s", e.what()), "Error", wxOK | wxICON_ERROR);
    }
    catch (...) {
        wxMessageBox("Nieznany błąd parsowania stacji.", "Error", wxOK | wxICON_ERROR);
    }

    return false;
}

bool MainWindow::tryLoadStationsFromDatabase() {
    std::string data = dbService.loadStationsData();
    if (data.empty()) {
        return false;
    }

    try {
        json stationsData = json::parse(data);

        if (!stationsData.is_array()) {
            return false;
        }

        stations.clear();
        stationChoice->Clear();

        // Dodaj stacje do wektora
        for (const auto& station : stationsData) {
            if (station.contains("id") && station.contains("stationName")) {
                int id = station["id"];
                std::string name = station["stationName"];
                stations.push_back({ id, name });
            }
        }

        // Sortuj alfabetycznie według nazwy
        std::sort(stations.begin(), stations.end(), compareStationsByName);

        // Dodaj posortowane stacje do kontrolki wyboru
        for (const auto& station : stations) {
            stationChoice->Append(wxString::FromUTF8(station.second.c_str()));
        }

        wxDateTime lastUpdate = dbService.getLastUpdateTime(dbService.getStationsFilePath());
        wxMessageBox(wxString::Format("Załadowano dane stacji z lokalnej bazy (ostatnia aktualizacja: %s)",
            lastUpdate.FormatDate()),
            "Informacja", wxOK | wxICON_INFORMATION);

        return true;
    }
    catch (json::exception& e) {
        wxMessageBox(wxString::Format("Błąd parsowania stacji z bazy: %s", e.what()),
            "Error", wxOK | wxICON_ERROR);
    }
    catch (...) {
        wxMessageBox("Nieznany błąd parsowania stacji z bazy.", "Error", wxOK | wxICON_ERROR);
    }

    return false;
}

bool MainWindow::tryLoadSensorsFromAPI(int stationId) {
    std::string url = "https://api.gios.gov.pl/pjp-api/rest/station/sensors/" + std::to_string(stationId);
    std::string response = ApiService::fetchDataFromAPI(url);

    if (response.empty()) {
        return false;
    }

    try {
        json sensorsData = json::parse(response);

        if (!sensorsData.is_array()) {
            return false;
        }

        sensors.clear();
        sensorChoice->Clear();

        for (const auto& sensor : sensorsData) {
            if (sensor.contains("id") && sensor.contains("param") &&
                sensor["param"].contains("paramName")) {
                int id = sensor["id"];
                std::string name = sensor["param"]["paramName"];
                sensors.push_back({ id, name });
                sensorChoice->Append(wxString::FromUTF8(name.c_str()));
            }
        }

        // Zapisz do bazy danych
        dbService.saveSensorsData(stationId, response);

        // Wyczyść panel wykresu i tekst pomiarów, ponieważ nie wybrano jeszcze czujnika
        measurementText->SetValue("Wybierz czujnik aby zobaczyć pomiary");
        chartPanel->SetData(std::vector<MeasurementData>(), "");

        return true;
    }
    catch (json::exception& e) {
        wxMessageBox(wxString::Format("Błąd parsowania czujników: %s", e.what()),
            "Error", wxOK | wxICON_ERROR);
    }
    catch (...) {
        wxMessageBox("Nieznany błąd parsowania czujników.", "Error", wxOK | wxICON_ERROR);
    }

    return false;
}

bool MainWindow::tryLoadSensorsFromDatabase(int stationId) {
    std::string data = dbService.loadSensorsData(stationId);
    if (data.empty()) {
        return false;
    }

    try {
        json sensorsData = json::parse(data);

        if (!sensorsData.is_array()) {
            return false;
        }

        sensors.clear();
        sensorChoice->Clear();

        for (const auto& sensor : sensorsData) {
            if (sensor.contains("id") && sensor.contains("param") &&
                sensor["param"].contains("paramName")) {
                int id = sensor["id"];
                std::string name = sensor["param"]["paramName"];
                sensors.push_back({ id, name });
                sensorChoice->Append(wxString::FromUTF8(name.c_str()));
            }
        }

        wxDateTime lastUpdate = dbService.getLastUpdateTime(dbService.getSensorsFilePath(stationId));
        wxMessageBox(wxString::Format("Załadowano czujniki z lokalnej bazy (ostatnia aktualizacja: %s)",
            lastUpdate.FormatDate()),
            "Informacja", wxOK | wxICON_INFORMATION);

        // Wyczyść panel wykresu i tekst pomiarów, ponieważ nie wybrano jeszcze czujnika
        measurementText->SetValue("Wybierz czujnik aby zobaczyć pomiary (dane lokalne)");
        chartPanel->SetData(std::vector<MeasurementData>(), "");

        return true;
    }
    catch (json::exception& e) {
        wxMessageBox(wxString::Format("Błąd parsowania czujników z bazy: %s", e.what()),
            "Error", wxOK | wxICON_ERROR);
    }
    catch (...) {
        wxMessageBox("Nieznany błąd parsowania czujników z bazy.", "Error", wxOK | wxICON_ERROR);
    }

    return false;
}


void MainWindow::OnSensorSelection(wxCommandEvent& event) {
    int index = sensorChoice->GetSelection();
    if (index == wxNOT_FOUND) return;
    int sensorId = sensors[index].first;
    wxString paramName = wxString::FromUTF8(sensors[index].second.c_str());

    // Resetujemy zakres dat na domyślny (ostatni tydzień)
    wxDateTime today = wxDateTime::Now();
    wxDateTime weekAgo = today;
    weekAgo.Subtract(wxDateSpan(0, 0, 1, 0)); // tydzień wstecz
    startDatePicker->SetValue(weekAgo);
    endDatePicker->SetValue(today);

    // Pozostała część metody bez zmian...
    bool apiAvailable = ApiService::isNetworkAvailable();
    bool hasLocalData = dbService.hasMeasurementData(sensorId);

    // Jeśli jest dostęp do internetu, najpierw próbujemy z API
    if (apiAvailable) {
        if (tryLoadMeasurementsFromAPI(sensorId)) {
            // Sukces - dane pobrane z API
        }
        else if (hasLocalData) {
            // Jeśli API nie zadziałało, próbujemy z bazy
            tryLoadMeasurementsFromDatabase(sensorId);
        }
        else {
            wxMessageBox(wxString::Format("Nie można załadować pomiarów dla czujnika %d ani z API, ani z lokalnej bazy danych.",
                sensorId), "Błąd", wxOK | wxICON_ERROR);
        }
    }
    else {
        // Brak internetu, próbujemy tylko z bazy
        if (!tryLoadMeasurementsFromDatabase(sensorId)) {
            wxMessageBox(wxString::Format("Nie można załadować pomiarów dla czujnika %d z lokalnej bazy danych (brak połączenia z internetem).",
                sensorId), "Błąd", wxOK | wxICON_ERROR);
        }
    }
}

bool MainWindow::tryLoadMeasurementsFromAPI(int sensorId) {
    std::string url = "https://api.gios.gov.pl/pjp-api/rest/data/getData/" + std::to_string(sensorId);
    std::string response = ApiService::fetchDataFromAPI(url);

    if (response.empty()) {
        return false;
    }

    try {
        json data = json::parse(response);
        wxString paramName;

        // Znajdź nazwę parametru dla tego czujnika
        for (const auto& s : sensors) {
            if (s.first == sensorId) {
                paramName = wxString::FromUTF8(s.second.c_str());
                break;
            }
        }

        // Sprawdź, czy są dane pomiarowe
        if (!data.contains("values") || data["values"].empty()) {
            wxMessageBox("Brak danych pomiarowych dla wybranego czujnika.",
                "Informacja", wxOK | wxICON_INFORMATION);
            measurementText->SetValue("Brak dostępnych danych.");
            chartPanel->SetData(std::vector<MeasurementData>(), paramName);
            return true;  // Pomimo braku danych, API odpowiedziało
        }

        wxString dataText = "Dane pomiarowe:\n";
        std::vector<MeasurementData> chartData;

        int valueCount = 0;
        for (const auto& value : data["values"]) {
            if (value.contains("date") && value.contains("value")) {
                valueCount++;
                if (valueCount > 24) break; // Ogranicz do 24 pomiarów

                std::string dateStr = value["date"];
                wxDateTime dt = Utils::parseDateTime(dateStr);
                wxString date = dt.Format("%Y-%m-%d %H:%M");

                // Dane dla wyświetlenia tekstowego
                wxString val = value["value"].is_null() ? wxString("Brak danych") :
                    wxString::Format("%.2f", value["value"].get<double>());
                dataText += wxString::Format("%s: %s\n", date, val);

                // Dane dla wykresu
                MeasurementData point;
                point.date = dt;
                point.hasValue = !value["value"].is_null();
                point.value = point.hasValue ? value["value"].get<double>() : 0.0;
                chartData.push_back(point);
            }
        }

        // Odwróć wektor dla wykresu, aby najstarsze dane były na początku
        std::reverse(chartData.begin(), chartData.end());

        measurementText->SetValue(dataText);

        // Ustaw dane wykresu
        chartPanel->SetTitle(wxString::Format("Pomiary: %s", paramName));
        chartPanel->SetData(chartData, paramName);

        // Zapisz do bazy danych
        dbService.saveMeasurementData(sensorId, response);
        return true;
    }
    catch (json::exception& e) {
        wxMessageBox(wxString::Format("Błąd parsowania danych pomiarowych: %s", e.what()),
            "Error", wxOK | wxICON_ERROR);
    }
    catch (...) {
        wxMessageBox("Nieznany błąd parsowania danych pomiarowych.", "Error", wxOK | wxICON_ERROR);
    }

    return false;
}

bool MainWindow::tryLoadMeasurementsFromDatabase(int sensorId) {
    std::string data = dbService.loadMeasurementData(sensorId);
    if (data.empty()) {
        return false;
    }

    try {
        json jsonData = json::parse(data);
        wxString paramName;

        // Znajdź nazwę parametru dla tego czujnika
        for (const auto& s : sensors) {
            if (s.first == sensorId) {
                paramName = wxString::FromUTF8(s.second.c_str());
                break;
            }
        }

        // Sprawdź, czy są dane pomiarowe
        if (!jsonData.contains("values") || jsonData["values"].empty()) {
            wxMessageBox("Brak danych pomiarowych w lokalnej bazie dla wybranego czujnika.",
                "Informacja", wxOK | wxICON_INFORMATION);
            measurementText->SetValue("Brak dostępnych danych.");
            chartPanel->SetData(std::vector<MeasurementData>(), paramName);
            return true;
        }

        wxString dataText = "Dane pomiarowe (z lokalnej bazy):\n";
        std::vector<MeasurementData> chartData;

        int valueCount = 0;
        for (const auto& value : jsonData["values"]) {
            if (value.contains("date") && value.contains("value")) {
                valueCount++;
                if (valueCount > 24) break; // Ogranicz do 24 pomiarów

                std::string dateStr = value["date"];
                wxDateTime dt = Utils::parseDateTime(dateStr);
                wxString date = dt.Format("%Y-%m-%d %H:%M");

                // Dane dla wyświetlenia tekstowego
                wxString val = value["value"].is_null() ? wxString("Brak danych") :
                    wxString::Format("%.2f", value["value"].get<double>());
                dataText += wxString::Format("%s: %s\n", date, val);

                // Dane dla wykresu
                MeasurementData point;
                point.date = dt;
                point.hasValue = !value["value"].is_null();
                point.value = point.hasValue ? value["value"].get<double>() : 0.0;
                chartData.push_back(point);
            }
        }

        // Odwróć wektor dla wykresu, aby najstarsze dane były na początku
        std::reverse(chartData.begin(), chartData.end());

        measurementText->SetValue(dataText);

        // Ustaw dane wykresu
        chartPanel->SetTitle(wxString::Format("Pomiary: %s (dane lokalne)", paramName));
        chartPanel->SetData(chartData, paramName);

        wxDateTime lastUpdate = dbService.getLastUpdateTime(dbService.getMeasurementFilePath(sensorId));
        wxMessageBox(wxString::Format("Załadowano dane pomiarowe z lokalnej bazy (ostatnia aktualizacja: %s)",
            lastUpdate.FormatDate()),
            "Informacja", wxOK | wxICON_INFORMATION);

        return true;
    }
    catch (json::exception& e) {
        wxMessageBox(wxString::Format("Błąd parsowania danych pomiarowych z bazy: %s", e.what()),
            "Error", wxOK | wxICON_ERROR);
    }
    catch (...) {
        wxMessageBox("Nieznany błąd parsowania danych pomiarowych z bazy.", "Error", wxOK | wxICON_ERROR);
    }

    return false;
}


bool MainWindow::tryLoadAirQualityFromAPI(int stationId) {
    std::string url = "https://api.gios.gov.pl/pjp-api/rest/aqindex/getIndex/" + std::to_string(stationId);
    std::string response = ApiService::fetchDataFromAPI(url);

    if (response.empty()) {
        return false;
    }

    try {
        json indexData = json::parse(response);

        // Sprawdź czy obiekt ma pole stIndexLevel i czy nie jest null
        if (!indexData.contains("stIndexLevel") || indexData["stIndexLevel"].is_null()) {
            airQualityIndex->SetLabel("Indeks jakości powietrza: Brak danych");
            airQualityIndex->SetForegroundColour(*wxBLACK);
            return true;  // API odpowiedziało, ale brak danych indeksu
        }

        // Sprawdź czy stIndexLevel zawiera potrzebne pola
        if (!indexData["stIndexLevel"].contains("id") || !indexData["stIndexLevel"].contains("indexLevelName")) {
            airQualityIndex->SetLabel("Indeks jakości powietrza: Niepełne dane");
            airQualityIndex->SetForegroundColour(*wxBLACK);
            return true;
        }

        int level = indexData["stIndexLevel"]["id"];
        std::string levelText = indexData["stIndexLevel"]["indexLevelName"];
        wxColour color;
        switch (level) {
        case 0: color = *wxGREEN; break;
        case 1: color = *wxYELLOW; break;
        case 2: color = *wxRED; break;
        default: color = *wxBLACK; break;
        }
        airQualityIndex->SetLabel(wxString::Format("Indeks jakości powietrza: %s", levelText));
        airQualityIndex->SetForegroundColour(color);

        // Zapisz do bazy danych
        dbService.saveAirQualityIndex(stationId, response);
        return true;
    }
    catch (json::exception& e) {
        wxMessageBox(wxString::Format("Błąd parsowania indeksu jakości powietrza: %s", e.what()),
            "Error", wxOK | wxICON_ERROR);
    }
    catch (...) {
        wxMessageBox("Nieznany błąd parsowania indeksu jakości powietrza.", "Error", wxOK | wxICON_ERROR);
    }

    return false;
}

bool MainWindow::tryLoadAirQualityFromDatabase(int stationId) {
    std::string data = dbService.loadAirQualityIndex(stationId);
    if (data.empty()) {
        return false;
    }

    try {
        json indexData = json::parse(data);

        // Sprawdź czy obiekt ma pole stIndexLevel i czy nie jest null
        if (!indexData.contains("stIndexLevel") || indexData["stIndexLevel"].is_null()) {
            airQualityIndex->SetLabel("Indeks jakości powietrza: Brak danych");
            airQualityIndex->SetForegroundColour(*wxBLACK);
            return true;
        }

        // Sprawdź czy stIndexLevel zawiera potrzebne pola
        if (!indexData["stIndexLevel"].contains("id") || !indexData["stIndexLevel"].contains("indexLevelName")) {
            airQualityIndex->SetLabel("Indeks jakości powietrza: Niepełne dane");
            airQualityIndex->SetForegroundColour(*wxBLACK);
            return true;
        }

        int level = indexData["stIndexLevel"]["id"];
        std::string levelText = indexData["stIndexLevel"]["indexLevelName"];
        wxColour color;
        switch (level) {
        case 0: color = *wxGREEN; break;
        case 1: color = *wxYELLOW; break;
        case 2: color = *wxRED; break;
        default: color = *wxBLACK; break;
        }
        airQualityIndex->SetLabel(wxString::Format("Indeks jakości powietrza: %s (dane lokalne)", levelText));
        airQualityIndex->SetForegroundColour(color);

        wxDateTime lastUpdate = dbService.getLastUpdateTime(dbService.getAirQualityFilePath(stationId));
        wxMessageBox(wxString::Format("Załadowano indeks jakości powietrza z lokalnej bazy (ostatnia aktualizacja: %s)",
            lastUpdate.FormatDate()),
            "Informacja", wxOK | wxICON_INFORMATION);

        return true;
    }
    catch (json::exception& e) {
        wxMessageBox(wxString::Format("Błąd parsowania indeksu jakości powietrza z bazy: %s", e.what()),
            "Error", wxOK | wxICON_ERROR);
    }
    catch (...) {
        wxMessageBox("Nieznany błąd parsowania indeksu jakości powietrza z bazy.", "Error", wxOK | wxICON_ERROR);
    }

    return false;
}

void MainWindow::OnSaveCurrentToDatabase(wxCommandEvent& event) {
    // Sprawdź czy wybrano stację
    int selectedIdx = stationChoice->GetSelection();
    if (selectedIdx == wxNOT_FOUND) {
        wxMessageBox("Proszę najpierw wybrać stację.", "Informacja", wxOK | wxICON_INFORMATION);
        return;
    }

    int stationId = stations[selectedIdx].first;
    wxString stationName = wxString::FromUTF8(stations[selectedIdx].second.c_str());

    // Pobierz czujniki dla wybranej stacji
    std::string sensorsUrl = "https://api.gios.gov.pl/pjp-api/rest/station/sensors/" + std::to_string(stationId);
    std::string sensorsResponse = ApiService::fetchDataFromAPI(sensorsUrl);

    if (sensorsResponse.empty()) {
        wxMessageBox("Nie można pobrać danych czujników. Sprawdź połączenie internetowe.", "Błąd", wxOK | wxICON_ERROR);
        return;
    }

    // Zapisz dane czujników
    dbService.saveSensorsData(stationId, sensorsResponse);

    // Utworzenie dialogu postępu
    wxProgressDialog progressDialog(
        "Pobieranie danych",
        wxString::Format("Pobieranie danych stacji %s...", stationName),
        100,  // Maksymalna wartość
        this,
        wxPD_APP_MODAL | wxPD_AUTO_HIDE | wxPD_CAN_ABORT | wxPD_ELAPSED_TIME | wxPD_REMAINING_TIME
    );

    try {
        json sensorsData = json::parse(sensorsResponse);
        int sensorCount = sensorsData.size();
        int currentSensor = 0;
        bool continueProcess = true;

        // Pobieranie indeksu jakości powietrza dla stacji
        progressDialog.Update(0, wxString::Format("Pobieranie indeksu jakości powietrza dla stacji %s...", stationName));
        std::string airQualityUrl = "https://api.gios.gov.pl/pjp-api/rest/aqindex/getIndex/" + std::to_string(stationId);
        std::string airQualityResponse = ApiService::fetchDataFromAPI(airQualityUrl);
        if (!airQualityResponse.empty()) {
            dbService.saveAirQualityIndex(stationId, airQualityResponse);
        }

        // Pobieranie danych dla każdego czujnika
        for (const auto& sensor : sensorsData) {
            if (!continueProcess) break;

            if (sensor.contains("id") && sensor.contains("param") && sensor["param"].contains("paramName")) {
                int sensorId = sensor["id"];
                std::string paramName = sensor["param"]["paramName"];

                int progress = (++currentSensor * 100) / sensorCount;
                continueProcess = progressDialog.Update(
                    progress,
                    wxString::Format("Pobieranie danych dla czujnika %s...", wxString::FromUTF8(paramName.c_str()))
                );

                // Pobieranie danych pomiarowych dla czujnika
                std::string dataUrl = "https://api.gios.gov.pl/pjp-api/rest/data/getData/" + std::to_string(sensorId);
                std::string dataResponse = ApiService::fetchDataFromAPI(dataUrl);
                if (!dataResponse.empty()) {
                    dbService.saveMeasurementData(sensorId, dataResponse);
                }

                // Krótka pauza, aby nie przeciążać API
                wxMilliSleep(100);
            }
        }

        if (continueProcess) {
            wxMessageBox(wxString::Format("Dane stacji %s zostały zapisane do lokalnej bazy danych.", stationName),
                "Informacja", wxOK | wxICON_INFORMATION);
        }
        else {
            wxMessageBox("Pobieranie danych zostało przerwane przez użytkownika.",
                "Informacja", wxOK | wxICON_INFORMATION);
        }
    }
    catch (json::exception& e) {
        wxMessageBox(wxString::Format("Błąd podczas przetwarzania danych: %s", e.what()),
            "Błąd", wxOK | wxICON_ERROR);
    }
    catch (...) {
        wxMessageBox("Wystąpił nieznany błąd podczas pobierania danych.",
            "Błąd", wxOK | wxICON_ERROR);
    }

    updateDatabaseStatus();
}

void MainWindow::OnSaveAllToDatabase(wxCommandEvent& event) {
    // Najpierw pobieramy i zapisujemy wszystkie stacje
    std::string stationsResponse = ApiService::fetchDataFromAPI("https://api.gios.gov.pl/pjp-api/rest/station/findAll");
    if (stationsResponse.empty()) {
        wxMessageBox("Nie można pobrać danych stacji. Sprawdź połączenie internetowe.", "Błąd", wxOK | wxICON_ERROR);
        return;
    }

    dbService.saveStationsData(stationsResponse);

    // Parsujemy odpowiedź aby uzyskać ID wszystkich stacji
    try {
        json stationsData = json::parse(stationsResponse);
        if (!stationsData.is_array()) {
            wxMessageBox("Błąd formatu danych stacji.", "Błąd", wxOK | wxICON_ERROR);
            return;
        }

        // Utworzenie dialogu postępu
        wxProgressDialog progressDialog(
            "Pobieranie danych",
            "Trwa pobieranie danych wszystkich stacji...",
            stationsData.size(),
            this,
            wxPD_APP_MODAL | wxPD_AUTO_HIDE | wxPD_CAN_ABORT | wxPD_ELAPSED_TIME | wxPD_REMAINING_TIME
        );

        int stationCounter = 0;
        bool continueProcess = true;

        // Iteracja przez wszystkie stacje
        for (const auto& station : stationsData) {
            if (!continueProcess) break;

            if (station.contains("id")) {
                int stationId = station["id"];
                wxString stationName = "Nieznana stacja";

                if (station.contains("stationName")) {
                    stationName = wxString::FromUTF8(station["stationName"].get<std::string>().c_str());
                }

                // Aktualizacja dialogu postępu
                continueProcess = progressDialog.Update(
                    stationCounter,
                    wxString::Format("Pobieranie danych stacji %s (ID: %d)...", stationName, stationId)
                );
                stationCounter++;

                // Pobieranie czujników dla stacji
                std::string sensorsUrl = "https://api.gios.gov.pl/pjp-api/rest/station/sensors/" + std::to_string(stationId);
                std::string sensorsResponse = ApiService::fetchDataFromAPI(sensorsUrl);

                if (!sensorsResponse.empty()) {
                    dbService.saveSensorsData(stationId, sensorsResponse);

                    // Pobieranie danych dla każdego czujnika
                    try {
                        json sensorsData = json::parse(sensorsResponse);
                        for (const auto& sensor : sensorsData) {
                            if (!continueProcess) break;

                            if (sensor.contains("id")) {
                                int sensorId = sensor["id"];

                                // Pobieranie danych pomiarowych dla czujnika
                                std::string dataUrl = "https://api.gios.gov.pl/pjp-api/rest/data/getData/" + std::to_string(sensorId);
                                std::string dataResponse = ApiService::fetchDataFromAPI(dataUrl);
                                if (!dataResponse.empty()) {
                                    dbService.saveMeasurementData(sensorId, dataResponse);
                                }

                                // Krótka pauza, aby nie przeciążać API
                                wxMilliSleep(100);
                            }
                        }
                    }
                    catch (json::exception& e) {
                        // Logowanie błędu, ale kontynuowanie procesu
                        std::ofstream logFile("error_log.txt", std::ios::app);
                        logFile << "Błąd parsowania czujników dla stacji " << stationId << ": " << e.what() << std::endl;
                        logFile.close();
                    }
                }

                // Pobieranie indeksu jakości powietrza dla stacji
                std::string airQualityUrl = "https://api.gios.gov.pl/pjp-api/rest/aqindex/getIndex/" + std::to_string(stationId);
                std::string airQualityResponse = ApiService::fetchDataFromAPI(airQualityUrl);
                if (!airQualityResponse.empty()) {
                    dbService.saveAirQualityIndex(stationId, airQualityResponse);
                }

                // Krótka pauza, aby nie przeciążać API
                wxMilliSleep(100);
            }
        }

        if (continueProcess) {
            wxMessageBox("Dane wszystkich stacji zostały zapisane do lokalnej bazy danych.", "Informacja", wxOK | wxICON_INFORMATION);
        }
        else {
            wxMessageBox("Pobieranie danych zostało przerwane przez użytkownika.", "Informacja", wxOK | wxICON_INFORMATION);
        }
    }
    catch (json::exception& e) {
        wxMessageBox(wxString::Format("Błąd podczas przetwarzania danych stacji: %s", e.what()), "Błąd", wxOK | wxICON_ERROR);
    }
    catch (...) {
        wxMessageBox("Wystąpił nieznany błąd podczas pobierania danych.", "Błąd", wxOK | wxICON_ERROR);
    }

    updateDatabaseStatus();
}

void MainWindow::OnApplyDateRange(wxCommandEvent& event) {
    // Pobierz aktualnie wybrany czujnik
    int index = sensorChoice->GetSelection();
    if (index == wxNOT_FOUND) {
        wxMessageBox("Najpierw wybierz czujnik", "Informacja", wxOK | wxICON_INFORMATION);
        return;
    }

    int sensorId = sensors[index].first;

    // Odśwież dane z uwzględnieniem zakresu dat
    if (ApiService::isNetworkAvailable()) {
        if (!tryLoadMeasurementsWithDateRange(sensorId)) {
            if (dbService.hasMeasurementData(sensorId)) {
                tryLoadMeasurementsWithDateRangeFromDatabase(sensorId);
            }
            else {
                wxMessageBox("Nie można załadować danych dla wybranego zakresu",
                    "Błąd", wxOK | wxICON_ERROR);
            }
        }
    }
    else if (dbService.hasMeasurementData(sensorId)) {
        tryLoadMeasurementsWithDateRangeFromDatabase(sensorId);
    }
    else {
        wxMessageBox("Brak połączenia z internetem i brak danych w bazie lokalnej",
            "Błąd", wxOK | wxICON_ERROR);
    }
}

bool MainWindow::tryLoadMeasurementsWithDateRange(int sensorId) {
    // Pobranie pełnych danych pomiarowych jak wcześniej
    std::string url = "https://api.gios.gov.pl/pjp-api/rest/data/getData/" + std::to_string(sensorId);
    std::string response = ApiService::fetchDataFromAPI(url);

    if (response.empty()) {
        return false;
    }

    try {
        json data = json::parse(response);
        wxString paramName;

        // Znajdź nazwę parametru dla tego czujnika
        for (const auto& s : sensors) {
            if (s.first == sensorId) {
                paramName = wxString::FromUTF8(s.second.c_str());
                break;
            }
        }

        // Pobierz wybrany zakres dat
        wxDateTime startDate = startDatePicker->GetValue();
        wxDateTime endDate = endDatePicker->GetValue();

        // Dodaj czas do dat, aby uwzględnić cały dzień
        startDate.SetHour(0);
        startDate.SetMinute(0);
        startDate.SetSecond(0);

        endDate.SetHour(23);
        endDate.SetMinute(59);
        endDate.SetSecond(59);

        // Sprawdź, czy są dane pomiarowe
        if (!data.contains("values") || data["values"].empty()) {
            wxMessageBox("Brak danych pomiarowych dla wybranego czujnika.",
                "Informacja", wxOK | wxICON_INFORMATION);
            measurementText->SetValue("Brak dostępnych danych.");
            chartPanel->SetData(std::vector<MeasurementData>(), paramName);
            return true;  // Pomimo braku danych, API odpowiedziało
        }

        wxString dataText = "Dane pomiarowe w zakresie od " +
            startDate.FormatDate() + " do " +
            endDate.FormatDate() + ":\n";
        std::vector<MeasurementData> chartData;

        for (const auto& value : data["values"]) {
            if (value.contains("date") && value.contains("value")) {
                std::string dateStr = value["date"];
                wxDateTime dt = Utils::parseDateTime(dateStr);

                // Sprawdź czy data pomiaru mieści się w wybranym zakresie
                if (dt.IsValid() && dt >= startDate && dt <= endDate) {
                    wxString date = dt.Format("%Y-%m-%d %H:%M");

                    // Dane dla wyświetlenia tekstowego
                    wxString val = value["value"].is_null() ? wxString("Brak danych") :
                        wxString::Format("%.2f", value["value"].get<double>());
                    dataText += wxString::Format("%s: %s\n", date, val);

                    // Dane dla wykresu
                    MeasurementData point;
                    point.date = dt;
                    point.hasValue = !value["value"].is_null();
                    point.value = point.hasValue ? value["value"].get<double>() : 0.0;
                    chartData.push_back(point);
                }
            }
        }

        // Jeśli nie znaleziono danych w zakresie
        if (chartData.empty()) {
            wxMessageBox("Brak danych pomiarowych w wybranym zakresie dat.",
                "Informacja", wxOK | wxICON_INFORMATION);
            measurementText->SetValue("Brak dostępnych danych w wybranym zakresie dat.");
            chartPanel->SetData(std::vector<MeasurementData>(), paramName);
            return true;
        }

        // Odwróć wektor dla wykresu, aby najstarsze dane były na początku
        std::reverse(chartData.begin(), chartData.end());

        measurementText->SetValue(dataText);

        // Ustaw dane wykresu
        chartPanel->SetTitle(wxString::Format("Pomiary: %s (%s - %s)",
            paramName, startDate.FormatDate(), endDate.FormatDate()));
        chartPanel->SetData(chartData, paramName);

        return true;
    }
    catch (json::exception& e) {
        wxMessageBox(wxString::Format("Błąd parsowania danych pomiarowych: %s", e.what()),
            "Error", wxOK | wxICON_ERROR);
    }
    catch (...) {
        wxMessageBox("Nieznany błąd parsowania danych pomiarowych.", "Error", wxOK | wxICON_ERROR);
    }

    return false;
}

bool MainWindow::tryLoadMeasurementsWithDateRangeFromDatabase(int sensorId) {
    std::string data = dbService.loadMeasurementData(sensorId);
    if (data.empty()) {
        return false;
    }

    try {
        json jsonData = json::parse(data);
        wxString paramName;

        // Znajdź nazwę parametru dla tego czujnika
        for (const auto& s : sensors) {
            if (s.first == sensorId) {
                paramName = wxString::FromUTF8(s.second.c_str());
                break;
            }
        }

        // Pobierz wybrany zakres dat
        wxDateTime startDate = startDatePicker->GetValue();
        wxDateTime endDate = endDatePicker->GetValue();

        // Dodaj czas do dat, aby uwzględnić cały dzień
        startDate.SetHour(0);
        startDate.SetMinute(0);
        startDate.SetSecond(0);

        endDate.SetHour(23);
        endDate.SetMinute(59);
        endDate.SetSecond(59);

        // Sprawdź, czy są dane pomiarowe
        if (!jsonData.contains("values") || jsonData["values"].empty()) {
            wxMessageBox("Brak danych pomiarowych w lokalnej bazie dla wybranego czujnika.",
                "Informacja", wxOK | wxICON_INFORMATION);
            measurementText->SetValue("Brak dostępnych danych.");
            chartPanel->SetData(std::vector<MeasurementData>(), paramName);
            return true;
        }

        wxString dataText = "Dane pomiarowe (z lokalnej bazy) w zakresie od " +
            startDate.FormatDate() + " do " +
            endDate.FormatDate() + ":\n";
        std::vector<MeasurementData> chartData;

        for (const auto& value : jsonData["values"]) {
            if (value.contains("date") && value.contains("value")) {
                std::string dateStr = value["date"];
                wxDateTime dt = Utils::parseDateTime(dateStr);

                // Sprawdź czy data pomiaru mieści się w wybranym zakresie
                if (dt.IsValid() && dt >= startDate && dt <= endDate) {
                    wxString date = dt.Format("%Y-%m-%d %H:%M");

                    // Dane dla wyświetlenia tekstowego
                    wxString val = value["value"].is_null() ? wxString("Brak danych") :
                        wxString::Format("%.2f", value["value"].get<double>());
                    dataText += wxString::Format("%s: %s\n", date, val);

                    // Dane dla wykresu
                    MeasurementData point;
                    point.date = dt;
                    point.hasValue = !value["value"].is_null();
                    point.value = point.hasValue ? value["value"].get<double>() : 0.0;
                    chartData.push_back(point);
                }
            }
        }

        // Jeśli nie znaleziono danych w zakresie
        if (chartData.empty()) {
            wxMessageBox("Brak danych pomiarowych w wybranym zakresie dat.",
                "Informacja", wxOK | wxICON_INFORMATION);
            measurementText->SetValue("Brak dostępnych danych w wybranym zakresie dat.");
            chartPanel->SetData(std::vector<MeasurementData>(), paramName);
            return true;
        }

        // Odwróć wektor dla wykresu, aby najstarsze dane były na początku
        std::reverse(chartData.begin(), chartData.end());

        measurementText->SetValue(dataText);

        // Ustaw dane wykresu
        chartPanel->SetTitle(wxString::Format("Pomiary: %s (%s - %s) (dane lokalne)",
            paramName, startDate.FormatDate(), endDate.FormatDate()));
        chartPanel->SetData(chartData, paramName);

        wxDateTime lastUpdate = dbService.getLastUpdateTime(dbService.getMeasurementFilePath(sensorId));
        wxMessageBox(wxString::Format("Załadowano dane pomiarowe z lokalnej bazy dla wybranego zakresu dat (ostatnia aktualizacja: %s)",
            lastUpdate.FormatDate()),
            "Informacja", wxOK | wxICON_INFORMATION);

        return true;
    }
    catch (json::exception& e) {
        wxMessageBox(wxString::Format("Błąd parsowania danych pomiarowych z bazy: %s", e.what()),
            "Error", wxOK | wxICON_ERROR);
    }
    catch (...) {
        wxMessageBox("Nieznany błąd parsowania danych pomiarowych z bazy.", "Error", wxOK | wxICON_ERROR);
    }

    return false;
}

wxDateTime MainWindow::parseDateTime(const std::string& dateStr) {
    wxDateTime dt;
    // Format daty w API to: "YYYY-MM-DD HH:MM:SS"
    dt.ParseFormat(dateStr.c_str(), "%Y-%m-%d %H:%M:%S");
    return dt;
}