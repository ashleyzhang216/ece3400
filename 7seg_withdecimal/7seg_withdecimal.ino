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


byte digitBeingWritten;
long numToDisplay = 98765; // This will be the frequency received from the PT/Robot
bool kDecimal = false;
volatile bool displayDecimalPoint;
void disp(byte number, bool displayDecimalPoint = 0);

// period = time_in_secs * 16000000/prescaler - 1
// Refresh display every 5 ms
int period_refresh_disp = 311; //for 5ms

void setup()
{
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
