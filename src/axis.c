/**
 * @file main.c
 * @brief
 * @author yangding@le.com
 * @version v0.1
 * @date 2016-08-29
 */
#include <stdio.h>
#include <math.h>
#include "axis.h"

static AxisSetting* setup;
static int rot;
static float cos_val =  -1;
static float sin_val =  -1;

/**
 * @brief axis_init
 * @param as
 * @return
 */
int axis_init(AxisSetting* as , int angle)
{
    if(as == NULL && (angle % 90)!= 0) {
        return 1;
    } else {
        setup = as;
        rot = angle;
        sin_val = sin((float)(2* PI * angle / 360));
        cos_val = cos((float)(2* PI * angle / 360));
        //printf("(sin cos) (%f,%f)\n",sin_val,cos_val);
    }
	//printf("axis_init setup\n");
	//printf("car screen base_x:%d\n",(int)setup->css.base_x);
	//printf("car screen base_y:%d\n",(int)setup->css.base_y);
	//printf("car screen full_width:%d\n",setup->css.full_width);
	//printf("car screen full_height:%d\n",setup->css.full_height);
	//printf("car screen view_width:%d\n",(int)setup->css.view_width);
	//printf("car screen view_height:%d\n",(int)setup->css.view_height);
	//printf("phone screen full_width:%d\n",(int)setup->pss.full_width);
	//printf("phone screen full_height:%d\n",(int)setup->pss.full_height);
    return 0;
}

#if 0
Point axis_convert(Point p)
{
    Point ret;
    float x,y;
#if 0
    if((p.x - setup->css.base_x ) > setup->css.view_width)
        p.x = setup->css.view_width;
    if((p.y - setup->css.base_y ) > setup->css.view_height)
        p.y = setup->css.view_height;
#endif
    //rotate
    x = cos_val * p.x + sin_val * p.y;
    y = cos_val * p.y - sin_val * p.x;

    //printf("(%d %d) rot (%d,%d)\n",p.x,p.y,(int)x,(int)y);
    // base point
    switch(rot) {
    case -270:
    case 90:
        y = y - setup->css.base_x + setup->css.full_width;
        x = x - setup->css.base_y;
        //printf("base point (%d,%d)\n",(int)x,(int)y);
        x = x * setup->pss.full_width / setup->css.view_height;
        y = y * setup->pss.full_height / setup->css.view_width;
        break;
    case -90:
    case 270:
        x = x - setup->css.base_y + setup->css.full_height;
        y = y - setup->css.base_x;
        //printf("base point (%d,%d)\n",(int)x,(int)y);
        x = x * setup->pss.full_width / setup->css.view_height;
        y = y * setup->pss.full_height / setup->css.view_width;
        break;
    case 180:
    case -180:
        x = x - setup->css.base_x + setup->css.full_width;
        y = y - setup->css.base_y + setup->css.full_height;
        //printf("base point (%d,%d)\n",(int)x,(int)y);
        x = x * setup->pss.full_width / setup->css.view_width;
        y = y * setup->pss.full_height / setup->css.view_height;
        break;
    case 0:
    case 360:
        x = x - setup->css.base_x;
        y = y - setup->css.base_y;
        //printf("base point (%d,%d)\n",(int)x,(int)y);
        x = x * setup->pss.full_width / setup->css.view_width;
        y = y * setup->pss.full_height / setup->css.view_height;
        break;
    default:
        printf("unsupported rot\n");
        break;
    }
    //printf("converted (%d,%d)\n",(int)x,(int)y);
    ret.x = (int)x;
    ret.y = (int)y;
    return ret;
}
#else
Point axis_convert(Point p)
{
	Point ret;
	ret.x = -1;
	ret.y = -1;
	int x1,y1;
	int x,y;
#if 0
	if((p.x - setup->css.base_x ) > setup->css.view_width) 
		return ret;
	if((p.y - setup->css.base_y ) > setup->css.view_height) 
		return ret;
#endif

	x = p.x - setup->css.base_x;
	y = p.y - setup->css.base_y;

	//printf("base (%d,%d)\n",(int)x,(int)y);
	//rotate
	x1 = (int)(cos_val * x + sin_val * y); 
	y1 = (int)(cos_val * y - sin_val * x); 

	x = x1;
	y = y1;

	//printf("rot (%d,%d)\n",x,y);
	switch(rot){
		case -270:
		case 90:
			y = y  + setup->css.view_width;
			x = x * setup->pss.full_width / setup->css.view_height;
			y = y * setup->pss.full_height / setup->css.view_width;
			break;
		case -90:
		case 270:
			x = x + setup->css.view_height;
			x = x * setup->pss.full_width / setup->css.view_height;
			y = y * setup->pss.full_height / setup->css.view_width;
			break;
		case 180:
		case -180:
			x = x + setup->css.view_width;
			y = y + setup->css.view_height;
			x = x * setup->pss.full_width / setup->css.view_width;
			y = y * setup->pss.full_height / setup->css.view_height;
			break;
		case 0:
		case 360:
			x = x * setup->pss.full_width / setup->css.view_width;
			y = y * setup->pss.full_height / setup->css.view_height;
			break;
		default:
			printf("not supported rot\n");
			break;
	}
	//printf("scale (%d,%d)\n",(int)x,(int)y);
	ret.x = (int)x;
	ret.y = (int)y;
	//printf("convert (%d %d) to (%d %d)\n",p.x,p.y,ret.x,ret.y);
	
	return ret;
}

#endif
/**
 * @brief inject_point
 * @param action
 * @param p
 * @return
 */
extern int write_event(int fd, int type, int code, int value);
extern struct EVENT_INFO ei;
#if 0
int axis_inject(int action, Point p)
{
	int x = p.x;
	int y = p.y;
    switch(action) {
		case TOUCH_DOWN:
			//Down
			//EV_ABS       ABS_MT_TRACKING_ID   00001b3a
			//EV_KEY       BTN_TOUCH            DOWN

			//EV_ABS       ABS_MT_POSITION_X    000000c6
			//EV_ABS       ABS_MT_POSITION_Y    0000037d
			//EV_ABS       ABS_MT_PRESSURE      0000003e
			//EV_SYN       SYN_REPORT           00000000
			//Tony Start
			//write_event(ei.fd_touch,EV_ABS,ABS_MT_SLOT,0 );
			//Tony End
			write_event(ei.fd_touch, EV_ABS, ABS_MT_TRACKING_ID, 0x1b3a);
			write_event(ei.fd_touch,EV_KEY,BTN_TOUCH,1);
			write_event(ei.fd_touch,EV_ABS,ABS_MT_POSITION_X,x);
			write_event(ei.fd_touch,EV_ABS,ABS_MT_POSITION_Y,y);
			write_event(ei.fd_touch,EV_ABS,ABS_MT_PRESSURE,0x3e);
			write_event(ei.fd_touch,0,0,0);
			//printf("Action_Down Event At %d	%d\r\n",(int)x,(int)y);
			//input_report_abs(touch->idev, ABS_X, x);
			//input_report_abs(touch->idev, ABS_Y, y);
			//input_report_abs(touch->idev, ABS_PRESSURE, rt);//ABS_PRESSURE		0x18
			//input_report_key(touch->idev, BTN_TOUCH, 1);
			break;
		case TOUCH_UP:
			//up
			//EV_ABS       ABS_MT_TRACKING_ID   ffffffff
			//EV_KEY       BTN_TOUCH            UP
			//EV_SYN       SYN_REPORT           00000000
			write_event(ei.fd_touch, EV_ABS, ABS_MT_TRACKING_ID, -1);
			write_event(ei.fd_touch,EV_KEY, BTN_TOUCH, 0);
			write_event(ei.fd_touch, 0, 0, 0);
			//printf("ACTION_UP Event At %d	%d\r\n",(int)x,(int)y);
			break;
		case TOUCH_MOVE:
			//Move
			//EV_ABS       ABS_MT_POSITION_X    000000ca
			//EV_ABS       ABS_MT_POSITION_Y    0000037f
			//EV_ABS       ABS_MT_PRESSURE      0000003d
			//EV_SYN       SYN_REPORT           00000000
			write_event(ei.fd_touch, EV_ABS, ABS_MT_POSITION_X, x);
			write_event(ei.fd_touch, EV_ABS, ABS_MT_POSITION_Y, y);
			write_event(ei.fd_touch, EV_ABS, ABS_MT_PRESSURE, 0x3d);
			write_event(ei.fd_touch, 0, 0, 0);
			break;
			return 0;
		}
}
#endif
    /**
     * @brief main
     * @param argv[]
     * @param argc
     * @return
     */
#if 0
    int main(char* argv[] ,int argc) {

        AxisSetting s;
#if 1
        s.css.base_x = 0;
        s.css.base_y = 0;

        s.css.view_width =  1280;
        s.css.view_height = 800;

        s.css.full_width = 1280;
        s.css.full_height = 800;

        s.pss.full_width = 1200;
        s.pss.full_height = 1600;
        Point in,out;
#else
        s.css.base_x = (1280 - 500)/2;
        s.css.base_y = 0;
        s.css.view_width =  500;
        s.css.view_height = 800;

        s.css.full_width = 1280;
        s.css.full_height = 800;

        s.pss.full_width = 800;
        s.pss.full_height = 1280;
        Point in,out;
#endif
        int lang = -1;
        int ang = 90;
        while(1) {
            if(lang != ang)
                axis_init(&s,ang);

            printf("input (x,y): ");
            scanf("%d",&in.x);
            scanf("%d",&in.y);
            out = axis_convert(in);
            printf("convert (%d,%d) to (%d,%d)\n",in.x,in.y,out.x,out.y);

            lang = ang;
        }

        return 0;
    }
#endif
