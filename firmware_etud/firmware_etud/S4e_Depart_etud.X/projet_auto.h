/* 
 * File:   projet_auto.h
 * Author: royj1311
 *
 * Created on 8 juillet 2021, 10:48
 */

#ifndef PROJET_AUTO_H
#define	PROJET_AUTO_H

#define TMR_TIME    0.001             // x us for each tick
#define PI 3.141592654

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
    
    int distance;
    int weighted_distance;
    
};

struct values values;
float fGRangeLSB;   // global variable used to pre-compute the value in g corresponding to each count of the raw value
int Flag_10us = 0;
int cpt_capt = 0;
int distance = 0;
int counter_time = 0;
int count2= 0;
int compteur_eco = 0;
int iflash = 0;          //compteur pour remplir le paquet de la flash
int iflash_interne = 0;  //compteur interne pour la flash
char paquet_flash[204];
unsigned char testFlash[204];

void affichagePmodLed(int diff);
void fct_writeText(char *string, int line, int index);
void fct_swCheck(int sw);
void fct_btnCheck(void);
void projet_tasks(int accelX, int accelY, int accelZ);
void fct_writeText2(char *string, char *string2);
int read_distance(void);
void manage_time(void);
void write_flash(void);
void clear_flash(void);
void read_flash(int* array);
void pmod_distance(int distance);





#endif	/* PROJET_AUTO_H */

