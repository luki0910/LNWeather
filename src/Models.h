/**
 * @file Models.h
 * @brief Definicje struktur danych u¿ywanych w aplikacji.
 */

#pragma once
#include <wx/datetime.h>

 /**
  * @struct MeasurementData
  * @brief Pomocnicza struktura do przechowywania danych pomiarowych.
  *
  * Przechowuje wartoœæ pojedynczego pomiaru wraz z dat¹ oraz flag¹ okreœlaj¹c¹
  * czy wartoœæ jest poprawna.
  */
struct MeasurementData {
    wxDateTime date;   /**< Data i czas pomiaru */
    double value;      /**< Wartoœæ pomiaru */
    bool hasValue;     /**< Flaga okreœlaj¹ca czy wartoœæ jest dostêpna */
};