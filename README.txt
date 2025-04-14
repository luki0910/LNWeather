# LNWeather

Aplikacja umożliwia monitorowanie jakości powietrza w różnych lokalizacjach w Polsce.
Program korzysta z danych pozyskiwanych z publicznej usługi REST, prezentuje je w formie wykresów
oraz umożliwia zapisywanie danych historycznych w lokalnej bazie danych.

## Funkcje:
- Wybór stacji pomiarowych  poprzez listę wszystkich stacji w Polsce uzyskanych z API GIOS.
- Wybór czujnika pomiarowego dla wybranej stacji.
- Pobieranie oraz wyświetlanie danych pomiarowych.
- Wizualizacja danych na wykresach.
- Zapisywane danych w lokalnej bazie danych (JSON).
- Pobieranie z lokalnej bazy danych w przypadku braku możliwości pobrania z API.
- Podstawowa analiza danych.

## Wymagania systemowe:
- Kompilator C++ z obsługą standardu C++17 lub nowszego.
- curl 8.11.1/libcurl 8.11.1 lub nowszy.
- wxWidgets 3.2.7 lub nowszy.
- json v3.10.2 lub nowszy.
- CMake 3.12 lub nowszy.

## Struktura projektu:

- 'main.cpp' - punkt wejściowy aplikacji, inicjalizacja okna.
- 'MainWindow.cpp' i 'MainWindow.h' - implementacja głównego okna aplikacji LNWeather, odpowiadająca ze interfejs graficzny i funkcjonalność aplikacji.
- 'DatabaseService.cpp' i  'DatabaseService.h' - implementacja usługi lokalnej bazy danych.
- 'ApiService.cpp' i 'ApiService.h' - implementacja usługi komunikacji z API.
- 'ChartPanel.cpp' i 'ChartPanel.h' - implementacja panelu wykresów danych pomiarowych.
-  'Utils.cpp' i 'Utils.h' - implementacja narzędzi pomocniczych do aplikacji.
- 'Models.h' - definicja struktury danych używanej do przechowywania danych pomiarowych.
- 'resource.h' - implementacja zasobów (ikony aplikacji).

## Pliki wyjściowe/generowane:
- 'response_debug.json' - odpowiedź serwera przy próbie pobrania danych z API (tylko do debugowania).
- 'airquality_x.json' - dane pomiarowe otrzymane z serwera API dotyczące jakości powietrza czujnika o ID=x.
- 'measurements_x.json' - dane pomiarowe otrzymane z serwera API dotyczące wszystkich parametrów czujnika o ID=x.
- 'sensors_x.json' - lista wszystkich czujników otrzymanych z serwera API dotyczące stacji o ID=x.
- 'stations.json' - lista wszystkich stacji otrzymanych z serwera API.

## Uruchomienie aplikacji:
1. Uruchom aplikację używając pliku wykonywalnego 'LNWeather.exe'.
2. Upewnij się, że masz stabilne połączenie z internetem, aby pobrać aktualne dane z serwera API GIOS (wykonywane automatycznie).
3. Wybierz stację z listy wszystkich stacji w Polsce z rozwijanej listy.
4. Wybierz czujnik z listy czujników dla wybranej stacji.

(opcjonalne)
1. Dokonaj prostej analizy danych klikając w zakładkę 'Analiza danych', a następnie w przycisk 'Wykonaj analizę danych'.
2. Zapisz dane bieżącej stacji do bazy lokalnej używając przeznaczonego do tego przycisku.
3. Zapisz dane wszystkich stacji do bazy lokalnej używając przeznaczonego do tego przycisku.
4. Zastosuj zakres dat dla danych pomiarowych wybranej stacji oraz czujnika.

## Autor:
Łukasz Noworolnik
Projekt wykonany na potrzeby edukacyjne.

