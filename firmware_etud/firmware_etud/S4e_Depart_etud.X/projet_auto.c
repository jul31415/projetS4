#include "projet_auto.h"

#include <xc.h>
#include <sys/attribs.h>
#include <stdbool.h>
#include <stdint.h>
#include "config.h"
#include "i2c.h"
#include "accel.h"
#include "system_definitions.h"
#include <stdio.h>
#include "lcd.h"
#include "ssd.h"
#include "app_commands.h"
#include <math.h>



void affichagePmodLed(int diff){
    if (diff<50){
        LED_SetGroupValue(1);
    }
    else if (diff<100){
        LED_SetGroupValue(3);
    }
    else if (diff<150){
        LED_SetGroupValue(7);
    }
    else if (diff<200){
        LED_SetGroupValue(15);
    }
    else if (diff<250){
        LED_SetGroupValue(31);
    }
    else if (diff<300){
        LED_SetGroupValue(63);
    }
    else if (diff<350){
        LED_SetGroupValue(127);
    }
    else if (diff>=400){
        LED_SetGroupValue(255);
    }
}



void projet_tasks(int accelX, int accelY, int accelZ)
{
    values.acl_x = accelX;
    values.acl_y = accelY;
    values.acl_z = accelZ;
                
    values.acl_x_dif = values.acl_x - values.acl_x_mem;
    values.acl_x_mem = values.acl_x;
    values.acl_y_dif = values.acl_y - values.acl_y_mem;
    values.acl_y_mem = values.acl_y;
    values.acl_z_dif = values.acl_z - values.acl_z_mem;
    values.acl_z_mem = values.acl_z;
    
    values.acl = sqrt((values.acl_x_dif*values.acl_x_dif) + (values.acl_y_dif*values.acl_y_dif) + (values.acl_z_dif*values.acl_z_dif)); 
    affichagePmodLed(values.acl);
    values.weighted_avg = values.weighted_avg * (35/36);
    values.weighted_avg = values.weighted_avg + values.acl;
    
    fct_swCheck(SWT_GetGroupValue());

    //Calcul theta (math.h)

    //Calcul phi (math.h))
}



void fct_writeText(char *string, int line, int index)
{ 
    LCD_DisplayClear();     // Clear what was previously written.
    LCD_WriteStringAtPos(string, line, index); 
}

void fct_swCheck(int sw)
{
    char buffer[0xFF];
    char buffer2[0xFF];
    switch (sw)
    {
        case (1):  //Time
            sprintf(buffer, "Time :%.2d:%.2d:%.2d", values.current_time.hour, values.current_time.minute, values.current_time.second);
            break;

        case (2): //Accelerometre x y z
            sprintf(buffer, "XYZ:%d %d %d",values.acl_x_dif, values.acl_y_dif , values.acl_z_dif );
            break;

        case (4): //Accelerometre x
            sprintf(buffer, "X:%d",values.acl_x_dif );
            break;

        case (8): //Accelerometre y
            sprintf(buffer, "Y:%d",values.acl_y_dif );
            break;

        case (16): //Accelerometre z
            sprintf(buffer, "Z:%d ",values.acl_z_dif );
            break;

        case (32): //Accelerometre magnitude
            sprintf(buffer, "Mag:%d", values.acl);
            sprintf(buffer2, "AVG:%d", values.weighted_avg);
            
            break;

        case (64): //weighted_avg
            sprintf(buffer, "AVG:%d", values.weighted_avg);
            break;
            
        default:
            sprintf(buffer, "Time :%.2d:%.2d:%.2d", values.current_time.hour, values.current_time.minute, values.current_time.second);
            break;
    }
    fct_writeText(buffer, 0, 0); 
    //fct_writeText(buffer2, 1, 0); 
}


/* *****************************************************************************
 End of File
 */

