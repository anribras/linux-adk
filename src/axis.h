/**
 * @file axis.h
 * @brief 
 * @author yangding@le.com
 * @version v0.1
 * @date 2016-08-30
 */
#ifndef _AXIS_H
#define _AXIS_H

//#define NULL ((void*)0)

#define PI  (3.1415926)

enum{
	TOUCH_DOWN = 5,
    TOUCH_MOVE = 4,
    TOUCH_UP   = 6,
} CarAction;

typedef struct
{
	int x;
	int y;
} Point;

typedef struct
{
	int base_x;
	int base_y;
	int view_height;
	int view_width;
	int full_height;
	int full_width;
} CarScreenSetting;

typedef struct
{
 	int full_height;
	int full_width;
} PhoneScreenSetting;

typedef struct
{
	CarScreenSetting css;
	PhoneScreenSetting pss;
} AxisSetting ;

/**
 * @brief axis_init 
 * @param as
 * @param angle
 * @return 
 */
int axis_init(AxisSetting* as , int angle);
/**
 * @brief axis_convert 
 * @param p
 * @return 
 */
Point axis_convert(Point p);
/**
 * @brief inject_point 
 * @param action
 * @param p
 * @return 
 */
int axis_inject(int action, Point p);

#endif

