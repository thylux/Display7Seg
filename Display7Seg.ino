//
// thylux
// 7-Segment 4-digit display control using a 74HC595N shift register
//
// This driver only supports writing the digits for the moment
// As the Shift Register controls the anodes (sink current), we need to set the outputs as LOW to turn a light ON

// Display definitions
//
//  __A__
// |     |          Vcc < 2.1V, 20mA --> R = 150 Ohm
// F     B          
// |__G__|
// |     |
// E     C          DIG1 = 1, DIG2 = 2, DIG3 = 6, DIG4 = 8, COLON_P = 4, APOSTROPHE_P = 10
// |__D__|	    A = 14, B = 16, C = 13, D = 3, E = 5, F = 11, G = 15, DP = 7, COLON_N = 12, APOSTROPHE_N = 9
//

// Shift Register definitions
//
//   VCC Q0  DAT ENB LAT CLK RES OVR
// ---+---+---+---+---+---+---+---+---		Vcc < 5V, 70mA --> R = 72 Ohm
// |                                  |		Qn < 20mA
// D 	        74HC595N              |
// |                                  |
// ---+---+---+---+---+---+---+---+---
//   Q1  Q2  Q3  Q4  Q5  Q6  Q7  GND
//

// Display connection to Shift Register (direct breadboard connections)
// Q1 -> B, Q2 -> G, Q3 -> A, Q4 -> C, Q5 -> D, Q6 -> F, Q7 -> E

// Character mapping
//                 HBGACDFE
byte chars[11] = {B10100000, // 0
		  B10110111, // 1 
		  B10001010, // 2
		  B10000011, // 3
		  B10010101, // 4
		  B11000001, // 5
		  B11000000, // 6
		  B10100111, // 7
		  B10000000, // 8
		  B10000101, // 9
                  B11111111};// blank

// Arduino pin definition
int _LATCH = 12;
int _CLOCK = 11;
int _DATA = 10;
int _DIG1 = 7;
int _DIG2 = 6;
int _DIG3 = 5;
int _DIG4 = 4;

#define _BUFF_SIZE   4
#define _SHOWTIME    50

// initializes the buffer with empty characters
byte buffer[_BUFF_SIZE] = { chars[10], chars[10], chars[10], chars[10] };
int digits[_BUFF_SIZE] = { _DIG1, _DIG2, _DIG3, _DIG4 };

void setup()
{
  pinMode(_LATCH, OUTPUT);
  pinMode(_CLOCK, OUTPUT);
  pinMode(_DATA, OUTPUT);

  // TODO: Can a 555 reduce these 4 pins to 1?
  pinMode(_DIG1, OUTPUT);
  pinMode(_DIG2, OUTPUT);
  pinMode(_DIG3, OUTPUT);
  pinMode(_DIG4, OUTPUT);
    
  Serial.begin(9600);
}

void loop()
{
  for(int i = 0; i < 100; i++)
  {
    fillBuffer(i);
    for(int j = 0; j < _SHOWTIME; j++)
      writeScreen();
  }
  
  // Clean the latch for the next execution
  // TODO : needed???
  digitalWrite(_LATCH, LOW); // Begin Write
  shiftOut(_DATA, _CLOCK, LSBFIRST, chars[10]);
  digitalWrite(_LATCH, HIGH);  // End Write
}

void fillBuffer(int num)
{
  bool cleanChar = false;
  
  if(num==0)
  {
    buffer[_BUFF_SIZE - 1] = chars[0];
    return;
  }
    
  for(int i = _BUFF_SIZE - 1; i >= 0; i--)
  {
    if(num==0 && cleanChar) // We need to make sure that all the unused buffer positions are cleaned
      buffer[i] = chars[10];
    else
    {
      buffer[i] = chars[num > 9 ? num%10 : num];
      num/=10;
      cleanChar = true;
    }
  }
}

void writeScreen()
{
  for(int i = 0; i < _BUFF_SIZE; i++)
  {
    for(int i = 0; i < _BUFF_SIZE; i++)
      // TODO: find if it is possible to turn HIGH and LOW an entire PORT
      digitalWrite(digits[i], LOW);
      
    /*
    shiftOut(dataPin, clockPin, bitOrder, value)

    dataPin: 	the pin on which to output each bit (int)
    clockPin: 	the pin to toggle once the dataPin has been set to the correct value (int)
    bitOrder: 	which order to shift out the bits; either MSBFIRST or LSBFIRST. (Most Significant Bit First, or, Least Significant Bit First)
    value: 		the data to shift out. (byte) 
    */
	
    digitalWrite(_LATCH, LOW); // Begin Write
    shiftOut(_DATA, _CLOCK, LSBFIRST, buffer[i]);
    digitalWrite(_LATCH, HIGH);  // End Write
    
    digitalWrite(digits[i], HIGH);
    
    delay(3);
  }
}

