// main.c
// Line Following Challenge
// Runs on MSP432
// Ivan Ocon Delgado



#include <stdio.h>
#include <stdint.h>
#include "msp.h"
#include "../inc/LaunchPad.h"
#include "../inc/Reflectance.h"
#include "../inc/Clock.h"
#include "../inc/Motor.h"
#include <string.h>
#include "..\inc\UART0.h"
#include "..\inc\EUSCIA0.h"
#include "..\inc\FIFO0.h"
#include "..\inc\CortexM.h"
#include "..\inc\BumpInt.h"
#include "..\inc\UART0.h"


/*(Left,Right) Motors, call LaunchPad_Output (positive logic)
3   1,1     both motors, yellow means go straight
2   1,0     left motor,  green  means turns right
1   0,1     right motor, red    means turn left
0   0,0     both off,    dark   means stop
(Left,Right) Sensors, call LaunchPad_Input (positive logic)
3   1,1     both buttons pushed means on line,
2   1,0     SW2 pushed          means off to right
1   0,1     SW1 pushed          means off to left
0   0,0     neither button      means lost
 */



// Linked data structure
struct State {
    uint32_t out;                // 2-bit output
    uint32_t delay;              // time to delay in 1ms
    const struct State *next[4]; // Next if 2-bit input is 0-3
};
typedef const struct State State_t;



#define Center  &fsm[0]
#define off     &fsm[1]
#define Right1  &fsm[2]
#define Right2  &fsm[3]
#define Right3  &fsm[4]
#define Right4  &fsm[5]
#define Left1   &fsm[6]
#define Left2   &fsm[7]
#define Left3   &fsm[8]
#define Left4   &fsm[9]

#define beat 50
State_t fsm[10]={
                 {0x03, beat, { off, Left1,   Right1,  Center }},  // Center
                 {0x00, beat, { off, Left3,   Right3,  Center }},  // Off
                 {0x01, beat, { off, Left1,   Right2,  Center }},   // Right1
                 {0x03, beat, { off, Left2,   Right3,  Center }},   // Right2
                 {0x01, beat, { off, Left3,   Right4,  Center }},   // Right3
                 {0x03, beat, { off, Left4,   Right1,  Center }},   // Right4
                 {0x02, beat, { off, Left2,   Right1,  Center }},   // Left1
                 {0x03, beat, { off, Left3,   Right2,  Center }},   // Left2
                 {0x02, beat, { off, Left4,   Right3,  Center }},   // Left3
                 {0x03, beat, { off, Left1,   Right4,  Center }},   // Left4
};



State_t *Spt;  // pointer to the current state
uint32_t Input;
uint32_t Output;
/*Run FSM continuously
1) Output depends on State (LaunchPad LED)
2) Wait depends on State
3) Input (LaunchPad buttons)
4) Next depends on (Input,State)
 */
int speed = 4000;
char string[10];
char n;

uint8_t Read_Position (void);
uint8_t CollisionData, CollisionFlag;  // mailbox

void HandleCollision(uint8_t bumpSensor){

    Motor_Stop();
    CollisionFlag = 1;
}


int main(void){
    uint32_t heart=0;
    Clock_Init48MHz();
    Reflectance_Init();
    LaunchPad_Init();
    Motor_Init();
    EUSCIA0_Init();
    BumpInt_Init(&HandleCollision);
    EnableInterrupts();
    Spt = Center;
    while(1){


        if(n == 'r')//start to run
        {
            Output = Spt->out;            // set output from FSM
            switch (Output){
            case 3:
                Motor_Forward(speed,speed);
                break;
            case 1:
                Motor_Left(speed, speed);
                break;
            case 2:
                Motor_Right(speed, speed);
                break;
            case 0:
                Motor_Stop();
                break;
            }

            printf("Output: %d\n\n",Output);
            LaunchPad_Output(Output);     // do output to two motors
            Clock_Delay1ms(Spt->delay);   // wait
            Input = Read_Position();    // read sensors
            Spt = Spt->next[Input];       // next depends on input and state
            heart = heart^1;
            LaunchPad_LED(heart);         // optional, debugging heartbeat
        }

        if((n == 'f') || (CollisionFlag == 1)) //off when collision
        {
            CollisionFlag = 0;
            Motor_Stop();
            EUSCIA0_InString(string,1);
            n = string[0];
        }

        if(n == 'u') //speed up
        {
            speed += 100;
            UART0_OutString("\nSpeed now is:");
            UART0_OutUDec(speed);
            UART0_OutString("\n");
            EUSCIA0_InString(string,1);
            n = string[0];
        }

        if(n == 'd') //speed down
        {
            speed -= 100;
            UART0_OutString("\nSpeed now is:");
            UART0_OutUDec(speed);
            UART0_OutString("\n");
            EUSCIA0_InString(string,1);
            n = string[0];
        }
    }
}

//Read_Position();
uint8_t Read_Position (void)
{
    uint8_t Data;
    int32_t position;
    Data = Reflectance_Read(1000);
    position = Reflectance_Position(Data);
    printf("Position: %d\n",(int)position);
    Clock_Delay1ms(10);
    if(position == 0)
        return 0x03; //Centered line
    if(position > 1)
        return 0x01; //Left line
    if(position < 0)
        return 0x02;  //Right line
    if(position == 1)
        return 0x00;  //All line sensors active, black box detected stop motors
    return 0;

}
// Color    LED(s) Port2
// dark     ---    0
// red      R--    0x01
// blue     --B    0x04
// green    -G-    0x02
// yellow   RG-    0x03
// sky blue -GB    0x06
// white    RGB    0x07
// pink     R-B    0x05
