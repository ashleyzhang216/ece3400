#ifndef RF_H
#define RF_H

#include "utility.h"

#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"

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
// The first value is for the TRANSMITTER
// The second value is for the RECEIVER
const uint64_t pipes [2] = { 0x0000000022LL, 0x0000000023LL } ;

// Timeout value for when a failed send happens, in ms
unsigned long failTimeout = 200;

// Counter to keep track of failed transmissions
int failedSends = 0;


void rf_setup(void)
{

  //Serial.begin(57600);
  //printf_begin();
  //printf("\nSetup started\n\r");
  //Serial.println("ROLE: Transmitter\n");

  //
  // Setup and configure rf radio
  //
  // Begin operation of the chip, returns TRUE if chip started OK.
  radio.begin();
  if (!radio.begin()) {
    Serial.println("Radio hardware not responding!");
    while (1) {} // hold program in infinite loop to prevent subsequent errors
  }


  // optionally, increase the delay between retries, and # of retries
  radio.setRetries(10, 10);

  // Enable auto-acknowledgment (enabled by default). Autoack responds to every
  // received timeLoopStart with an empty ACK packet which gets sent from the receiving
  // radio to the transmitting radio. If it isn't enabled, the transmitting radio
  // will always report a received timeLoopStart EVEN IF it wasn't. This setting must appear
  // in both radios.
  radio.setAutoAck(true);

  // Set the channel, calculated as: 2400MHz + <channel number>
  radio.setChannel(0x64);

  // Set the power
  // RF24_PA_MIN=-18dBm, RF24_PA_LOW=-12dBm, RF24_PA_MED=-6dBM, and RF24_PA_HIGH=0dBm.
  // OK to use low power when testing on bench, probably want higher/highest power for demo.
  radio.setPALevel(RF24_PA_LOW);

  // Set the transmission data rate
  // RF24_250KBPS for 250kbs, RF24_1MBPS for 1Mbps, or RF24_2MBPS for 2Mbps
  radio.setDataRate(RF24_250KBPS);

  // Open the pipe for writing: when sending data
  radio.openWritingPipe(pipes[0]);
  // Open the pipe for reading: when receiving acknowledge, e.g.
  radio.openReadingPipe(1, pipes[1]);

  // Leave low-power mode - required for normal radio operation
  radio.powerUp();

  // Go to listening mode so as not to block the channel more than needed
  radio.startListening();

  // Print the configuration of the transceiver for debugging
  radio.printPrettyDetails();
  Serial.println("\n\n");
  Serial.println("RF Setup complete.");

}

bool transmit_to_base(long numToDisplay) // frequency sent to base station
{
  bool tx_result;
  failedSends  = 0;
  
  // Take note of the time, and send it. Also acts as a time stamp for the start of loop()
  unsigned long timeLoopStart = millis();
    
  /*Serial.print("Now sending time value: ");
  Serial.print(timeLoopStart);
  Serial.println(" ms");*/

  // Stop listening because we want to send info over RF
  radio.stopListening();

  // Send data
  if (radio.write( &numToDisplay, sizeof(numToDisplay)) ) {
    //Serial.println("Transmission OK!");
    tx_result = true;
  }
  else {
    //Serial.println("Transmission failed...");
    failedSends++;
    tx_result = false;
  }

//  while(!radio.write( &numToDisplay, sizeof(numToDisplay)) && failedSends < 100) {
//    failedSends++;
//  }

  if(failedSends >= 100) {
    tx_result = false;
  } else {
    tx_result = true;
  }

  // Resume listening
  radio.startListening();

  // Keep track of how long we wait
  unsigned long timeStartToWait = millis();
  // Will we have a timeout?
  bool timeout = false;

  while (!radio.available() && !timeout) {
    if (millis() - timeStartToWait  > 200 ) {
      timeout = true;
    }

    if (timeout) {
      // try another attempt
      //Serial.println("Nothing received from PRX...");
    }
    else {// Read what was sent back from PRX
      unsigned long recvdFromPRX;
      radio.read( &recvdFromPRX, sizeof(recvdFromPRX) );
      printf("Received response from PRX: %lu, round-trip delay: %lu\n\r", 
              recvdFromPRX, millis() - recvdFromPRX); 
    }
  }

  // Try again
  //delay_ms(2000); 

  return tx_result;
}

#endif
