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

int inPin = 8; // pushbutton connected to digital pin 8
int program = 0;
boolean stillPressed = false;

void setup() { 
    // Uncomment/edit one of the following lines for your leds arrangement.
    // ## Clockless types ##
    FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);  // GRB ordering is assumed
    pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
    pinMode(echoPin, INPUT); // Sets the echoPin as an Input
    pinMode(inPin, INPUT); // Sets the echoPin as an Input
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
  int buttonPress = digitalRead(inPin);
  if (buttonPress == 1) {
    // The button is pressed. Change the program. as long as the button was not still pressed by the user.
    if (stillPressed == false) {
      // If this is the first time the press is registred, change the program. We define 3 different programs at this point.
      // program 0: The program that the whole project started for. Which is the distance sensor which determines where the leds are shown and what colour they are
      // program 1: The FASTLed rainbow wave with some random settings
      // program 2: Some yellow/white light for normal hallway lighting.
      if (program == 0) {
        program = 1;
      } else if (program == 1) {
        program = 2;
      } else if (program == 2) {
        program = 0;
      }
    }
    // The button is pressed, so we set the boolean to check this.
    stillPressed = true;
  } else {
    // The button is no longer pressed. Set the boolean to reflect this.
    stillPressed = false;
  }
  if (program == 0) {
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
  
  //    Serial.println("distance");
  //    Serial.println(distance);
  
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
  } else if (program == 1) {
    // simple colour cycle for the led strip
     rainbow_wave(30, 20);                                      // Speed, delta hue values.
  } else if (program == 2) {
      for (int led = 0; led < NUM_LEDS; led++) {
        // If the LED was on recently, we don't turn it off
        leds[led] = CRGB::White;
      }
  }
//  delay(25); /* the lower the value the faster your colors move (and vice versa) */
  FastLED.show();
}

void rainbow_wave(uint8_t thisSpeed, uint8_t deltaHue) {     // The fill_rainbow call doesn't support brightness levels.
 
  // uint8_t thisHue = beatsin8(thisSpeed,0,255);                // A simple rainbow wave.
 uint8_t thisHue = beat8(thisSpeed,255);                     // A simple rainbow march.
  
 fill_rainbow(leds, NUM_LEDS, thisHue, deltaHue);            // Use FastLED's fill_rainbow routine.
 
} // rainbow_wave()

void giveLedColour(int ledIndex) {
  // When the index increases the colour will traverse the spectrum
  // My plan right now is Blue -> Green -> Red -> Yellow
  int colourIndex = ledIndex;
  if (colourIndex > 255) {
    colourIndex = 255;
  }
  leds[ledIndex] = ColorFromPalette(myPal, colourIndex , 255, LINEARBLEND);
}
