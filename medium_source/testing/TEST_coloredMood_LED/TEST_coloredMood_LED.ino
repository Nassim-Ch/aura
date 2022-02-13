/// Inputs Relay
// --- Air IN
const int relay_Pin_IN_4 = 0, relay_Pin_IN_3 = 1, relay_Pin_IN_2 = 2, relay_Pin_IN_1 = 4;
const int max_cooldowntime = 100;
int cooldowntime[4] = {max_cooldowntime, max_cooldowntime, max_cooldowntime, max_cooldowntime};
int activePumpTime[4] = {0, 0, 0, 0};
const int max_activePumpTime = 30;
bool pumpActive[4] = {false, false, false, false};

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

int moods[4] = {0, 0, 0, 0}; //0: sleepting, 1: curious, 2: aggressive
// --- LED Color Palette
const Color color_sleeping_full = Color(200, 00, 255, 150);
const Color color_sleeping_dimmed = Color(0, 56, 56, 50); //also used for curious
const Color color_active_aggressive_full = Color(255, 61, 61, 255);
const Color color_active_aggressive_dimmed = Color(227, 104, 170, 10);
const Color color_active_curious_full = Color(184,255,228, 200);
const Color color_singleArm = Color(200, 200, 200, 200);

// --- LED Sleeping effect
Color prevColor[4] = {color_sleeping_full, color_sleeping_full, color_sleeping_full, color_sleeping_full};
Color nextColor[4] = {color_sleeping_full, color_sleeping_full, color_sleeping_full, color_sleeping_full};
Color targetColor[4] = {color_sleeping_full, color_sleeping_full, color_sleeping_full, color_sleeping_full};
Color prevColorDimmed[4] = {color_sleeping_dimmed, color_sleeping_dimmed, color_sleeping_dimmed, color_sleeping_dimmed};
Color nextColorDimmed[4] = {color_sleeping_dimmed, color_sleeping_dimmed, color_sleeping_dimmed, color_sleeping_dimmed};
Color targetColorDimmed[4] = {color_sleeping_dimmed, color_sleeping_dimmed, color_sleeping_dimmed, color_sleeping_dimmed};

int mixerCounter[4] = {0, 0, 0, 0};
int max_mixerCounter = 5;
int max_moodDuration = 45;
int curr_moodDuration[4] = {max_moodDuration, max_moodDuration, max_moodDuration, max_moodDuration};
int max_pumpGap = 50;
int currGap[4] = {max_pumpGap,max_pumpGap,max_pumpGap,max_pumpGap};
Color baseColors[NUMPIXELS];
Color blendColors[NUMPIXELS];
Color randomBegin;
Color randomEnd;
uint8_t randomPhase = 0;
uint8_t randomSpeed = 0;

// --- Behaviour value
const int aggressiveValue = 2000;
const int min_curiousValue = 200, max_curiousValue = aggressiveValue;

int test = 0;
bool interaction_active = false;
int currTouch_1 = 0, currTouch_2 = 0, currTouch_3 = 0, currTouch_4 = 0;
uint8_t speedLedSleep = 16, speedLedAggressive = 64;
//int counterLed = 0;
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
  // initialize Relay-Module
  initRelay();

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
  setQuartal();
  checkPumps();
  calcTargetColor();
  //outputMoods();
  
  ledColorDisplay();
  //counterLed++;
  deductMoodDuration();
  delay(50);
}

void outputMoods()
{
  for (int i = 0; i < 4; i++)
  {
    Serial.println("mood " + String(i) + ": " + String(moods[i]) + " -- prev " + String(prevColor[i].getValue()) + " - next " + String(nextColor[i].getValue()) + " - target " + String(targetColor[i].getValue()));
  }
}

void deductMoodDuration() {
  for (int i = 0; i<4; i++) {
    if (curr_moodDuration[i] > 0) {
      curr_moodDuration[i]--;
    }
  }
}

/// RELAY Methods  –––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––
void initRelay() {
  pinMode(relay_Pin_IN_1, OUTPUT);
  pinMode(relay_Pin_IN_2, OUTPUT);
  pinMode(relay_Pin_IN_3, OUTPUT);
  pinMode(relay_Pin_IN_4, OUTPUT);
  setPumpsHigh();
}

void setPumpsHigh() {
  digitalWrite(relay_Pin_IN_1, HIGH);
  digitalWrite(relay_Pin_IN_2, HIGH);
  digitalWrite(relay_Pin_IN_3, HIGH);
  digitalWrite(relay_Pin_IN_4, HIGH);
}

void deactivatePump (int selectedPump) {
  switch (selectedPump) {
    case 0:
      digitalWrite(relay_Pin_IN_1, HIGH);
      break;
    case 1:
      digitalWrite(relay_Pin_IN_2, HIGH);
      break;
    case 2:
      digitalWrite(relay_Pin_IN_3, HIGH);
      break;
    case 3:
      digitalWrite(relay_Pin_IN_4, HIGH);
      break;
  }
  pumpActive[selectedPump] = false;
  cooldowntime[selectedPump] = 0;
  activePumpTime[selectedPump] = 0;
}

void activatePump(int selectedPump) {
  //Serial.println("Requested pump: " + String(selectedPump));

  if (cooldowntime[selectedPump] >= max_cooldowntime && activePumpTime[selectedPump] == 0) {
    //Serial.println("Im hereee!");

    switch (selectedPump) {
      case 0:
        digitalWrite(relay_Pin_IN_1, LOW);
        break;
      case 1:
        digitalWrite(relay_Pin_IN_2, LOW);
        break;
      case 2:
        digitalWrite(relay_Pin_IN_3, LOW);
        break;
      case 3:
        digitalWrite(relay_Pin_IN_4, LOW);
        break;
    }
    pumpActive[selectedPump] = true;
    activePumpTime[selectedPump] = 0;
  }
}

void activateAllPumps() {
  for (int i = 0; i <= 3; i++) {
    activatePump(i);
  }
}

void checkPumps() {
  for (int i = 0; i <= 3; i++) {
    if (pumpActive[i]) {
      activePumpTime[i]++;
      if (activePumpTime[i] > max_activePumpTime) {
        deactivatePump(i);
      }
    } else {
      if (cooldowntime[i] < max_cooldowntime) {
        cooldowntime[i]++;
      }
    }
  }
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

  if (data_touch_1 + data_touch_2 + data_touch_3 + data_touch_4 > max_curiousValue) {
    for (int i = 0; i < 4; i++)
    {
      if (moods[i] != 2 )
      {
        setNextColor(i, color_active_aggressive_full, color_active_aggressive_dimmed);
        moods[i] = 2;
        curr_moodDuration[i] = max_moodDuration;
        activateAllPumps();
      }
    }
  }
  else
  {
    int data[4] = {data_touch_1, data_touch_2, data_touch_3, data_touch_4};
    for (int i = 0; i < 4; i++)
    {
      if (data[i] > min_curiousValue) {
        if (moods[i] != 1 )
        {
          activatePump(i);
          
          setNextColor(i, color_active_curious_full, color_sleeping_dimmed);
          moods[i] = 1;
          curr_moodDuration[i] = max_moodDuration;
        }
      } else {
        if (moods[i] != 0 && curr_moodDuration[i] == 0)
        {
          setNextColor(i, color_sleeping_full, color_sleeping_dimmed);
          moods[i] = 0;
        }
      }
    }
  }
}

void setNextColor(int i, Color col1, Color col2) {
  //Serial.println("color " + String(i) + " changed to " + col1.getValue());
  nextColor[i] = col1;
  nextColorDimmed[i] = col2;
  mixerCounter[i] = 0;
}


void calcTargetColor()
{
  for (int i = 0; i < 4; i++) {
    if (mixerCounter[i] <= max_mixerCounter) {
      uint8_t weight = 255 * (float) mixerCounter[i] /  (float) max_mixerCounter;
      //uint8_t weight = 0x100;
      //targetColor[i] = nextColor[i];
      targetColor[i] = prevColor[i].mix(nextColor[i], weight);
      //Serial.println(String(mixerCounter[i]) + " / " + String(max_mixerCounter) + " = " + String(weight));
      targetColorDimmed[i] = prevColorDimmed[i].mix(nextColorDimmed[i], weight);
      mixerCounter[i]++;
    }
    else
    {
      prevColor[i] = nextColor[i];
    }
  }
}

/// LED Methods –––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––––

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

  generateNewRandomBlend();

  for (int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, blendColors[i].getValue());
  }

  pixels.show();
}

uint8_t getSimpleRandom()
{
  static uint16_t seed = 70;
  seed = 181 * seed + 359;
  return (uint8_t)(seed >> 8);
}

void generateNewRandomBlend()
{
  for (int area = 0; area < 4; area++) {
    for (int p = 0; p < 3; p++) {
      uint8_t weight = getSimpleRandom();
      weight = weight/2 + 256/4; //limit random range around center
      //blendColors[area * 3 + p] = targetColor[area]; //nur heller Zielwert
      //blendColors[area * 3 + p] = targetColor[area].mix(targetColorDimmed[area], weight); //Mischung aus hell und dunkel
      blendColors[area * 3 + p] = blendColors[area * 3 + p].mix(targetColor[area].mix(targetColorDimmed[area], weight),80); //Mischung aus hell, dunkel, vorheriger wert
    }
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
