#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include "graphics.h"
#include "genlib.h"
#include "conio.h"
 
#include <windows.h>
#include <olectl.h>
#include <mmsystem.h>
#include <wingdi.h>
#include <ole2.h>
#include <ocidl.h>
#include <winuser.h>
#include <math.h> 
 
#define PI 3.1415926
 
void forward(double distance);
void turn(double angle);
void move(double distance);
void draw();
 
double nowangle=0.0;//write down the current direction,east is 0.0
double indexangle;
void Main() 
{
	double cx, cy;
	int i;
    InitGraphics();
    cx=GetWindowWidth()/2;
    cy=GetWindowHeight()/2;
    SetPenColor("green");
    for(i=0;i<18;i++){
    	MovePen(cx,cy);
    	draw();
    	nowangle=nowangle+20.0;
    	indexangle=nowangle;
	}
	_getch();
}
 
void draw()
{
	forward(1);
	turn(60);
	forward(1);
	turn(-120);
	forward(1);
	turn(-60);
	forward(1);
	turn(-120);
	forward(1);
}
 
void forward(double distance)
{
	DrawLine(distance*sin(indexangle*PI/180),distance*cos(indexangle*PI/180));
}
 
void turn(double angle)
{
	indexangle=indexangle+angle;
}
 
void move(double distance)
{
	MovePen((double)GetCurrentX()+distance*sin(indexangle*PI/180),(double)GetCurrentY()+distance*cos(indexangle*PI/180));
}
