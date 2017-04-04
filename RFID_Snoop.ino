
// digital pin 2 has the signal
volatile int buffindex=0;
volatile int lastreading=0;
volatile char sumoflastreading=0;
char buffer[1200];
volatile int processbuffer=0;

// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(115200);
  Serial.println("Looking for HID codes...");
  attachInterrupt(0, measurewave, RISING); // call measurewave when pin 2 is rising

  noInterrupts(); // disable all interrupts
  TCCR1A = 0;
  TCCR1B = 0;
  
  TCNT1 = 0; // preload timer, max is 65535
  TCCR1B |= (1 << CS11); // divide clock by 8... do << B011 for divide by 64
  //TIMSK1 |= (1 << TOIE1); // enable timer overflow interrupt
  interrupts(); // enable all interrupts
}
/*
ISR(Timer1_OVF_vect) // interrupt service routine that wraps a user defined function supplied by attachInterrupt
{
TCNT1 = 34286; // preload timer
digitalWrite(ledPin, digitalRead(ledPin) ^ 1);
}
*/

void measurewave()
{
  int thiscount = TCNT1;
  TCNT1 = 0;
  
  // > 140 is a 0, < 140 is a 1
  // there are 5 long pulses or seven short pulses per bit
  
  if (thiscount < 140)  
  {
    if (lastreading) // 1 and 1, increment and continue
    {
      sumoflastreading++;
    }
    else  // state changed to a 1, record zeros...
    {
      buffer[buffindex] = 1;
      buffindex++;
      if (sumoflastreading > 6) // two zeros
      {
        buffer[buffindex] = 1;
        buffindex++;
      }
      if (sumoflastreading > 11) // three zeros
      {
        buffer[buffindex] = 1;
        buffindex++;
      }        
      if (sumoflastreading > 17) // four zeros
      {
        buffer[buffindex] = 1;
        buffindex++;
      }        
      lastreading = 1;
      sumoflastreading = 0;
    }
  }
  else // 0 is this reading
  {
    if (lastreading) // 0 and 1, state changed to a zero
    {
      buffer[buffindex] = 0;
      buffindex++;
      if (sumoflastreading > 7) // two ones
      {
        buffer[buffindex] = 0;
        buffindex++;
      }
      if (sumoflastreading > 14) // three ones
      {
        buffer[buffindex] = 0;
        buffindex++;
      }        
      if (sumoflastreading > 21) // four ones
      {
        buffer[buffindex] = 0;
        buffindex++;
      }        
      lastreading = 0;
      sumoflastreading = 0;
    }
    else  // state changed...
    {
      sumoflastreading++;
    }
  }
  if (buffindex > 1150)
  {
    processbuffer = 1;
  }
}

// the loop routine runs over and over again forever:
void loop() 
{
  if (processbuffer)
  {
    detachInterrupt(0); // stop calling measurewave
    //Serial.println("Full Buffer:");
    //kevprintbuffer(buffindex);
    checkforhidmessage();
    buffindex = 0;
    processbuffer = 0;
    attachInterrupt(0, measurewave, RISING); // call measurewave when pin 2 is rising

  }
  delay(1);        // delay in between reads for stability
  //kevprintbuffer(buffindex);
}

void kevprintbuffer (int maxlen)
{
  Serial.println("Buffer:");
  for (int foo = 0; foo < maxlen; foo++)
  {
    Serial.print(buffer[foo],DEC);
    //Serial.print(",");
  }
  Serial.println("");
}

void checkforhidmessage ()
{
  //Serial.print ("called checkforhidmessage at ");
  //Serial.println (buffindex);
  //look for 00011101 preamble
  for (int i = 0; i < (buffindex-94); i++)
  {
    if ( !buffer[i] && !buffer[i+1] && !buffer[i+2] && buffer[i+3] && buffer[i+4] && buffer[i+5]  && !buffer[i+6] && buffer[i+7] ) // preambe found
    {
      checkformanchesterat(i+8);
    }
  }
}

void checkformanchesterat(int startindex)
{
  // skip ahead to the 
  // skip 18 bits (post manchester)
  // 19th bit is Even Parity
  // bits 20-27 are facility code
  // bits 28-43 are card no
  // bit 44 is Odd Parity
  //Serial.print ("called checkformanchster at ");
  //Serial.println (startindex);
  // first check if all the bits are manchester, if so just use first bit or the pair
  for (int y = startindex; y < (startindex + 88); y = y+2)
  {
    if (buffer[y] == buffer[y+1])
    {
      return; // not manchester
    }
  }
  
  int sitecode = buffer[startindex+16] + (2 * buffer[startindex+14]) + (4 * buffer[startindex+12]) + (8 * buffer[startindex+10]) + (16 * buffer[startindex+8]) + (32 * buffer[startindex+6]) + (64 * buffer[startindex+4]) + (128 * buffer[startindex+2]);
  int loopcount = 1;
  int cardno = 0;
  for (int i = startindex + 84; i > (startindex + 84 - 32); i = i - 2)
  {
    cardno = cardno + (loopcount * buffer[i]);
    loopcount = loopcount * 2;
  }
  
  for (int foo = (startindex); foo < (startindex + 88); foo++)
  {
    Serial.print(buffer[foo],DEC);
    //Serial.print(",");
  }
  Serial.println("");
  //Serial.print ("Sitecode: ");
  //Serial.println(sitecode);
  Serial.print ("Cardno: ");
  Serial.println(cardno);
  return;
}
  

