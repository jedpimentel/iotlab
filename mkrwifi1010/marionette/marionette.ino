#include "arduino_secrets.h"
#include <WiFiNINA.h>

IPAddress ip(SECRET_IP[0], SECRET_IP[1], SECRET_IP[2], SECRET_IP[3]);

const char separator[] = "----------------"; 
const char ssid[] = SECRET_SSID;//network name string
const char pass[] = SECRET_PASS;//network password string
int status = WL_IDLE_STATUS;//WiFi radio status
WiFiServer server(80);

WiFiClient client = server.available();

/* I'm starting off referencing the MKRWiFi1010's 
 * "connectingto a wifi network" tutorial as a starting
 * point
  */
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  // initialize serial and wait for port to opwn
  Serial.begin(9600);
  while (!Serial);

  listNetworks();

  //attempt to connect to wifi network:
  WiFi.config(ip);
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to network:");
    Serial.println(ssid);
    Serial.println(pass);
    //connect to wpa/wpa2 network:
    status = WiFi.begin(ssid, pass);

    //wait 10 seconds for connection
    //seems excesive to me but okay
    delay(10000);
  }

  Serial.println("Device is connected to network");
  Serial.println(separator);
  printData();
  Serial.println(separator);

  server.begin();
}

void loop() {
  //check the network connection once every 10 seonds:
  delay(10000);
  printData();
  Serial.println(separator);
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
