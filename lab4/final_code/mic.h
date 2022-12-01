#ifndef MIC_H
#define MIC_H

#define LOG_OUT 1 // use the log output function
#define FFT_N 256 // set to 256 point fft

#include <FFT.h> // for fft
#include <avr/io.h> // for adc
#include <stdbool.h> // for adc

// temp values and value storage for ADC
volatile uint16_t adcVal;
volatile uint16_t adc_vals[257];
volatile int counter = 0;

// variables to store initial state of ADC
int temp_ctrla, temp_ctrlc, muxpos;

// period = time_in_secs * 16000000/prescaler - 1
// Refresh display every 0.41667 ms
// period = (.41667/1000) * 16000000 / 64 - 1 ~= 103
int period_refresh_disp = 103; 

void adc_setup() {

  // store initial adc values
  temp_ctrla = ADC0.CTRLA;
  temp_ctrlc = ADC0.CTRLC;
  muxpos = ADC0.MUXPOS;
  
  /* Disable digital input buffer */
  PORTD.PIN5CTRL &= ~PORT_ISC_gm;
  PORTD.PIN5CTRL |= PORT_ISC_INPUT_DISABLE_gc;
  
  /* Disable pull-up resistor */
  PORTD.PIN5CTRL &= ~PORT_PULLUPEN_bm;
  
  ADC0.CTRLC = ADC_PRESC_DIV16_gc /* CLK_PER divided by 16 */
   | ADC_REFSEL_VDDREF_gc; /* Internal reference */
  
  ADC0.CTRLA = ADC_ENABLE_bm /* ADC Enable: enabled */
   | ADC_RESSEL_10BIT_gc; /* 10-bit mode */
  
  /* Select ADC channel */
  ADC0.MUXPOS = ADC_MUXPOS_AIN5_gc;
  
  /* Enable FreeRun mode */
  ADC0.CTRLA |= ADC_FREERUN_bm;

  // start the ADC conversion
  ADC0.COMMAND = ADC_STCONV_bm;
}

void interrupt_setup() {
  /* enable overflow interrupt */
  TCA0.SINGLE.INTCTRL = TCA_SINGLE_OVF_bm;

  /* set Normal mode */
  TCA0.SINGLE.CTRLB = TCA_SINGLE_WGMODE_NORMAL_gc;

  /* disable event numToDisplaying */
  TCA0.SINGLE.EVCTRL &= ~(TCA_SINGLE_CNTEI_bm);

  /* set the period */
  TCA0.SINGLE.PER = period_refresh_disp;
  TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV64_gc /* 64 prescaler for the clock */
                      | TCA_SINGLE_ENABLE_bm; /* start timer */

  /* enable global interrupts */
  sei();
}

uint16_t ADC0_read(void)
{
 /* Clear the interrupt flag by writing 1: */
 ADC0.INTFLAGS = ADC_RESRDY_bm;

 return ADC0.RES;
}

ISR(TCA0_OVF_vect)   // Interrupt routine that is called at every TCA timed interrupt
{
  // prevent index overflow
  if(counter <= 256) {
    adcVal = ADC0_read(); // read adc
    adc_vals[counter] = adcVal; // record value
    counter++; // increment index
  }
  
  /* The interrupt flag has to be cleared manually */
  TCA0.SINGLE.INTFLAGS = TCA_SINGLE_OVF_bm;
}

void mic_setup() {
  // put your setup code here, to run once:

  // for printing later
  //Serial.begin(9600);
  
  adc_setup();
  interrupt_setup();
}

bool listen_for_440() {
  bool return_val = false;
  while(counter <= 256) {}
  
  // when we get all of the adc values
  if(counter > 256) {
    // disable TCA
    cli();

    // restore ADC values
    ADC0.CTRLA = temp_ctrla;
    ADC0.CTRLC = temp_ctrlc;
    ADC0.MUXPOS = muxpos;
    
    // populate fft input array
    for(int i = 1; i < 257; i++) {
      fft_input[2*(i-1)] = adc_vals[i];
      fft_input[2*(i-1) + 1] = 0;
    }

    // run fft
    fft_window(); // window the data for better frequency response
    fft_reorder(); // reorder the data before doing the fft
    fft_run(); // process the data in the fft
    fft_mag_log(); // take the output of the fft

    Serial.println("440: " + String(fft_log_out[48]));
    
    if (fft_log_out[48] > 40) {
      return_val = true;
     }    
    sei();
  }

  counter = 0;
  return return_val;
}

#endif
