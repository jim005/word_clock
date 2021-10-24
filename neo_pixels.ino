
// inspiration:  https://github.com/Chnalex/arduino



// horloge avec 40 LEDs adressables ( 22 mots ) permettant d'afficher l'heure
// utilisation de 3 registres a decalage pour illuminer les 23 blocs de leds
// un BP (bouton poussoir) pour passage heure hiver/ete
// horloge RTC DS1307
// remise à l'heure via le port serie (envoie d'une chaine formatée : HHMMSSJJMMAAAA)
//
//
//                                                    _________| |__
//                                                TX  |  |()|  | |  |  Vin  --------- +9v
//                                                RX  |        | |  |  GND  --------- 0v
//                                                RST |       ICSP  |  RST
//                                                GND |             |  +5V  --------- alim horloge et LED
//                                                D2  |             |  A7
//                         LED data   ---------   D3  |   NANO      |  A6
//                                                D4  |             |  A5   --------- Bus I2C SCL pour horloge RTC (bleu)
//                                                D5  |             |  A4   --------- Bus I2C SDA pour horloge RTC (jaune)
//                                                D6  |             |  A3
//                                                D7  |             |  A2
//                                                D8  |             |  A1
//                                                D9  |             |  A0
//                                                D10 |             |  AREF
//                                                D11 |    _____    |  3.3V
//                  BP heure ete/hiver  --------- D12 |   [ USB ]   |  D13
//                                                    -----|___|-----
//

// Lib
#include <Time.h>
#include <TimeAlarms.h>
#include <TimeLib.h>
#include <Wire.h>
#include <DS1307RTC.h>

// Adafruit NeoPixel
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif


// NeoPixels LED data
#define PIN 3

// Button for winter / summer time
#define BP 12 
#include <EEPROM.h>

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS 40

// Initial brightness (0-255)
// #define NEOPIXEL_BRIGHTNESS 20

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

// Delay value
#define DELAYVAL 30000  // micro seconde

// Time variables
int heure_courante = 0;
int minute_courante = 0;
uint32_t valeur_courante = 0;

//Declare integer array with size corresponding to number of Neopixels in chain
int individualPixels[NUMPIXELS];

// Mots
const int IL[] = {0};
const int EST[] = {1};
const int UNE[] = {2};
const int DEUX[] = {3, 4};
const int QUATRE[] = {5, 6, 7};
const int TROIS[] = {8, 9};
const int CINQ[] = {10, 11};
const int SIX[] = {12};
const int SEPT[] = {13, 14};
const int NEUF[] = {15, 16};
const int HUIT[] = {17, 18};
const int DIX[] = {19};
const int MIDI[] = {22, 23};
const int MINUIT[] = {20, 21};
const int DIX_MINUTES[] = {24}; // doublons sur la grille d'affichage.
const int MOINS[] = {25, 26};
const int HEURE[] = {27, 28};
const int ET[] = {29};
const int QUART[] = {30, 31};
const int DEMI[] = {32, 33};
const int CINQ_MINUTES[] = {34, 35};  // doublons sur la grille d'affichage.
const int VINGT[] = {36, 37};
const int ONZE[] = {38, 39};

char buffer_serie[32];

uint8_t sensorVal;



void wordShowOn(int Param[], int count = 1, int red = 0, int green = 0, int blue = 255);

void setup() {

  Serial.begin(9600); //Begin Serial for debugging purposes
  Serial.flush();
  Serial.println("Let s start !");

  pinMode(BP,INPUT_PULLUP); //bp pour passage heure d'ete/heure d'hiver

  pixels.begin();
  pixels.clear();


  // Test de toutes les LEDs
  for (int i = 0; i < sizeof(individualPixels); i++) {
    pixels.clear();
    pixels.setPixelColor(i, pixels.Color(0, 255, 0));
    pixels.show();
    delay(200);
  }



  // Demarrage du bus I2C
  delay(400);
  Wire.begin();


  //definition de la synchro de l'horloge interne avec le module RTC toutes les 300 secondes
  setSyncProvider(RTC.get);
  setSyncInterval(3600);
  delay(200);


  if (timeStatus() == timeSet) {
    Serial.println("timeStatus() == timeSet");
  } else {
    Serial.println("BUG =>> timeStatus() != timeSet");
  }

  delay(1000);

  //ttes les leds en OFF
  ledsTurnOff ();

  // Declaration d'un timer toutes les secondes pour le cadencement des taches
  Alarm.timerRepeat(5, Cadenceur);

}

void ledsTurnOff () {

  pixels.clear();
  for (int i = 0; i < sizeof(individualPixels); i++) {
    pixels.setPixelColor(i, pixels.Color(0, 0, 0));
  }
  pixels.show();

}


void loop() {

  //trigger de declenchement de verif des timers d'alarme toues les 10 millisecondes
  Alarm.delay(100);

  ecoute_serie();

//  sensorVal = digitalRead(BP);
//
//  if (sensorVal == LOW) {
//    heure_hiver_ete();
//    delay(1000);
//  }

}




void print2digits(int number) {
  if (number >= 0 && number < 10) {
    Serial.write('0');
  }
  Serial.print(number);
}



void Cadenceur() {

  //uint32_t valeur = 0;
  //int heure_valeur = 0;
  int heure_tempo = 0;
  //uint32_t minute_valeur = 0;


  if (minute() != minute_courante || hour() != heure_courante) {

    ledsTurnOff ();


    Serial.println("IL EST");
    wordShowOn(IL, 1, 255, 255, 255);
    wordShowOn(EST, 1, 255, 255, 255);


    // Heure
    if (hour() == 12 and minute() < 35  ) {
      heure_tempo = 12;
    } else {
      heure_tempo = hour() % 12;
    }

    if (minute() > 35 ) {
      heure_tempo++;
    }


    switch (heure_tempo) {
      case 0:
        Serial.println("MINUIT");
        wordShowOn(MINUIT, 2);
        break;

      case 1:
        Serial.println("UNE HEURE");
        wordShowOn(UNE);
        wordShowOn(HEURE, 2);
        break;

      case 2:
        Serial.println("DEUX HEURE");
        wordShowOn(DEUX, 2);
        wordShowOn(HEURE, 2);
        break;

      case 3:
        Serial.println("TROIS HEURE");
        wordShowOn(TROIS, 2);
        wordShowOn(HEURE, 2);
        break;

      case 4:
        Serial.println("QUATRE HEURE");
        wordShowOn(QUATRE, 3);
        wordShowOn(HEURE, 2);
        break;


      case 5:
        Serial.println("CINQ HEURE");
        wordShowOn(CINQ, 2);
        wordShowOn(HEURE, 2);
        break;

      case 6:
        Serial.println("SIX HEURE");
        wordShowOn(SIX);
        wordShowOn(HEURE, 2);
        break;

      case 7:
        Serial.println("SEPT HEURE");
        wordShowOn(SEPT, 2);
        wordShowOn(HEURE, 2);
        break;

      case 8:
        Serial.println("HUIT HEURE");
        wordShowOn(HUIT, 2);
        wordShowOn(HEURE, 2);
        break;

      case 9:
        Serial.println("NEUF HEURE");
        wordShowOn(NEUF, 2);
        wordShowOn(HEURE, 2);
        break;

      case 10:
        Serial.println("DIX HEURE");
        wordShowOn(DIX);
        wordShowOn(HEURE, 2);
        break;

      case 11:
        Serial.println("ONZE HEURE");
        wordShowOn(ONZE);
        wordShowOn(HEURE, 2);
        break;

      case 12:
        Serial.println("MIDI");
        wordShowOn(MIDI, 2);
        break;

      default:
        // BUG
        Serial.println("MINUIT");
        wordShowOn(MINUIT, 2, 255, 0, 0);
        break;
    }


    ///heure_valeur = leds[heure_tempo];

    //
    //
    //        valeur = heure_valeur + leds[heure] + minute_valeur;
    //
    //        if (valeur != valeur_courante) {
    //          regOne.allOff();
    //          regOne.pinOn(valeur);
    //          valeur_courante = valeur;
    //        }

    // Minutes

    if (minute() >= 0 && minute() < 5) {

    }
    if (minute() >= 5 && minute() < 10) {
      Serial.println("ET CINQ_MINUTES");
      wordShowOn(ET, 1, 0, 255, 0);
      wordShowOn(CINQ_MINUTES, 2, 0, 255, 0);
    }
    if (minute() >= 10 && minute() < 15) {
      Serial.println("ET DIX_MINUTES");
      wordShowOn(ET, 1, 0, 255, 0);
      wordShowOn(DIX_MINUTES, 1, 0, 255, 0);
    }
    if (minute() >= 15 && minute() < 20) {
      Serial.println("ET QUART");
      wordShowOn(ET, 1, 0, 255, 0);
      wordShowOn(QUART, 2, 0, 255, 0);
    }
    if (minute() >= 20 && minute() < 25) {
      Serial.println("ET VINGT");
      wordShowOn(ET, 1, 0, 255, 0);
      wordShowOn(VINGT, 2, 0, 255, 0);
    }
    if (minute() >= 25 && minute() < 30) {
      Serial.println("ET VINGT CINQ_MINUTES");
      wordShowOn(ET, 1, 0, 255, 0);
      wordShowOn(ET, 1, 0, 255, 0);
      wordShowOn(CINQ_MINUTES, 2, 0, 255, 0);
    }
    if (minute() >= 30 && minute() < 35) {
      Serial.println("ET DEMI");
      wordShowOn(ET, 1, 0, 255, 0);
      wordShowOn(DEMI, 2, 0, 255, 0);
    }
    if (minute() >= 35 && minute() < 40) {
      Serial.println("MOINS VINGT CINQ_MINUTES");
      wordShowOn(MOINS, 2, 0, 255, 0);
      wordShowOn(VINGT, 2, 0, 255, 0);
      wordShowOn(CINQ_MINUTES, 2, 0, 255, 0);
    }
    if (minute() >= 40 && minute() < 45) {
      Serial.println("MOINS VINGT");
      wordShowOn(MOINS, 2, 0, 255, 0);
      wordShowOn(VINGT, 2, 0, 255, 0);;
    }
    if (minute() >= 45 && minute() < 50) {
      Serial.println("MOINS QUART");
      wordShowOn(MOINS, 2, 0, 255, 0);
      wordShowOn(QUART, 2, 0, 255, 0);
    }
    if (minute() >= 50 && minute() < 55) {
      Serial.println("MOINS DIX_MINUTES");
      wordShowOn(MOINS, 2, 0, 255, 0);
      wordShowOn(DIX_MINUTES, 1, 0, 255, 0);
    }
    if (minute() >= 55 && minute() < 60) {
      Serial.println("MOINS CINQ_MINUTES");
      wordShowOn(MOINS, 2, 0, 255, 0);
      wordShowOn(CINQ_MINUTES, 2, 0, 255, 0);
    }

    pixels.show();


    minute_courante = minute();
    heure_courante = hour();
  }

  Serial.println("******************************");
  Serial.println("Reglage Horloge");
  Serial.println("");
  Serial.print(hour());
  Serial.print(F(":"));
  Serial.print(minute());
  Serial.print(F(":"));
  Serial.print(second());
  Serial.print("  ");
  Serial.print(day());
  Serial.print(F("/"));
  Serial.print(month());
  Serial.print(F("/"));
  Serial.println(year());
  Serial.println("");
  Serial.println("pour mettre a jour l'heure");
  Serial.println("taper une chaine comme suit:");
  Serial.println("HHMMSSJJMMAAAA");
  Serial.print(hour());
  Serial.print(minute());
  Serial.print(second());
  Serial.print(day());
  Serial.print(month());
  Serial.println(year());
  Serial.println("******************************");
  Serial.println("");
  Serial.println("");
  Serial.println("");

}


void wordShowOn(int Param[], int count, int red, int green, int blue) {

  // Turn ON a single word
  //for (byte i = 0; i < (sizeof(Param) / sizeof(Param[0])); i++) {
  for (byte i = 0; i < count; i++) {
    Serial.print("LED a allumer : ");
    Serial.println(Param[i]);
    pixels.setPixelColor(Param[i], pixels.Color(red, green, blue));
  }

}




void ecoute_serie()  {

  int cpt = 0;
  int hr = 0;
  int mi = 0;
  int sec = 0;
  int jr = 0;
  int mo = 0;
  int an = 0;
  char inByte;

  raz_buffer();
  while (Serial.available() > 0) {
    inByte = Serial.read();
    buffer_serie[cpt] = inByte;
    cpt++;
    delay(5);
  }

  if (cpt > 0) {
    Serial.println(buffer_serie);

    //format HHMMSSJJMMAAAA
    hr = ((buffer_serie[0] - '0') * 10) + (buffer_serie[1] - '0');
    mi = ((buffer_serie[2] - '0') * 10) + (buffer_serie[3] - '0');
    sec = ((buffer_serie[4] - '0') * 10) + (buffer_serie[5] - '0');
    jr = ((buffer_serie[6] - '0') * 10) + (buffer_serie[7] - '0');
    mo = ((buffer_serie[8] - '0') * 10) + (buffer_serie[9] - '0');
    an = ((buffer_serie[10] - '0') * 1000) + ((buffer_serie[11] - '0') * 100) + ((buffer_serie[12] - '0') * 10) + (buffer_serie[13] - '0');
    Serial.print(hr);
    Serial.print(" ");
    Serial.print(mi);
    Serial.print(" ");
    Serial.print(sec);
    Serial.print(" ");
    Serial.print(jr);
    Serial.print(" ");
    Serial.print(mo);
    Serial.print(" ");
    Serial.print(an);
    Serial.println(" ");
    setTime(hr, mi, sec, jr, mo, an);
    RTC.set(now());
    Cadenceur();
  }
}


void raz_buffer()  {
  for (int i = 0; i < 32; i++) {
    buffer_serie[i] = '\0';
  }
}

void heure_hiver_ete(){
    int heures=hour();
    int minutes=minute();
    int secondes=second();
    int jour_mois=day();
    int mois=month();
    int annee=year();
    int heure_ete=EEPROM.read(200);
    if(heure_ete==0){
          if(heures==23){
            heures=0;
          }else{
            heures++;
          }
          setTime(heures,minutes,secondes,jour_mois,mois,annee);
          RTC.set(now());
          EEPROM.write(200,1);   
     }else{
          if(heures==0){
            heures=23;
          }else{
            heures--;
          }
          setTime(heures,minutes,secondes,jour_mois,mois,annee);
          RTC.set(now());
          EEPROM.write(200,0);       
     }
     Cadenceur(); 
}


