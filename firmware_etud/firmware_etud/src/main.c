/*******************************************************************************
  MPLAB Harmony Application Source File
 
  Company:
    Microchip Technology Inc.
 
  File Name:
    main.c

  Summary:
    This file contains the source code for the MPLAB Harmony application.

  Description:
    This file contains the source code for the MPLAB Harmony application.  It
    implements the logic of the application's state machine and it may call
    API routines of other MPLAB Harmony modules in the system, such as drivers,
    system services, and middleware.  However, it does not call any of the
    system interfaces (such as the "Initialize" and "Tasks" functions) of any of
    the modules in the system or make any assumptions about when those functions
    are called.  That is the responsibility of the configuration-specific system
    files.
 *******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2013-2014 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
 *******************************************************************************/
// DOM-IGNORE-END


// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <xc.h>
#include <sys/attribs.h>
#include "main.h"
#include "system_config.h"
#include "system/common/sys_module.h"   // SYS function prototypes
#include "driver/spi/src/dynamic/drv_spi_internal.h"
#include "UDP_app.h"
#include "led.h"
#include "ssd.h"
#include "accel.h"
#include "lcd.h"
#include "app_commands.h"
#include "swt.h"
#include "btn.h"
#include <math.h>
#include "projet_auto.h"


extern int Flag_10us;


void __ISR(_TIMER_2_VECTOR, IPL2AUTO) Timer2ISR(void) 
{  
   Flag_10us = 1;           //    Indique à la boucle principale qu'on doit traiter
  
   IFS0bits.T2IF = 0;     //    clear interrupt flag
}

#define TMR_TIME    0.001             // x us for each tick

void initialize_timer_interrupt(void) { 
  T2CONbits.TCKPS = 0;                //    256 prescaler value
  T2CONbits.TGATE = 0;                //    not gated input (the default)
  T2CONbits.TCS = 0;                  //    PCBLK input (the default)
  PR2 = (int)(((float)(TMR_TIME * PB_FRQ) / 256) + 0.5);   //set period register, generates one interrupt every 1 ms
                                      //    48 kHz * 1 ms / 256 = 188
  TMR2 = 0;                           //    initialize count to 0
  IPC2bits.T2IP = 2;                  //    INT step 4: priority
  IPC2bits.T2IS = 0;                  //    subpriority
  IFS0bits.T2IF = 0;                  //    clear interrupt flag
  IEC0bits.T2IE = 1;                  //    enable interrupt
  T2CONbits.ON = 1;                   //    turn on Timer5
} 

// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    This structure should be initialized by the TCPIP_Initialize function.
   
    Application strings and buffers are be defined outside this structure.
 */

MAIN_DATA mainData;

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************

/* TODO:  Add any necessary callback functions.
 */





/*
void getValues(void)
{
    int i;
    
    memcpy(temp, UDP_Send_Buffer, 484);
    
    for (i = 0; i<=121; i++)
    {
        char buffer[4];
        sprintf(buffer, "%c%c%c%c", temp[(4*i)], temp[(4*i)+1], temp[(4*i)+2], temp[(4*i)+3]);
        
        strncpy(&test[i], buffer, 4);
        //sscanf(test[i], "%d", &j);
        test2[i] = (int)test[i];
        //RGBLED_SetValue((int)test[i+1],(int)test[i+41],(int)test[i+81]);
        //RGBLED_SetValue(0xFF,0,0);
    }
    for (j = 0; j<=40; j++)
    {
        RGBLED_SetValue(test2[j+1], test2[j+41],test2[j+81]);
    }
};

void storeValues(signed int x, signed int y, signed int z)
{
    //SYS_CONSOLE_MESSAGE("\r\nClient: storeValues\r\n");
    array[0] = paquet_nbr;
    array[counter_acl] = x;
    array[counter_acl + 40] = y;
    array[counter_acl + 80] = z;
    
    counter_acl ++;
   
    if (counter_acl > 40)
    {
        PMODS_SetValue(1, 3, 1);
        memcpy(UDP_Send_Buffer,array, 484);
       
        UDP_Send_Packet = true;
        counter_acl = 1;
        paquet_nbr++;
        getValues();
        PMODS_SetValue(1, 3, 0);
    };
}*/




// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************


/* Application's LED Task Function */
static unsigned long int counter=0;
static void LedTask(void) {
    if(counter++ == 20000){
        //LED_ToggleValue(1);
        counter = 0;
    }  
}


static bool sw0_old;
void ManageSwitches()
{
    bool sw0_new = SWITCH0StateGet();
    if((sw0_new != sw0_old) && sw0_new)
    {
        //strcpy(UDP_Send_Buffer, "Bonjour S4\n\r");
        //UDP_bytes_to_send = strlen(UDP_Send_Buffer);
        UDP_Send_Packet = true;      
    }

    sw0_old = sw0_new;
}


// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void MAIN_Initialize ( void )

  Remarks:
    See prototype in main.h.
 */

void MAIN_Initialize ( void )
{
    /* Place the App state machine in its initial state. */
    mainData.state = MAIN_STATE_INIT;

    mainData.handleUSART0 = DRV_HANDLE_INVALID;
    initialize_timer_interrupt();
    PMODS_InitPin(1,2,0,0,0);
    PMODS_InitPin(1,3,1,0,0);
    UDP_Initialize();
    LCD_Init();
    ACL_Init();
    SSD_Init();
    SWT_Init();
    BTN_Init();
}


/******************************************************************************
  Function:
    void MAIN_Tasks ( void )

  Remarks:
    See prototype in main.h.
 */

void MAIN_Tasks ( void )
{

    /* Check the application's current state. */
    switch ( mainData.state )
    {
            /* Application's initial state. */
        case MAIN_STATE_INIT:
        {
            bool appInitialized = true;
            SYS_CONSOLE_MESSAGE("Init\r\n");

            if (mainData.handleUSART0 == DRV_HANDLE_INVALID)
            {
                mainData.handleUSART0 = DRV_USART_Open(MAIN_DRV_USART, DRV_IO_INTENT_READWRITE|DRV_IO_INTENT_NONBLOCKING);
                appInitialized &= (DRV_HANDLE_INVALID != mainData.handleUSART0);
            }


            if (appInitialized)
            {
                mainData.state = MAIN_STATE_SERVICE_TASKS;
            }
            break;
        }

        case MAIN_STATE_SERVICE_TASKS:
        {
            LedTask();
            accel_tasks();
            UDP_Tasks();
            ManageSwitches();
            JB1Toggle();
            break;
        }

            /* The default state should never be executed. */
        default:
        {
            /* TODO: Handle error in application's state machine. */
            break;
        }
    }
}

int main(void) {
   
   
    SYS_Initialize(NULL);
    MAIN_Initialize();
    SYS_INT_Enable();
    SSD_WriteDigitsGrouped(0x0000,0x1);
   
    while (1) {
        SYS_Tasks();
        MAIN_Tasks();
        //PROJET_Tasks();
    };

    return 0;
}



/*******************************************************************************
 End of File
 */

