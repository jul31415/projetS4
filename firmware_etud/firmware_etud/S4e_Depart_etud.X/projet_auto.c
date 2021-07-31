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
#include <math.h>
#include "pmods.h"
#include "utils.h"
#include "btn.h"
#include "swt.h"
#include "led.h"
#include "spiflash.h"



//affichage termométrique des valeursde l'accélérateur
void affichagePmodLed(int diff){
    compteur_eco++;
    if (diff<50){
        LED_SetGroupValue(1);
        values.indice_eco+=1*7;
    }
    else if (diff<100){
        LED_SetGroupValue(3);
        values.indice_eco+=4*7;
    }
    else if (diff<150){
        LED_SetGroupValue(7);
        values.indice_eco+=5*7;
    }
    else if (diff<200){
        LED_SetGroupValue(15);
        values.indice_eco+=10*7;
    }
    else if (diff<250){
        LED_SetGroupValue(31);
        values.indice_eco+=20*7;
    }
    else if (diff<300){
        LED_SetGroupValue(63);
        values.indice_eco+=40*7;
    }
    else if (diff<350){
        LED_SetGroupValue(127);
        values.indice_eco+=80*7;
    }
    else if (diff>=400){
        LED_SetGroupValue(255);
        values.indice_eco+=180*7;
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
    if (values.acl > 10)
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
    if (values.acl_x_dif != 0 && values.acl > 10)
    {
        values.phi = atan((float)values.acl_y_dif / (float)values.acl_x_dif);
        values.phi = values.phi * 360 / (2*PI);
    }
    else 
    {
        if (values.acl_y_dif > 5)
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
        case(0)://indice �colo
            sprintf(buffer, "Indice ecolo:");
            sprintf(buffer2, "%d",values.indice_eco/compteur_eco);
            break;
        case (1):  //Time
            sprintf(buffer, "Time :%.2d:%.2d:%.2d", values.current_time.hour, values.current_time.minute, values.current_time.second);
            sprintf(buffer2, " ");
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
            sprintf(buffer2," ");
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
        fct_writeText2("Clearing Flash !", " ");
        SPIFLASH_EraseAll();
        iflash = 0;
        iflash_interne = 0;
        
        
    
        values.current_time.second += 10;

        if (values.current_time.second >= 60)
        {
            values.current_time.second -= 60;
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
    while (PMODS_GetValue(1, 3) == 0 && stuck < 10000)
    {
        stuck ++;
    }
    while (PMODS_GetValue(1, 3) == 1 && stuck < 10000)
    {
        cpt_distance++;
        delay1us(5);
        stuck ++;
    }
    
    if (stuck == 10000)
    {
        
        values.distance = -1;
        return(-1);
    }

 

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
        values.time++;
    }
    
    if (counter_time == 25)
    {
        counter_time = 0;
        values.current_time.second ++;
        values.time++;
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


void write_flash(void)
{
    paquet_flash[iflash*4 + 4] = values.acl >> 24 & 0xff;  //module acc�l�ration
    paquet_flash[iflash*4 + 5] = values.acl >> 16 & 0xff; 
    paquet_flash[iflash*4 + 6] = values.acl >> 8 & 0xff;
    paquet_flash[iflash*4 + 7] = values.acl >> 0 & 0xff;
    
            
    paquet_flash[iflash*4 + 44] = (int)values.phi >> 24 & 0xff;//orientation horizontale
    paquet_flash[iflash*4 + 45] = (int)values.phi >> 16 & 0xff; 
    paquet_flash[iflash*4 + 46] = (int)values.phi >> 8 & 0xff;
    paquet_flash[iflash*4 + 47] = (int)values.phi >> 0 & 0xff;
            
    paquet_flash[iflash*4 + 84] = (int)values.theta >> 24 & 0xff; //orientation verticale
    paquet_flash[iflash*4 + 85] = (int)values.theta >> 16 & 0xff; 
    paquet_flash[iflash*4 + 86] = (int)values.theta >> 8 & 0xff;
    paquet_flash[iflash*4 + 87] = (int)values.theta >> 0 & 0xff;
     
            
    paquet_flash[iflash*4 + 124] = values.speed_x >> 24 & 0xff; //vitesse
    paquet_flash[iflash*4 + 125] = values.speed_x >> 16 & 0xff; 
    paquet_flash[iflash*4 + 126] = values.speed_x >> 8 & 0xff;
    paquet_flash[iflash*4 + 127] = values.speed_x >> 0 & 0xff;
            
    paquet_flash[iflash*4 + 164] = values.indice_eco >> 24 & 0xff; //indice �cologique
    paquet_flash[iflash*4 + 165] = values.indice_eco >> 16 & 0xff; 
    paquet_flash[iflash*4 + 166] = values.indice_eco >> 8 & 0xff;
    paquet_flash[iflash*4 + 167] = values.indice_eco >> 0 & 0xff;
    
    iflash++;
    
    if(iflash == 10)  // send
    {
        paquet_flash[0] = values.time >> 24 & 0xff; //indice de temps
        paquet_flash[1] = values.time >> 16 & 0xff; 
        paquet_flash[2] = values.time >> 8 & 0xff;
        paquet_flash[3] = values.time >> 0 & 0xff;
        
        SPIFLASH_ProgramPage(iflash_interne, paquet_flash, 204);
        //test de la flash
        //SPIFLASH_Read(iflash_interne, testFlash, 204);
        
        
        iflash_interne += 204;

        iflash = 0;
    }
}

/* *****************************************************************************
 End of File
 */

