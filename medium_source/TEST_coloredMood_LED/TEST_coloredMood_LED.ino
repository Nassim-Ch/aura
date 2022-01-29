/// Inputs TouchSensor
const int touch_1 = 15, touch_2 = 16, touch_3 = 17, touch_4 = 18;
int data_touch_1, data_touch_2, data_touch_3, data_touch_4;

/// Inputs LEDs
#include "Color.hpp"
const int led_ring_IN = 23;
const int NUMPIXELS = 12;
const int LEDBRIGHTNESS = 10; // Set LED Brightness
const int LED_QUARTAL = NUMPIXELS / 4;
const float MAX_BRIGHTNESS = 200.0;

// --- LED Color Palette
const Color color_sleeping_full = Color(200,00,255,255);
const Color color_sleeping_dimmed = Color(0,56,56,0);
const Color color_active_aggressive_full = Color(255, 61, 61, 255);
const Color color_active_aggressive_dimmed = Color(227, 104, 170, 10);
const Color color_active_curious_full = Color(196, 255, 119, 150);
const Color color_singleArm = Color(200, 200, 200, 200);

// --- LED Sleeping effect
Color baseColors[NUMPIXELS]; 
Color blendColors[NUMPIXELS];
Color randomBegin;
Color randomEnd;
uint8_t randomPhase = 0;
uint8_t randomSpeed = 0;

// --- Behaviour value
const int aggressiveValue = 1500;
const int min_curiousValue = 600, max_curiousValue = aggressiveValue;

int test = 0;
bool interaction_active = false;
int currTouch_1 = 0, currTouch_2 = 0, currTouch_3 = 0, currTouch_4 = 0;
uint8_t speedLedSleep = 16, speedLedAggressive = 64;
int counterLed = 0;
int allowInteractionTime = 20;
int addTouch_1, addTouch_2, addTouch_3, addTouch_4;
int beginQuartal = 99, endQuartal = 99;


#include <Adafruit_NeoPixel.h>
//#ifdef __AVR__
//#include <avr/power.h> 
//#endif
Adafruit_NeoPixel pixels(NUMPIXELS, led_ring_IN, NEO_GRB + NEO_KHZ800);
//#define DELAYVAL 100 // Time (in milliseconds) to pause between pixels

/// MAIN Methods –––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––
void setup() {

  // initialize NeoPixel-Ring
  initLED();

  // All Graphs have the same height
  normalizeGraph();
}

void loop() {
  /* print colors
    Serial.println(test, HEX);
    Serial.println(randomBegin.getValue(),HEX);
    Serial.println(randomEnd.getValue(),HEX);
    Serial.println("----");
  */
  
  runSensors();

  /* Aggressive Behaviour > turns red */
  if (data_touch_1+data_touch_2+data_touch_3+data_touch_4 > aggressiveValue && counterLed > allowInteractionTime) { 
    if (!interaction_active) {
      setQuartal();
      setRandomBlendColors(color_active_aggressive_full, color_active_aggressive_dimmed, speedLedAggressive);
      interaction_active = true;
      counterLed = 0;
    } else {
      interaction_active = false;  
    }
  /* Curious Behaviour > turns partially white and completely green */
  } else if (((data_touch_1 < max_curiousValue && data_touch_1 > min_curiousValue) || (data_touch_2 < max_curiousValue && data_touch_2 > min_curiousValue) || (data_touch_3 < max_curiousValue && data_touch_3 > min_curiousValue) || (data_touch_4 < max_curiousValue && data_touch_4 > min_curiousValue)) && counterLed > allowInteractionTime) {
    if (!interaction_active) {
      setQuartal();
      setRandomBlendColors(color_active_curious_full, color_sleeping_dimmed, speedLedAggressive);
      interaction_active = true;
      counterLed = 0;
    } else {
      interaction_active = false;  
    }
  /* Sleeping Behaviour > blue/pruple colors */
  } else if (counterLed > allowInteractionTime) { 
    if (!interaction_active) {
      setQuartal();
      setRandomBlendColors(color_sleeping_full, color_sleeping_dimmed, speedLedSleep);
      interaction_active = true;
      counterLed = 0;
    } else {
      interaction_active = false;  
    }
  }
  
  ledColorDisplay();
  counterLed++;
  delay(50);
}

/// SENSOR Methods  –––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––

void runSensors() {
  data_touch_1 = touchRead(touch_1) + addTouch_1;
  data_touch_2 = touchRead(touch_2) + addTouch_2;
  data_touch_3 = touchRead(touch_3) + addTouch_3;
  data_touch_4 = touchRead(touch_4) + addTouch_4;
  
  Serial.print(data_touch_1);
  Serial.print(" ");
  Serial.print(data_touch_2);
  Serial.print(" ");
  Serial.print(data_touch_3);
  Serial.print(" ");
  Serial.println(data_touch_4);
}

/* All capacity sensors start at the same x-value */
void normalizeGraph () {
  int avgTouch = 0;
  data_touch_1 = touchRead(touch_1);
  data_touch_2 = touchRead(touch_2);
  data_touch_3 = touchRead(touch_3);
  data_touch_4 = touchRead(touch_4);
  
  for (int i = 0; i > 5; i++) {
    Serial.println("Initialising...");
    avgTouch = (avgTouch + data_touch_1 + data_touch_2 + data_touch_3 + data_touch_4) / 5;
    delay(1000);
  }

  addTouch_1 = avgTouch - data_touch_1;
  addTouch_2 = avgTouch - data_touch_2;
  addTouch_3 = avgTouch - data_touch_3;
  addTouch_4 = avgTouch - data_touch_4;
}

/* if one sensor is touched slightly, the correct LEDs are set */
void setQuartal() {
  if (data_touch_1 < max_curiousValue && data_touch_1 > min_curiousValue) {
    beginQuartal = 0;
    endQuartal = 2;
  } else if (data_touch_2 < max_curiousValue && data_touch_2 > min_curiousValue) {
    beginQuartal = 3;
    endQuartal = 5;
  } else if (data_touch_3 < max_curiousValue && data_touch_3 > min_curiousValue) {
    beginQuartal = 6;
    endQuartal = 8;
  } else if (data_touch_4 < max_curiousValue && data_touch_4 > min_curiousValue) {
    beginQuartal = 9;
    endQuartal = 11;
  } else {
    beginQuartal = 99;
    endQuartal = 99;
  }
}

/// LED Methods –––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––
  // Reihefolge: pixels.clear() > pixels.setPixelColor(numberPixel, pixels.Color(r,g,b)) > pixels.show() > delay() falls benötigt
  
void initLED() {
  pixels.begin();
  pixels.clear();
  pixels.show();
  pixels.setBrightness(50);

  setRandomBlendColors(color_sleeping_full, color_sleeping_dimmed, speedLedSleep);
}

void ledColorDisplay() { 
  pixels.clear();
  pixels.show();
  if (beginQuartal > 15) { // if the whole ring should change color
      for (int i=0; i<NUMPIXELS; i++) {
        const Color result = baseColors[i].mix(blendColors[i], randomPhase);
        pixels.setPixelColor(i, result.getValue());
      }
  } else { // if one sensor is touched, 3 LEDs should have a different color
    for (int i=0; i<NUMPIXELS; i++) {
      Color result;
      if (i >= beginQuartal && i <= endQuartal) {
        result = color_singleArm.mix(blendColors[i], randomPhase);
      } else {
        result = baseColors[i].mix(blendColors[i], randomPhase);
      }
      pixels.setPixelColor(i, result.getValue());
    } 
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

void setRandomBlendColors(Color a, Color b, uint8_t speedLED)
{
    randomBegin = a;
    randomEnd = b;
    randomPhase = 0;
    randomSpeed = speedLED;
    //fillWithColor(Color());
    generateNewRandomBlend(); 
    test = a.getValue();
}

/// Testing NeoPixel Methods –––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––

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
