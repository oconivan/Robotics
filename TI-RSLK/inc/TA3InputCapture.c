// TA3InputCapture.c
// Runs on MSP432
// Use Timer A3 in capture mode to request interrupts on rising
// edges of P10.4 (TA3CCP0) and P8.2 (TA3CCP2) and call user
// functions.
// Daniel Valvano
// May 30, 2017

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

// external signal connected to P8.2 (TA3CCP2) (trigger on rising edge)
// external signal connected to P10.4 (TA3CCP0) (trigger on rising edge)
#include <stdio.h>
#include <stdint.h>
#include "msp.h"

void ta3dummy(uint16_t t){};       // dummy function
void (*CaptureTask0)(uint16_t time) = ta3dummy;// user function
void (*CaptureTask2)(uint16_t time) = ta3dummy;// user function

//------------TimerA3Capture_Init------------
// Initialize Timer A3 in edge time mode to request interrupts on
// the rising edges of P10.4 (TA3CCP0) and P8.2 (TA3CCP2).  The
// interrupt service routines acknowledge the interrupt and call
// a user function.
// Input: task0 is a pointer to a user function called when P10.4 (TA3CCP0) edge occurs
//              parameter is 16-bit up-counting timer value when P10.4 (TA3CCP0) edge occurred (units of 0.083 usec)
//        task2 is a pointer to a user function called when P8.2 (TA3CCP2) edge occurs
//              parameter is 16-bit up-counting timer value when P8.2 (TA3CCP2) edge occurred (units of 0.083 usec)
// Output: none
// Assumes: low-speed subsystem master clock is 12 MHz
void TimerA3Capture_Init(void(*task0)(uint16_t time), void(*task2)(uint16_t time)){
    // write this as part of lab 16
    CaptureTask0 = task0;
    CaptureTask2 = task2;

    //Init P10.4 (TA3.0)
    P10DIR &=~0x30;
    P10SEL0 |= 0x30;
    //Init P8.2 (TA3.2)
    P8DIR &=~0x04;
    P8SEL0 |= 0x04;
    // Timer0_A3 Setup
    TIMER_A3->CCTL[0] = TIMER_A_CCTLN_CM_1 | // Capture rising edge,
            TIMER_A_CCTLN_CCIS_0 |          // Use CCI2A=ACLK,
            TIMER_A_CCTLN_CCIE |            // Enable capture interrupt
            TIMER_A_CCTLN_CAP |             // Enable capture mode,
            TIMER_A_CCTLN_SCS;              // Synchronous capture

    TIMER_A3->CCTL[1] = TIMER_A_CCTLN_CM_1 | // Capture rising edge,
            TIMER_A_CCTLN_CCIS_0 |          // Use CCI2A=ACLK,
            TIMER_A_CCTLN_CCIE |            // Enable capture interrupt
            TIMER_A_CCTLN_CAP |             // Enable capture mode,
            TIMER_A_CCTLN_SCS;              // Synchronous capture

    TIMER_A3->CTL |= TIMER_A_CTL_TASSEL_2 | // Use SMCLK as clock source,
            TIMER_A_CTL_MC__CONTINUOUS |    // Start timer in continuous mode
            TIMER_A_CTL_CLR;                // clear TA0R
    NVIC->ISER[0] = 1 << ((TA3_0_IRQn) & 31);
    NVIC->ISER[0] = 1 << ((TA3_N_IRQn) & 31);



}

void TA3_0_IRQHandler(void){
    // write this as part of lab 16
    uint16_t time = TIMER_A3->CCR[0];
    CaptureTask0(time);
    // Clear the interrupt flag
    TIMER_A3->CCTL[0] &= ~(TIMER_A_CCTLN_CCIFG);
}

void TA3_N_IRQHandler(void){
    // write this as part of lab 16
    CaptureTask2(TIMER_A3->CCR[1]);
    // Clear the interrupt flag
    TIMER_A3->CCTL[1] &= ~(TIMER_A_CCTLN_CCIFG);
}

