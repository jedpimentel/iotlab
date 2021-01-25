#include "arduino_secrets.h"
#include <WiFiNINA.h>
#include <RTCZero.h>

const char separator[] = "----------------"; 
const char ssid[] = SECRET_SSID;//network name string
const char pass[] = SECRET_PASS;//network password string
int status = WL_IDLE_STATUS;//WiFi radio status
WiFiServer server(80);
WiFiClient client = server.available();

RTCZero rtc;
const int time_zone = -5;//TODO: move into a config file

void setup() {
  Serial.begin(9600);
  while (!Serial);// comment this out for headless mode?

  init_network_connection();
  // server.begin();
  init_rtc_with_network_time();
  Serial.print("Startup OK!");

  pinMode(LED_BUILTIN, OUTPUT);
}

// toggle led
void loop() {
  digitalWrite(LED_BUILTIN, HIGH);
  delay(3000);
  digitalWrite(LED_BUILTIN, LOW);
  delay(5000);
}

void init_network_connection() {
  // listNetworks();
  while (status != WL_CONNECTED) {
    Serial.print("Attempting connection to WiFi: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, pass);

    for (int ms = 10000; status != WL_CONNECTED && ms > 0; ms--) {
      delay(1);
    }
  }

  Serial.println("Device is connected to network");
  Serial.println(separator);
  printData();
}


void init_rtc_with_network_time() {
  rtc.begin();
  
  unsigned long epoch;
  int attempts = 0, maxAttempts = 6;
  Serial.print("Getting Network Time");
  do {
    epoch = WiFi.getTime();
    Serial.print(".");
    attempts++;
    if (epoch == 0) {
      delay(2000);
    }
  }
  while (epoch == 0 && attempts < maxAttempts);
  Serial.println();
  
  if (attempts == maxAttempts) {
    Serial.println("Network Time Protocol unreachable :(");
    Serial.println("device will now give up initializing");
    Serial.println("firmware will someday handle this case better");
    while(1);//TODO: better way to handle failure
  }
  else {
    Serial.println("Epoch received:");
    Serial.println(epoch);
    rtc.setEpoch(epoch + time_zone * 60 * 60);
    Serial.println();
  }

  printDateTime();
  Serial.println();
}

void printData() {
  Serial.println("Connection Information:");
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  Serial.println();
  Serial.println("Network Information:");
  Serial.print("SSID:");
  Serial.println(WiFi.SSID());

  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.println(rssi);

  Serial.println(separator);
}

// https://www.arduino.cc/en/Reference/WiFiNINAScanNetworks
void listNetworks() {
  // scan for nearby networks:
  Serial.println("** Scan Networks **");
  int numSsid = WiFi.scanNetworks();
  if (numSsid == -1) {
    Serial.println("Couldn't get a WiFi connection");
    while (true);
  }

  // print the list of networks seen:
  Serial.print("number of available networks:");
  Serial.println(numSsid);

  // print the network number and name for each network found:
  for (int thisNet = 0; thisNet < numSsid; thisNet++) {
    Serial.print(thisNet);
    Serial.print(") ");
    Serial.print(WiFi.SSID(thisNet));
    Serial.print("\tSignal: ");
    Serial.print(WiFi.RSSI(thisNet));
    Serial.println(" dBm");
  }
}

//https://en.wikipedia.org/wiki/ISO_8601
void printDateTime() {
  printDate();
  Serial.print("L");
  printTime();
}
void printTime() {
//  the "print2digits(rtc.getHours() + GMT);" line in reference feels 
// like it might break near beggining or end of day
// I'm going against reference, and instead implementing the time offset clockside
//https://www.arduino.cc/en/Tutorial/WiFiRTC
//  print2digits(rtc.getHours() + time_zone);
  print2digits(rtc.getHours());
  Serial.print(":");
  print2digits(rtc.getMinutes());
  Serial.print(":");
  print2digits(rtc.getSeconds());
}

void printDate() {
  Serial.print(rtc.getYear());
  Serial.print("-");
  print2digits(rtc.getMonth());
  Serial.print("-");
  print2digits(rtc.getDay());
}

void print2digits(int number) {
  if (number < 10) {
    Serial.print("0");
  }
  Serial.print(number);
}
