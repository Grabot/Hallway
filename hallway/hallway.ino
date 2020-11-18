#include <FastLED.h>

// How many leds in your strip?
#define NUM_LEDS 300

#define DATA_PIN 5

// It will not light up just one led, but a bulk of 5 leds
int led_width = 20;

// Define the array of leds
CRGB leds[NUM_LEDS];

// defines pins numbers for sensor
const int trigPin = 3;
const int echoPin = 2;
// defines variables
long duration;
int distance;
int halfwayDistance;
boolean halfway;
int reliabilityCycles;
int offSet;

int halfwayLedIndex;

DEFINE_GRADIENT_PALETTE( heatmap_gp ) {
  0,     0,  0,  255,   // blue
128,   0,  255,  0,   // green
224,   255,0,  0,   // red
255,   255,255,255 }; //full white

CRGBPalette16 myPal = heatmap_gp;

void setup() { 
    // Uncomment/edit one of the following lines for your leds arrangement.
    // ## Clockless types ##
    FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);  // GRB ordering is assumed
    pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
    pinMode(echoPin, INPUT); // Sets the echoPin as an Input
    Serial.begin(9600); // Starts the serial communication
    halfway = false;
    halfwayDistance = 140;
    // Because of the sometimes unreliable inputs from the distance sensor we want it to be above the halfway point for a number of cycles.
    reliabilityCycles = 4;
    // There is also an offset by how far the distance sensor needs to detect a value.
    offSet = 20;
    halfwayLedIndex = 0;
}

void loop() { 
  if (halfway == false) {
  
    // Clears the trigPin
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    // Sets the trigPin on HIGH state for 10 micro seconds
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    // Reads the echoPin, returns the sound wave travel time in microseconds
    duration = pulseIn(echoPin, HIGH);
    // Calculating the distance
    distance= duration*0.034/2;

    // Turn the LEDS off.
    for (int led = 0; led < NUM_LEDS; led++) {
      // If the LED was on recently, we don't turn it off
      leds[led] = CRGB::Black;
    }
    // Turn the LEDS on which need to be turned on.
    for (int i = 0; i < led_width; i++) {
      if (distance < halfwayDistance + 50) {
        int ledIndex = int(distance) - offSet + i;
        if (ledIndex < 0) {
          ledIndex = 0;
        }
        giveLedColour(ledIndex);
      }
    }
  
    // If the sensor reliably spots that the user is halfway, it starts a preprogrammed end fase
    // anything above 500 is correctly assumed as invalid.
    if (distance > halfwayDistance && distance < 500) {
      if (reliabilityCycles == 0) {
        halfway = true;
        halfwayLedIndex = halfwayDistance;
        // We also leave the leds around the halfway point on
        for (int i = 20; i < 20; i++) {
          int ledIndex = halfwayDistance - i;
          giveLedColour(ledIndex);
        }
      }
      reliabilityCycles = reliabilityCycles - 1;
    } else {
      reliabilityCycles = 4;
    }
  } else {
    
      giveLedColour(halfwayLedIndex);
      halfwayLedIndex = halfwayLedIndex + 1;

      // We keep the program running untill the entire strip is lit.
      if (halfwayLedIndex > NUM_LEDS+1) {
        // a delay of a few seconds!
        delay(3000);
        halfway = false;
        halfwayLedIndex = 0;
      }
  }
  
  FastLED.show();
}

void giveLedColour(int ledIndex) {
  // When the index increases the colour will traverse the spectrum
  // My plan right now is Blue -> Green -> Red -> Yellow
  int colourIndex = ledIndex;
  if (colourIndex > 255) {
    colourIndex = 255;
  }
  leds[ledIndex] = ColorFromPalette(myPal, colourIndex , 255, LINEARBLEND);
}
