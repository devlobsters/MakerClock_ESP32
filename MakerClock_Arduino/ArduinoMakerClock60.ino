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
#include <Wire.h>         // für die Kommunikation mit dem DS1307 Chip
#include <RTClib.h>       // für die Ansteuerung des DS1307 Chips
#include <Encoder.h>      // für den Encoder Knopf
#include <OneButton.h>
#include <Adafruit_NeoPixel.h>  // für die LEDs

#ifdef __AVR__            //
#include <avr/power.h>    // wird für die LEDs benötigt
#endif                    // 

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

// Die Breite des Stundenzeigers (nur ungerade Zahlen)
#define BREITE_ZEIGER_STUNDEN 5

// Objekte erzeugen
RTC_DS1307 rtc; 
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUM_PIXELS, PIN_LED, NEO_GRB + NEO_KHZ800);
Encoder drehknopf(PIN_ENCODER_A, PIN_ENCODER_B);
OneButton knopf(PIN_BUTTON, true);   // pin, lowactive = true

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


/**************************************************************************/
// Animation zur vollen Stunde
// wie eine Kuckucksuhr
void volleStundeAnimation(int stunde){
  // SELBER MACHEN    ;)
  // zum Beispiel auch: Farbwechsel der Zeiger in den Morgenstunden...
  // folgende Farbvariablen stehen zum Ändern zur Auswahl
  // farbeZeigerStunde  = 
  // farbeZeigerMinute  = 
  // farbeZeigerSekunde = 

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
  brightnessFilter = (29 * brightnessFilter + getBright()) / 30; // Moving Average Filter
  pixels.setBrightness(brightnessFilter);   // Helligkeit einstellen
  pixels.show();                            // LEDs aktualisieren
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

