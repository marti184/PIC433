/* 
Author Martin Lints lintsmartin@gmail.com on 2016
Do whatever you want with this software, don't blame me for anything you do.
*/

#ifndef RX_H
#define RX_H

#define __16F628A
#include "pic16f628a.h"

// to RX pin of the module. RB0 is the only suitable interrupt pin on 
// 16f628a
#define RXPIN   RB0   // 
#define RXTRI   TRISB0 //

// these two need to be defined. They are modified by rxData function
extern volatile unsigned char adrbuf; // last captured address
extern volatile unsigned char datbuf; // last captured data

void delay_ms(unsigned int ms);
void setup(void);
unsigned char rxData(void); // returns 0 if success

#endif /* RX_H */
