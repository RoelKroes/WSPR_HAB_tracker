#include <si5351.h>     // You need to download this one https://github.com/etherkit/Si5351Arduino
#include <TinyGPS++.h>  // you need to download this one https://github.com/mikalhart/TinyGPSPlus
#include <SoftwareSerial.h>
#include <JTEncode.h>   // you need to download this one https://github.com/etherkit/JTEncode
#include "Wire.h"
#include "settings.h"

// Mode defines
#define WSPR_TONE_SPACING       146          // ~1.46 Hz
#define WSPR_DELAY              683          // Delay value for WSPR

// Choose one of the values for 16MHz or 8MHz processor
// #define WSPR_CTC             10672        // CTC value for WSPR for 16 MHz processor
// #define WSPR_CTC             5336         // CTC value for WSPR for 8 MHz processor
#define WSPR_CTC                5336

// Different modes which can be used with the JTEncode library. Only MODE_WSPR is used in this software.
enum mode {MODE_JT9, MODE_JT65, MODE_JT4, MODE_WSPR, MODE_FSQ_2, MODE_FSQ_3,MODE_FSQ_4_5, MODE_FSQ_6, MODE_FT8};
// Mode used to transmit
#define DEFAULT_MODE            MODE_WSPR

// Class instantiation
Si5351 si5351;
JTEncode jtencode;
TinyGPSPlus gps;

#define MAX_TX_LEN WSPR_SYMBOL_COUNT

// Global variables
unsigned long long freq;  
uint8_t tx_buffer[255];
enum mode cur_mode = DEFAULT_MODE;
uint8_t symbol_count;
uint16_t tone_delay, tone_spacing;
char MaidenHead[7];

// Global variables used in ISRs
volatile bool proceed = false;

/***********************************************************************************
* DATA STRUCTS
*  
* Normally no change necessary
************************************************************************************/
// Struct to hold GPS data
struct TGPS
{
  int Hours, Minutes, Seconds;
  float Longitude, Latitude;
  long Altitude;
  unsigned int Satellites;
  byte FlightMode;
  bool sendMsg1 = false;
  bool sendMsg2 = false;
  
  char call_2[7]; // HAM call for message 2
  char MH_1[5] = {'A', 'A', '0', '0', (char) 0};  // Maidenhead for message 1
  char MH_2[5] = {'A', 'A', '0', '0', (char) 0};  // Maidenhead for message 2
  uint8_t dbm_1;  // dbm for message 1
  uint8_t dbm_2;  // dbm for message 2
  // Below are values which are used in encoding the second wspr message 
  int GPS_valid;
  int Sats_valid;
  float volts;
  int speed_knots;
  int temperature;
} UGPS;


/***********************************************************************************
* GLOBALS
*  
* Normally no change necessary
************************************************************************************/
SoftwareSerial SerialGPS(Rx, Tx);

/****************************************************************************************************************
*  Setup function
*  
****************************************************************************************************************/
void setup()
{
  bool i2c_found;

  if (LED_PIN > 0)
  {
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN,LOW);
  }
  
  // Start serial monitor if in debug mode
#if defined(DEVMODE)
  Serial.begin(57600);
  while (!Serial) { }
  Serial.println(F("WSPR HAB by Roel Kroes."));
#endif

  // Startup up the GPS
  SerialGPS.begin(GPSBaud);
  
  // Initialize the Si5351
  // ********************************
  // IMPORTANT
  // ********************************
  // Change the 2nd parameter in init to the frequency used in your oscillator  
  // if using a ref osc other than 25 MHz.
  // Specify 0 if you are using 25 MHz
  
 i2c_found = si5351.init(SI5351_CRYSTAL_LOAD_10PF, SI5351FREQ, 0);
  
  // Start on target frequency
  // Set the calibration factor for this module 
  // use the program in examples from the library to calibrate or use a reliable receiver
  // There is a parameter for the calibration offset of your oscillator in settings.h
  si5351.set_correction(SI5351_CORRECTION, SI5351_PLL_INPUT_XO);
  si5351.set_pll(SI5351_PLL_FIXED, SI5351_PLLA);
  
  // Set CLK0 output, other clocks are not used.
  si5351.drive_strength(SI5351_CLK0, SI5351_DRIVE_8MA); // Set for max power
  si5351.output_enable(SI5351_CLK0, 0); // Disable the clock initially

  // Check the i2c bus for the si5351
  if (!i2c_found)
  {
#if defined(DEVMODE)    
    Serial.println(F("No device on I2C bus!"));
#endif
  }

  // Set the mode to use
  cur_mode = MODE_WSPR;

  // Set the proper frequency, tone spacing, symbol count, and
  // tone delay depending on mode
  switch(cur_mode)
  {
    case MODE_WSPR:
      symbol_count = WSPR_SYMBOL_COUNT; // From the library defines
      tone_spacing = WSPR_TONE_SPACING;
      tone_delay = WSPR_DELAY;
    break;
  }

  UGPS.sendMsg1 = false; 
  UGPS.sendMsg2 = false; 
  
  // Set up Timer1 for interrupts every 'symbol' period.
  // I did not get the timing for WSPR to work correctly with the ATMEL328, without using interrupts
  noInterrupts();          // Turn off interrupts.
  TCCR1A = 0;              // Set entire TCCR1A register to 0; disconnects
                           //   interrupt output pins, sets normal waveform
                           //   mode.  We're just using Timer1 as a counter.
  TCNT1  = 0;              // Initialize counter value to 0.
  TCCR1B = (1 << CS12) |   // Set CS12 and CS10 bit to set prescale
    (1 << CS10) |          //   to /1024
    (1 << WGM12);          //   turn on CTC
                           //   which gives, 64 us ticks
  TIMSK1 = (1 << OCIE1A);  // Enable timer compare interrupt.
  OCR1A = WSPR_CTC;       // Set up interrupt trigger count;
  interrupts();            // Re-enable interrupts.
}

// Timer interrupt vector.  This toggles the variable we use to gate
// each column of output to ensure accurate timing.  Called whenever
// Timer1 hits the count set in WSPR_CTC
ISR(TIMER1_COMPA_vect)
{
    proceed = true;
}

/****************************************************************************************************************
* Loop through the string, transmitting one character at a time.  
****************************************************************************************************************/
void encode()
{
  uint8_t i;

  // Encode the message in the transmit buffer
  // This is RAM intensive and should be done separately from other subroutines
  set_tx_buffer();

  // Reset the tone to the base frequency and turn on the output
  si5351.output_enable(SI5351_CLK0, 1);

  for(i = 0; i < symbol_count; i++)
  {
    si5351.set_freq((freq * 100) + (tx_buffer[i] * tone_spacing), SI5351_CLK0);
    proceed = false;  
    while(!proceed); // Wait for the timer interrupt to fire
  }

  // Turn off the output
  si5351.output_enable(SI5351_CLK0, 0);
}

/****************************************************************************************************************
* Prepping the TX buffer
*  
****************************************************************************************************************/
void set_tx_buffer()
{
  // Clear out the transmit buffer
  memset(tx_buffer, 0, 255);

  // Check if it is time to transmit
  switch(cur_mode)
  {
  case MODE_WSPR:
    if (UGPS.sendMsg1)
    {
      #if defined(DEVMODE)    
         Serial.print(F("Sending: ")); 
         Serial.print(MYCALL); Serial.print(F(" "));
         Serial.print(UGPS.MH_1); Serial.print(F(" "));
         Serial.println(UGPS.dbm_1);
      #endif
         jtencode.wspr_encode(MYCALL, UGPS.MH_1, UGPS.dbm_1, tx_buffer);
    }
    else
    {
      if (UGPS.sendMsg2)
      {
        #if defined(DEVMODE)    
         Serial.print(F("Sending: ")); 
         Serial.print(UGPS.call_2); Serial.print(F(" "));
         Serial.print(UGPS.MH_2); Serial.print(F(" "));
         Serial.println(UGPS.dbm_2);
        #endif   
        jtencode.wspr_encode(UGPS.call_2, UGPS.MH_2, UGPS.dbm_2, tx_buffer);
      }
    }
  break;
  }
}


/****************************************************************************************************************
* Loop
****************************************************************************************************************/
void loop()
{
  
  // Process data from the GPS about every second
  smartDelay(950);
  CheckGPS(); 
  
  if (UGPS.sendMsg1 || UGPS.sendMsg2)
  {
    // Encode and TX
    freq = WSPR_FREQ_1;
    #if defined(DEVMODE)    
      Serial.print(F("TX..")); Serial.print(F("Frequency: ")); printull(freq);
    #endif
  
    encode();
  
    #if defined(DEVMODE)        
      Serial.println(F(" ->> Done!"));
    #endif
    UGPS.sendMsg1 = false;
    UGPS.sendMsg2 = false;

    // Goto sleep
    // This is currently not implemented but you can program your own power saving code
    if (USE_DEEPSLEEP)
    {
      gotoSleep();
    }
  }
}
