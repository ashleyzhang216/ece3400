//
// ECE 3400 - Fall 2022 - Carl Bernard
// Ashley Zhang & Sun Lee
//

// The base station recieves the measured frequency from the PTs on the
// robot, then displays the frequency on the 7-segment display.

// The functions used here are all described in the RF24.h file 
// located in C:\Users\<username>\Documents\Arduino\libraries\RF24-1.4.1

#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"

//
// Hardware configuration
//
// Set up nRF24L01 radio
// CE on Pin 9
// CS on pin 10
RF24 radio(9, 10);

// Radio pipe address
// The default length is 5 bytes
// See handout on how to calculate your own team's pipe address and enter
// the value below.

// pipe numbers = 2*(3*1 + 14) + X = 34 + X

// The first value is for the TRANSMITTER (X = 0)
// TRANSMITTER = 34 = 0x22 (in hexadecimal) 

// The second value is for the RECEIVER (X = 1)
// RECEIVER = 35 = 0x23 (in hexadecimal)

const uint64_t pipes [2] = { 0x0000000022LL, 0x0000000023LL } ;

// Variable in which the received data will be stored
//int payloadReceived;

// Volatile used to capture the period of the measured signal
volatile int period;

// Variables to measure the first pulse, and one whole period with the AC
float signal_pulse;

void setup(void)
{

  Serial.begin(9600);
  Serial.println("ROLE: Receiver");

  //
  // Setup and configure rf radio
  //
  // Begin operation of the chip, returns TRUE if chip started OK.
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

// Open the pipe for reading
  radio.openReadingPipe(1, pipes[0]);

  // Start listening on the pipes opened for reading.
  radio.startListening();

  // Print the configuration of the transceiver for debugging
  radio.printDetails();
}

void loop(void)
{
  // Check whether there are bytes available to be read. The length of data read 
  // is usually the next available payload's length.
  if (radio.available() ) {
    // Read payload data from the RX FIFO buffer(s).
    radio.read(&signal_pulse, sizeof(float) );

    // Output the received result
    Serial.println("Frequency measured received: ");
    Serial.println(signal_pulse);
  }
  else {
    Serial.println("No radio available!");
  }
  // Delay so that the Serial Monitor doesn't print values that fly by!
  delay(2000);
}
