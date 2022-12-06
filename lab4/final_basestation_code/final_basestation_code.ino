#include "7seg.h"
#include "rx.h"

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  seg_display_setup();
  rx_setup();
}

void loop(void)
{
  // Check whether there are bytes available to be read. The length of data read
  // is usually the next available payload's length.
  if (radio.available() ) {

    unsigned long payloadReceived; // frequency sent from robot
    
    bool finished = false;
    while (!finished) {
      // Read payload data from the RX FIFO buffer(s).
      radio.read(&payloadReceived, sizeof(payloadReceived) );

      // Output the received result
      Serial.print("Frequency received: ");
      Serial.println(payloadReceived);
      Serial.println();

      finished = true;

      // Delay PTX complete transaction with PRX
      delay(20);

    }
    // Stop listening so we can send to TRX what we received
    radio.stopListening();

    // Send what we received
    radio.write( &payloadReceived, sizeof(payloadReceived) );
    printf("Sent response to PTX.\n\r\n\n");
    Serial.println("Sent response");

    // if the frequency is more than 4-digits we want the
    // frequency to be converted to kHz 
    if(payloadReceived > 9999) { 
      numToDisplay = payloadReceived / 10; 
      kDecimal = true; // this turns the decimal after the 2nd digit on
    } else {
      numToDisplay = payloadReceived;
      kDecimal = false;
    }

    // Go back to listening - we are the PRX after all!
    radio.startListening();
  }
}
