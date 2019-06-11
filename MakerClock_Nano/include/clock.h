/***********************************************************************/
// File: clock.h
// Typ: Header File
// Author: Yannick Ducret
// Created: 06/11/2019 (MM/DD/YYYY)
// Changed: 06/11/2019 (MM/DD/YYYY)
// Version: v1.00
// ----------------------------------------------------------------------
// description:
// 		Sets up the clock rutines, colors and functions declarations
// ----------------------------------------------------------------------
// dependancies:	
// ----------------------------------------------------------------------
// legend:
// 		a)	added
// 		c)	commented
// 		d)	deleted
// 		m)	modified
// 		ut)	untested
// 
// Changelog:
// 
// v1.00:
// a) created file
// 06/11/2019 - YD
// 
/***********************************************************************/

// declare dependencies
#include "lib.h"

// declare all methods
#ifndef CLOCK_H
#define CLOCK_H

// Die Breite des Stundenzeigers (nur ungerade Zahlen)
#define BREITE_ZEIGER_STUNDEN 5

/**************************************************************************/
// FARBEN DEFINIEREN
#define RED           pixels.Color(225,0,0)
#define GREEN         pixels.Color(0,255,0)
#define BLUE          pixels.Color(0,0,255)
#define PURPLE        pixels.Color(255,5,255)
#define LIGHT_RED     pixels.Color(150,0,0)
#define LIGHT_WHITE   pixels.Color(205,205,65)
#define LIGHT_BLUE    pixels.Color(0,58,238)
#define DARK_BLUE     pixels.Color(0, 71, 186)
// Hier kann man neue Farben definieren:
// Im Internet nach einem "hex color picker" suchen und die Farbe wählen
#define MEINE_FARBE   0x50AB00
// Oder selber Farben mischen:
// pixels.Color (ROT-Anteil, GRÜN-Anteil, BLAU-Anteil)
// 0 = dunkel / 255 = hell
#define MEINE_FARBE_2 pixels.Color(55, 0, 80)

// FARBEN FÜR DIE ZEIGER AUSWÄHLEN
// In den Klammern unten können 3 Farben für die Zeiger aus den Farben von 
// oben gewählt werden. 
// Tipp: Im Programm können die Farben dynamisch geändert werden
//       z.B. eine eigene Farbe für den Nachmittag oder während der Nacht
uint32_t farbeZeigerStunde  = (DARK_BLUE);
uint32_t farbeZeigerMinute  = (PURPLE);
uint32_t farbeZeigerSekunde = (GREEN);
/**************************************************************************/

// Modus Definitionen
#define MODUS_UHR_NORMAL      0
#define MODUS_ZEIT_EINSTELLEN 1
#define SUBMODUS_STUNDE  0
#define SUBMODUS_MINUTE  1
#define SUBMODUS_SEKUNDE 2

#define ENTPRELL_ZEIT 50

float brightnessFilter = 0;
long encoderPositionOld  = 0;  
int modus = MODUS_UHR_NORMAL;
int submodus = SUBMODUS_STUNDE;
int button_counter = 0;
int time_h, time_m, time_s;
int neue_stunde, neue_minute, neue_sekunde;

/**************************************************************************/

// Function prototypes

/**************************************************************************/
// Animation zur vollen Stunde
// wie eine Kuckucksuhr
void volleStundeAnimation(int stunde);

/**************************************************************************/
// Animation zum Wechsel in den Zeit-Einstell-Modus
void modusWechselAnimation();

/**************************************************************************/
// Animation zum Bestätigen der neu eingestellten Uhrzeit
void zeitSpeichernAnimation();

/**************************************************************************/
// Animation beim Einstellen der Zeit
void clickAnimation();

/**************************************************************************/
// Wenn der Knopf lange gedrückt wurde
void buttonLongPressFunktion();

/**************************************************************************/
// Wenn der Knopf nur kurz gedrückt wurde
void buttonClickFunktion();

/**************************************************************************/
// Helligkeit abfragen
// *_* this is where some magic happens ??
int getBright();

/**************************************************************************/
// Uhrzeit auf den LEDs anzeigen
void setClock(int zeit_h, int zeit_m, int zeit_s);

#endif