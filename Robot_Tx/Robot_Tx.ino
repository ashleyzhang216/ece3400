//
// ECE 3400 - Fall 2022 - Carl Bernard
// Ashley Zhang & Sun Lee
//

// The phototransistor (PT) measures the frequency on the robot then transmits 
// this frequency to the base station using the RF comms

// The functions used here are all described in the RF24.h file 
// located in C:\Users\<username>\Documents\Arduino\libraries\RF24-1.4.1

#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"

// Volatile used to capture the period of the measured signal
volatile int period;

// Variables to measure the first pulse, and one whole period with the AC
float signal_pulse;


// Hardware configuration

// Set up nRF24L01 radio
// CE on Pin 9
// CS on pin 10
RF24 radio(9, 10);

// Radio pipe address
// The default length is 5 bytes

// pipe numbers = 2*(3*1 + 14) + X = 34 + X

// The first value is for the TRANSMITTER (X = 0)
// TRANSMITTER = 34 = 0x22 (in hexadecimal)

// The second value is for the RECEIVER (X = 1)
// RECEIVER = 35 = 0x23 (in hexadecimal)

const uint64_t pipes [2] = { 0x0000000022LL, 0x0000000023LL } ;


/* %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% */


void setup(void)
{

  Serial.begin(9600);
  Serial.println("ROLE: Transmitter");

  // Setup and configure rf radio
  // Begin operation of the chip, returns TRUE if chip started OK.
  
  radio.begin();
  if (!radio.begin()) {
    Serial.println("Radio hardware not responding!");
    while (1) {} // hold program in infinite loop to prevent subsequent errors
  }

  // optionally, increase the delay between retries & # of retries
  radio.setRetries(10, 10);

  // Enable auto-acknowledgment (enabled by default). Autoack responds to every
  // received payload with an empty ACK packet which gets sent from the receiving
  // radio to the transmitting radio. If it isn't enabled, the transmitting radio
  // will always report a received payload EVEN IF it wasn't. This setting must appear
  // in both radios.
  radio.setAutoAck(true);

  // Set the channel, calculated as: 2400MHz + <channel number>
  radio.setChannel(0x64);

  // Set the power
  // RF24_PA_MIN=-18dBm, RF24_PA_LOW=-12dBm, RF24_PA_MED=-6dBM, and RF24_PA_HIGH=0dBm.
  // OK to use low power when testing on bench, probably want higher/highest power for demo.
  radio.setPALevel(RF24_PA_MIN);

  // Set the transmission data rate
  // RF24_250KBPS for 250kbs, RF24_1MBPS for 1Mbps, or RF24_2MBPS for 2Mbps
  radio.setDataRate(RF24_250KBPS);

  // Open the pipe for writing
  radio.openWritingPipe(pipes[0]);

  // Print the configuration of the transceiver for debugging
  radio.printDetails();

  // Setup and configure PT
  // Internally reroute the phototransistor Analog pin directly to TCB.
  
  // A4 (PA2): Analog pin Front PT is connected to
  EVSYS.CHANNEL0 = EVSYS_GENERATOR_PORT0_PIN2_gc; // Route pin A4, PA2
  EVSYS.USERTCB0 = EVSYS_CHANNEL_CHANNEL0_gc; // to TCB0

  // Program TCB for frequency measurements
  
  /* Input Capture Frequency measurement mode*/
  TCB0.CTRLB = TCB_CNTMODE_FRQ_gc;
  /* Event Input Enable: enabled */
  TCB0.EVCTRL = 0x41;// that's 1000001: Enable Capture Event, Enable Filter

  TCB0.CTRLA = TCB_CLKSEL_CLKDIV1_gc /* CLK_PER/1 (From Prescaler) */
               | TCB_ENABLE_bm /* Enable: enabled */
               | TCB_RUNSTDBY_bm; /* Run Standby: enabled */
  sei();

}


void loop(void)
{
  if (TCB0.INTFLAGS) // TCB0 set the flag, indicating that it measured the signal's period
  {
    signal_pulse = TCB0.CCMP; // CCMP is the time between a rising edge and the next falling
    delay(500);

    // Disable interrupts while we calculate and display the frequency.
    // A better way would be to disable ONLY TCB interrupts.
    cli(); 


    // The first value (or first 2-4 values) may be unusable.
    // Keep that in mind in your code.
    Serial.print("Now sending measured frequency: ");
    Serial.println(16000000.0 / signal_pulse / 1000, 6);

    bool ok = radio.write( &signal_pulse, sizeof(float) );
      if (ok) {
        Serial.println("ok...");
      }
      else {
    Serial.println("Send failed! Need to debug...");
    }
    Serial.println();

    delay(2000);
    
    
    // Re-enable interrupts. 
    // Or if we disabled TCB interrupts, re-enable TCB interrupts.
    sei(); 
  }
  
}
