/*
   analogStickUART
*/

//#define DEBUGMODE

/*
   Use CTC mode on Timer1 to trigger analog sampling
   Timer0 on ATmega328p is 8 bit so maximum count is 255
   Timer1 on ATmega328p is 16 bit so maximum count is 65535
   Possible values for prescaler include 1, 8, 64, 256, 1024
   To relate desired count to input frequency (Fi--16MHz),
   prescaler, and target frequency (Ft):
   count = (Fi / prescaler) / Ft - 1

   prescale and CMR for some exact target frequencies:
   100 Hz:  (8, 19999) (64, 2499) (256, 624)
   200 Hz:  (8, 9999) (64, 1249)
   500 Hz:  (1, 31999) (8, 3999) (64, 499) (256 124)
   1000 Hz: (1, 15999) (8, 1999) (64, 249)
*/
const int __prescaler = 64;
const int __compareMatchRegister = 2499;

// Count ticks since start inside ISR
volatile unsigned long ticksSinceStart = 0;

// Sample write control
volatile boolean sampleWritten = false;
volatile boolean sampleWriteEnabled = false;

// Analog samples and pins
volatile int analogSamples[2];
const int __analogPin0 = A0;
const int __analogPin1 = A1;

void setup() {
  // Initialize and enable timer1 for analog sampling
  TIMSK1 &= (0 << OCIE1A);                  // Clear OCIE1A to DISABLE compare A match interrupt on TIMER1_COMPA_vect
  TIMSK1 &= (0 << OCIE1B);                  // Clear OCIE1B to DISABLE compare B match interrupt on TIMER1_COMBB_vect
  TCCR1A = 0;                               // Clear TCCR1A register (normal operation)
  TCCR1B = 0;                               // Clear TCCR1B register (normal operation)
  TCNT1  = 0;                               // Initialize counter value to 0
  OCR1A  = __compareMatchRegister;          // Set compare match register
  TCCR1B |= (1 << WGM12);                   // Set TCCR1B bit WGM12 to enable CTC mode
  switch (__prescaler) {
    case 1:
      TCCR1B |= (1 << CS10);                // Set TCCR1B bit CS10 for no prescaling
      break;
    case 8:
      TCCR1B |= (1 << CS11);                // set TCRR1B bit CS11 for 8 prescaler
      break;
    case 64:
      TCCR1B |= (1 << CS11) | (1 << CS10);  // Set TCCR1B bit CS11 and CS10 for 64 prescaler
      break;
    case 256:
      TCCR1B |= (1 << CS12);                // Set TCCR1B bit CS12 for 256 prescaler
      break;
    case 1024:
      TCCR1B |= (1 << CS12) | (1 << CS10);  // Set TCCR1B bits CS12 and CS10 for 1024 prescaler
      break;
  }
  TIMSK1 |= (1 << OCIE1A);              // Set OCIE1A to enable interrupt TIMER1_COMPA_vect
  Serial.begin(115200);
}

void loop() {
  if (!sampleWritten && sampleWriteEnabled) {

#ifdef DEBUGMODE
    Serial.print(ticksSinceStart);
    Serial.print("\t");
    Serial.print(analogSamples[0]);
    Serial.print("\t");
    Serial.println(analogSamples[1]);
#else
    Serial.write((byte*)&ticksSinceStart, 4);
    Serial.write((byte*)analogSamples, 4);
#endif

    sampleWritten = true;
  }
}

/*
   serialEvent

   Toggle sampleWriteEnabled when data is detected on serial port.
*/
void serialEvent() {
  Serial.read();
  sampleWriteEnabled = !sampleWriteEnabled;
}


/*
   Interrupt service routine triggered at TIMER1_COMPA_vect

   Update time count and obtain analog samples
*/
ISR(TIMER1_COMPA_vect) {
  ticksSinceStart++;
  analogSamples[0] = analogRead(__analogPin0);
  analogSamples[1] = analogRead(__analogPin1);
  sampleWritten = false;
}

