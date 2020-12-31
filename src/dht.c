/*
 *  dht.c:
 *  read temperature and humidity from DHT11 or DHT22 sensor
 *
 * ncuster modified from http://www.uugear.com/portfolio/read-dht1122-temperature-humidity-sensor-from-raspberry-pi/
 * The difference here is that when I read, on occasion I'll get ridiculously high values (e.g. 200F),
 * so this will read TEMPERATURE_READS times, and take the median just as a super simple way to hopefully
 * remove some of the bad data, although I have seen where subsequent reads still read with too high
 * of a value but at least this will try to reduce that.
 *
 *  cc -Wall dht_sensor.c dht.c -o ../bin/dht -lwiringPi
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <wiringPi.h>
#include "dht_sensor.h"

#define TEMPERATURE_READS  3  // read the sensor this many times, then take the media temp
#define READ_ERROR_SLEEP 500  // time to sleep if we get a read error on the sensor

// output the sensor metric in prometheus format
void to_prometheus(char* label, float temperature, float humidity) {
    printf("dht_sensor_temperature{sensor=\"%s\", units=\"F\"} %.1f\ndht_sensor_humidity{sensor=\"%s\"}  %.1f\n", label, temperature, label, humidity);
}

int main( int argc, char** argv )
{
    if ( argc != 2 ) {
        fprintf(stderr, "Must specify a label for the prometheus output metric\n");
        return(1);
    }

    // TEMPERATURE_READS number of sensor reads.  [0] is the temp, [1] is the humidity
    float readings[TEMPERATURE_READS][2];

    for ( int i=0; i < TEMPERATURE_READS; ++i ) {
        int rc = 1;
        int k = 0;
        float temp = 0.0;
        float humidity = 0.0;

        while ( rc != 0 && k++ < 100 && temp < 120.0 ) {
            rc = read_dht_data(&temp, &humidity);

            if ( rc == 0 ) {
                fprintf(stderr, "We got a successful read of %.1f,%.1f\n", temp, humidity);
            } else {
                fprintf(stderr, "Got non-zero rc, sleeping\n");
                delay(READ_ERROR_SLEEP);
            }

        }

        readings[i][0] = temp;
        readings[i][1] = humidity;
    }

    /*
    for ( int i=0; i < TEMPERATURE_READS; ++i ) {
        printf("arrdump: %.1f,%.1f\n", readings[i][0], readings[i][1]);
    }
    */

    // we got our TEMPERATURE_READS done for temp and humdity, now take the median
    int median_idx = TEMPERATURE_READS / 2;

    //printf("%.1f,%.1f\n", readings[median_idx][0], readings[median_idx][1]);
    to_prometheus(argv[1], readings[median_idx][0], readings[median_idx][1]);

    return(0);
}
