#include <wx/wx.h>
#include <wx/choice.h>
#include <curl/curl.h>
#include <json.hpp>
#include <string>
#include <fstream>
#include <wx/graphics.h>
#include <wx/dcbuffer.h>
#include <vector>
#include <ctime>
#include <iomanip>
#include <sstream>

using json = nlohmann::json;

// Callback do zapisywania odpowiedzi
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t totalSize = size * nmemb;
    output->append((char*)contents, totalSize);
    return totalSize;
}

// Pomocnicza klasa do przechowywania danych pomiarowych
struct MeasurementData {
    wxDateTime date;
    double value;
    bool hasValue;
};

// Funkcja pomocnicza do konwersji string daty na wxDateTime
wxDateTime parseDateTime(const std::string& dateStr) {
    wxDateTime dt;
    // Format daty API: "2024-03-29 12:00:00"
    wxString wxDateStr = wxString::FromUTF8(dateStr.c_str());
    dt.ParseFormat(wxDateStr, "%Y-%m-%d %H:%M:%S");
    return dt;
}

// Klasa panelu wykresu
class ChartPanel : public wxPanel {
private:
    std::vector<MeasurementData> measurements;
    wxString title;
    wxString paramName;
    bool hasData;

public:
    ChartPanel(wxWindow* parent) : wxPanel(parent, wxID_ANY), hasData(false) {
        SetBackgroundStyle(wxBG_STYLE_PAINT);
        Bind(wxEVT_PAINT, &ChartPanel::OnPaint, this);
    }

    void SetData(const std::vector<MeasurementData>& data, const wxString& param) {
        measurements = data;
        paramName = param;
        hasData = !data.empty();
        Refresh();
    }

    void SetTitle(const wxString& chartTitle) {
        title = chartTitle;
        Refresh();
    }

    void OnPaint(wxPaintEvent& event) {
        wxAutoBufferedPaintDC dc(this);
        dc.Clear();

        // Rysuj ramkÍ
        dc.SetPen(wxPen(wxColour(200, 200, 200), 1));
        dc.SetBrush(*wxWHITE_BRUSH);
        wxSize size = GetClientSize();
        dc.DrawRectangle(0, 0, size.x, size.y);

        if (!hasData) {
            dc.SetTextForeground(*wxBLACK);
            dc.DrawText("Brak danych do wyúwietlenia", 10, size.y / 2);
            return;
        }

        // Marginesy
        int leftMargin = 60;
        int rightMargin = 20;
        int topMargin = 40;
        int bottomMargin = 40;

        // Obszar wykresu
        int chartWidth = size.x - leftMargin - rightMargin;
        int chartHeight = size.y - topMargin - bottomMargin;

        // Znajdü min/max wartoúci
        double minValue = 0;
        double maxValue = 0;
        bool firstValidValue = true;

        for (const auto& point : measurements) {
            if (!point.hasValue) continue;

            if (firstValidValue) {
                minValue = maxValue = point.value;
                firstValidValue = false;
            }
            else {
                minValue = std::min(minValue, point.value);
                maxValue = std::max(maxValue, point.value);
            }
        }

        // Dodaj margines do zakresu wartoúci
        double valueRange = maxValue - minValue;
        if (valueRange < 0.1) valueRange = 1.0; // Zapobiegaj dzieleniu przez wartoúci bliskie zeru
        minValue = std::max(0.0, minValue - 0.1 * valueRange);
        maxValue = maxValue + 0.1 * valueRange;

        // Tytu≥ wykresu
        dc.SetFont(wxFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
        wxString fullTitle = title.IsEmpty() ? paramName : title;
        wxSize titleSize = dc.GetTextExtent(fullTitle);
        dc.DrawText(fullTitle, (size.x - titleSize.GetWidth()) / 2, 10);

        // Osie
        dc.SetPen(wxPen(*wxBLACK, 1));
        dc.DrawLine(leftMargin, topMargin, leftMargin, topMargin + chartHeight);
        dc.DrawLine(leftMargin, topMargin + chartHeight, leftMargin + chartWidth, topMargin + chartHeight);

        // Etykiety osi Y
        dc.SetFont(wxFont(9, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
        int yLabels = 5;
        for (int i = 0; i <= yLabels; ++i) {
            double value = minValue + (maxValue - minValue) * (yLabels - i) / yLabels;
            int y = topMargin + i * chartHeight / yLabels;
            wxString label = wxString::Format("%.1f", value);
            wxSize labelSize = dc.GetTextExtent(label);
            dc.DrawText(label, leftMargin - labelSize.GetWidth() - 5, y - labelSize.GetHeight() / 2);
            dc.SetPen(wxPen(wxColour(200, 200, 200), 1, wxPENSTYLE_DOT));
            dc.DrawLine(leftMargin, y, leftMargin + chartWidth, y);
        }

        // Sprawdü czy mamy co najmniej dwa punkty
        if (measurements.size() < 2) return;

        // Etykiety osi X (daty) - wyúwietl kilka punktÛw
        int xLabels = std::min(5, (int)measurements.size());
        for (int i = 0; i < xLabels; ++i) {
            int index = i * (measurements.size() - 1) / (xLabels - 1);
            wxDateTime date = measurements[index].date;
            int x = leftMargin + index * chartWidth / (measurements.size() - 1);
            wxString label = date.Format("%H:%M\n%d-%m");
            dc.DrawText(label, x - 20, topMargin + chartHeight + 5);
        }

        // Rysowanie linii wykresu
        dc.SetPen(wxPen(wxColour(0, 120, 215), 2));

        for (size_t i = 0; i < measurements.size() - 1; ++i) {
            if (!measurements[i].hasValue || !measurements[i + 1].hasValue) continue;

            int x1 = leftMargin + i * chartWidth / (measurements.size() - 1);
            int y1 = topMargin + chartHeight -
                (measurements[i].value - minValue) * chartHeight / (maxValue - minValue);

            int x2 = leftMargin + (i + 1) * chartWidth / (measurements.size() - 1);
            int y2 = topMargin + chartHeight -
                (measurements[i + 1].value - minValue) * chartHeight / (maxValue - minValue);

            dc.DrawLine(x1, y1, x2, y2);
        }

        // Rysowanie punktÛw danych
        for (size_t i = 0; i < measurements.size(); ++i) {
            if (!measurements[i].hasValue) continue;

            int x = leftMargin + i * chartWidth / (measurements.size() - 1);
            int y = topMargin + chartHeight -
                (measurements[i].value - minValue) * chartHeight / (maxValue - minValue);

            dc.SetBrush(wxBrush(wxColour(0, 120, 215)));
            dc.SetPen(wxPen(*wxWHITE, 1));
            dc.DrawCircle(x, y, 4);
        }
    }
};

// Funkcja pobierania danych z API
std::string fetchDataFromAPI(const std::string& url) {
    CURL* curl = curl_easy_init();
    std::string response;
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            wxMessageBox(wxString::Format("B≥πd cURL: %s", curl_easy_strerror(res)), "Error", wxOK | wxICON_ERROR);
        }
        curl_easy_cleanup(curl);
    }
    // Zapisz odpowiedü do pliku debugowego
    std::ofstream debugFile("response_debug.json");
    debugFile << response;
    debugFile.close();
    return response;
}

// Klasa g≥Ûwnego okna
class MainWindow : public wxFrame {
private:
    wxChoice* stationChoice;
    wxChoice* sensorChoice;
    wxStaticText* measurementText;
    wxStaticText* airQualityIndex;
    ChartPanel* chartPanel;
    std::vector<std::pair<int, std::string>> stations;
    std::vector<std::pair<int, std::string>> sensors;

public:
    MainWindow() : wxFrame(NULL, wxID_ANY, "Dane pomiarowe - GIOå", wxDefaultPosition, wxSize(1000, 700)) {
        wxPanel* mainPanel = new wxPanel(this, wxID_ANY);
        wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
        wxBoxSizer* dataSizer = new wxBoxSizer(wxHORIZONTAL);

        // Panel kontrolny (lewy)
        wxBoxSizer* controlSizer = new wxBoxSizer(wxVERTICAL);
        stationChoice = new wxChoice(mainPanel, wxID_ANY);
        sensorChoice = new wxChoice(mainPanel, wxID_ANY);
        measurementText = new wxStaticText(mainPanel, wxID_ANY, "Dane pomiarowe", wxDefaultPosition, wxSize(300, 400), wxST_NO_AUTORESIZE | wxALIGN_LEFT);
        airQualityIndex = new wxStaticText(mainPanel, wxID_ANY, "Indeks jakoúci powietrza", wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE_HORIZONTAL);

        controlSizer->Add(new wxStaticText(mainPanel, wxID_ANY, "Wybierz stacjÍ"), 0, wxALL, 5);
        controlSizer->Add(stationChoice, 0, wxEXPAND | wxALL, 10);
        controlSizer->Add(new wxStaticText(mainPanel, wxID_ANY, "Wybierz czujnik"), 0, wxALL, 5);
        controlSizer->Add(sensorChoice, 0, wxEXPAND | wxALL, 10);
        controlSizer->Add(airQualityIndex, 0, wxALL, 10);
        controlSizer->Add(measurementText, 1, wxALL | wxEXPAND, 10);

        // Panel wykresu (prawy)
        chartPanel = new ChartPanel(mainPanel);

        // Dodaj panele do g≥Ûwnego sizer'a
        dataSizer->Add(controlSizer, 1, wxEXPAND | wxALL, 10);
        dataSizer->Add(chartPanel, 2, wxEXPAND | wxALL, 10);

        mainSizer->Add(dataSizer, 1, wxEXPAND);
        mainPanel->SetSizer(mainSizer);

        stationChoice->Bind(wxEVT_CHOICE, &MainWindow::OnStationSelection, this);
        sensorChoice->Bind(wxEVT_CHOICE, &MainWindow::OnSensorSelection, this);

        loadStations();
    }

    void loadStations() {
        std::string response = fetchDataFromAPI("https://api.gios.gov.pl/pjp-api/rest/station/findAll");
        if (response.empty()) {
            wxMessageBox("Brak odpowiedzi z API podczas pobierania stacji.", "Error", wxOK | wxICON_ERROR);
            return;
        }
        try {
            json stationsData = json::parse(response);

            // Sprawdü, czy mamy do czynienia z tablicπ (zgodnie z dokumentacjπ API)
            if (!stationsData.is_array()) {
                wxMessageBox("Nieoczekiwana struktura odpowiedzi API - nie jest tablicπ.", "Error", wxOK | wxICON_ERROR);
                return;
            }

            for (const auto& station : stationsData) {
                // Sprawdü czy zawiera wymagane pola
                if (station.contains("id") && station.contains("stationName")) {
                    int id = station["id"];
                    std::string name = station["stationName"];
                    stations.push_back({ id, name });
                    stationChoice->Append(wxString::FromUTF8(name.c_str()));
                }
            }
        }
        catch (json::exception& e) {
            wxMessageBox(wxString::Format("B≥πd parsowania stacji: %s", e.what()), "Error", wxOK | wxICON_ERROR);
        }
        catch (...) {
            wxMessageBox("Nieznany b≥πd parsowania stacji.", "Error", wxOK | wxICON_ERROR);
        }
    }

    void OnStationSelection(wxCommandEvent& event) {
        int index = stationChoice->GetSelection();
        if (index == wxNOT_FOUND) return;
        int stationId = stations[index].first;

        // Debug: Informacja o wybranej stacji
        //wxMessageBox(wxString::Format("Wybrano stacjÍ: %s, ID: %d", stations[index].second, stationId), "Debug - Stacja", wxOK | wxICON_INFORMATION);

        sensorChoice->Clear();
        std::string url = "https://api.gios.gov.pl/pjp-api/rest/station/sensors/" + std::to_string(stationId);
        std::string response = fetchDataFromAPI(url);

        // Zapisz odpowiedü do pliku debugowego
        std::ofstream debugFile("sensors_response_" + std::to_string(stationId) + ".json");
        debugFile << response;
        debugFile.close();

        if (response.empty()) {
            wxMessageBox("Brak odpowiedzi z API podczas pobierania czujnikÛw.", "Error", wxOK | wxICON_ERROR);
            return;
        }

        try {
            json sensorsData = json::parse(response);

            // Sprawdü, czy mamy do czynienia z tablicπ (zgodnie z dokumentacjπ API)
            if (!sensorsData.is_array()) {
                wxMessageBox("Nieoczekiwana struktura odpowiedzi API dla czujnikÛw.", "Error", wxOK | wxICON_ERROR);
                return;
            }

            sensors.clear();
            for (const auto& sensor : sensorsData) {
                // Sprawdü czy zawiera wymagane pola
                if (sensor.contains("id") && sensor.contains("param") &&
                    sensor["param"].contains("paramName")) {

                    int sensorId = sensor["id"];
                    std::string paramName = sensor["param"]["paramName"];
                    sensors.push_back({ sensorId, paramName });
                    sensorChoice->Append(wxString::FromUTF8(paramName.c_str()));
                }
            }
        }
        catch (json::exception& e) {
            wxMessageBox(wxString::Format("B≥πd parsowania czujnikÛw: %s", e.what()), "Error", wxOK | wxICON_ERROR);
        }
        catch (...) {
            wxMessageBox("Nieznany b≥πd parsowania czujnikÛw.", "Error", wxOK | wxICON_ERROR);
        }

        // PrÛbujemy za≥adowaÊ indeks jakoúci powietrza
        loadAirQualityIndex(stationId);

        // Zresetuj wykres
        chartPanel->SetData(std::vector<MeasurementData>(), "");
    }

    void OnSensorSelection(wxCommandEvent& event) {
        int index = sensorChoice->GetSelection();
        if (index == wxNOT_FOUND) return;
        int sensorId = sensors[index].first;
        wxString paramName = wxString::FromUTF8(sensors[index].second.c_str());

        // Debug: Informacja o wybranym sensorze
        //wxMessageBox(wxString::Format("Wybrano czujnik: %s, ID: %d", sensors[index].second, sensorId), "Debug - Sensor", wxOK | wxICON_INFORMATION);

        std::string url = "https://api.gios.gov.pl/pjp-api/rest/data/getData/" + std::to_string(sensorId);
        std::string response = fetchDataFromAPI(url);

        // Zapisz odpowiedü do pliku debugowego
        std::ofstream debugFile("data_response_" + std::to_string(sensorId) + ".json");
        debugFile << response;
        debugFile.close();

        if (response.empty()) {
            wxMessageBox("Brak odpowiedzi z API podczas pobierania danych pomiarowych.", "Error", wxOK | wxICON_ERROR);
            return;
        }
        try {
            json data = json::parse(response);

            // Sprawdü, czy sπ dane pomiarowe
            if (!data.contains("values") || data["values"].empty()) {
                wxMessageBox("Brak danych pomiarowych dla wybranego czujnika.", "Informacja", wxOK | wxICON_INFORMATION);
                measurementText->SetLabel("Brak dostÍpnych danych.");
                chartPanel->SetData(std::vector<MeasurementData>(), paramName);
                return;
            }

            wxString dataText = "Dane pomiarowe:\n";
            std::vector<MeasurementData> chartData;

            // OdwrÛÊ kolejnoúÊ, aby najnowsze dane by≥y na poczπtku (dla tekstowego formatu)
            // ale dla wykresu zachowujemy chronologicznπ kolejnoúÊ (od najstarszych do najnowszych)
            int valueCount = 0;
            for (const auto& value : data["values"]) {
                if (value.contains("date") && value.contains("value")) {
                    valueCount++;
                    if (valueCount > 24) break; // Ogranicz do 24 pomiarÛw (zazwyczaj 1 dzieÒ)

                    std::string dateStr = value["date"];
                    wxDateTime dt = parseDateTime(dateStr);
                    wxString date = dt.Format("%Y-%m-%d %H:%M");

                    // Dane dla wyúwietlenia tekstowego
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

            // OdwrÛÊ wektor dla wykresu, aby najstarsze dane by≥y na poczπtku
            std::reverse(chartData.begin(), chartData.end());

            measurementText->SetLabel(dataText);
            measurementText->Wrap(300); // Dopasowanie tekstu do szerokoúci pola

            // Ustaw dane wykresu
            chartPanel->SetTitle(wxString::Format("Pomiary: %s", paramName));
            chartPanel->SetData(chartData, paramName);
        }
        catch (json::exception& e) {
            wxMessageBox(wxString::Format("B≥πd parsowania danych pomiarowych: %s", e.what()), "Error", wxOK | wxICON_ERROR);
        }
        catch (...) {
            wxMessageBox("Nieznany b≥πd parsowania danych pomiarowych.", "Error", wxOK | wxICON_ERROR);
        }
    }

    void loadAirQualityIndex(int stationId) {
        std::string url = "https://api.gios.gov.pl/pjp-api/rest/aqindex/getIndex/" + std::to_string(stationId);
        std::string response = fetchDataFromAPI(url);

        // Zapisz odpowiedü do pliku debugowego
        std::ofstream debugFile("aqindex_response_" + std::to_string(stationId) + ".json");
        debugFile << response;
        debugFile.close();

        if (response.empty()) {
            wxMessageBox("Brak odpowiedzi z API podczas pobierania indeksu jakoúci powietrza.", "Error", wxOK | wxICON_ERROR);
            return;
        }
        try {
            json indexData = json::parse(response);

            // Sprawdü czy obiekt ma pole stIndexLevel i czy nie jest null
            if (!indexData.contains("stIndexLevel") || indexData["stIndexLevel"].is_null()) {
                wxMessageBox("Brak indeksu jakoúci powietrza dla wybranej stacji.", "Informacja", wxOK | wxICON_INFORMATION);
                airQualityIndex->SetLabel("Indeks jakoúci powietrza: Brak danych");
                return;
            }

            // Sprawdü czy stIndexLevel zawiera potrzebne pola
            if (!indexData["stIndexLevel"].contains("id") || !indexData["stIndexLevel"].contains("indexLevelName")) {
                wxMessageBox("Niepe≥ne dane indeksu jakoúci powietrza.", "Informacja", wxOK | wxICON_INFORMATION);
                airQualityIndex->SetLabel("Indeks jakoúci powietrza: Niepe≥ne dane");
                return;
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
            airQualityIndex->SetLabel(wxString::Format("Indeks jakoúci powietrza: %s", levelText));
            airQualityIndex->SetForegroundColour(color);
        }
        catch (json::exception& e) {
            wxMessageBox(wxString::Format("B≥πd parsowania indeksu jakoúci powietrza: %s", e.what()), "Error", wxOK | wxICON_ERROR);
        }
        catch (...) {
            wxMessageBox("Nieznany b≥πd parsowania indeksu jakoúci powietrza.", "Error", wxOK | wxICON_ERROR);
        }
    }
};

class App : public wxApp {
public:
    virtual bool OnInit() {
        MainWindow* window = new MainWindow();
        window->Show();
        return true;
    }
};

wxIMPLEMENT_APP(App);