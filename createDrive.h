//list of opcodes with explanations can be found here: http://www.irobot.com/filelibrary/pdfs/hrd/create/Create%20Open%20Interface_v2.pdf
#ifndef _FINALCREATE
#define _FINALCREATE

#include "generic.h"

//required for buffering and scripting
#define WHEEL_DROP 1
#define CLIFF 10
#define BUMP 5
#define LEFT_BUMP 6
#define RIGHT_BUMP 7
#define BUTTON_ADVANCED 16
#define BUTTON_PLAY 17//TODO: finish all events.  p16 of create docs
#define SEN_0 18

//used to convert an int (16 bits) into 2 bytes (8 bits each) so that the create can read it
#define get_high_byte2(a) ((a)>>8)//finds the top 8 bits
#define get_low_byte2(a) ((a)&255)//finds the bottom 8 bits

#define create_write_int(integer) create_write_byte(get_high_byte2(integer));create_write_byte(get_low_byte2(integer))//writes an int as 2 bytes

#define IN_TO_MM 25.4

void forward_time(int time, int speed)//time in milliseconds, speed from 1-500
{//note that this doesn't use scripting, so treat it as you would a motor or servo command in regards to using create_block
	create_drive_direct(speed, speed);
	msleep(time);
	create_stop();
}
void backward_time(int time, int speed)//time in milliseconds, speed from 1-500
{//note that this doesn't use scripting, so treat it as you would a motor or servo command in regards to using create_block
	create_drive_direct(-speed, -speed);
	msleep(time);
	create_stop();
}

//this is for just good old plain scripting
void create_wait_time(int time)//time is in deciseconds
{
	create_write_byte(155);
	create_write_byte(time);
}
void create_wait_dist(int dist)//dist is in mm
{
	create_write_byte(156);
	create_write_int(dist);
}
void create_wait_angle(int angle)//degrees, negative = right
{
	create_write_byte(157);
	create_write_int(angle);
}
void create_wait_event(int event)//see #defines for possible answers.  Use 255-event for the inverse
{
	create_write_byte(158);
	create_write_byte(event);
}

void create_drive_direct_dist(int r_speed, int l_speed, int dist)//speeds from 1-500, distance in mm
{
	create_write_byte(145);
	create_write_int(r_speed);
	create_write_int(l_speed);
	create_wait_dist(dist);
}
void create_drive_direct_left(int r_speed, int l_speed, int angle)//speeds from 1-500, angle in degrees
{
	create_write_byte(145);
	create_write_int(r_speed);
	create_write_int(l_speed);
	create_wait_angle(angle);
}
void create_drive_direct_right(int r_speed, int l_speed, int angle)//speeds from 1-500, angle in degrees
{
	create_write_byte(145);
	create_write_int(r_speed);
	create_write_int(l_speed);
	create_wait_angle(-angle);
}
void create_right(int angle, int radius, int speed)//angle in degrees, radius in mm, speed 1-500
{
	create_write_byte(137);
	create_write_int(speed);
	if (radius == 0){
		create_write_int(-1);
	}else{
		create_write_int(-radius);
	}
	create_wait_angle(-angle);
}
void create_left(int angle, int radius, int speed)//angle in degrees, radius in mm, speed 1-500
{
	create_write_byte(137);
	create_write_int(speed);
	if (radius == 0){
		create_write_int(1);
	}else{
		create_write_int(radius);
	}
	create_wait_angle(angle);
}
void create_forward(float dist, int speed)//distance in inches, speed from 1-500
{
	dist = dist * IN_TO_MM;
	create_write_byte(145);
	create_write_int(speed);
	create_write_int(speed);
	create_wait_dist(round(dist));
}
void create_backward(float dist, int speed)//distance in inches, speed from 1-500
{
	dist = dist * IN_TO_MM;
	create_write_byte(145);
	create_write_int(-speed);
	create_write_int(-speed);
	create_wait_dist(round(-dist));
}
void create_crash()//I don't really know what this does...opcode 7 doesn't appear in the docs
{
	create_write_byte(7);
}

void digital_outputs(boolean d0, boolean d1, boolean d2)//d(n) means whether output n is to be turned on (true) or off (false)
{//output 0 is pin 19, 1 is pin 7, and 2 is pin 20
	create_write_byte(147);
	create_write_byte((d0*1)+(d1*2)+(d2*4));//works because booleans are just 0/1 ints-->any combination of on/off will have a unique result
}

void stop_output()//turns off pins 7, 19, and 20
{//can also be achieved with digital_outputs(false, false, false)
	create_write_byte(147);
	create_write_byte(0);
}

void create_send()//used for create_block only
{
	create_write_byte(142);//this requests data
	create_write_byte(35);//specifies that the create should return its mode (0/1/2/3 for off/passive/safe/full)
}
void create_recieve()//used for create_block only
{	
	char buffer[1];
	char *bptr = buffer;
	create_read_block(bptr,1);//this asks the link to receive data from the create and put it into the array buffer (bptr is the pointer)
}
void create_block()//blocks program until create finishes
{
	create_stop();//stops the robot
	create_send();//asks the create for data
	create_recieve();//recieves the information-->doesn't progress until it gets it(meaning it waits until the create catches up)
}
//so long as we are connected to the controller, no reason to use the direct motor command instead of this one
void create_motors(int speed)//speed is from 0 to 128 inclusive
{
	create_write_byte(144);
	create_write_byte(speed);
	create_write_byte(speed);
	create_write_byte(speed);
}

#define lcliff get_create_lcliff_amt(.002)
#define rcliff get_create_rcliff_amt(.002)
void create_lineup()//lines up the create on a black line
{
	int done = 0;
	float tstart = seconds();
	int retry = 1;//don't retry ever
	while(done < 4){//WORK ON THIS
		msleep(5);
		int lspd,rspd;
		lspd = rspd = 0;
		if (lcliff > 800) lspd = 20;
		if (lcliff < 500) lspd = -20;
		if (rcliff > 800) rspd = 20;
		if (rcliff < 500) rspd = -20;
		
		if (seconds()-tstart > 4){lspd/=2;rspd/=2;}
		if (seconds()-tstart > 6){
			create_stop();
			return;//failure, timeout
		}
		//printf("\n%6d%6d",lcliff,rcliff);
		create_drive_direct(lspd,rspd);
		if (lspd == rspd && lspd == 0)
			done++;
		else
			done=0;
	}
	create_stop();
}
#endif
