#ifndef ULTRASONIC_H
#define ULTRASONIC_H

// This code triggers 3 Ultrasonic sensors at once (their trigger is connected
// together), waits for the interrupts to happen for the Echo of each US, and/or
// stops after the timeout value. The distance measured by each sensor is then displayed
// with a 0 for a timed out sensor.

// Number of interrupts
# define numInterrupts 3

// Speed of sound in meters/sec
float soundSpeed = 343.0;

// Time in microsecs of flight for the farthest possible wall, at 81" :
float timeout = ( 81.0 * 2.0 / ((float)soundSpeed * 39.3701) ) * 1000000.0;

// used to calculate and store distanceToObjects to objects, in cm, for each sensor
float distanceToObject[numInterrupts];


#endif ULTRASONIC_H
