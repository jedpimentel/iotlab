marionette.ino requires an arduino_secrets.h header file

- create a "arduino_secrets.h" file inside "marionette" folder,
reference: https://www.arduino.cc/en/Guide/MKRWiFi1010/connecting-to-wifi-network

marionette/arduino_secrets.h should contain:
=== START arduino_secrets.h ===
#define SECRET_SSID "TheNetworkName"
#define SECRET_PASS "PasswordGoesHere"
char SECRET_IP[4] = {num1, num2, num3, num4};
=== END arduino_secrets.h ===

SECRET_IP is an array of integers (0-255) representing the device's local IP

2020017 UPDATE: WiFi.config(ip); (in marionette.ino) was using issues with network
connection, I assume since my router doesn't like clients to choose their own IP