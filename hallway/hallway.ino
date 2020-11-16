#include <FastLED.h>

// How many leds in your strip?
#define NUM_LEDS 300

// For led chips like WS2812, which have a data line, ground, and power, you just
// need to define DATA_PIN.  For led chipsets that are SPI based (four wires - data, clock,
// ground, and power), like the LPD8806 define both DATA_PIN and CLOCK_PIN
// Clock pin only needed for SPI based chipsets when not using hardware SPI
#define DATA_PIN 5
#define CLOCK_PIN 13

// It will not light up just one led, but a bulk of 20 leds
int led_width = 20;

// Define the array of leds
CRGB leds[NUM_LEDS];

// defines pins numbers for sensor
const int trigPin = 3;
const int echoPin = 2;
// defines variables
long duration;
int distance;

void setup() { 
    // Uncomment/edit one of the following lines for your leds arrangement.
    // ## Clockless types ##
    FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);  // GRB ordering is assumed
    pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
    pinMode(echoPin, INPUT); // Sets the echoPin as an Input
    Serial.begin(9600); // Starts the serial communication
}

void loop() { 

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
  // The distance variable now holds the distance
  Serial.print("Distance: ");
  Serial.println(distance);
  
  // Turn all the LEDS off.
  for (int led = 0; led < NUM_LEDS; led++) {
    leds[led] = CRGB::Black;
  }

  // Turn the LEDS on which need to be turned on.
  for (int i = 0; i < led_width; i++) {
    leds[distance + i] = CRGB::Blue;
  }

  FastLED.show();
}
