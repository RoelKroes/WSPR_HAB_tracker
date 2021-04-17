/**************************************************************************************
 *  This module takes care of sending a morse message about each 60 seconds when there 
 *  is no GPS fix.
 *  
 *  Based on the code of: Thomas S. Knutsen <la3pna@gmail.com>
 *  https://github.com/la3pna/si5351_beacon/blob/master/si5351_beacon.ino
 *  
 **************************************************************************************/

struct t_mtab {
  char c, pat;
} ;

struct t_mtab morsetab[] = {
  {'.', 106},
  {',', 115},
  {'?', 76},
  {'/', 41},
  {'A', 6},
  {'B', 17},
  {'C', 21},
  {'D', 9},
  {'E', 2},
  {'F', 20},
  {'G', 11},
  {'H', 16},
  {'I', 4},
  {'J', 30},
  {'K', 13},
  {'L', 18},
  {'M', 7},
  {'N', 5},
  {'O', 15},
  {'P', 22},
  {'Q', 27},
  {'R', 10},
  {'S', 8},
  {'T', 3},
  {'U', 12},
  {'V', 24},
  {'W', 14},
  {'X', 25},
  {'Y', 29},
  {'Z', 19},
  {'1', 62},
  {'2', 60},
  {'3', 56},
  {'4', 48},
  {'5', 32},
  {'6', 33},
  {'7', 35},
  {'8', 39},
  {'9', 47},
  {'0', 63}
} ;

#define N_MORSE  (sizeof(morsetab)/sizeof(morsetab[0]))

#define SPEED  (MORSE_SPEED)
#define DOTLEN  (1200/SPEED)
#define DASHLEN  (3*(1200/SPEED))

//******************************************************
// Send a dash
//******************************************************
void dash()
{
  si5351.output_enable(SI5351_CLK0, 1);
  delay(DASHLEN);
  si5351.output_enable(SI5351_CLK0, 0);
  delay(DOTLEN) ;
}

//******************************************************
// Send a dit
//******************************************************
void dit()
{ 
  si5351.output_enable(SI5351_CLK0, 1);
  delay(DOTLEN);
  si5351.output_enable(SI5351_CLK0, 0);
  delay(DOTLEN);
}


//******************************************************
// Process a character in Morse
//******************************************************
void send(char c)
{
  int i ;
  if (c == ' ') 
  {
    delay(7 * DOTLEN) ;
    return ;
  }
  for (i = 0; i < N_MORSE; i++) 
  {
    if (morsetab[i].c == c) 
    {
      unsigned char p = morsetab[i].pat ;
      while (p != 1) 
      {
        if (p & 1)
          dash() ;
        else
          dit() ;
        p = p / 2 ;
      }
      delay(2 * DOTLEN) ;
      return ;
    }
  }
}

//******************************************************
// Send *str in Morse
//******************************************************
void send_morse_msg(const char *str)
{
#if defined(DEVMODE)   
  Serial.print(F("Sending Morse: ")); 
  Serial.print(str);
#endif
  while (*str)
  {
    send(*str++) ;
  }
#if defined(DEVMODE)   
  Serial.println(F("  <<-- Done!")); 
#endif  
    Serial.print(str);
}
