#include <avr/io.h>
#include <stdbool.h>

uint16_t adcVal;

void setup() {
  // put your setup code here, to run once:

  Serial.begin(230400);
  
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

uint16_t ADC0_read(void)
{
 /* Clear the interrupt flag by writing 1: */
 ADC0.INTFLAGS = ADC_RESRDY_bm;

 return ADC0.RES;
}

bool ADC0_conversionDone() {
  return (ADC0.INTFLAGS & ADC_RESRDY_bm);
}

void loop() {
  // put your main code here, to run repeatedly:

  if (ADC0_conversionDone())
  {
    adcVal = ADC0_read();
    /* In FreeRun mode, the next conversion starts automatically */

    Serial.println(adcVal);
  }

}
