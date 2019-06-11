/***********************************************************************/
// File: clock.cpp
// Typ: c++ File
// Author: YD95
// Created: 06/11/2019 (MM/DD/YYYY)
// Changed: 06/11/2019 (MM/DD/YYYY)
// Version: v1.00
// ----------------------------------------------------------------------
// description:
//      Sets up the clock rutines, colors and functions declarations
// ----------------------------------------------------------------------
// dependancies:	board.h
//					clock.h
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

#include "board.h"
#include "clock.h"

/**************************************************************************/
// Animation zur vollen Stunde
// wie eine Kuckucksuhr
void volleStundeAnimation(int stunde){



}

/**************************************************************************/
// Animation zum Wechsel in den Zeit-Einstell-Modus
void modusWechselAnimation(){
  // ANPASSBAR ;)
  for(int j = 0; j < NUM_PIXELS; j++){
	pixels.setPixelColor(j, 128, 128, 128);
  }
  pixels.show();
  delay(200);
  pixels.clear();
  delay(200);
}

/**************************************************************************/
// Animation zum Bestätigen der neu eingestellten Uhrzeit
void zeitSpeichernAnimation(){
  // ANPASSBAR ;)
  for(int i = 0; i < 128; i++){
	for(int j = 0; j < NUM_PIXELS; j++){
	  pixels.setPixelColor(j, i, i*2, 0);
	}
	pixels.show();
	delay(1);
  }
  for(int i = 0; i < 128; i++){
	for(int j = 0; j < NUM_PIXELS; j++){
	  pixels.setPixelColor(j, (128-i)/2, (128-i)*2, 0);  // es leuchtet grün
	}
	pixels.show();
	delay(1);
  }
}

/**************************************************************************/
// Animation beim Einstellen der Zeit
void clickAnimation(){
  // ANPASSBAR ;)
  for(int i = 0; i < 128; i++){
	for(int j = 0; j < NUM_PIXELS; j++){
	  pixels.setPixelColor(j, i, i, i);
	}
	pixels.show();
	delay(1);
  }
}

/**************************************************************************/
// Wenn der Knopf lange gedrückt wurde
void buttonLongPressFunktion(){
  modusWechselAnimation();
  modus = MODUS_ZEIT_EINSTELLEN;
  submodus = SUBMODUS_STUNDE;
  neue_stunde = time_h;
  neue_minute = time_m;
  neue_sekunde = time_s;
}

/**************************************************************************/
// Wenn der Knopf nur kurz gedrückt wurde
void buttonClickFunktion(){
  if(modus == MODUS_ZEIT_EINSTELLEN){
	submodus ++;
	if(submodus == (SUBMODUS_SEKUNDE + 1)){
	  zeitSpeichernAnimation();
	  DateTime now = rtc.now();
	  rtc.adjust(DateTime(now.year(), now.month(), now.day(), neue_stunde, neue_minute, neue_sekunde));
	} else {
	  clickAnimation();
	}
  }
}

/**************************************************************************/
// Helligkeit abfragen
// *_* this is where some magic happens ??
int getBright() {
  signed int bright = 0;
  bright = analogRead(PIN_LIGHT_SENSOR);
  if (bright >= 512) {
	bright = (234 - (bright - 512) / 2);
  }
  else {
	bright = 255;
  }
  if (bright < 3) {
	bright = 3;
  }
  return bright;
}

/**************************************************************************/
// Uhrzeit auf den LEDs anzeigen
void setClock(int zeit_h, int zeit_m, int zeit_s){
  pixels.clear();
  // Stundenzeiger
  int i = 0;
  do{
	pixels.setPixelColor(((zeit_h + NUM_PIXELS) * 5 + LED_OFFSET+i) % NUM_PIXELS, farbeZeigerStunde);
	pixels.setPixelColor(((zeit_h + NUM_PIXELS) * 5 + LED_OFFSET-i) % NUM_PIXELS, farbeZeigerStunde);
	i++;
  }while(i <= (BREITE_ZEIGER_STUNDEN/2));
  // Minutenzeiger
  pixels.setPixelColor((zeit_m + LED_OFFSET) % NUM_PIXELS, farbeZeigerMinute);
  // Sekundenzeiger
  pixels.setPixelColor((zeit_s + LED_OFFSET) % NUM_PIXELS, farbeZeigerSekunde);

  // Helligkeit überprüfen
  brightnessFilter = (29 * brightnessFilter + getBright() ) / 30; // Moving Average Filter
  pixels.setBrightness(brightnessFilter);   // Helligkeit einstellen
  pixels.show();                            // LEDs aktualisieren
}