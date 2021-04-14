/***********************************************************************************
* Use serial console Output for debugging and development
*  
* Change if needed
************************************************************************************/
// Comment out if do not want Serial console output
// Comment out DEVMODE in a production environment as it will degrade performance!
#define DEVMODE

// Your own HAM call. Change it
#define MYCALL "MYCALL"


// If you have a valid GPS lock, the software will make this pin HIGH twice for 20ms
// You could connect a LED to it.
// Set LED_PIN to 0 if you do not want to use a pin
#define LED_PIN LED_BUILTIN  
// LED will only blink if the tracker is below LED_ALTITUDE (in meters)
#define LED_ALTITUDE 1000

// Frequency of the SI5351 Oscillator in Hertz
// Use 0 if you have a 25MHz Oscillator
// #define SI5351FREQ 26000000
#define SI5351FREQ 0


/***********************************************************************************
* All Si5351 chips have an accuracy error
* Use the example calibration program in the etherkit si5351 library to get that correction
* Or use a frequency counter or reliable VFO to check frequency accuracy.
*  
* Correction is in hundreths of Herz.
************************************************************************************/
// Change this value!
#define SI5351_CORRECTION 0


// These constants define when your WSPR calls get transmitted
// First transmit is a standard WSPR transmission with your call and current maidenhead locator
// Second transmit is a special telemetry call which is used to get a more accurate position
// and to log some more info like temperature and voltage. It was designed for tracking high altitude 
// balloons. But you can use it for basically anything alse.
// Read this page for more info about what these constants mean: http://hojoham.blogspot.com/2016/10/known-flight-ids.html
// Read this if you want to know more about the telemetry protocol used: https://qrp-labs.com/flights/s4.html
// ***************************************************************************
//
// IMPORTANT: CHANGE THIS
// read: http://hojoham.blogspot.com/2016/10/known-flight-ids.html
// Your flight-ID should be unique!
//
// ***************************************************************************
const char TELEM_CHAR1 = '1';    // First character of the special callsign of the second rtransmission 
const char TELEM_CHAR2 = '1';    // Third character of the special callsign of the second rtransmission 
const int minute_message_1 = 4;  // Send first message at minute 04,14,24,34,44 and 54
const int minute_message_2 = 6;  // Send telemetry message at minute 06,16,26,36,46 and 56

/***********************************************************************************
* wspr SETTINGS
*  
* Change if needed
************************************************************************************/
// These are well working frequencies for the 10, 20 and 40 meter bands.
// If you want you can use these
//unsigned long long WSPR_FREQ_1 = 28126124UL;   // for 10 meter
unsigned long long WSPR_FREQ_1 = 14097100UL;     // for 20 meter and usually used for HAB's
// unsigned long long WSPR_FREQ_1 = 7040060UL;   // for 40 meter


/***********************************************************************************
* GPS SETTINGS
*  
* Change if needed
************************************************************************************/
// GPS Serial device
// I recommend using the ATMGM336H GPS, which is well available, cheap and has proven to be reliable
// We use SoftwareSerial so these pin numbers are basically free to choose
static const int Rx = 8, Tx = 7; // Arduino pins to connect to Tx and Rx of the GPS
static const uint32_t GPSBaud = 9600;  // GPS BAUD rate. Modern GPS usually 9600, older ones might use 4800.

/***********************************************************************************
* SENSOR SETTINGS
*  
* Change if needed
* 
*  You can connect an external voltage directly to the EXTERNALVOLTAGE_PIN as long as the the pin is rated for that voltage
*  Alteratively, you can use a voltage divider so you can connect a higher voltage, but then you have to calculate the DIVIDER_RATIO yourself
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
#define USE_EXTERNAL_VOLTAGE false  // Set to true if you want to measure an external voltage on the EXTERNALVOLTAGE_PIN 
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
// Set this to true if you want to use deepsleep 
// See power.ini if you want to implement power saving when the module is not transmitting
#define USE_DEEPSLEEP false
