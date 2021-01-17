marionette.ino requires an arduino_secrets.h header file

- create a "arduino_secrets.h" file inside "marionette" folder,
reference: https://www.arduino.cc/en/Guide/MKRWiFi1010/connecting-to-wifi-network

marionette/arduino_secrets.h should contain:
=== START arduino_secrets.h ===
#define SECRET_SSID "TheNetworkName"
#define SECRET_PASS "PasswordGoesHere"
char SECRET_IP[4] = {num1, num2, num3, num4};
=== END arduino_secrets.h ===