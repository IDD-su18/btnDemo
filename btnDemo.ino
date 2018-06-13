/*
 * btnDemo
 * 
 * Uses 3 types of button input logic as a demonstration.
 * 
 * Released under the Apache 2.0 License
 * Copyright 2018 (c) Eldon Schoop
 */

#define LED_L A0
#define LED_M A3
#define LED_R A1
#define BTN_L 6
#define BTN_M 9
#define BTN_R 5

#define DEBOUNCE_THRESH 50

uint8_t ACTIVE = 0;

byte BTN_L_RD = HIGH;
byte BTN_L_LAST = HIGH;

byte BTN_M_RD = HIGH;
byte BTN_M_LAST = HIGH;
bool BTN_M_PRESSED = false;
int BTN_M_DEBOUNCE = 0;

int BTN_R_DEBOUNCE = 0;


void setup() {
  // init pins
  pinMode(BTN_L, INPUT_PULLUP);
  pinMode(BTN_M, INPUT_PULLUP);
  pinMode(BTN_R, INPUT_PULLUP);
  pinMode(LED_L, OUTPUT);
  pinMode(LED_M, OUTPUT);
  pinMode(LED_R, OUTPUT);

  // call handleBtnRPress() if BTN_R transitions to being pressed
  attachInterrupt(digitalPinToInterrupt(BTN_R), handleBtnRPress, FALLING);

  // leds default off
  digitalWrite(LED_L, LOW);
  digitalWrite(LED_M, LOW);
  digitalWrite(LED_R, LOW);

  Serial.begin(9600);
}

void loop() {
  
  // no debounce for left button
  BTN_L_RD = digitalRead(BTN_L);
  if (BTN_L_RD == LOW && BTN_L_LAST == HIGH) { // falling edge
    ACTIVE ^= 1 << 0; // toggle left button's led in global state
  }
  BTN_L_LAST = BTN_L_RD;

  // good debounce for mid button make sure button is held for DEBOUNCE_THRESH ms
  BTN_M_RD = digitalRead(BTN_M);
  if (BTN_M_RD == LOW) {  // button is being held
    if (BTN_M_LAST == HIGH) {  // has transitioned from high to low (falling edge)
      BTN_M_DEBOUNCE = millis();  // start timer
      BTN_M_PRESSED = false;  // button press not registered yet
    }
    // if the timer has expired and we haven't registered the press yet
    if (millis() - BTN_M_DEBOUNCE >= DEBOUNCE_THRESH && BTN_M_PRESSED == false) {
      ACTIVE ^= 1 << 1; // toggle button led
      BTN_M_PRESSED = true;  // led has been toggled, don't toggle it again
    }
  }
  BTN_M_LAST = BTN_M_RD;  

  // right button is handled via interrupt, no need to deal with it except for writing its state

  writeStatusToLEDs();
}

void handleBtnRPress() {
  // naive debounce: if button is pressed, ignore it for DEBOUNCE_THRES ms afterwards.
  if ((millis() - BTN_R_DEBOUNCE) >= DEBOUNCE_THRESH) {
    ACTIVE ^= 1 << 2;
    BTN_M_DEBOUNCE = millis();
  }
}

void writeStatusToLEDs() {
  digitalWrite(LED_L, 1 & ACTIVE >> 0);
  digitalWrite(LED_M, 1 & ACTIVE >> 1);
  digitalWrite(LED_R, 1 & ACTIVE >> 2);
}

