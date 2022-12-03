#ifndef PHOTOTRANS_H
#define PHOTOTRANS_H

#include "utility.h"

// Volatile used to capture the period of the measured signal
volatile int period;

// Variables to measure the first pulse, and one whole period with the AC
float signal_pulse;

int max_phototrans_readings = 5;

int num_pt_readings = 10;
int microseconds_between_pt_readings = 5;

void phototrans_setup() {
  // Program TCB for frequency measurements
  
  /* Input Capture Frequency measurement mode*/
  TCB0.CTRLB = TCB_CNTMODE_FRQ_gc;
  
  /* Event Input Enable: enabled */
  TCB0.EVCTRL = 0x41; // that's 1000001: Enable Capture Event, Enable Filter

  TCB0.CTRLA = TCB_CLKSEL_CLKDIV1_gc /* CLK_PER/1 (From Prescaler) */
               | TCB_ENABLE_bm /* Enable: enabled */
               | TCB_RUNSTDBY_bm; /* Run Standby: enabled */
  sei();
}

float left_pt() {

  float reading;
  StackArray<float> readings;
  
  // MEASURE FOR LEFT PT

  for(int i = 0; i < max_phototrans_readings; i++) {
    cli(); // Disable interrupts
    TCB0.CTRLA &= ~TCB_ENABLE_bm; // Disable TCB
  
    // Set pin A2 (PD1) to use with TCB for frequency measurement (Right PT)
    EVSYS.CHANNEL2 = EVSYS_GENERATOR_PORT1_PIN1_gc; // Route pin A2, PD1
    EVSYS.USERTCB0 = EVSYS_CHANNEL_CHANNEL2_gc; // to TCB0
  
    TCB0.CTRLA |= TCB_ENABLE_bm;      // Enable TCB
    
    sei();                            // Enable interrupts
  
    delayMicroseconds(1000);
    
    if (TCB0.INTFLAGS) // TCB0 set the flag, indicating that it measured the signal's period
    {
      cli(); // Disable interrupts while we calculate/display the frequency.
      
      TCB0.INTFLAGS = 1; // Clear the TCB flag
      
      signal_pulse = TCB0.CCMP; // CCMP is the time between a rising edge and the next falling
  
      reading = 16000000.0 / signal_pulse;
  
      sei(); // Re-enable interrupts
  
      TCB0.INTFLAGS = 1; // This clears the TCB flag for the next PT

      readings.push(reading);
    }
  }

  if(readings.isEmpty()) return -1;

  double sum = 0;
  double num_readings = 0;

  while(!readings.isEmpty()) {
    sum += readings.pop();
    num_readings++;
  }

  return sum / num_readings;
}

float front_pt() {

  float reading;
  StackArray<float> readings;

  // MEASURE FOR FRONT PT

  for(int i = 0; i < max_phototrans_readings; i++) {
    cli(); // Disable interrupts
    TCB0.CTRLA &= ~TCB_ENABLE_bm; // Disable TCB
  
    // A4 (PA2): Front PT
    EVSYS.CHANNEL0 = EVSYS_GENERATOR_PORT0_PIN2_gc; // Route pin A4, PA2
    EVSYS.USERTCB0 = EVSYS_CHANNEL_CHANNEL0_gc; // to TCB0
  
    TCB0.CTRLA |= TCB_ENABLE_bm;      // Enable TCB
    
    sei();                            // Enable interrupts
  
    delayMicroseconds(1000);
    
    if (TCB0.INTFLAGS) // TCB0 set the flag, indicating that it measured the signal's period
    {
      cli(); // Disable interrupts while we calculate/display the frequency.
      
      TCB0.INTFLAGS = 1; // Clear the TCB flag
      
      signal_pulse = TCB0.CCMP; // CCMP is the time between a rising edge and the next falling
  
      reading = 16000000.0 / signal_pulse;
  
      sei(); // Re-enable interrupts
  
      TCB0.INTFLAGS = 1; // This clears the TCB flag for the next PT
  
      return reading;
    }
  }

  if(readings.isEmpty()) return -1;

  double sum = 0;
  double num_readings = 0;

  while(!readings.isEmpty()) {
    sum += readings.pop();
    num_readings++;
  }

  return sum / num_readings;
}

float right_pt() {
  
  float reading;
  StackArray<float> readings;

  // MEASURE FOR RIGHT PT

  for(int i = 0; i < max_phototrans_readings; i++) {
  
    cli(); // Disable interrupts
    TCB0.CTRLA &= ~TCB_ENABLE_bm; // Disable TCB
  
    // A3 (PD0): Left PT
    EVSYS.CHANNEL2 = EVSYS_GENERATOR_PORT1_PIN0_gc; // Route pin A3, PD0
    EVSYS.USERTCB0 = EVSYS_CHANNEL_CHANNEL2_gc; // to TCB0
  
    TCB0.CTRLA |= TCB_ENABLE_bm;      // Enable TCB
    
    sei();                            // Enable interrupts
  
    delayMicroseconds(1000);
    
    if (TCB0.INTFLAGS) // TCB0 set the flag, indicating that it measured the signal's period
    {
      cli(); // Disable interrupts while we calculate/display the frequency.
      
      TCB0.INTFLAGS = 1; // Clear the TCB flag
      
      signal_pulse = TCB0.CCMP; // CCMP is the time between a rising edge and the next falling

      reading = 16000000.0 / signal_pulse;
  
      sei(); // Re-enable interrupts
  
      TCB0.INTFLAGS = 1; // This clears the TCB flag for the next PT

      return reading;
    }
  }

  if(readings.isEmpty()) return -1;

  double sum = 0;
  double num_readings = 0;

  while(!readings.isEmpty()) {
    sum += readings.pop();
    num_readings++;
  }

  return sum / num_readings;
}

float check_treasure() {
  float pt_reading[3];
  for(int i = 0; i < num_pt_readings; i++) {
    pt_reading[0] = left_pt();
    if(pt_reading[0] != -1) return pt_reading[0];
    
    pt_reading[1] = front_pt();
    if(pt_reading[1] != -1) return pt_reading[1];
    
    pt_reading[2] = right_pt();
    if(pt_reading[2] != -1) return pt_reading[2];
    
    delayMicroseconds(microseconds_between_pt_readings);
  }

  return -1;
}

#endif
