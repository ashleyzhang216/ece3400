/* 
Ashley Zhang (ayz27), Sun Lee (syl49)
ECE 3400 Fall 2022

Code first drafted by Professor Carl Bernard (Fall 2022)
"This sketch measures the frequency of a signal connected to an Analog pin.
It does so by internally multiplexing the Nanos analog pin directly to the
TCB (i.e., no AC used)."
 */


// This code cycles through each of the three PTs (front, left, right) to measure 
// the frequency of a treasure. 


// Volatile used to capture the period of the measured signal
volatile int period;

// Variables to measure the first pulse, and one whole period with the AC
float signal_pulse;


void setup() {
  Serial.begin(9600);

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


void loop() {
  
  // MEASURE FOR LEFT PT
  
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
    
    Serial.println("Frequency measured by LEFT PT: ");
    Serial.println(16000000.0 / signal_pulse / 1000, 6);

    sei(); // Re-enable interrupts

    TCB0.INTFLAGS = 1; // This clears the TCB flag for the next PT

    //[TAKE 3-5 MEASUREMENTS TO MAKE SURE READINGS ARE CORRECT BEFORE MOVING ON]

    delay(2000);
  }



  // MEASURE FOR FRONT PT
  
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
    
    Serial.println("Frequency measured by FRONT PT: ");
    Serial.println(16000000.0 / signal_pulse / 1000, 6);

    sei(); // Re-enable interrupts

    TCB0.INTFLAGS = 1; // This clears the TCB flag for the next PT

    //[TAKE 3-5 MEASUREMENTS TO MAKE SURE READINGS ARE CORRECT BEFORE MOVING ON]

    delay(2000);
    
  }



  // MEASURE FOR RIGHT PT
  
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
    
    Serial.println("Frequency measured by RIGHT PT: ");
    Serial.println(16000000.0 / signal_pulse / 1000, 6);

    sei(); // Re-enable interrupts

    TCB0.INTFLAGS = 1; // This clears the TCB flag for the next PT

    delay(2000);

    //[TAKE 3-5 MEASUREMENTS TO MAKE SURE READINGS ARE CORRECT BEFORE MOVING ON]
  }
}
