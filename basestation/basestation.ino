// Carl Bernard, Fall 2022
// Drive a 4-segment 7-segment display with a shift register.


// Shift register pins
#define clockPin  7     // clock pin
#define dataPin   6     // data pin

// Pins on 7-degment display, one for each digit
#define Digit1    5
#define Digit2    4
#define Digit3    3
#define Digit4    2

#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"

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

byte digitBeingWritten;
long numToDisplay = payloadReceived; // This will be the frequency received from the PT/Robot
bool kDecimal = false;
volatile bool displayDecimalPoint;
void disp(byte number, bool displayDecimalPoint = 0);

// period = time_in_secs * 16000000/prescaler - 1
// Refresh display every 5 ms
int period_refresh_disp = 311; //for 5ms

void setup()
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
  
  //Serial.begin(9600);
  pinMode(Digit1, OUTPUT);
  pinMode(Digit2, OUTPUT);
  pinMode(Digit3, OUTPUT);
  pinMode(Digit4, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);

  display_OFF();  // turn off the display

  // Setup of TCA which will be used to trigger an interrupt
  // every period_refresh_disp to refresh the display.
  //

  /* enable overflow interrupt */
  TCA0.SINGLE.INTCTRL = TCA_SINGLE_OVF_bm;

  /* set Normal mode */
  TCA0.SINGLE.CTRLB = TCA_SINGLE_WGMODE_NORMAL_gc;

  /* disable event numToDisplaying */
  TCA0.SINGLE.EVCTRL &= ~(TCA_SINGLE_CNTEI_bm);

  /* set the period */
  TCA0.SINGLE.PER = period_refresh_disp;
  TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV256_gc /* 256 prescaler for the clock */
                      | TCA_SINGLE_ENABLE_bm; /* start timer */

  /* enable global interrupts */
  sei();

  Serial.begin(9600);
  
  if(numToDisplay > 9999) {
    numToDisplay /= 10;
    kDecimal = true;
    Serial.println(">= 10,000 val");
  } Serial.println(numToDisplay);
}

void loop()
{
  // Check whether there are bytes available to be read. The length of data read
  // is usually the next available payload's length.
  if (radio.available() ) {

    unsigned long payloadReceived;
    
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

    // Go back to listening - we are the PRX after all!
    radio.startListening();

  }
}


ISR(TCA0_OVF_vect)   // Interrupt routine that is called at every TCA timed interrupt
{
  display_OFF();  // turn off the display

  // If the number is > 9999, then it will be displayed with a decimal point
  // Ex. 98765 will be displayed as 98.76, where kilo is implied
  
//  if (numToDisplay > 9999) {
//    numToDisplay = numToDisplay/10;
//    kDecimal = true;
//    displayDecimalPoint = kDecimal;
//  } else {
//    kDecimal = false;
//    displayDecimalPoint = kDecimal;
//  }
  
  switch (digitBeingWritten)
  {
    case 1:
      disp(numToDisplay / 1000, false);   // isolate left thousand digit
      digitalWrite(Digit1, LOW);  // turn on digit 1
      break;

    case 2:
      disp( (numToDisplay / 100) % 10, bool(kDecimal));   // isolate hundred digit
      digitalWrite(Digit2, LOW);     // turn on digit 2
      break;

    case 3:
      disp( (numToDisplay / 10) % 10, false );   // isolate tens digit
      digitalWrite(Digit3, LOW);    // turn on digit 3
      break;

    case 4:
      disp(numToDisplay % 10, false);   // isolate unit digit
      digitalWrite(Digit4, LOW);  // turn on digit 4
  }

  digitBeingWritten = (digitBeingWritten % 4) + 1;

  /* The interrupt flag has to be cleared manually */
  TCA0.SINGLE.INTFLAGS = TCA_SINGLE_OVF_bm;
}


// Display the digits on the 7-segment display
// The Arduino function shiftOut() is used which sends data to the shift register serially.
// The boolean displayDecimalPoint determines if a decimal point will be displayed,
// displayDecimalPoint = 0: DP is off on 7-segment display,
// displayDecimalPoint = 1: DP is on.
void disp(byte numberToDisplay, bool displayDecimalPoint)
{
  switch (numberToDisplay)
  {    
    case 0:  // display a 0
      shiftOut(dataPin, clockPin, MSBFIRST, 0x02 | !displayDecimalPoint);
      digitalWrite(clockPin, HIGH);
      digitalWrite(clockPin, LOW);
      break;

    case 1:  // display a 1
      shiftOut(dataPin, clockPin, MSBFIRST, 0x9E | !displayDecimalPoint);
      digitalWrite(clockPin, HIGH);
      digitalWrite(clockPin, LOW);
      break;

    case 2:  // display a 2
      shiftOut(dataPin, clockPin, MSBFIRST, 0x24 | !displayDecimalPoint);
      digitalWrite(clockPin, HIGH);
      digitalWrite(clockPin, LOW);
      break;

    case 3:  // display a 3
      shiftOut(dataPin, clockPin, MSBFIRST, 0x0C | !displayDecimalPoint);
      digitalWrite(clockPin, HIGH);
      digitalWrite(clockPin, LOW);
      break;

    case 4:  // display a 4
      shiftOut(dataPin, clockPin, MSBFIRST, 0x98 | !displayDecimalPoint);
      digitalWrite(clockPin, HIGH);
      digitalWrite(clockPin, LOW);
      break;

    case 5:  // display a 5
      shiftOut(dataPin, clockPin, MSBFIRST, 0x48 | !displayDecimalPoint);
      digitalWrite(clockPin, HIGH);
      digitalWrite(clockPin, LOW);
      break;

    case 6:  // display a 6
      shiftOut(dataPin, clockPin, MSBFIRST, 0x40 | !displayDecimalPoint);
      digitalWrite(clockPin, HIGH);
      digitalWrite(clockPin, LOW);
      break;

    case 7:  // display a 7
      shiftOut(dataPin, clockPin, MSBFIRST, 0x1E | !displayDecimalPoint);
      digitalWrite(clockPin, HIGH);
      digitalWrite(clockPin, LOW);
      break;

    case 8:  // display a 8
      shiftOut(dataPin, clockPin, MSBFIRST, !displayDecimalPoint);
      digitalWrite(clockPin, HIGH);
      digitalWrite(clockPin, LOW);
      break;

    case 9:  // display a 9
      shiftOut(dataPin, clockPin, MSBFIRST, 0x08 | !displayDecimalPoint);
      digitalWrite(clockPin, HIGH);
      digitalWrite(clockPin, LOW);
  }
}

void display_OFF() // turn on display on 7-segment display
{
  digitalWrite(Digit1, HIGH);
  digitalWrite(Digit2, HIGH);
  digitalWrite(Digit3, HIGH);
  digitalWrite(Digit4, HIGH);
}
