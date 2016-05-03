/* 
Author Martin Lints lintsmartin@gmail.com on 2016
Do whatever you want with this software, don't blame me for anything you do.
*/

#ifndef TX_H
#define TX_H

#define __12F675
#include "pic12f675.h"

#define TXPIN   GPIO5   
#define TXTRI   TRISIO5 

// these two need to be defined
extern volatile unsigned char adrbuf; // last captured address
extern volatile unsigned char datbuf; // last captured data


void setup(void);
void delay_ms(unsigned char);
void sendbyte(unsigned char);

#endif /* TX_H */

