#include "samples.h"
#include "boost.h"

#include <ADCTouchSensor.h>
#define USE_RTC

#ifdef USE_RTC
//#include <RTClock.h>
//#include <libmaple/rcc.h>
#include "safe_rtc.h"
#define SAFE_TIME_SEC (60*30-10)
#define CLOCK_MIN_SEC (10*60) // used to detect if clock lost power
#endif

#define DELAY 30000

int pins[] = {PA0,PA1,PA4,PA5,PA6,PA7,PA8,PA9};
const int numPins = sizeof(pins)/sizeof(*pins);
ADCTouchSensor* sensors[numPins];

#define LED PC13

uint32 t0;
bool stopped;
#ifdef USE_RTC
bool rtc;
#endif

void setup() {
  Serial.begin();
  //while(!Serial);
  pinMode(LED, OUTPUT);
  digitalWrite(LED, 0);
  setBoostFunction(boost12);
  initializePlayback();
  stopped = false;
  for (int i=0; i<numPins; i++) {
      sensors[i] = new ADCTouchSensor(pins[i]);
      sensors[i]->begin();
  }

#ifdef USE_RTC
  rtc = false;
  Serial.println("rtc initializing");
  if (!safe_rtc_start_lse()) {
    Serial.println("failed to start");
    return;
  }
  Serial.print("count ");
  uint32_t curT = safe_rtc_get_count(&rtc);
  if (!rtc)
    return;
  Serial.println(curT);
  if (curT >= CLOCK_MIN_SEC && curT <= CLOCK_MIN_SEC+SAFE_TIME_SEC) {
    playSample(Ready_u8,Ready_u8_len,1);
    terminatePlayback();
    stopped = true;
    for (int i=0;i<7;i++) {
      digitalWrite(LED,1);
      delay(50);
      digitalWrite(LED,0);
      delay(50);
    }    
  }
  else {
    safe_rtc_set_count(0);
  }
#endif    
}

bool checkForExit(int32 dt) {
  uint32 t0 = millis();
  do {
    for (int i=0; i<numPins; i++) {
      if (sensors[i]->read() > 100) {
        stopped = true;
        break;
      }
    }
  } while(dt > 0 && millis() - t0 < dt);
  if (stopped) {
    playSample(Canceled_u8, Canceled_u8_len, 1);
    terminatePlayback();
  }
  return stopped;
}

void loop() {
  if (stopped) {
    digitalWrite(LED, 1);
#ifdef USE_RTC    
    if (rtc) {
      safe_rtc_set_count(CLOCK_MIN_SEC);
      Serial.println("setting RTC count");
    }
#endif    
    delay(5000);
    return;
  }

  uint32_t t0 = millis();
  playSample(PleaseWait_u8,PleaseWait_u8_len,0);
  if (checkForExit(5000-(millis()-t0)))
    return;
  playSample(n25_u8,n25_u8_len,0);
  if (checkForExit(10000-(millis()-t0)))
    return;
  playSample(n20_u8,n20_u8_len,0);
  if (checkForExit(15000-(millis()-t0)))
    return;
  playSample(n15_u8,n15_u8_len,0);
  if (checkForExit(20000-(millis()-t0)))
    return;
  playSample(n10_u8,n10_u8_len,0);
  if (checkForExit(25000-(millis()-t0)))
    return;
  playSample(n5_u8,n5_u8_len,0);
  if (checkForExit(26000-(millis()-t0)))
    return;
  playSample(n4_u8,n4_u8_len,0);
  if (checkForExit(27000-(millis()-t0)))
    return;
  playSample(n3_u8,n3_u8_len,0);
  if (checkForExit(28000-(millis()-t0)))
    return;
  playSample(n2_u8,n2_u8_len,0);
  if (checkForExit(29000-(millis()-t0)))
    return;
  playSample(n1_u8,n1_u8_len,0);
  if (checkForExit(30000-(millis()-t0)))
    return;
  playSample(Ready_u8,Ready_u8_len,1);
  terminatePlayback();
  stopped = true;
}
