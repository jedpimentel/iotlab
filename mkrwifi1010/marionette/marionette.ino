#include "arduino_secrets.h"
#include <WiFiNINA.h>
#include <RTCZero.h>

//IPAddress ip(SECRET_IP[0], SECRET_IP[1], SECRET_IP[2], SECRET_IP[3]);

const char separator[] = "----------------"; 
const char ssid[] = SECRET_SSID;//network name string
const char pass[] = SECRET_PASS;//network password string
int status = WL_IDLE_STATUS;//WiFi radio status
WiFiServer server(80);
WiFiClient client = server.available();

RTCZero rtc;
const int timeZone = -5;//TODO: move into a config file

void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  // initialize serial and wait for port to opwn
  Serial.begin(9600);
  while (!Serial);

//  listNetworks();

//  attempt to connect to wifi network:
//  WiFi.config(ip);
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to network: ");
    Serial.println(ssid);
//    Serial.println(pass);
//    connect to wpa/wpa2 network:
    status = WiFi.begin(ssid, pass);

//    wait 10-ish seconds for connection
//    seems excesive to me but okay
//    delay(10000);
    for (int mSec = 10000; status != WL_CONNECTED && mSec > 0; mSec--) {
      delay(1);
    }
  }

  Serial.println("Device is connected to network");
  Serial.println(separator);
  printData();
  Serial.println(separator);
//  server.begin();

  
  //init real time clock and give it the correct time
  //depends on network time protocol to be reachable via WiFiNINA
  {
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
      Serial.println("reset device to try again");
      Serial.println("device will now stop");
      while(1);//TODO: better way to handle failure
    }
    else {
      Serial.println("Epoch received:");
      Serial.println(epoch);
      rtc.setEpoch(epoch + timeZone * 60 * 60);
      Serial.println();
    }

    printDateTime();
    Serial.println();
  }

  Serial.println("Initialization Good");
}

void loop() {
  delay(10000);
//  printData();
//  Serial.println(separator);
}

void printData() {
  Serial.println("Board Information:");
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  Serial.println();
  Serial.println("Network Information:");
  Serial.print("SSID:");
  Serial.println(WiFi.SSID());

  //print the received signal strength
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.println(rssi);
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
//    Serial.print("\tEncryption: ");
//    printEncryptionType(WiFi.encryptionType(thisNet));
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
//  print2digits(rtc.getHours() + timeZone);
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
