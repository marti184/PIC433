/* 
Author Martin Lints lintsmartin@gmail.com on 2016
Do whatever you want with this software, don't blame me for anything you do.
*/

#include "rx.h"

#define TWAIT   54      // put 850 us into timer, 860/16 = 54
#define TWA     40      // TWAIT*3/4 sensible minimum wait
#define TWB     68      // TWAIT*5/4 sensible maximum wait

volatile unsigned char tper; // full measured period
volatile unsigned char t4; // quarter measured period t/4
volatile unsigned char ta; // measurement before transition a) t - t/4
volatile unsigned char tb; // measurement after transition b) t + t/4


void delay_ms(unsigned int ms)
{
  unsigned char u;
  while (ms--)
    {
      /* Inner loop takes about 10 cycles per iteration + 4 cycles setup*/
      for (u=0; u < 100; u++)
	{
	  __asm nop __endasm;
	}
    }
}



/* initial setup of special function registers */
void setup(void)
{
  /* Option_reg*/
  /* PORTB pullup disable 1 , int on rise RB0 1, t0cs int 0, 
   tmr0 edge x, presc to tmr0 0, presc 1:16 011 - 110x 0011 */
  OPTION_REG = 0xc3; 
  
  /* setup interrupts */
  INTCON = 0xd0; // GIE 1, PEIE 1, T0IE 0, INTE (RB0) 1, RBIE 0 , T0IF 0 (miks mitte), INTF 0, GPIF 0

  /* CMCON off */
  CMCON = 0x07; // comparators off

  /* set rx pin to input*/
  RXTRI = 1;
}


/* 
 * check the sanity of measured period number
 */
unsigned char sanityCheck(unsigned char tp)
{
  if (tp < (2*TWA)) {  return 0;}
  if (tp > (2*TWB)) {  return 0;}
  return 1;
}


/* reads a new bit and wastes time from current moment on
 * TMR0 should be running and set since last read
 * Will reset TMR0 on successful read 
 */
unsigned char readBit()
{
  unsigned char i;
  while (TMR0 < ta) {} // nothing will happen
  i = RXPIN; // save initial state.
  while (RXPIN == i) // while nothing has happened, spend time
    {
      if (TMR0 > tb) { return 2; } // outatime
    }
  // now something has happened, RXPIN is not like it was at the beg.
 
  /*******************************************************/
  /* THIS SHOULD BE THE ONLY RESET OF TMR0 IN READ CYCLE */
  /*******************************************************/
  TMR0 = 0; // got a change of RXPIN, restart TMR0
  /*******************************************************/

  if (i == 0) { return 1;} // went from 0 to 1, or bit 1
  if (i == 1) { return 0;} // went from 1 to 0, or bit 0
  return 2; // none of the above? error
}


unsigned char rxData(void)
{
  unsigned char rb;
  int j;
  TMR0 = 0;
  while (RXPIN) { if (TMR0 > (TWB)) return 1;} // wait until it goes low
  // now the first lo-hi 1 is done, do another 

  // second bit 1
  //while (!INTF)
  TMR0 = 0;
  while (!RXPIN)  { if (TMR0 > (TWB)) { return 1;}} // lo of lo-hi
  while (RXPIN)  { if (TMR0 > (2*TWB)) { return 1;}} // hi of lo-hi

  // now we have a fresh start with a true zero TMR0 at low-high transition
  /* clear timer, we are low now */
  TMR0 = 0; // and immidiately start timer
  T0IF = 0; // clearing any possible overflows
  INTF = 0; // and ext int flag

  // third bit 1, now with timing
  while (!RXPIN) { if (TMR0 > (TWB)) return 1; } // 3rd lo of lo-hi 1
  while (RXPIN) { if (TMR0 > (2*TWB)) return 1; } // 3rd hi of lo-hi 1
  tper = TMR0; // measured full period of low and high

  /* clear timer */  

  TMR0 = 0; // and immidiately start timer
  T0IF = 0; // clearing any possible overflows
  INTF = 0; // and ext int flag

  /* now we have some time to work, check sanity */
  if (!sanityCheck(tper)) {return 2; }
  
  /* find half-periods, quarter p. , new ta, tb */
  t4 = tper >> 2;
  /* transitions should occur between the two */
  ta = tper - t4; // measurement time a
  tb = tper + t4; // measurement time b

  /*delay another half period and then start counting at the transition*/
  while (!RXPIN) { if (TMR0 > (TWB)) return 1; } // 4th lo of lo-hi 1
  TMR0 = 0; // reset timer at transition
  
   
  /* start measuring and discarding 1-s until find 0 */
  // wait until we get hi-lo, not lo-hi
  j = 0;

  while (1)
    {
      rb = readBit();
      if (rb == 0) { goto address;} // end of preambule
      if (rb > 1) { return 3;} // error
      
      // test if we don't loop too long
      j++;
      if (j>8) { return 4;} // error
    }

 address:
  adrbuf = 0; // clear last address 
  for (j=7; j>=0; j--)
    {
      rb = readBit();
      if ( rb > 1) {return 3;} 
      adrbuf |= rb << j; // capture into address buffer
    }
 	    

 datacap:

  datbuf = 0; // clear last address 
  for (j=7; j>=0; j--)
    {
      rb = readBit();
      if ( rb > 1) { return 3;} // error
      datbuf |= rb << j; // capture into address buffer
    }

  return 0;
}



