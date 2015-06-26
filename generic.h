#define boolean int//this is the most important part of the code...
#define true 1
#define false 0

//general use "fake" functions
#define WAIT(thing); while(!(thing)){msleep(1);}//waits for thing to happen
#define LIMIT(thing,time); {double _tmptime = seconds()+time; while(!(thing) && (_tmptime > seconds())){msleep(1);}}//waits for thing to happen, times out after time seconds
#define gmpc get_motor_position_counter//this just makes things easier...
#define cmpc clear_motor_position_counter
#define light_s() analog10(sensor)//this is used in lightstart

//motors and servos go here (ports, positions, etc)


//UTILITY
float sign(float input)//returns 1 for positive, 0 for 0, -1 for negative
{
	if(input>0)
		return 1.;
	if(input<0)
		return -1.;
	return 0.;//implied else
}
float my_abs(float input)//my own absolute value function, because the built in one doesn't work
{
	if(input<0)
		return -input;
	return input;
}
//NOTE: This is only really useful for input_float and input_int, so you probably shouldn't call it in any other context
void change_b_button(int digit)//makes the b button the right digit-->only really useful for input_float and input_int
{//this is the hard way to do it, cause the set button commands don't work well with combining strings and numbers
	if(digit==0)
		set_b_button_text("0");
	else if(digit==1)
		set_b_button_text("1");
	else if(digit==2)
		set_b_button_text("2");
	else if(digit==3)
		set_b_button_text("3");
	else if(digit==4)
		set_b_button_text("4");
	else if(digit==5)
		set_b_button_text("5");
	else if(digit==6)
		set_b_button_text("6");
	else if(digit==7)
		set_b_button_text("7");
	else if(digit==8)
		set_b_button_text("8");
	else//9
		set_b_button_text("9");
}
float input_float(int x, int y)//gets the user to input a float, then returns it
{//x and y are where the display_printf starts
	int digits=0;//how many digits have been inputted
	int curr_digit=0;//what number is selected right now
	float curr_number=0;//current output
	int digits_after_dec=-1;//number of digits after the decimal point-->-1 means no decimal point yet
	extra_buttons_show();
	set_a_button_text("-");//move curr_digit down by one
	set_c_button_text("+");//up
	set_x_button_text("backspace");//delete the last digit
	set_y_button_text(".");//decimal point
	set_z_button_text("enter");//done with input
	while(1)
	{
		change_b_button(curr_digit);//I have to do this the hard way because there's no good way to convert between ints and strings...
		WAIT(a_button()||b_button()||c_button()||x_button()||y_button()||z_button());
		if(a_button())
			curr_digit--;//I'll sanitize the result at the end of the loop
		else if(b_button())
		{
			if(digits_after_dec<0)//no dec point yet
			{
				curr_number*=10;
				curr_number+=(float)(curr_digit);
			}
			else//has a dec point
			{
				curr_number+=((float)(curr_digit))/exp10(digits_after_dec+1);//trust me, it works
				digits_after_dec++;//has an extra digit after the decimal point
			}
			display_printf(x+digits, y, "%d", curr_digit);
			digits++;
		}
		else if(c_button())
			curr_digit++;
		else if(x_button())
		{
			if(digits>0)//has a digit to delete-->otherwise, will ignore the button tap
			{
				if(digits_after_dec>0)//deleting a digit after the dec point
				{
					digits_after_dec--;
					curr_number=((float)((int)(curr_number*exp10(digits_after_dec))))/exp10(digits_after_dec);//don't question it...
				}
				else if(digits_after_dec==0)//deleting the decimal point
				{
					digits_after_dec=-1;//no more dec point
				}
				else//deleting a digit with no dec point
				{
					curr_number=(float)((int)(curr_number/10));//again, it works, just go with it
				}
				digits--;
				display_printf(x+digits, y, " ");//clear the input it deleted
			}
		}
		else if(y_button())
		{
			if(digits_after_dec<0)//doesn't have a decimal point yet-->can add one
			{//if not, will ignore button tap
				if(digits==0)//no digits yet-->add a leading 0
				{
					display_printf(x,y,"0");
					digits=1;
				}
				display_printf(x+digits, y, ".");
				digits_after_dec=0;//has a dec, but no numbers after it yet
				digits++;//counts as a digit for the purposes of printing in the right locations
			}
		}
		else//z
		{
			if(digits>0)//has put in a number-->if not, can't exit, so will ignore button tap
				break;//exit the loop-->get to the return
		}
		curr_digit+=10;//will make it positive; if was already positive, will deal with that in the next step
		curr_digit=curr_digit%10;//this will get in in the range of 0-9
		WAIT(!(a_button()||b_button()||c_button()||x_button()||y_button()||z_button()));
		msleep(50);
	}
	return curr_number;
}
int input_int(int x, int y)//gets the user to input an int, then returns it
{//x and y are where the display_printf starts
	int digits=0;//how many digits have been inputted
	int curr_digit=0;//what number is selected right now
	int curr_number=0;//current output
	extra_buttons_show();
	set_a_button_text("-");//move curr_digit down by one
	set_c_button_text("+");//up
	set_x_button_text("backspace");//delete the last digit
	set_y_button_text("");//nothing-->can't have a decimal point
	set_z_button_text("enter");//done with input
	while(1)
	{
		change_b_button(curr_digit);//I have to do this the hard way because there's no good way to convert between ints and strings...
		WAIT(a_button()||b_button()||c_button()||x_button()||z_button());
		if(a_button())
			curr_digit--;//I'll sanitize the result at the end of the loop
		else if(b_button())
		{
			curr_number*=10;
			curr_number+=(float)(curr_digit);
			display_printf(x+digits, y, "%d", curr_digit);
			digits++;
		}
		else if(c_button())
			curr_digit++;
		else if(x_button())
		{
			if(digits>0)//has a digit to delete-->otherwise, will ignore the button tap
			{
				curr_number=curr_number/10;//casts to an int automatically, cause both are ints
				digits--;
				display_printf(x+digits, y, " ");//clear the input it deleted
			}
		}
		else//z
		{
			if(digits>0)//has put in a number-->if not, can't exit, so will ignore button tap
				break;//exit the loop-->get to the return
		}
		curr_digit+=10;//makes it positive; if was already positive, will be undone in the next step
		curr_digit=curr_digit%10;//this will get in in the range of 0-9
		WAIT(!(a_button()||b_button()||c_button()||x_button()||z_button()));
		msleep(50);
	}
	return curr_number;
}

//LIGHT START
void light_start(int sensor)//a replacement for kipr's lightstart-->better UI, more reliable
{//sensor is the port of your light sensor
	WAIT(!(b_button() || c_button()));
	extra_buttons_hide();
	set_a_button_text("-");
	set_a_button_text("arm");
	set_a_button_text("skip");
	int max = 0,min = 9999,curr = 0,avg = 0;
	display_clear();
	display_printf(0,0,"Max:");
	display_printf(0,1,"Min:");
	display_printf(0,2,"Curr:");
	display_printf(0,3,"avg:");
	while(!b_button() && !c_button())//INTERACTION BUTTON
	{
		curr = light_s();
		if (curr > max)
		{max = curr;}
		if (curr < min)
		{min = curr;}
		avg = (max+min)/2;
		display_printf(5,0,"%d   ",max);
		display_printf(5,1,"%d   ",min);
		display_printf(6,2,"%d   ",curr);
		display_printf(5,3,"%d   ",avg);
		if (curr > avg)
		{
			display_printf(10,5,"XX");
		}
		else
		{
			display_printf(10,5,"OO");
		}
		msleep(50);
	}
	display_clear();
	display_printf(0,4,"Prepared to begin: c to skip");
	while(light_s() > avg && !(c_button()))
	{
		msleep(50);
	}
}

//TIMING
long _start_time=0;//just in case someone forgets start-->will still work ish
void start()//whenever you call this is time 0
{
	_start_time = systime();
}
float curr_time()//returns the current time in seconds (using the time start was called as time 0)
{
	return (systime()-_start_time)/1000.0;
}
void now()//prints out the current time
{
	printf("now %f\n",curr_time());
}
void nowstr(char *s)//prints out the current time with a string before it
{
	printf("%s %f\n",s,curr_time());
}
void wait_till(float t)//waits until time t (with the time start was called as time 0), then continues
{
	now();
	msleep((long)((t-curr_time())*1000));
}

//SERVOS
void servo_set(int port,int end,float time)//moves the servo over a given time (so it's slower and smoother)
{//position is from 0-2047, time in seconds
	float increment = .01;
	float curr,start = get_servo_position(port);
	float i = ((end-start)/time)*increment;
	curr = start;
	if (start > end)
	{
		while(curr > end)
		{
			set_servo_position(port,curr);
			curr+=i;
			msleep((long)(increment*1000));
		}
	}
	else if (start < end)
	{
		while(curr < end)
		{
			set_servo_position(port,curr);
			curr+=i;
			msleep((long)(increment*1000));
		}
	}
	set_servo_position(port,end);
}

//CAMERA
int cam_area(int channel)//returns area largest blob in channel, or 0 if none
{
	if (get_object_count(channel) > 0)
	{
		return get_object_area(channel,0);
	}
	return 0;
}
void update_wait()//updates the camera, and waits until success
{
	while(!camera_update()) msleep(1);
}

int currstate;
#define state(State) if (currstate == State)
#ifndef MENUa
   #define next(State); {currstate = State;}
#else//If menu code is in use, print the time of each state switch
	extern struct menuitem menu[];
	void next(int State) {
		int i;
		currstate = State;
		i = -1;
		while (!strcmp(menu[++i].name,"FIN")){
			if (menu[i].snum==State){
				nowstr(menu[i].name);
				return;
			}
		}
		now();
}
#endif


int getabbutton(){//returns 0,1 on a,b, -1 if something goes wrong
	WAIT(!(a_button() || b_button()));
	WAIT(a_button() || b_button());
	if (a_button()) return 0;
	if (b_button()) return 1;
	printf("ERROR!");beep();
	msleep(2000);beep();
	return -1;//if something broke
}
int getabcbutton(){//returns 0,1,2 on a,b,c, -1 if something goes wrong
	WAIT(!(a_button() || b_button() || c_button()));
	WAIT(a_button() || b_button() || c_button());
	if (a_button()) return 0;
	if (b_button()) return 1;
	if (c_button()) return 2;
	printf("ERROR!");beep();
	msleep(2000);beep();
	return -1;//if something broke
}

void reset_buttons()//puts the buttons back in the standard setup
{
	extra_buttons_hide();
	set_a_button_text("A");
	set_b_button_text("B");
	set_c_button_text("C");
}

int wait_time;
void wait_shutdown()//this shouldn't be called from anywhere except shutdownin (without creating the thread, your robot won't do anything)
{
	msleep(wait_time);
	//if you want to do anything specific when the robot shuts down, put the commands here
	printf("ended based off time\n");
	_exit(0);//this throws a warning, but you can ignore it
}
void shutdownin(float time)//this makes it such that you can do specific commands once you want the robot to shut down (and it doesn't print out the time at shutdown)
{//time in seconds
	wait_time=(int)(time*1000);
	thread tw=thread_create(wait_shutdown);
	thread_start(tw);
}
