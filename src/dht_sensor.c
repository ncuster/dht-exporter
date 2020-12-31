/*
 *  dht_sensor.c:
 *  This does the sensor reading logic
 *
 * ncuster modified from http://www.uugear.com/portfolio/read-dht1122-temperature-humidity-sensor-from-raspberry-pi/
 *
 *  cc -Wall dht_sensor.c dht.c -o ../bin/dht -lwiringPi
 */

#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define MAX_TIMINGS 85
#define DHT_PIN     7

int data[5] = { 0, 0, 0, 0, 0 };
int piSetup = 0;

int read_dht_data( float *temp, float *humidity )
{
    uint8_t laststate   = HIGH;
    uint8_t counter     = 0;
    uint8_t j           = 0, i;
    data[0] = data[1] = data[2] = data[3] = data[4] = 0;

    if ( piSetup == 0 && wiringPiSetup() == -1 )
        exit(1);

    pinMode(DHT_PIN, OUTPUT);
    digitalWrite(DHT_PIN, LOW);
    delay(20); // set the pin for 20ms

    /* prepare to read the pin */
    pinMode( DHT_PIN, INPUT );

    /* detect change and read data */
    for ( i = 0; i < MAX_TIMINGS; i++ )
    {
        counter = 0;
        while ( digitalRead( DHT_PIN ) == laststate ) {
            counter++;
            delayMicroseconds(1);
            if ( counter == 255 ) {
                break;
            }
        }

        laststate = digitalRead(DHT_PIN);

        if ( counter == 255 ) {
            break;
        }

        /* ignore first 3 transitions */
        if ( (i >= 4) && (i % 2 == 0) )
        {
            /* shove each bit into the storage bytes */
            data[j / 8] <<= 1;
            if ( counter > 16 )
                data[j / 8] |= 1;
            j++;
        }
    }

    /*
     * check we read 40 bits (8bit x 5 ) + verify checksum in the last byte
     * print it out if data is good
     */
    if ( (j >= 40) &&
         (data[4] == ( (data[0] + data[1] + data[2] + data[3]) & 0xFF) ) )
    {
        float h = (float)((data[0] << 8) + data[1]) / 10;
        if ( h > 100 )
        {
            h = data[0];    // for DHT11
        }
        float c = (float)(((data[2] & 0x7F) << 8) + data[3]) / 10;
        if ( c > 125 )
        {
            c = data[2];    // for DHT11
        }
        if ( data[2] & 0x80 )
        {
            c = -c;
        }
        float f = c * 1.8f + 32;
        //printf( "Humidity = %.1f %% Temperature = %.1f *C (%.1f *F)\n", h, c, f );
        //printf( "%.1f,%.1f\n", f, h );
        *temp = f;
        *humidity = h;
        return 0;
    } else  {  // data read was bad
        return 1;
    }
}

