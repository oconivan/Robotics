// BumpInt.c
// Runs on MSP432, interrupt version
// Provide low-level functions that interface bump switches the robot.
// Daniel Valvano and Jonathan Valvano
// July 2, 2017

/* This example accompanies the books
   "Embedded Systems: Introduction to the MSP432 Microcontroller",
       ISBN: 978-1512185676, Jonathan Valvano, copyright (c) 2017
   "Embedded Systems: Real-Time Interfacing to the MSP432 Microcontroller",
       ISBN: 978-1514676585, Jonathan Valvano, copyright (c) 2017
   "Embedded Systems: Real-Time Operating Systems for ARM Cortex-M Microcontrollers",
       ISBN: 978-1466468863, , Jonathan Valvano, copyright (c) 2017
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/

Simplified BSD License (FreeBSD License)
Copyright (c) 2017, Jonathan Valvano, All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are
those of the authors and should not be interpreted as representing official
policies, either expressed or implied, of the FreeBSD Project.
 */

// Negative logic bump sensors
// P4.7 Bump5, left side of robot
// P4.6 Bump4
// P4.5 Bump3
// P4.3 Bump2
// P4.2 Bump1
// P4.0 Bump0, right side of robot

#include <stdint.h>
#include "msp.h"
#include "stdio.h"

void (*function)(uint8_t);
// Initialize Bump sensors
// Make six Port 4 pins inputs
// Activate interface pullup
// pins 7,6,5,3,2,0 = 1110 1101
// Interrupt on falling edge (on touch)
void BumpInt_Init(void(*task)(uint8_t)){
    // write this as part of Lab 14
    function = task;
    P4DIR &=~0xED;
    P4REN |= 0xED;
    P4IE  |= 0xED;
    P4IES |= 0xED;
    P4IFG = 0;
    // Enable Port 1 interrupt on the NVIC
    NVIC->ISER[1] = 1 << ((PORT4_IRQn) & 31);

}
// Read current state of 6 switches
// Returns a 6-bit positive logic result (0 to 63)
// bit 5 Bump5
// bit 4 Bump4
// bit 3 Bump3
// bit 2 Bump2
// bit 1 Bump1
// bit 0 Bump0
uint8_t Bump_Read(void){
    // write this as part of Lab 14
    uint8_t read = 0;
    if((P4IN & 0x01) == 0x00)
    {
        read |= BIT0;
    }
    if((P4IN & 0x04) == 0x00)
    {
        read |= BIT1;
    }
    if((P4IN & 0x08) == 0x00)
    {
        read |= BIT2;
    }
    if((P4IN & 0x20) == 0x00)
    {
        read |= BIT3;
    }
    if((P4IN & 0x40) == 0x00)
    {
        read |= BIT4;
    }
    if((P4IN & 0x80) == 0x00)
    {
        read |= BIT5;
    }

    return read; // replace this line
}
// we do not care about critical section/race conditions
// triggered on touch, falling edge
void PORT4_IRQHandler(void){
    // write this as part of Lab 14
      function(Bump_Read());

    uint8_t bumper = Bump_Read();
    if(bumper & BIT0)
        printf("Bump 1\n");
    if(bumper & BIT1)
        printf("Bump 2\n");
    if(bumper & BIT2)
        printf("Bump 3\n");
    if(bumper & BIT3)
        printf("Bump 4\n");
    if(bumper & BIT4)
        printf("Bump 5\n");
    if(bumper & BIT5)
        printf("Bump 6\n");

    int i;
    for (i = 100000 ; i > 0 ; i--)
        P4IFG = 0;
}

