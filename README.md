# dht-exporter
Prometheus exporter for exposing DHTxx (e.g. DHT11) temperatures

## Notes
This is a kludge of some C code and netcat to provide a prometheus exporter for DHT-11 or DHT-22 temperature/humidity sensors.  In hindsight using the prometheus C library would have been easier, but this works as a quick and dirty hack.

## Build
- You'll need the wiringPi dev package/library installed
- See dht.c for the compiler build and link

## Run
bin/dht_exporter.sh <label name> <port>
