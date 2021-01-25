#iotlab

Requires Arduino mkrwifi1010

Currently, Marionette connects to a local wifi, and gets the network time.

Roadmap:
* Option to run script without a Serial connection
* Handle situations where network isn't available on startup
* Wake/Sleep cycle

Bug Notes:
* there's no daylight savings logic
* does not have a fallback in case WiFi isn't available
* does not have a fallback in case NTP is unreachable
* listNetworks() hangs indefinitely if there's no connection