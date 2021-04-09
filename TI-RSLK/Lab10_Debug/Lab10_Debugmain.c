// Lab10_Debugmain.c
// Runs on MSP432
// Student version to Debug lab
// Daniel and Jonathan Valvano
// July 2, 2017
// Interrupt interface for QTR-8RC reflectance sensor array
// Pololu part number 961.
// Debugging dump, and Flash black box recorder

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
// P8.7 Bump5
// P8.6 Bump4
// P8.5 Bump3
// P8.4 Bump2
// P8.3 Bump1
// P8.0 Bump0

// reflectance LED illuminate connected to P5.3
// reflectance sensor 1 connected to P7.0 (robot's right, robot off road to left)
// reflectance sensor 2 connected to P7.1
// reflectance sensor 3 connected to P7.2
// reflectance sensor 4 connected to P7.3 center
// reflectance sensor 5 connected to P7.4 center
// reflectance sensor 6 connected to P7.5
// reflectance sensor 7 connected to P7.6
// reflectance sensor 8 connected to P7.7 (robot's left, robot off road to right)

#include "msp.h"
#include "..\inc\bump.h"
#include "..\inc\Reflectance.h"
#include "..\inc\Clock.h"
#include "..\inc\SysTickInts.h"
#include "..\inc\CortexM.h"
#include "..\inc\LaunchPad.h"
#include "..\inc\FlashProgram.h"

uint8_t sensor[256];
uint8_t bump[256];
uint8_t cnt = 0, tic = 0;
uint8_t semaphore = 0;

void Debug_Init(void){
    // write this as part of Lab 10
    Bump_Init();
    Debug_FlashInit();
}

void Debug_Dump(uint8_t x, uint8_t y){
    // write this as part of Lab 10
    bump[cnt] = y;
    cnt++;
}

void Debug_FlashInit(void){ 
    // write this as part of Lab 10
    Flash_Init(48);
    Flash_Erase(0x00020000);
}

void Debug_FlashRecord(uint16_t *pt){
    // write this as part of Lab 10
    Flash_Erase(0x00020000);
    Flash_WriteArray(pt, 0x00020000, 255);
}

void SysTick_Handler(void){ // every 1ms
    // write this as part of Lab 10
    tic++;
    if(tic < 9)
    {

    }
    if(tic == 9)
    {
        Reflectance_Start();
    }
    if(tic == 10)
    {
        sensor[cnt] = Reflectance_End();
        bump[cnt] = Bump_Read();
        cnt++;
        tic = 0;
        if(cnt == 255)
        {
            semaphore = 1;
            cnt = 0;
        }
    }
}

#define SIZE 256  // feel free to adjust the size
uint16_t Buffer[SIZE];
int main(void){
    // write this as part of Lab 10
    uint16_t i;
    Clock_Init48MHz();
    LaunchPad_Init(); // built-in switches and LEDs
    SysTick_Init(48000,2);
    Debug_Init();
    while(1){
        // write this as part of Lab 10
        if(semaphore == 1)
        {
            for(i = 0; i < 255 ; i++)
            {
                Buffer[i] = ((uint16_t) sensor[i] << 8) + bump[i];
            }
            P2OUT |= 0x01;
            Debug_FlashRecord(Buffer);
            P2OUT &=~ 0x01;
            semaphore = 0;
        }
    }
}

int Program10_1(void){ uint8_t data=0;
Clock_Init48MHz();
Debug_Init();
LaunchPad_Init();
SysTick_Init(480000,2);
    while(1){
        P1->OUT |= 0x01;
        Debug_Dump(data,data+1);// linear sequence
        P1->OUT &= ~0x01;
        data=data+2;
    }
}


// Driver test
#define SIZE 256  // feel free to adjust the size
uint16_t Buffer[SIZE];
int Program10_2(void){ uint16_t i;
Clock_Init48MHz();
LaunchPad_Init(); // built-in switches and LEDs
for(i=0;i<SIZE;i++){
    Buffer[i] = i;//(i<<8)+(255-i); // test data
}
i = 0;
while(1){
    P1->OUT |= 0x01;
    Debug_FlashInit();
    P1->OUT &= ~0x01;
    P2->OUT |= 0x01;
    Debug_FlashRecord(Buffer); // 114us
    P2->OUT &= ~0x01;
    i++;
}
}

int Program10_3(void){
    uint16_t i;
    Clock_Init48MHz();
    LaunchPad_Init(); // built-in switches and LEDs
    for(i=0;i<SIZE;i++){
        Buffer[i] = (i<<8)+(255-i); // test data
    }
    P1->OUT |= 0x01;
    Debug_FlashInit();
    P1->OUT &= ~0x01;
    i = 0;
    while(1){
        P2->OUT |= 0x01;
        Debug_FlashRecord(Buffer);
        P2->OUT &= ~0x01;
        i++;
    }
}
