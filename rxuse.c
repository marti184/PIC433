/* 
Author Martin Lints lintsmartin@gmail.com on 2016
Do whatever you want with this software, don't blame me for anything you do.
*/
#include <stdint.h> // for uint16_t
#include "rx.h"

// this shows that a legit signal was received
#define LED    RA2   // 
#define LEDTRI TRISA2 //


#define GARAGE    0x12    // alarm device number 
#define ALARM     0x10    // alarm code to be sent

volatile unsigned char adrbuf; // last captured address
volatile unsigned char datbuf; // last captured data

static __code uint16_t __at (_CONFIG) configWord = _INTRC_OSC_NOCLKOUT & _WDT_OFF & _PWRTE_OFF & _MCLRE_OFF & _BODEN_OFF & _LVP_OFF & _CP_OFF;

/* got some new data, check it out */
void gotData(void)
{
  if (adrbuf == GARAGE)
    {
      if (datbuf == ALARM) { LED = 1;}
    }
}

/* interrupt - something happened */
void Intr(void) __interrupt 0
{
  unsigned char resi;
  /* do we have an incoming ? */
  GIE = 0; // disable all further interrupts
  // is RB0 ? 
  /*
  if (!INTF) { goto endl; }
  */
  INTF = 0; // clear ext int flag
  // are we still high?
  if (!RXPIN) { return; }
  /* now wait for a new ones
     The RXPIN should still be high, so wait out until it goes low
   */

  /*
    return value from rxData();
    0 - no error, 1 - error in timing preamblue,
    2 - error in sanity of period
    3 - erroneus byteread
    4 - preambule too long
  */
  resi = rxData();
  if (resi != 0)
    {
      return;
    }


  if (adrbuf == GARAGE)
    {
      if (datbuf == ALARM)
	{
	  LED = 1;
	}
    }


  INTE = 1; // re-enable interrupt in case it got disabled
  INTF = 0; // and ext int flag
  GIE = 1;

}

void main(void)
{
  setup();
  LEDTRI = 0;
  LED = 0;

  LED = 1;
  delay_ms(255);
  LED = 0;
  delay_ms(255);
  LED = 1;
  delay_ms(255);
  LED = 0;
  delay_ms(255);
  LED = 1;
  delay_ms(255);
  LED = 0;
  delay_ms(255);
  while(1)
    {

      __asm
	//sleep
	nop
	__endasm;

    }
}
