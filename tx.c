/* 
Author Martin Lints lintsmartin@gmail.com on 2016
Do whatever you want with this software, don't blame me for anything you do.
*/

#include "tx.h"

#define TWAIT   215      // put 850 us into timer, 860/4 = 215, 255-215 = 40 (PRESC. 4)


/* initial setup of special function registers */
void setup(void)
{
  /* Option_reg*/
  OPTION_REG = 0x11; /* GPIO en, gp2 int on fall, t0clocksource intern., incr hi-lo , prescal to tmr, prescale 1:4  */
  
  /* setup interrupts */
  INTCON = 0xc8; // GIE 1, PEIE 1, T0IE 0, INTE (GP2) 0, GPIE 1 (vaja), T0IF 0 (miks mitte), INTF 0, GPIF 0
  //GIE = 0;

  /* Calibrate osc*/

  __asm  
    bsf   STATUS, 5 
   call  3FFh 
   movwf OSCCAL
   bcf   STATUS, 5 
    __endasm;

  /* ANSEL off (analog select register)*/
  ANSEL = 0x00; // all analog inputs (last nibble)
  
  /* CMCON off */
  CMCON = 0x07; // comparator off

  /* set TXPIN to output*/
  TXTRI = 0;
}

/* Wait for half period of the Manchester 
 */
void waitT2(void)
{
  TMR0 = 0;
  while (TMR0 < TWAIT) {}
  /*
  TMR0 = TWAIT;  // initialize timer value
  T0IF = 0;  // clear previous overflows
  // now wait 
  while (!T0IF) {}
  */
}


/* send lo-hi as bit 1 */
/* turn on TXPIN for Manchester half period */
/* trying without modulation and with timer */
/* aiming at ~ 850 us                       */
/* REMEMBER TO SET TXPIN TO 0 AFTER SENDING! */
void send1(void)
{
  TXPIN = 0;
  waitT2();   
  __asm
    nop
    nop
    nop
    nop
    __endasm;
  TXPIN = 1;
  waitT2();   
}

/* send hi-lo as bit 0 */
/* turn off TXPIN for Manchester half period */
/* aiming at ~ 850 us                        */
/* REMEMBER TO SET TXPIN TO 0 AFTER SENDING! */
void send0(void)
{
  TXPIN = 1;
  waitT2();   
  __asm
    goto $+1
    goto $+1
    __endasm;
  TXPIN = 0;
  waitT2();   
}


void sendbyte(unsigned char datau)
{
  unsigned char j;
  unsigned char mask = 0x80 ; // starting at 1000 0000
  for (j=0; j<8; j++)
    {
      if (datau&mask) // send 1
	{
	  send1();
	}
      else
	{
	  send0();
	}
      mask >>= 1; // shift mask right one bit
    }
  TXPIN = 0; // reset TXPIN to 0 in case last was send0
}


void delay_ms(unsigned char ms)
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




