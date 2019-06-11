/***********************************************************************/
// File: board.h
// Typ: Header File
// Author: Yannick Ducret
// Created: 06/11/2019 (MM/DD/YYYY)
// Changed: 06/11/2019 (MM/DD/YYYY)
// Version: v1.00
// ----------------------------------------------------------------------
// description:
// 		Initializes board related configs
//
// Target boards:	Nano (atmega328P)
// 					Nano (atmega328P (old))
// 					Nano (BL: UNO)
//
// ----------------------------------------------------------------------
// dependancies:	Wire.h
//					RTClib.h
//					Encoder.h
//					OneButton.h
//					Adafruit_NeoPixel.h
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

// declare dependancies
#include "lib.h"

#ifdef __AVR__            //
#include <avr/power.h>    // wird für die LEDs benötigt
#endif 

// declare all methods
#ifndef BOARD_H
#define BOARD_H

// Pins definieren
// Dreh-Encoder Pins
#define PIN_ENCODER_A      3         // Interrupt Pin
#define PIN_ENCODER_B      4         // Interrupt Pin
#define PIN_BUTTON         5
#define PIN_LIGHT_SENSOR   3

//Die zwei LEDs auf der Frontseite:
#define LED1 A1
#define LED2 A2

// LED Einstellungen
#define PIN_LED     6  // LEDs sind am Pin D6 angeschlossen
#define NUM_PIXELS 60  // Anzahl LEDs = 60
#define LED_OFFSET 41  // Anpassung der Ausrichtung an das Gehäuse (Uhr drehen)

// Objekte erzeugen
RTC_DS1307 rtc; 
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUM_PIXELS, PIN_LED, NEO_GRB + NEO_KHZ800);
Encoder drehknopf(PIN_ENCODER_A, PIN_ENCODER_B);
OneButton knopf(PIN_BUTTON, true);   // pin, lowactive = true

#endif