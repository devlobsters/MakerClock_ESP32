/**************************************************************************
  FHNW Maker Studio - Maker Clock

  file:   ArduinoMakerClock60.ino
  author: Simon Burkhardt (github.com/mnemocron)
  date:   2018.09.15

  Mit dem Arduino Nano, 60 NeoPixel LEDs und der DS1307 Real Time Clock
  Basirend auf den Library Beispielen zu RTClib.h / Encoder.h / OneButton.h 
  und der alten MakerClock Software

  Folgende Libraries müssen zusätzlich installiert sein:
  - RTClib.h
  - Encoder.h
  - OneButton.h
  - Adafruit_NeoPixel.h

  Weitere Informationen zur MakerClock:
  makerstudio.fhnw.ch
  github.com/fhnw-makerstudio

 **************************************************************************
*/


// Bibliotheken einbinden
#include "lib.h"

// Additional Header files
#include "board.h"    // board pinning, variables and extarnal settings
#include "clock.cpp"  // clock settings, clock functions


/**************************************************************************/
void setup () {
  Serial.begin(9600);     // Serielle Kommunikation starten
  Wire.begin();           // I2C Kommunikation zur DS1307 Clock starten
  rtc.begin();            // DS1307 Clock starten
  pixels.begin();         // LEDs Starten

  // Pin Modes setzen
  pinMode(PIN_ENCODER_A, INPUT_PULLUP);
  pinMode(PIN_ENCODER_B, INPUT_PULLUP);
  pinMode(PIN_BUTTON, INPUT_PULLUP);
  pinMode(PIN_LED, OUTPUT);

  // Lichtsensor einlesen
  int brightness = analogRead(PIN_LIGHT_SENSOR);

  // Die Variablen __DATE__ und __TIME__ beinhaltet das Datum + Zeit
  // im Moment des Kompilierens. Damit wird die Uhr beim Programmieren
  // gleich auf die richtige Zeit eingestellt
  if (! rtc.isrunning()) {
	Serial.println("RTC is NOT running!");
	// following line sets the RTC to the date & time this sketch was compiled
	rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  knopf.attachDuringLongPress(buttonLongPressFunktion); // Lang-Drück-Funktion anhängen
  knopf.attachClick(buttonClickFunktion);               // Kurz-Drück-Funktion anhängen

  pixels.show();                      // Alle LEDs ausgeschaltet
  pixels.setBrightness(brightness);   // Helligkeit einstellen
  Serial.println("MakerClock");
}

/**************************************************************************/
// Endlosschleife
void loop () {
  int delta_t = 0;
  long encoderPositionNew;
  DateTime theTime;
  switch(modus){
	/************************************/
	case MODUS_UHR_NORMAL:
	  theTime = rtc.now();        // neue Zeit von der DS1307 Clock holen
	  time_s = theTime.second();  // get seconds
	  time_m = theTime.minute();  // get minutes
	  time_h = theTime.hour();    // get hours
	  if(time_m == 0){
		volleStundeAnimation(time_h);       // Animation für Kuckucksuhr
	  }
	  setClock(time_h, time_m, time_s);   // Uhrzeit anzeigen
	  break;
	/************************************/
	case MODUS_ZEIT_EINSTELLEN:
	  delta_t = 0;
	  encoderPositionNew = drehknopf.read();
	  delta_t = (-(encoderPositionNew - encoderPositionOld))/2;   // der Encoder generiert 2 Ticks pro Step 
	  encoderPositionOld = encoderPositionNew;
	  switch(submodus){
		case SUBMODUS_STUNDE:
		  neue_stunde += delta_t;
		  break;
		case SUBMODUS_MINUTE:
		  neue_minute += delta_t;
		  break;
		case SUBMODUS_SEKUNDE:
		  neue_sekunde += delta_t;
		  break;
		default:
		  modus = MODUS_UHR_NORMAL;
	  }
	  setClock(neue_stunde, neue_minute, neue_sekunde);
	  break;
	/************************************/
	default:
	  modus = MODUS_UHR_NORMAL;
  }

  knopf.tick();                   // knopf abfragen
  delay(ENTPRELL_ZEIT);           // 0.05 Sekunden warten
}