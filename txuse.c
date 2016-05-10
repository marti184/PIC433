/* 
Author Martin Lints lintsmartin@gmail.com on 2016
Do whatever you want with this software, don't blame me for anything you do.
*/
#include <stdint.h> // for uint16_t
#include "tx.h"

static __code uint16_t __at (_CONFIG) configWord = _INTRC_OSC_NOCLKOUT & _WDT_OFF & _PWRTE_OFF & _MCLRE_OFF & _BODEN_OFF;

#define DOOR    GPIO3   // input pin with interrupt. Alarm when voltage goes away from this pin.
#define DOORTRI TRISIO3 //
#define DOORIOC IOC3    // interrupt setup

#define THIS    0x12    // this device number to be sent 0001 0010
#define ALARM   0x10    // alarm code to be sent 0001 0000

volatile unsigned char adrbuf; // last captured address
volatile unsigned char datbuf; // last captured data


void sendalarm(void)
{
  /* sending an alarm */
  /* for that send a header */
  sendbyte(0xfe); // 7 ones and 1 zero as header
  sendbyte(THIS); // my device address
  sendbyte(ALARM); // general intruder alert code
  delay_ms(10);
}


/* interrupt - something happened */
void Intr(void) __interrupt 0
{
  unsigned char i;
  unsigned char portstate;

  /* did a pin change ? */
  if (GPIF)
    {
      /*is the door open?*/
      if (DOOR)
	{
	  /* send 8 alarm signals */
	  for (i=0; i<8; i++)
	    {
	      sendalarm();
	    }
	}
      else // door is closed, clear GPIF
	{
	  portstate = GPIO; // needs to be read before GPIF can be cleared
	  GPIF = 0;
	}
    }
}


void main(void)
{
  setup();
  DOORIOC = 1; /* set door to interrupt */
  DOORTRI = 1;
  TRISIO1 = 0; // DEBUG pin

  while(1)
    {
      //delay_ms(1);
      /*
      if (DOOR == 1)
	{
	  sendalarm();
	}
      */
      __asm
	sleep
	nop
	nop
	__endasm;
    }
}
