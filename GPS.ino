/*********************************************************************************************************************************/
// Get data from the GPS, Get other variables, generate the two messages, print some debug info
/*********************************************************************************************************************************/
void CheckGPS()
{    
  processGPSData();
  UGPS.temperature = ReadTemp();
  UGPS.volts = ReadExternalVoltage();
  get_satsvalid();
  process_first_message();
  process_second_message();
  printGPSData();
}

/*********************************************************************************************************************************/
// This custom version of delay() ensures that the gps object is being "fed".
/*********************************************************************************************************************************/
static void smartDelay(unsigned long ms)
{
  unsigned long start = millis();
  do 
  {
    while (SerialGPS.available())
      gps.encode(SerialGPS.read());
  } while (millis() - start < ms);
}

/*********************************************************************************************************************************/
// Blink a LED for visual status information. 
// LED should be connected through a resistor on pin LED_PIN 
// Blink 'nr' times for 'ms' milliseconds 
/*********************************************************************************************************************************/
void BlinkLED(unsigned int nr, unsigned int ms)
{
  if (LED_PIN > 0 && UGPS.Altitude <= LED_ALTITUDE)
  {
    int i = 0;

    for (i=0; i<nr; i++)
    {
      digitalWrite(LED_PIN,HIGH);
      delay(ms);
      digitalWrite(LED_PIN,LOW); 
      delay(ms);
    }
  }
}

/*********************************************************************************************************************************/
// Print an unsigned long long to Serial output as the standard Serial.println does not support ULL;
/*********************************************************************************************************************************/
void printull(unsigned long long ull)
{
   char buf[16];
   if(ull > 0xFFFFFFFFLL) 
   {
      sprintf(buf, "%lu%08lu", (unsigned long)(ull>>32), (unsigned long)(ull&0xFFFFFFFFULL));
   } 
   else 
   {
      sprintf(buf, "%lu", (unsigned long)ull);
   }
   Serial.println( buf ); 
}


/*********************************************************************************************************************************/
// Process all data from the GPS and check the GPS time to see if we need to start send WSPR
/*********************************************************************************************************************************/
static void processGPSData()
{
  byte DesiredMode;

  // Get the statyus of the satellites  
  if (gps.satellites.isValid())
  {
    UGPS.Satellites = gps.satellites.value();
  }
  else
  {
    UGPS.Satellites = 0;
  }

 // Time
 if (gps.time.isValid())
 {
    UGPS.Hours = gps.time.hour();
    UGPS.Minutes = gps.time.minute();
    UGPS.Seconds = gps.time.second();
 }
 else
 {
    UGPS.Hours = 0;
    UGPS.Minutes = 0;
    UGPS.Seconds = 0;
 }

 // Position
 if (gps.location.isValid())
 {
    // Set the GPS location
    UGPS.Longitude = gps.location.lng();
    UGPS.Latitude = gps.location.lat();
    UGPS.speed_knots = gps.speed.knots();
    // Translate the GPS position to a Maidenhead location
    Get_MaidenHead();

    // Blink the LED twice for 50ms to show that the GPS location is valid
    BlinkLED(2,50);
    UGPS.validLocation = true;
    
    // Check if it is time to transmit
    if ((UGPS.Seconds <= 2) && (UGPS.Minutes % 10 == minute_message_1)) 
    {
      UGPS.sendMsg1 = true;
      UGPS.sendMsg2 = false;
    } else
    if ((UGPS.Seconds <= 2) && (UGPS.Minutes % 10 == minute_message_2)) 
    {
      UGPS.sendMsg2 = true;
      UGPS.sendMsg1 = false;
    }   
    else
    {
      UGPS.sendMsg2 = false;
      UGPS.sendMsg1 = false;      
    }
 }
 else
 {
   // Invalid GPS location
   UGPS.validLocation = false;
   UGPS.Longitude = 0;
   UGPS.Latitude = 0;
 }

 // Altitude
 if (gps.altitude.isValid())
     UGPS.Altitude = gps.altitude.meters();
 else
    // Invalid altitude
    UGPS.Altitude = 0;    

 if (UGPS.Altitude < 0)
   UGPS.Altitude = 0;      
}

/*********************************************************************************************************************************/
// Print all known GPS and calculated data for debug purposes
/*********************************************************************************************************************************/
void printGPSData()
{
#if defined(DEVMODE)   
  Serial.print(F("       Valid Time: ")); Serial.println(gps.time.isValid()); 
  Serial.print(F("   Valid Location: ")); Serial.println(gps.location.isValid()); 
  Serial.print(F("       Sats valid: ")); Serial.println(UGPS.Sats_valid);
  Serial.print(F("             Time: ")); Serial.print(UGPS.Hours); Serial.print(F(":")); Serial.print(UGPS.Minutes); Serial.print(F(":")); Serial.println(UGPS.Seconds);
  Serial.print(F("         Latitude: ")); Serial.println(UGPS.Latitude, 6);
  Serial.print(F("        Longitude: ")); Serial.println(UGPS.Longitude, 6);
  Serial.print(F("     Altitude (m): ")); Serial.println(UGPS.Altitude);  
  Serial.print(F("       Speed (kn): ")); Serial.println(UGPS.speed_knots);
  Serial.print(F("       Satellites: ")); Serial.println(UGPS.Satellites);
  Serial.print(F("       Maidenhead: ")); Serial.println(MaidenHead);
  Serial.print(F("      Temperature: ")); Serial.println(UGPS.temperature);
  Serial.print(F("             Volt: ")); Serial.println(UGPS.volts);
  Serial.println();
  Serial.println(F("-------------------------"));  
#endif
}


/*********************************************************************************************************************************/
// Calculate the Maidenhead locator out of GPS coordinates
/*********************************************************************************************************************************/
void Get_MaidenHead()
{
  int o1, o2, o3;
  int a1, a2, a3;
  double remainder;
  
  // longitude
  remainder = UGPS.Longitude + 180.0;
  o1 = (int)(remainder / 20.0);
  remainder = remainder - (double)o1 * 20.0;
  o2 = (int)(remainder / 2.0);
  remainder = remainder - 2.0 * (double)o2;
  o3 = (int)(12.0 * remainder);

  // latitude
  remainder = UGPS.Latitude + 90.0;
  a1 = (int)(remainder / 10.0);
  remainder = remainder - (double)a1 * 10.0;
  a2 = (int)(remainder);
  remainder = remainder - (double)a2;
  a3 = (int)(24.0 * remainder);
  MaidenHead[0] = (char)o1 + 'A';
  MaidenHead[1] = (char)a1 + 'A';
  MaidenHead[2] = (char)o2 + '0';
  MaidenHead[3] = (char)a2 + '0';
  MaidenHead[4] = (char)o3 + 'A';
  MaidenHead[5] = (char)a3 + 'A';
  MaidenHead[6] = (char)0;
}

/*********************************************************************************************************************************/
// If there are 5 or more sats available, the software will report to telemetry that the number of
// satellites is sufficient.
/*********************************************************************************************************************************/
void get_satsvalid()
{
  // Sats are valid when number of sats >= 5
  if (UGPS.Satellites >= 5) 
    UGPS.Sats_valid = 1;
  else
   UGPS.Sats_valid = 0;
}
