/* 
 * File:   projet_auto.h
 * Author: royj1311
 *
 * Created on 8 juillet 2021, 10:48
 */

#ifndef PROJET_AUTO_H
#define	PROJET_AUTO_H

#define TMR_TIME    0.001             // x us for each tick

struct time{
    int hour;
    int minute;
    int second;
};

struct values{
    struct time current_time;
    int time;
    int acl_x;
    int weighted_x;
    int acl_x_mem;
    int acl_x_dif;
    
    int acl_y;
    int weighted_y;
    int acl_y_mem;
    int acl_y_dif;
    
    int acl_z;
    int weighted_z;
    int acl_z_mem;
    int acl_z_dif;
    
    int acl;
    
    double theta;  //Vertical
    double phi;    //Horizontale
    
    int speed_x;
    int speed_mem_x;
    
    int speed_y;
    int speed_mem_y;
    
    int indice_eco;
};

struct values values;
float fGRangeLSB;   // global variable used to pre-compute the value in g corresponding to each count of the raw value
int Flag_10us = 0;
int cpt_capt = 0;
int distance = 0;

void affichagePmodLed(int diff);float fGRangeLSB;   // global variable used to pre-compute the value in g corresponding to each count of the raw value
void fct_writeText(char *string, int line, int index);
void fct_swCheck(int sw);
void fct_btnCheck(void);
void projet_tasks(int accelX, int accelY, int accelZ);
void fct_writeText2(char *string, char *string2);
int read_distance();




#endif	/* PROJET_AUTO_H */

