#include <Arduino.h>
// #include <WS2812FX.h>
#include <Adafruit_NeoPixel.h>
#include <SimpleTimer.h>
#include "OneButton.h"

#define BUTTON01 D3
#define BUTTON02 D2
#define LED_COUNT 16
#define LED_PIN D1

#define AMPLULHETA 90000 // 90 milisegundos
// #define AMPLULHETA 5000

OneButton btn1(BUTTON01, true);
OneButton btn2(BUTTON02, true);

bool isInConfigMode = false;
bool isInAmplulheta = false;
int modeIndicator = 0; // 0 - Tabuleiro | 1 -  Pomodoro
int ampulhetaID; // ID do Timer da Ampulheta
unsigned int timerTimeout = 0;
unsigned int timerStart = 0;

// WS2812FX ws2812fx = WS2812FX(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel pixels(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
SimpleTimer ampulhetaTimer;

void ampulhetaLoop() {
  Serial.print("Uptime (s): "); Serial.println(millis() / 1000);

  size_t value = map(millis(),timerStart,timerTimeout,0,15);
  Serial.println(value);


  for (size_t i = 0; i < value; i++) {
    if(value > 14) {
      pixels.setPixelColor(i, pixels.Color(255,0,0));
    } else {
      pixels.setPixelColor(i, pixels.Color(0,255,0));
    }
  }

  pixels.show();

  // detecta o fim
  if(value >= 16) {
    ampulhetaTimer.disable(ampulhetaID);
    isInAmplulheta = false;
    delay(1000);
    digitalWrite(D5, HIGH);

    for (size_t i = 0; i < 12; i++) { // efeito
      pixels.clear();

      for (size_t u = 0; u < LED_COUNT; u++) {
        pixels.setPixelColor(u, pixels.Color(3, 252, 240));
      }
      pixels.show();
      delay(100);
      pixels.clear();
      pixels.show();
      delay(100);
    }

    pixels.clear();
    pixels.show();
    digitalWrite(D5, LOW);
  }
}

void click1() {
  Serial.println("Button 1 click.");

  if(isInConfigMode) {
    modeIndicator++;
    if(modeIndicator > 4) {
      modeIndicator = 0;
    }

    pixels.clear();
    pixels.setPixelColor(0,  pixels.Color(255,0,0));
    pixels.setPixelColor(4,  pixels.Color(255,0,0));
    pixels.setPixelColor(8,  pixels.Color(255,0,0));
    pixels.setPixelColor(12, pixels.Color(255,0,0));
    pixels.setPixelColor(15, pixels.Color(255,0,0));

    switch (modeIndicator) {
    case 0:
        pixels.setPixelColor(0,  pixels.Color(0,255,0));
      break;
    case 1:
        pixels.setPixelColor(4,  pixels.Color(0,255,0));
      break;
    case 2:
        pixels.setPixelColor(8,  pixels.Color(0,255,0));
      break;
    case 3:
        pixels.setPixelColor(12,  pixels.Color(0,255,0));
      break;
    case 4:
        pixels.setPixelColor(15,  pixels.Color(0,255,0));
      break;
    }

    pixels.show();
  }
}

void click2() {
  Serial.println("Button 2 click.");
  pixels.clear();
  pixels.show();

  if(isInAmplulheta) {
    ampulhetaTimer.disable(ampulhetaID);
    isInAmplulheta = false;
    digitalWrite(D5, HIGH); delay(85); digitalWrite(D5, LOW); delay(85);
    digitalWrite(D5, HIGH); delay(85); digitalWrite(D5, LOW); delay(85);
    digitalWrite(D5, HIGH); delay(85); digitalWrite(D5, LOW);


    for (size_t i = 0; i < LED_COUNT; i++) {
      pixels.setPixelColor(i, pixels.Color(0,255,0));
    }
    pixels.show();
    Serial.println("Deveria acabar aqui");
    return;
  }


  for (size_t i = 0; i < LED_COUNT; i++) {
    pixels.setPixelColor(i,pixels.Color(5,211,237));
  }
  pixels.show();
  delay(100);

  for (size_t i = LED_COUNT; i > 0; i--) {
    Serial.println(i);
    pixels.setPixelColor(i,pixels.Color(0,0,0));
    pixels.show();
    delay(35);
  }

  switch (modeIndicator)
  {
    case 0:
      timerTimeout = AMPLULHETA +  millis();
      isInAmplulheta = true;
      timerStart = millis();
      ampulhetaTimer.enable(ampulhetaID);
    break;

  default:
    break;
  }
}

void selectMode() {
  if(!isInConfigMode) {
    isInConfigMode = true;
    Serial.println("Em Modo de configuracao");
    pixels.clear();
    pixels.setPixelColor(0, pixels.Color(0,255,0));
    pixels.setPixelColor(4, pixels.Color(255,0,0));
    pixels.setPixelColor(8, pixels.Color(255,0,0));
    pixels.setPixelColor(12, pixels.Color(255,0,0));
    pixels.setPixelColor(15, pixels.Color(255,0,0));
    pixels.show();
  }
  else {
    Serial.println("Mostra effeito");
    isInConfigMode = false;
    pixels.clear();
    int ledEffect = 0;

    switch (modeIndicator) {
    case 0:
      ledEffect = 0;
      break;
    case 1:
        ledEffect = 4;
      break;
    case 2:
        ledEffect = 8;
      break;
    case 3:
        ledEffect = 12;
      break;
    case 4:
        ledEffect = 15;
      break;
    }

    for (size_t i = 0; i < 6; i++)
    {
      pixels.clear();
      pixels.setPixelColor(ledEffect, pixels.Color(0,255,0));
      pixels.show();
      delay(100);
      pixels.clear();
      pixels.show();
      delay(100);
    }
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(D5, OUTPUT);

  pixels.begin();
  pixels.clear();
  pixels.setBrightness(50);
  pixels.fill(pixels.Color(0,0,255));
  pixels.show();

  btn1.attachClick(click1);
  btn1.attachLongPressStop(selectMode);

  btn2.attachClick(click2);

  ampulhetaID = ampulhetaTimer.setInterval(1000, ampulhetaLoop);
  ampulhetaTimer.disable(ampulhetaID);
}

void loop() {
  btn1.tick();
  btn2.tick();
  ampulhetaTimer.run();
}