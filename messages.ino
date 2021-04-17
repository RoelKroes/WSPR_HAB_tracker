//******************************************************
// Generate the messages to encode into the telemetry messages
// Read more about the used protocol here:
// https://qrp-labs.com/flights/s4.html
//
// First mesage is a standard WSPR message
//******************************************************
void process_first_message()
{
  // Second get the first 4 maidenhead characters
  for (int j=0; j<4; j++)
  {
    UGPS.MH_1[j] = MaidenHead[j];
  }
  
  // Third get the dbm value which is 10mW for the si5351
  UGPS.dbm_1 = 10;

#if defined(DEVMODE)
  Serial.print(F("First message: "));
  Serial.print(MYCALL); Serial.print(F(" "));
  Serial.print(UGPS.MH_1); Serial.print(F(" "));
  Serial.println(UGPS.dbm_1);
#endif  

}

//******************************************************
// Generate the message to encode into telemetry into the wspr protocol
// 
// This message is encoded to contain more telemetry messages
//******************************************************
void process_second_message()
{
    
  long lat, lon, oldlon, oldlat;

  lon = (UGPS.Longitude * 100000) + 18000000L;
  lat = (UGPS.Latitude * 100000) +  9000000L;

  char MH[2] = {'A', 'A'};
  MH[0] += ((lon % 200000) / 8333);
  MH[1] += ((lat % 100000) / 4166);

  // See also : http://hojoham.blogspot.com/2016/10/known-flight-ids.html
  UGPS.call_2[0] = TELEM_CHAR1; // telemetry channel 
  UGPS.call_2[2] = TELEM_CHAR2; // telemetry channel  

  int a = MH[0] - 'A';
  int b = MH[1] - 'A';
  int c = a * 24 + b;
  int d = int(UGPS.Altitude / 20);

  long e = (long)(1068L * c + d);
  long f = float(e / 17576L);

  if (f < 10)
    UGPS.call_2[1] = f + '0';
  else
    UGPS.call_2[1] = f - 10 + 'A';

  long g = e - f * 17576L;
  int h = int(g / 676);
  UGPS.call_2[3] = h + 'A';

  long i = g - h * 676L;
  int j = int(i / 26);
  UGPS.call_2[4] = j + 'A';

  long k = i - j * 26L;
  int l = int(k / 1);
  UGPS.call_2[5] = l + 'A';
  
  int GPS = 0;
  if ((lon != oldlon) || (lat != oldlat))
    GPS = 1;
  else
    GPS = 0;

  oldlon = lon;
  oldlat = lat;

  int temp_raw = (int)(1024L * (UGPS.temperature * 0.01 + 2.73)) / 5;
  temp_raw = (int)(temp_raw - 457) / 2;

  float batt_raw = (int)(1024L * UGPS.volts) / 5;

  batt_raw = (batt_raw - 614) / 10;

  float round_number = round(batt_raw);
  if (round_number > batt_raw) round_number = round_number - 1;

  batt_raw = (int)round_number;

  long x = (temp_raw * 40L ) + batt_raw;
  long y = (x * 42L) + (int)UGPS.speed_knots / 2;
  long z = (y * 2L) + (int)GPS;
  long xy = (z * 2L) + (int)UGPS.Sats_valid;

  long aa = (int)(xy / 34200L);
  UGPS.MH_2[0] = aa + 'A';

  long bb = xy - (34200L * aa);
  long cc = (int)(bb / 1900L);
  UGPS.MH_2[1] = cc + 'A';

  long dd = bb - (1900L * cc);
  long ee = (int)(dd / 190L);
  UGPS.MH_2[2] = ee + '0';

  long ff = dd - (190L * ee);
  long gg = (int) (ff / 19L);
  UGPS.MH_2[3] = gg + '0';

  UGPS.dbm_2  = ff - (gg * 19L);

  if (UGPS.dbm_2 == 0) UGPS.dbm_2  = 0;
  else if (UGPS.dbm_2 == 1) UGPS.dbm_2  = 3;
  else if (UGPS.dbm_2  == 2) UGPS.dbm_2  = 7;
  else if (UGPS.dbm_2  == 3) UGPS.dbm_2  = 10;
  else if (UGPS.dbm_2  == 4) UGPS.dbm_2  = 13;
  else if (UGPS.dbm_2  == 5) UGPS.dbm_2  = 17;
  else if (UGPS.dbm_2  == 6) UGPS.dbm_2  = 20;
  else if (UGPS.dbm_2  == 7) UGPS.dbm_2  = 23;
  else if (UGPS.dbm_2  == 8) UGPS.dbm_2  = 27;
  else if (UGPS.dbm_2  == 9) UGPS.dbm_2  = 30;
  else if (UGPS.dbm_2  == 10) UGPS.dbm_2  = 33;
  else if (UGPS.dbm_2  == 11) UGPS.dbm_2  = 37;
  else if (UGPS.dbm_2  == 12) UGPS.dbm_2  = 40;
  else if (UGPS.dbm_2  == 13) UGPS.dbm_2  = 43;
  else if (UGPS.dbm_2  == 14) UGPS.dbm_2  = 47;
  else if (UGPS.dbm_2  == 15) UGPS.dbm_2  = 50;
  else if (UGPS.dbm_2  == 16) UGPS.dbm_2  = 53;
  else if (UGPS.dbm_2  == 17) UGPS.dbm_2  = 57;
  else if (UGPS.dbm_2  == 18) UGPS.dbm_2  = 60;

#if defined(DEVMODE)
 Serial.print(F("Second message: "));
 Serial.print(UGPS.call_2); Serial.print(F(" "));
 Serial.print(UGPS.MH_2); Serial.print(F(" "));
 Serial.println(UGPS.dbm_2);
#endif 
}
