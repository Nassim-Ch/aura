/// Inputs Relay
// --- Air IN
const int relay_Pin_IN_1 = 0;
const int relay_Pin_IN_2 = 1;
const int relay_Pin_IN_3 = 2;
const int relay_Pin_IN_4 = 3;
// -- Air OUT
const int relay_Pin_OUT_1 = 4;
const int relay_Pin_OUT_2 = 5;
const int relay_Pin_OUT_3 = 6;
const int relay_Pin_OUT_4 = 7;

/// Inputs TouchSensor
const int touch_1 = 15;
const int touch_2 = 16;
const int touch_3 = 17;
const int touch_4 = 18;
int data_touch_1;

/// Inputs LEDs
#include "Color.hpp"
const int led_ring_IN = 23;
const int NUMPIXELS = 12;
const int LEDBRIGHTNESS = 10; // Set LED Brightness
const int LED_QUARTAL = NUMPIXELS / 4;
const float MAX_BRIGHTNESS = 200.0;
int prev_LED_brightness[12];
// --- LED Palette Sleeping
//uint32_t color_sleeping_full = /*0x979AD3*/ 0x12345678 ;
//uint32_t color_sleeping_dimmed = /*0x555555*/ 0x01020304;
Color color_sleeping_full = Color(200,00,255,255);
Color color_sleeping_dimmed = Color(0,56,56,0);

// --- LED Palette Active
uint32_t color_blue = 0x80C9E0;
uint32_t color_mad = 0xFF0000;
uint32_t color_curious = 0x00FF00;
uint32_t currMood;
// --- LED Sleeping effect
Color baseColors[NUMPIXELS]; 
Color blendColors[NUMPIXELS];
Color randomBegin;
Color randomEnd;
uint8_t randomPhase = 0;
uint8_t randomSpeed = 0;
int currLedColors[NUMPIXELS];

int test = 0;


#include <Adafruit_NeoPixel.h>
//#ifdef __AVR__
//#include <avr/power.h> 
//#endif
Adafruit_NeoPixel pixels(NUMPIXELS, led_ring_IN, NEO_GRB + NEO_KHZ800);
//#define DELAYVAL 100 // Time (in milliseconds) to pause between pixels

/// MAIN Methods –––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––
void setup() {

  
  // initialize Relay-Module
  initRelay();

  // initialize NeoPixel-Ring
  initLED();

  // initialize Sensors
  initSensors();

}

void loop() {
  /* print colors
    Serial.println(test, HEX);
    Serial.println(randomBegin.getValue(),HEX);
    Serial.println(randomEnd.getValue(),HEX);
    Serial.println("----");
  */
  
  runSensors();
  setPumpsLow();

  if (data_touch_1 > 1500) {
    updateCurrColors();
    //Serial.println(data_touch_1);
    ledActive();
    activatePump(1);
    delay(300);

    /*
    uint8_t R = (currLedColors[1] >> 16); // read Red-value from hex
    uint8_t G = (currLedColors[1] >> 8); // read Green-value from hex
    uint8_t B = (currLedColors[1]); // read Blue-value from hex
    Serial.println("R: " + String(R) + "  G: " + String(G) + "  B: " + String(B));
    */
  } else {
     ledSleeping();
  }
}

/// RELAY Methods  –––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––
void initRelay() {
  pinMode(relay_Pin_OUT_1, OUTPUT);
  pinMode(relay_Pin_OUT_2, OUTPUT);
  pinMode(relay_Pin_OUT_3, OUTPUT);
  pinMode(relay_Pin_OUT_4, OUTPUT);

  pinMode(relay_Pin_IN_1, OUTPUT);
  pinMode(relay_Pin_IN_2, OUTPUT);
  pinMode(relay_Pin_IN_3, OUTPUT);
  pinMode(relay_Pin_IN_4, OUTPUT);
}

void setPumpsLow() {
  digitalWrite(relay_Pin_IN_1, LOW);  
  digitalWrite(relay_Pin_IN_2, LOW);  
  digitalWrite(relay_Pin_IN_3, LOW);  
  digitalWrite(relay_Pin_IN_4, LOW);  
}

void activatePump(int selectedPump) {
    switch (selectedPump) {
      case 1:
        digitalWrite(relay_Pin_IN_1, HIGH);
        break;
      case 2:
        digitalWrite(relay_Pin_IN_2, HIGH);
        break;
      case 3:
        digitalWrite(relay_Pin_IN_3, HIGH);
        break;
      case 4:
        digitalWrite(relay_Pin_IN_4, HIGH);
        break;  
    }
}

/// SENSOR Methods  –––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––
void initSensors() {
  // Serial läuft schon!
  //Serial.begin(57600); // Sensor
}

void runSensors() {
  data_touch_1 = touchRead(touch_1);
  //Serial.println(data_touch_1);
  delay(100);  
}

/// LED Methods –––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––
  // Reihefolge: pixels.clear() > pixels.setPixelColor(numberPixel, pixels.Color(r,g,b)) > pixels.show() > delay() falls benötigt
  
void initLED() {
    
  for (int i = 0; i < NUMPIXELS; i++) {
      prev_LED_brightness[i] = 0;
  }
  pixels.begin();
  pixels.clear();
  pixels.show();
  pixels.setBrightness(50);

  setRandomBlendColors(color_sleeping_full, color_sleeping_dimmed);
}

void ledActive() { // LED-Behaviour when sensor is touched on one side /*int touchQuart*/
  pixels.clear();
  pixels.show();
  
  //pixels.fill(0x220000,0,NUMPIXELS); // blend colors
  setMood(data_touch_1);
  pixels.fill(currMood,0,NUMPIXELS);
  

  pixels.show(); 
  delay(50);
}

void setMood(int data_sensor) {
  if (data_sensor > 1500 && data_sensor < 1800) {
      currMood = color_curious;
  } else if (data_sensor > 1800 && data_sensor < 2500) {
      currMood = color_mad;
  }
}

void updateCurrColors() {
  for (int i=0; i<NUMPIXELS; i++) {
    currLedColors[i] = pixels.getPixelColor(i);
  }
}

void testLED() {
  pixels.clear();
  pixels.show();
  
  pixels.fill(color_blue,0,NUMPIXELS);
  pixels.show(); 
  delay(1000);
}

void ledSleeping() { // LED-Behaviour when sensor is NOT touched
  pixels.clear();
  pixels.show();
  for (int i=0; i<NUMPIXELS; i++) {
    const Color result = baseColors[i].mix(blendColors[i], randomPhase);
    pixels.setPixelColor(i, result.getValue());
  }
  pixels.show();

  delay(50);
  const uint8_t oldPhase = randomPhase;
  randomPhase += randomSpeed;
  if (oldPhase > randomPhase) {
    for(uint8_t i = 0; i < NUMPIXELS; i++) {
      baseColors[i] = blendColors [i];
    }
    generateNewRandomBlend(); 
  }
  /* easy tests
  int i;
  float j;
  for (j = 1.0; j < MAX_BRIGHTNESS; j+=0.2) {
    for (i = 0; i < 12; i++) {
      prev_LED_brightness[i] = pixels.getPixelColor(i);
      pixels.setPixelColor(i,j,j,j);
    } 
      pixels.show();
      delay(5);
  } */
}

void disableNeoPixels()
{
    for (int i = 0; i < NUMPIXELS; ++i) {
        pixels.setPixelColor(i, 0);
    }    
    pixels.show();
}

void fillWithColor(const Color color)
{
    for (int i = 0; i < NUMPIXELS; ++i) {
        baseColors[i] = color;
    }
}

uint8_t getSimpleRandom()
{
    static uint16_t seed = 70;
    seed = 181 * seed + 359;
    return (uint8_t)(seed >> 8);
}

void generateNewRandomBlend()
{
    for (int i = 0; i <NUMPIXELS; ++i) {
      uint8_t weight = getSimpleRandom();
      blendColors[i] = randomBegin.mix(randomEnd, weight);
    }
    randomSpeed = (getSimpleRandom() >> 3) + 5;
}

void setRandomBlendColors(Color a, Color b)
{
    randomBegin = a;
    randomEnd = b;
    randomPhase = 0;
    randomSpeed = 16;
    fillWithColor(Color());
    generateNewRandomBlend(); 
    test = a.getValue();
}
