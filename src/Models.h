/**
 * @file Models.h
 * @brief Definicje struktur danych u�ywanych w aplikacji.
 */

#pragma once
#include <wx/datetime.h>

 /**
  * @struct MeasurementData
  * @brief Pomocnicza struktura do przechowywania danych pomiarowych.
  *
  * Przechowuje warto�� pojedynczego pomiaru wraz z dat� oraz flag� okre�laj�c�
  * czy warto�� jest poprawna.
  */
struct MeasurementData {
    wxDateTime date;   /**< Data i czas pomiaru */
    double value;      /**< Warto�� pomiaru */
    bool hasValue;     /**< Flaga okre�laj�ca czy warto�� jest dost�pna */
};