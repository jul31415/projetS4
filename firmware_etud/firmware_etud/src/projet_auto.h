/* 
 * File:   projet_auto.h
 * Author: royj1311
 *
 * Created on 8 juillet 2021, 10:48
 */

#ifndef PROJET_AUTO_H
#define	PROJET_AUTO_H


struct time{
    int hour;
    int minute;
    int second;
};

struct values{
    struct time current_time;
    int time;
    int acl_x;
    int acl_y;
    int acl_z;
    int acl_x_mem;
    int acl_y_mem;
    int acl_z_mem;
    int acl_x_dif;
    int acl_y_dif;
    int acl_z_dif;
    int acl;
    int theta;
    int phi;
    int weighted_avg;
};
struct values values;

float fGRangeLSB;   // global variable used to pre-compute the value in g corresponding to each count of the raw value


void affichagePmodLed(int diff);float fGRangeLSB;   // global variable used to pre-compute the value in g corresponding to each count of the raw value
void fct_writeText(char *string, int line, int index);
void fct_swCheck(int sw);
void projet_tasks(int accelX, int accelY, int accelZ);





#endif	/* PROJET_AUTO_H */

