#include "projet_auto.h"
#include "system/common/sys_common.h"
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
#define PI 3.141592654
int count2= 0;
#include <math.h>
#include "pmods.h"
#include "utils.h"


//distance, pour l'instant retourne un nombre de 20us
int read_distance()
{
        cpt_capt = 0;
        int cpt_fucker = 0;
        PMODS_SetValue(1,2,1);
        DelayAprox10Us(0.1);
        PMODS_SetValue(1,2,0);
        while(PMODS_GetValue(1,3) == 0 && cpt_fucker < 1000)
        {
            cpt_capt = 0;
            if (Flag_10us)
            {
                Flag_10us = 0;
                cpt_fucker++;
            }
        }
        while(PMODS_GetValue(1,3) == 1)
        {
            if (Flag_10us)
            {
                Flag_10us = 0;
                cpt_capt++;
            }
        }
        return cpt_capt;
}


//affichage termométrique des valeursde l'accélérateur
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


//calcul des valeurs de laccéléromètre
void projet_tasks(int accelX, int accelY, int accelZ)
{    
     //Filtre Moyenne Pond�r�e
    values.acl_x = accelX;
    values.weighted_x = values.weighted_x + values.acl_x;
    values.weighted_x = values.weighted_x / 2;
    
    values.acl_y = accelY;
    values.weighted_y = values.weighted_y + values.acl_y;
    values.weighted_y = values.weighted_y / 2;
    
    values.acl_z = accelZ;
    values.weighted_z = values.weighted_z + values.acl_z;
    values.weighted_z = values.weighted_z / 2;
    
    //Calcul de la diff�rence d'acc�l�ration
    values.acl_x_dif = values.weighted_x - values.acl_x_mem;
    values.acl_x_mem = values.weighted_x;
    values.acl_y_dif = values.weighted_y - values.acl_y_mem;
    values.acl_y_mem = values.weighted_y;
    values.acl_z_dif = values.weighted_z - values.acl_z_mem;
    values.acl_z_mem = values.weighted_z;
    
    //Calcul Magnitude Acc�l�ration
    values.acl = sqrt((values.acl_x_dif*values.acl_x_dif) + (values.acl_y_dif*values.acl_y_dif) + (values.acl_z_dif*values.acl_z_dif)); 
    affichagePmodLed(values.acl);
    
    
    //Calcul theta (Orientation Verticale)
    if (values.acl != 0)
    {
        values.theta = acos((float)values.acl_z_dif / (float)values.acl);
        values.theta = (PI/2) - values.theta;
        values.theta = values.theta * 360 / (2*PI);
    }
    else 
    {
        values.theta = 0;
    }

    //Calcul phi (Orientation Horizontale)
    if (values.acl_x_dif != 0)
    {
        values.phi = atan((float)values.acl_y_dif / (float)values.acl_x_dif);
        values.phi = values.phi * 360 / (2*PI);
    }
    else 
    {
        if (values.acl_y_dif != 0)
        {
            values.phi = PI / 2;
        }
        else 
        {
            values.phi = 0;
        }
    }
    
    
    
    //Calcul de la vitesse
    //ACL_ConvertRawToValueG(values.acl);
    values.speed_x = (values.acl_x_dif * 0.640) + values.speed_mem_x;
    values.speed_mem_x = values.speed_x;
    
    values.speed_y = (values.acl_y_dif * 0.640) + values.speed_mem_y;
    values.speed_mem_y = values.speed_y;
            
    if (count2 == 6)
    {
        count2 = 0;
        fct_swCheck(SWT_GetGroupValue());  //Affichage selon les switchs
    }
    count2++;
    
    
    fct_btnCheck();
} 

//ecriture des valeurs sur lecran
void fct_writeText(char *string, int line, int index)
{ 
    LCD_DisplayClear();     // Clear what was previously written.
    LCD_WriteStringAtPos(string, line, index);   
}




void fct_writeText2(char *string, char *string2)
{ 
    LCD_DisplayClear();     // Clear what was previously written.
    LCD_WriteStringAtPos(string, 0, 0); 
    LCD_WriteStringAtPos(string2, 1, 0); 
}

//sélectionne l'affichage des paramètre
void fct_swCheck(int sw)
{
    char buffer[0xFF];
    char buffer2[0xFF];
    switch (sw)
    {
        case (1):  //Time
            sprintf(buffer, "Time :%.2d:%.2d:%.2d", values.current_time.hour, values.current_time.minute, values.current_time.second);
            sprintf(buffer2, "");
            break;

        case (2): //Accelerometre x y z
            sprintf(buffer, "XYZ:%d %d %d",values.acl_x_dif, values.acl_y_dif , values.acl_z_dif );
            break;

        case (4): //Accelerometre x
            sprintf(buffer, "X:%d,Xpond: %d ",values.acl_x,values.weighted_x);
            sprintf(buffer2, "X diff:%d",values.acl_x_dif);
            break;

        case (8): //Accelerometre y
            sprintf(buffer, "Y:%d,Ypond: %d ",values.acl_y,values.weighted_y);
            sprintf(buffer2, "Y diff:%d",values.acl_y_dif);
            break;

        case (16): //Accelerometre z
            sprintf(buffer, "Z:%d,Zpond: %d ",values.acl_z,values.weighted_z);
            sprintf(buffer2, "Z diff:%d",values.acl_z_dif);
            break;

        case (32): //Accelerometre magnitude
            sprintf(buffer, "Mag:%d", values.acl);
            //sprintf(buffer2, "AVG:%d", values.weighted_avg);
            break;

        case (64): //Orientation
            sprintf(buffer, "Verti:%.2f", values.theta);
            sprintf(buffer2, "Hori:%.2f", values.phi);
            break;
            
        case (128): //Speed
            sprintf(buffer, "SpeedX:%d", values.speed_x);
            sprintf(buffer2,"SpeedY:%d", values.speed_y);
            break;
            
        case (129): //Distance
            sprintf(buffer, "Distance:%d", values.distance);
            sprintf(buffer2,"");
            break;
               
        default:
            sprintf(buffer, "Time :%.2d:%.2d:%.2d", values.current_time.hour, values.current_time.minute, values.current_time.second);
            break;
    }
    fct_writeText2(buffer, buffer2); 
}

void fct_btnCheck(void)
{
    int BTN_value = BTN_GetGroupValue();

    if (BTN_value == 16)  //DOWN
    {
        values.speed_mem_x = 0;
        values.speed_x = 0;
        values.speed_mem_y = 0;
        values.speed_y = 0;
        
        while (BTN_GetGroupValue() == 16){};
    }

    else if (BTN_value == 8)   //RIGHT
    {
        
        while (BTN_GetGroupValue() == 8){};
    }   

    else if (BTN_value == 4)  //CENTER
    {
        while (BTN_GetGroupValue() == 4){};
    }  

    else if (BTN_value == 2) //LEFT
    {
        
        while (BTN_GetGroupValue() == 2){};
    }  

    else if (BTN_value == 1)  //UP
    {
        
        while (BTN_GetGroupValue() == 1){};
    }
}

int read_distance(void)
{
    int cpt_distance = 0;
    //int final_distance = 0;
    int stuck = 0;
    PMODS_SetValue(1, 2, 1);
    delay1us(10);
    PMODS_SetValue(1, 2, 0);
    while (PMODS_GetValue(1, 3) == 0)// && stuck < 1000000)
    {
        stuck ++;
    }
    while (PMODS_GetValue(1, 3) == 1)
    {
        cpt_distance++;
        delay1us(5);
    }
    
//    if (stuck == 1000000)
//    {
//        values.distance = -1;
//        return(-1);
//    }

   // final_distance = cpt_distance * 5 * 0,034/2;   //Distance = duration * 0,034/2
    values.distance = cpt_distance / 17;
    return(values.distance);
}

void manage_time(void)
{
    counter_time ++;
    
    if (counter_time == 12)
    {
        values.current_time.second ++;
    }
    
    if (counter_time == 25)
    {
        counter_time = 0;
        values.current_time.second ++;
    }
    
    if (values.current_time.second == 60)
    {
        values.current_time.second = 0;
        values.current_time.minute ++;
    }

    if (values.current_time.minute == 60)
    {
        values.current_time.minute = 0;
        values.current_time.hour ++;
    }

    if (values.current_time.hour == 24)
    {
        values.current_time.hour = 0;
    }
}

/* *****************************************************************************
 End of File
 */

