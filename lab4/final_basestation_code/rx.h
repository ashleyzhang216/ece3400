#include <nRF24L01.h>
#include <printf.h>
#include <RF24.h>
#include <RF24_config.h>

#ifndef RX_H
#define RX_H

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

// Variable in which the received data will be stored
float payloadReceived;

void rx_setup(void)
{
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
  radio.setPALevel(RF24_PA_LOW);

  // Set the transmission data rate
  // RF24_250KBPS for 250kbs, RF24_1MBPS for 1Mbps, or RF24_2MBPS for 2Mbps
  radio.setDataRate(RF24_250KBPS);


  // Open the pipe for writing: when sending data
  radio.openWritingPipe(pipes[1]);
  // Open the pipe for reading
  radio.openReadingPipe(1, pipes[0]);

  // Leave low-power mode - required for normal radio operation
  radio.powerUp();

  // Start listening on the pipes opened for reading.
  radio.startListening();

  // Print the configuration of the transceiver for debugging
  radio.printPrettyDetails();
}

#endif
