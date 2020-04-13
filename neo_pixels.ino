
// Lib
#include <Time.h>
#include <TimeLib.h>
#include <Wire.h>
#include <DS1307RTC.h>

// Adafruit NeoPixel
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif


// Which pin on the Arduino is connected to the NeoPixels?
#define PIN 3

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS 40

// Initial brightness (0-255)
#define NEOPIXEL_BRIGHTNESS 20

// When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.
// Note that for older NeoPixel strips you might need to change the third parameter--see the strandtest
// example for more information on possible values.
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

// Delay value
#define DELAYVAL 10000

// Time variables
int h;
int m;
int s;

// RGB color variables
int red = 0;
int green = 0;
int blue = 255;

//Declare integer array with size corresponding to number of Neopixels in chain
int individualPixels[NUMPIXELS];


void setup() {

#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif

  // setTime(23, 44, 0, 29, 3, 2020); //Initialize current time as Midnight/noon 04/01/2020
  pixels.begin();

  Serial.begin(9600); //Begin Serial for debugging purposes

}


void loop()
{

  tmElements_t tm;

  if (RTC.read(tm)) {
    Serial.print("Ok, Time = ");
    print2digits(tm.Hour);
    Serial.write(':');
    print2digits(tm.Minute);
    Serial.write(':');
    print2digits(tm.Second);
    Serial.print(", Date (D/M/Y) = ");
    Serial.print(tm.Day);
    Serial.write('/');
    Serial.print(tm.Month);
    Serial.write('/');
    Serial.print(tmYearToCalendar(tm.Year));
    Serial.println();
  } else {
    if (RTC.chipPresent()) {
      Serial.println("The DS1307 is stopped.  Please run the SetTime");
      Serial.println("example to initialize the time and begin running.");
      Serial.println();
    } else {
      Serial.println("DS1307 read error!  Please check the circuitry.");
      Serial.println();
    }
    delay(9000);
  }
  delay(1000);



  pixels.clear();


  //Declare integer array with size corresponding to number of Neopixels in chain
  int individualPixels[NUMPIXELS] = {0};

  // Time
  if ( tm.Hour == 0 )
    return 12; // 12 midnight
  else if ( tm.Hour  > 12)
    h = tm.Hour - 12 ;
  else
    h = tm.Hour ;
    
  m = tm.Minute;

  /* Parse time values to light corresponding pixels */
  individualPixels[0] = 1; //Light "IL"
  individualPixels[1] = 1; //Light "EST"


  /* Minutes between 35-60 - Light "MOINS" & MODIFY CURRENT HOUR VALUE */
  if (m >= 35) {
    // MOINS
    individualPixels[25] = 1;
    individualPixels[26] = 1;
    h++; //Add 1 from current hour
    /*Set time to twelve for hour around midnight, noon */
    if (h == 0) {
      h = 12;
    }
    /*Corner case for 12:35-12:59 */
    if (h == 13) {
      h = 1;
    }
  }


  /* Hour=1 - Light "UNE" */
  if (h == 1) {
    individualPixels[2] = 1;
  }

  /* Hour=2 - Light "DEUX" */
  if (h == 2) {
    individualPixels[3] = 1;
    individualPixels[4] = 1;
  }

  /* Hour=3 - Light "TROIS" */
  if (h == 3) {
    individualPixels[8] = 1;
    individualPixels[9] = 1;
  }

  /* Hour=4 - Light "QUATRE" */
  if (h == 4) {
    individualPixels[5] = 1;
    individualPixels[6] = 1;
    individualPixels[7] = 1;
  }

  /* Hour=5 - Light "CINQ" */
  if (h == 5) {
    individualPixels[10] = 1;
    individualPixels[11] = 1;
  }

  /* Hour=6 - Light "SIX" */
  if (h == 6) {
    individualPixels[12] = 1;
  }

  /* Hour=7 - Light "SEPT" */
  if (h == 7) {
    individualPixels[13] = 1;
    individualPixels[14] = 1;
  }

  /* Hour=8 - Light "HUIT" */
  if (h == 8) {
    individualPixels[17] = 1;
    individualPixels[18] = 1;
  }

  /* Hour=9 - Light "NEUF" */
  if (h == 9) {
    individualPixels[15] = 1;
    individualPixels[16] = 1;
  }

  /* Hour=10 - Light "DIX" */
  if (h == 10) {
    individualPixels[19] = 1;
  }

  /* Hour=11 - Light "ONZE" */
  if (h == 11) {
    individualPixels[38] = 1;
    individualPixels[39] = 1;
  }



  /* HEURE ou  MIDI ou MINUIT */
  if (h == 12) {

    /* Hour=12 - Light "MIDI" */
    if (isAM() == true) {
      individualPixels[20] = 1;
      individualPixels[21] = 1;
    }

    /* Hour=12 - Light "MINUIT" */
    if (isPM() == true) {
      individualPixels[22] = 1;
      individualPixels[23] = 1;
    }

  } else {

    // HEURE
    individualPixels[27] = 1;
    individualPixels[28] = 1;
  }




  /* Minutes between 0-5 - Light "O CLOCK" */
  if ((m >= 0 && m < 5)) {

  }

  /* Minutes between 5-10 or 55-60 - Light "CING ("MINUTES) */
  if ((m >= 5 && m < 10) || (m >= 55 && m < 60)) {
    individualPixels[35] = 1;
    individualPixels[34] = 1;
  }

  /* Minutes between 10-15 or 50-55 - Light "DIX (MINUTES)" */
  if ((m >= 10 && m < 15) || (m >= 50 && m < 55)) {
    individualPixels[24] = 1;
  }

  /* Minutes between 15-20 or 45-50 - Light "QUART" */
  if ((m >= 15 && m < 20) || (m >= 45 && m < 50)) {
    individualPixels[30] = 1;
    individualPixels[31] = 1;
  }

  /* Minutes between 20-25 or 40-45 - Light "VINGT (MINUTES) */
  if ((m >= 20 && m < 25) || (m >= 40 && m < 45)) {
    individualPixels[36] = 1;
    individualPixels[37] = 1;
  }

  /* Minutes between 25-30 or 35-40 - Light "VINGT (MINUTES)", "CINQ (MINUTES)" */
  if ((m >= 25 && m < 30) || (m >= 35 && m < 40)) {
    // VINGT
    individualPixels[36] = 1;
    individualPixels[37] = 1;

    // CINQ
    individualPixels[35] = 1;
    individualPixels[34] = 1;
  }

  /* Minutes between 30-35 - Light "DEMI" */
  if ((m >= 30 && m < 35)) {
    individualPixels[32] = 1;
    individualPixels[33] = 1;
  }

  /* Minutes between 5-35 - Light "ET" */
  if ((m >= 5) && (m < 35)) {
    individualPixels[29] = 1;
  }



  Serial.print(h);
  Serial.print("h");
  Serial.print(m);
  Serial.println(" <== heure pour éclairage de LED  ( +1 heure si minute > 35 )");



  /* Light pixels corresponding to current time */
  for (int i = 0; i < sizeof(individualPixels); i++) {
    if (individualPixels[i] == 1) {
      pixels.setPixelColor(i, pixels.Color(red, green, blue)); //Set Neopixel color

      Serial.print("Pixels ON : ");
      Serial.println(i);

    }
    else {
      pixels.setPixelColor(i, pixels.Color(0, 0, 0));
    }
  }


  pixels.show(); //Display Neopixel color

  delay(DELAYVAL);

  /* Clear pixel values for re-assignment during next iteration */
  for (int j = 0; j < sizeof(individualPixels); j++) {
    individualPixels[j] = 0; //Set array values to 0
    pixels.setPixelColor(j, pixels.Color(0, 0, 0)); //Set Neopixel color to 0 brightness, i.e. off
  }

}


void print2digits(int number) {
  if (number >= 0 && number < 10) {
    Serial.write('0');
  }
  Serial.print(number);
}
