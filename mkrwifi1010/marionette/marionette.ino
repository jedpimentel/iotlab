#include "arduino_secrets.h"
#include <WiFiNINA.h>
#include <utility/wifi_drv.h>

#include <RTCZero.h>
#include <Servo.h>

#define motor_a_pwm 20
#define motor_a_pin_1 1
#define motor_a_pin_2 2

#define motor_b_pwm 21
#define motor_b_pin_1 3
#define motor_b_pin_2 4

#include "motor_controller.h"

void setup_motor_driver() {
  pinMode(motor_a_pwm, OUTPUT);
  pinMode(motor_a_pin_1, OUTPUT);
  pinMode(motor_a_pin_2, OUTPUT);
  pinMode(motor_b_pwm, OUTPUT);
  pinMode(motor_b_pin_1, OUTPUT);
  pinMode(motor_b_pin_2, OUTPUT);

  analogWrite(motor_a_pwm, 255);
  analogWrite(motor_b_pwm, 255);
}

void motor_driver_demo() {
  digitalWrite(motor_a_pin_1, HIGH);
  digitalWrite(motor_a_pin_2, LOW);
  digitalWrite(motor_b_pin_1, HIGH);
  digitalWrite(motor_b_pin_2, LOW);
  delay(1000);
  digitalWrite(motor_a_pin_1, LOW);
  digitalWrite(motor_a_pin_2, HIGH);
  digitalWrite(motor_b_pin_1, LOW);
  digitalWrite(motor_b_pin_2, HIGH);
  delay(1000);
  digitalWrite(motor_a_pin_1, LOW);
  digitalWrite(motor_a_pin_2, LOW);
  digitalWrite(motor_b_pin_1, LOW);
  digitalWrite(motor_b_pin_2, LOW);
  
}

void setup_rgb() {
  WiFiDrv::pinMode(25, OUTPUT);
  WiFiDrv::pinMode(26, OUTPUT);
  WiFiDrv::pinMode(27, OUTPUT);
}
int yo = 0;
int colors[7][3] = {
  {127, 0, 127},
  {127, 0,0},
  {64, 127,0},
  {0, 127, 0},
  {0, 127, 64},
  {0, 0, 255},
  {127, 127, 127},
};
void change_rgb() {
//  int yo = 1;
  WiFiDrv::analogWrite(26, colors[yo][0]); //Red
  WiFiDrv::analogWrite(25, colors[yo][1]); //Green
  WiFiDrv::analogWrite(27, colors[yo][2]); //Blue
  yo++;
  yo %= sizeof(colors) / sizeof(colors[0]);
}

// TODO: create a Debug.print function instead of Serial.print directly
// debug mode prints stuff via Serial, "while(!Serial)" is an infinite loop if !Serial
#define SERIAL_DEBUG_MODE false  // headless mode shouldn't have Serial calls 

const char separator[] = "----------------"; 
const char ssid[] = SECRET_SSID;  // network name string
const char pass[] = SECRET_PASS;  // network password string
int status = WL_IDLE_STATUS;  // WiFi radio status
WiFiServer server(80);
WiFiClient client = server.available();

RTCZero rtc;
const int time_zone = -5;  // TODO: move into a config file

void setup() {
  setup_motor_driver();
  setup_rgb();

  pinMode(LED_BUILTIN, OUTPUT);

  #if SERIAL_DEBUG_MODE
    Serial.begin(9600);
    while (!Serial);
  #endif

  init_network_connection();
  init_rtc_with_network_time();
}

// toggle led
void loop() {
  change_rgb();
  motor_driver_demo();
  toggle(true);
  delay(500);
  toggle(false);
  delay(500);
}

void toggle(bool on) {
  if (on) {
    digitalWrite(LED_BUILTIN, HIGH);
  }
  else {
    digitalWrite(LED_BUILTIN, LOW);
  }
}

void init_network_connection() {
  listNetworks();
  while (status != WL_CONNECTED) {
    #if SERIAL_DEBUG_MODE
      Serial.print("Attempting connection to WiFi: ");
      Serial.println(ssid);
    #endif

    status = WiFi.begin(ssid, pass);
    for (int ms = 10000; status != WL_CONNECTED && ms > 0; ms--) {
      delay(1);
    }
  }
  #if SERIAL_DEBUG_MODE
    Serial.print("Startup OK!");
    Serial.println("Device is connected to network");
    Serial.println(separator);
    printData();
  #endif
}


void init_rtc_with_network_time() {
  rtc.begin();
  
  unsigned long epoch;
  int attempts = 0, maxAttempts = 6;
  #if SERIAL_DEBUG_MODE
    Serial.println("Getting Network Time");
  #endif
  do {
    epoch = WiFi.getTime();
    attempts++;
    if (epoch == 0) {
      delay(2000);
    }
  }
  while (epoch == 0 && attempts < maxAttempts);
  
  if (attempts == maxAttempts) {
    #if SERIAL_DEBUG_MODE
      Serial.print("Network Time Protocol unreachable, attempted");
      Serial.print(maxAttempts);
      Serial.println("times");
      Serial.println("device will now give up initializing");
      Serial.println("firmware will someday handle this case better");
    #endif
    // TODO: better way to handle failure
    while(1);
  }
  #if SERIAL_DEBUG_MODE
    Serial.println("Epoch received:");
    Serial.println(epoch);
    Serial.println();
    printDateTime();
    Serial.println();
  #endif

  rtc.setEpoch(epoch + time_zone * 60 * 60);
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
