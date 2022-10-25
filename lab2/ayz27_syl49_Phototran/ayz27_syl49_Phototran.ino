// Carl Bernard, Fall 2022
// Code to use AC and TCB to measure the frequency of a signal.
//
// We assume that pin A1 is used for the input
// which is pin AIN[2], which is pin PD2.
// If you use another pin, you need to change things accordingly in the file.

// Variables to measure the first pulse, and one whole period with the AC
float signal_pulse = 0, signal_period = 0;
// Variable to calculate the frequency of the measured signal
float freqSignal;

/* Uncomment line 16 when assuming that input voltage to AC is 1V
    otherwise, refer to DACREF_VALUE in the loop */
/* set DACREF to 1.1 Volts for Vref = 1.5Volts 
// DACREF_VALUE = V_dacref*256/Vref
int DACREF_VALUE = 170;*/

float nanoFreq = 16000000.0; // 16MHz is the Nano's main clock frequency

void setup() {
  Serial.begin(9600);

  //
  // Initialize input pin (A1, PD2) and output (PA7)
  //
  /* Positive Input - Disable digital input buffer */
  // Pin A1, which is PD2: port D pin 2.
  PORTD.PIN2CTRL = PORT_ISC_INPUT_DISABLE_gc;
  /*Enable output buffer on PA7: which is where the output of the AC is sent*/
  PORTA.DIR |= PIN7_bm;

  //
  // Initialize AC
  //
  /* Negative input uses internal reference - voltage reference should be enabled */
  VREF.CTRLA = VREF_AC0REFSEL_1V5_gc; /* Voltage reference at 1.5V */
  /* AC0 DACREF reference enable:enabled */
  VREF.CTRLB = VREF_AC0REFEN_bm;
  /* Set DAC voltage reference */
  //AC0.DACREF = DACREF_VALUE;
  /*Select proper inputs for comparator*/
  AC0.MUXCTRLA = AC_MUXPOS_PIN0_gc /* Positive Input - Pin 0 */
                 | AC_MUXNEG_DACREF_gc; /* Negative Input - DAC Voltage Reference */
  AC0.CTRLA = AC_ENABLE_bm /* Enable analog comparator */
              //| AC_HYSMODE_25mV_gc // Hysterisis 25mV
              | AC_OUTEN_bm; /* Output Buffer Enable: enabled */
  AC0.INTCTRL = 0; /* Analog Comparator 0 Interrupt disabled */

  //
  // Initialize Enable event generation from Analog comparator to TCB
  //
  /* Analog Comparator 0 out linked to Event Channel 0 */
  EVSYS.CHANNEL0 = EVSYS_GENERATOR_AC0_OUT_gc;
  /* TCB uses Event Channel 0 */
  EVSYS.USERTCB0 = EVSYS_CHANNEL_CHANNEL0_gc;

  //
  // Initialize TCB in pulse width-frequency measurement mode, input from Analog
  //Comparator through Event System
  //
  /* Input Capture Frequency measurement mode*/
  TCB0.CTRLB = TCB_CNTMODE_FRQ_gc;
  /* Event Input Enable: enabled */
  TCB0.EVCTRL = TCB_CAPTEI_bm;
  /* Enable filter */
  TCB0.EVCTRL |= TCB_FILTER_bm;
  // Keep the following commented out - it messes things up later in the course.
  //TCB0.INTCTRL = TCB_CAPT_bm;
  TCB0.CTRLA = TCB_CLKSEL_CLKDIV1_gc /* CLK_PER/1 (From Prescaler) */
               | TCB_ENABLE_bm /* Enable: enabled */
               | TCB_RUNSTDBY_bm; /* Run Standby: enabled */
}

void loop() {

  if (TCB0.INTFLAGS) // TCB0 set the flag, indicating that it measured the signal'a period
  {
    /**
      First read the CNT register. The interrupt flag is cleared by writing 1 to it,
      or when the Capture register is read in Capture mode.
    */
    signal_period = TCB0.CNT;
    signal_pulse = TCB0.CCMP; // CCMP is the time between a rising edge and the next falling edge

    // Calculate the frequency of the measured signal
    freqSignal = signal_pulse / nanoFreq;
    Serial.print("Frequency: ");
    Serial.print(nanoFreq / signal_pulse / 1000, 6);
    Serial.println(" kHz"); 
  }

  /* Determining background (offset from ambient lighting) */
    float x = analogRead(A1);
    float correspondingVoltageAmbientLight;
    float DACREF_VALUE;
    
    correspondingVoltageAmbientLight = x / 1023 * 5; // Volts
    DACREF_VALUE = correspondingVoltageAmbientLight * 256 / 1.5; // Vref = 1.5V defined in line 37
    
    Serial.print("DACREF_VALUE: ");
    Serial.println(DACREF_VALUE);
  
}
