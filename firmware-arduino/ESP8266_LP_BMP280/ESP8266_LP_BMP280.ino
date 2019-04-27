#define DEBUG_SERIAL
#ifdef DEBUG_SERIAL
#define DBG_PRINT(x)    Serial.print(x)
#define DBG_PRINTLN(x)  Serial.println(x)
#define BLYNK_PRINT Serial
#else
#define DBG_PRINT(x)
#define DBG_PRINTLN(x)
#endif

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

#include <Wire.h>
#include "BMP280.h"
BMP280 bmp;

#define RTC_VALID_DATA  0xC4
typedef struct rtc {
  uint8_t dataValidation;
  int32_t messageID;
};
rtc rtcMem;

ADC_MODE(ADC_VCC);

// CONFIG PART
// modify the below to set parameters

#define DEEPSLEEP_INTERVAL    60    //in minutes
#define MSGID_VWIRE           V0
#define BATT_VWIRE            V1
#define TEMP_VWIRE            V2
#define HUM_VWIRE             V3
#define PRES_VWIRE            V4

char* ssid = "";
char* pass = "";
char* auth = "";

void setup() 
{
  gpio_init();
  DBG_PRINTLN("");
  DBG_PRINTLN("Device started");

  if (bmp.begin(5, 4))
    {
      DBG_PRINTLN("BMP/BME sensor successfully initialized");
      bmp.setOversampling(4);
    }
    else
    {
      DBG_PRINTLN("BMP/BME sensor not found!");
      deepSleep();
    }

  DBG_PRINT("Battery voltage: ");
  float Vbat = ESP.getVcc() / 1000.0f;
  DBG_PRINT(Vbat);
  DBG_PRINTLN(" V");

  DBG_PRINT("Reading RTC Memory...");
  ESP.rtcUserMemoryRead(0, (uint32_t*)&rtcMem, sizeof(rtcMem));
  if (rtcMem.dataValidation == RTC_VALID_DATA)
  {
    DBG_PRINT("Found valid message ID: ");
    DBG_PRINTLN(rtcMem.messageID);
  }
  else
  {
    DBG_PRINTLN("No valid data in RTC found, message ID set to 0");
    rtcMem.messageID = 0;
  }
  rtcMem.dataValidation = RTC_VALID_DATA;

  if (startMeasurement())
    DBG_PRINTLN("Reading data from sensor...");
  else
  {
    DBG_PRINTLN("Error reading from sensor...");
    deepSleep();
  }

  DBG_PRINTLN("Configuring Blynk...");
  WiFi.forceSleepWake();
  delay(1);
  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);
  //WiFi.config(ip, gateway, subnet);
  Blynk.begin(auth, ssid, pass);
  if (Blynk.connected())
    led_green();
  else
    led_red();
  
  Blynk.virtualWrite(MSGID_VWIRE, rtcMem.messageID++);
  Blynk.virtualWrite(BATT_VWIRE, Vbat);
  Blynk.virtualWrite(TEMP_VWIRE, getTemperature());
  //Blynk.virtualWrite(HUM_VWIRE, getHumidity());
  Blynk.virtualWrite(PRES_VWIRE, getPressure());
  
  Blynk.run();

  ESP.rtcUserMemoryWrite(0, (uint32_t*) &rtcMem, sizeof(rtcMem));
  DBG_PRINTLN("RTC data saved.");

  delay(300);
  led_none();

  deepSleep();
}

void loop() {}

void deepSleep()
{
  led_none();
  DBG_PRINT("Going to deep sleep");
  ESP.deepSleep(DEEPSLEEP_INTERVAL * 60 * 1e6);
  while (1)
  {
    DBG_PRINT(".");
    delay(100);
  }
}
