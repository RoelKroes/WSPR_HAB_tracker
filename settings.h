/***********************************************************************************
* Use serial console Output for debugging and development
*  
* Change if needed
************************************************************************************/
// Comment out if do not want Serial console output
// Comment out DEVMODE in a production environment as it will degrade performance!
#define DEVMODE


/***********************************************************************************  
* Always change this to your HAM call. 
* You need a HAM license to use WSPR in the HAM bands
************************************************************************************/
#define MYCALL "MYCALL"


/***********************************************************************************  
* These constants define when your WSPR calls get transmitted
* First transmit is a standard WSPR transmission with your call and current maidenhead locator
* Second transmit is a special telemetry call which is used to get a more accurate position
* and to log some more info like temperature and voltage. It was designed for tracking high altitude 
* balloons. But you can use it for basically anything alse.
* Read this page for more info about what these constants mean: http://hojoham.blogspot.com/2016/10/known-flight-ids.html
* Read this if you want to know more about the telemetry protocol used: https://qrp-labs.com/flights/s4.html
*
* IMPORTANT: CHANGE THIS
* read: http://hojoham.blogspot.com/2016/10/known-flight-ids.html
* Your flight-ID should be unique!
*
************************************************************************************/
const char TELEM_CHAR1 = '1';    // First character of the special callsign of the second transmission 
const char TELEM_CHAR2 = '1';    // Third character of the special callsign of the second transmission 
const int minute_message_1 = 4;  // Send first message at minute 04,14,24,34,44 and 54
const int minute_message_2 = 6;  // Send telemetry message at minute 06,16,26,36,46 and 56

/***********************************************************************************  
* Adjust this value for your processor speed.
* The value is ued in an interrupt routine to get accurate timing
* Use the following values:
* for a 4 MHz processor:  2668
* for a 8 MHz processor:  5336
* for a 16 MHz processor: 10672
************************************************************************************/
#define WSPR_CTC 5336 


/***********************************************************************************
* WSPR SETTINGS - Change if needed
* 
* These are well working frequencies for the 10, 20 and 40 meter bands.
* If you want you can use these values:
* 28126124ULL // for 10 meter
* 14097100ULL // for 20 meter and usually used for HAB's 
* 7040060ULL  // for 40 meter
* 
* Note that these values are in Hz and should be within the WSPR band
************************************************************************************/
#define WSPR_FREQ 14097100ULL

/***********************************************************************************  
* MORSE SETTINGS - Change if needed   
* 
* When there is no valid GPS fix, the tracker will send a morse message about 
* each minute at the frequency as specified in MORSE_FREQ (a good value is 200Hz above the 
* WSPR frequency)
* Good values:
* for 10 meter: 28126324ULL
* for 20 meter: 14097300ULL
* for 40 meter: 7040460UL
* 
* The MORSE_MESSAGE should have only capitals, numbers or spaces. The Morse speed is in
* words per minute. 12 or 14 are reasonable values.
* 
* If you do not want to use morse, comment out the line "#define USE_MORSE"
************************************************************************************/
#define USE_MORSE
#define MORSE_MESSAGE "MYCALL BALLOON NO GPS"
#define MORSE_SPEED 14
#define MORSE_FREQ 14097300ULL


/***********************************************************************************  
* These are the settings for the internal (or external) LED
*
* If you have a valid GPS lock, the software will make the pin with the LED HIGH twice 
* for 20ms.
* 
* LED will only blink if the tracker is below LED_ALTITUDE (in meters)
* 
* Set LED_PIN to 0 if you do not want to use a pin
************************************************************************************/
#define LED_PIN LED_BUILTIN  
#define LED_ALTITUDE 1000


/***********************************************************************************  
* Frequency of the SI5351 Oscillator in Hertz (usually printed on the breakout board)
* for example: #define SI5351FREQ 26000000
* Use 0 if you have a 25MHz Oscillator
************************************************************************************/
#define SI5351FREQ 0


/***********************************************************************************
* All Si5351 IC's in combination with their oscillator have an accuracy error.
* Usually this is not a problem but for WSPR it can be a problem as accuracy is very
* important.
* 
* Use the example calibration program in the etherkit si5351 library to get that correction
* Or use a frequency counter or reliable VFO to check frequency accuracy.
*  
* If your si5351 transmits too low, use a negative value 
* If your si5351 transmits too high, use a positive value
* Correction is in 1/100 Herz.
************************************************************************************/
#define SI5351_CORRECTION -12000


/***********************************************************************************
* GPS SETTINGS - Change if needed
* 
* You will need a serial GPS device like a Ublox NEO6, NEO7, NEO8 or an ATGM336H.
* I recommend using the ATMGM336H GPS, which is well available, very cheap and has proven 
* to be reliable at high altitudes and needs no extra programming
* We use SoftwareSerial so these pin numbers are basically free to choose
************************************************************************************/
static const int Rx = 8, Tx = 7;       // Arduino pins to connect to Tx and Rx of the GPS
static const uint32_t GPSBaud = 9600;  // Modern GPS usually use 9600, older ones 4800.

/***********************************************************************************
* SENSOR SETTINGS
*  
* Change if needed
* 
*  You can connect an external voltage directly to the EXTERNALVOLTAGE_PIN as long as 
*  the the pin is rated for that voltage.
*  Alteratively, you can use a voltage divider so you can connect a higher voltage, but 
*  then you have to calculate the DIVIDER_RATIO yourself
*  
*  Voltage divider schema:
*  
*                          |-----------------
*                          |                |
*                          |               R1            
*                          |                |
*                    +/+ ---                |
*    to external voltage                    |------ To EXTERNALVOLTAGE_PIN
*                    -/- ---                |
*                          |                |
*                          |               R2  
*                          |                |
*                          |----------------------- To Arduino GND
*                          
*   DIVIDER_RATIO can be calculated by (R1+R2) / R2                       
*   
*   Optionally add a 100nF capacitor between A1 and GND if the measured voltage seems unstable
************************************************************************************/
#define USE_EXTERNAL_VOLTAGE false // Set to true if you want to measure an external voltage on the EXTERNALVOLTAGE_PIN 
#define VCC_OFFSET 0.00            // Offset for error correction in Volts for the internal voltage. Ideally this should be 0.0 but usually is between -0.1 and +0.1 and is chip specific. 
#define EXT_OFFSET 0.00            // Offset for error correction in Volts for the external voltage. Use it to correct errors when necessary.
#define EXTERNALVOLTAGE_PIN A0     // Pin to read the external voltage from
#define SAMPLE_RES 1024            // 1024 for Uno, Mini, Nano, Mega, Micro. Leonardo. 4096 for Zero, Due and MKR  
#define DIVIDER_RATIO 3.083        // Leave at 1.00 when using no voltage divider. Set to (R1+R2)/R2 when using a voltage divider.
                                   // R1 = 7500 /  R2 = 3600
/***********************************************************************************
* POWER settings
*  
* Change if needed
************************************************************************************/
// Set this to true if you want to use deepsleep for power saving 
// See power.ini if you want to implement power saving when the module is not transmitting
#define USE_DEEPSLEEP false
