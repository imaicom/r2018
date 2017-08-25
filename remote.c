// cc -o remote remote.c -lwiringPi -lm
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/joystick.h>

#include <wiringPi.h>
#include <softPwm.h>

#include "controller.h"
#include <math.h>


struct ps3ctls {

	int fd;
	unsigned char nr_buttons;	// Max number of Buttons
	unsigned char nr_sticks;	// Max number of Sticks
	short *button;			// button[nr_buttons]
	short *stick;			// stick[nr_sticks]
};

int fds;
int mode = 10;
int btn_tri = 0;
int b_btn_tri = 0;
int btn_cir = 0;
int b_btn_cir = 0;
int btn_squ = 0;
int b_btn_squ = 0;
int btn_up = 0;
int b_btn_up = 0;
int btn_down = 0;
int b_btn_down = 0;
int btn_upd = 0;
int b_btn_upd = 0;
int btn_downd = 0;
int b_btn_downd = 0;
int btn_left = 0;
int b_btn_left = 0;
int btn_right = 0;
int b_btn_right = 0;
int btn_cross = 0;
int b_btn_cross = 0;
int btn_square = 0;
int b_btn_square = 0;
int btn_circle = 0;
int b_btn_circle = 0;

int btn_r1 = 0;
int b_btn_r1 = 0;
int btn_r2 = 0;
int b_btn_r2 = 0;
int btn_l1 = 0;
int b_btn_l1 = 0;
int btn_l2 = 0;
int b_btn_l2 = 0;

int l_mode = 3;
int l_mode_a = 55;
int l_mode_b = -80;
int r_mode = 3;
int r_mode_a = 72 ;
int r_mode_b = +90;
int t_mode = 11;
int h_mode = 2;
int d_mode = 1;


int resetPCA9685(int fd) {
	wiringPiI2CWriteReg8(fd,0,0);
}

int setPCA9685Freq(int fd , float freq) {
	float prescaleval;
	int prescale , oldmode , newmode;
	freq = 0.9 * freq;
	prescaleval = 25000000.0;
	prescaleval /= 4096.0;
	prescaleval /= freq;
	prescaleval -= 1.0;
	prescale = prescaleval + 0.5;
	oldmode = wiringPiI2CReadReg8(fd,0x00);
	newmode = (oldmode & 0x7F)|0x10;
	wiringPiI2CWriteReg8(fd , 0x00 , newmode);
	wiringPiI2CWriteReg8(fd , 0xFE , prescale);
	wiringPiI2CWriteReg8(fd , 0x00 , oldmode);
	sleep(0.005);
	wiringPiI2CWriteReg8(fd , 0x00 , oldmode | 0xA1);
}


int setPCA9685Duty(int fd , int channel , int off) {
	int channelpos;
	int on;

	on   = 0;
	off += 276;
	channelpos = 0x6 + 4 * channel;
	wiringPiI2CWriteReg16(fd , channelpos   , on  & 0x0FFF);
	wiringPiI2CWriteReg16(fd , channelpos+2 , off & 0x0FFF);
}

int ps3c_test(struct ps3ctls *ps3dat) {

	int i;
	unsigned char nr_btn = ps3dat->nr_buttons;
	unsigned char nr_stk = ps3dat->nr_sticks;
	int xx,yy,x,y,z,v1,v2,ww,c1,c2,c3,c4;

//	printf("%d %d\n",nr_btn,nr_stk);

//  	printf(" 1=%2d ",ps3dat->button[PAD_KEY_LEFT]);
//	printf(" 2=%2d ",ps3dat->button[PAD_KEY_RIGHT]);
//	printf(" 3=%2d ",ps3dat->button[PAD_KEY_UP]);
//	printf(" 4=%2d ",ps3dat->button[PAD_KEY_DOWN]);
//	printf(" 5=%4d ",ps3dat->stick [PAD_LEFT_X]);
//	printf(" 6=%4d ",ps3dat->stick [PAD_LEFT_Y]);
//	printf(" 7=%4d ",ps3dat->stick [PAD_RIGHT_X]);
//	system("clear");

    v1 = (-ps3dat->stick [PAD_LEFT_X])*(1-ps3dat->button[PAD_KEY_L_JOYSTICK]);    // 横軸入力
    v2 = ps3dat->stick [PAD_LEFT_Y];    // 縦軸入力
    ww = (-ps3dat->stick [PAD_LEFT_X])*(ps3dat->button[PAD_KEY_L_JOYSTICK]);   // 回転入力

    c1 = ( 8 * v1 +  8 * v2 + -6 * -ww ) / 10;   // 左前
    c2 = ( 8 * v1 + -8 * v2 +  6 * ww ) / 10;   // 右前
    c3 = ( 8 * v1 + -8 * v2 +  6 * -ww ) / 10;   // 左後
    c4 = ( 8 * v1 +  8 * v2 + -6 * ww ) / 10;   // 右後
    
    if (ps3dat->button[PAD_KEY_L2]) {
		c1 = c1 / 15;
		c2 = c2 / 15;
		c3 = c3 / 15;
		c4 = c4 / 15;
	};

    printf(" 左前=%4d ",c1);
    printf(" 右前=%4d ",c2);
    printf("\n");
    printf(" 左後=%4d ",c3);
    printf(" 右後=%4d ",c4);
    printf(" d_mode=%4d ",d_mode);
    printf(" L_JOYSTICK=%4d ",ps3dat->button[PAD_KEY_L_JOYSTICK]);
    printf("\n");
    printf("\n");

	if(abs(c1) < 5) {   // 左前
		softPwmWrite(27,0);
		softPwmWrite(26,0);
	} else if(c1 > 0) {
		softPwmWrite(27,0);
		softPwmWrite(26,abs(c1));
	} else {
		softPwmWrite(27,abs(c1));
		softPwmWrite(26,0);
	};

	if(abs(c2) < 5) {   // 右前
		softPwmWrite(29,0);
		softPwmWrite(28,0);
	} else if(c2 > 0) {
		softPwmWrite(29,0);
		softPwmWrite(28,abs(c2));
	} else {
		softPwmWrite(29,abs(c2));
		softPwmWrite(28,0);
	};

	if(abs(c3) < 5) {   // 左後
		softPwmWrite(5,0);
		softPwmWrite(6,0);
	} else if(c3 > 0) {
		softPwmWrite(5,0);
		softPwmWrite(6,abs(c3));
	} else {
		softPwmWrite(5,abs(c3));
		softPwmWrite(6,0);
	};

	if(abs(c4) < 5) {   // 右後
		softPwmWrite(25,0);
		softPwmWrite(24,0);
	} else if(c4 > 0) {
		softPwmWrite(25,0);
		softPwmWrite(24,abs(c4));
	} else {
		softPwmWrite(25,abs(c4));
		softPwmWrite(24,0);
	};

//   if(ps3dat->stick [PAD_RIGHT_Y] > +40) {d_mode++; if(d_mode > +100) d_mode = +100; };
//   if(ps3dat->stick [PAD_RIGHT_Y] < -40) {d_mode--; if(d_mode < -100) d_mode = -100; };
//	 setPCA9685Duty(fds , 11 ,  d_mode);

	if((ps3dat->button[PAD_KEY_UP])&&(ps3dat->button[PAD_KEY_L2])) btn_upd++;
	if((!ps3dat->button[PAD_KEY_UP])&&(ps3dat->button[PAD_KEY_L2])) btn_upd = 0;
	if(b_btn_upd > btn_upd) {
		d_mode++; if (d_mode > 21) d_mode = 21;
		if(d_mode == 1 ) system("mpg123 /home/pi/Music/Direction_1.mp3 &");
		if(d_mode == 2 ) system("mpg123 /home/pi/Music/Direction_2.mp3 &");
		if(d_mode == 3 ) system("mpg123 /home/pi/Music/Direction_3.mp3 &");
		if(d_mode == 4 ) system("mpg123 /home/pi/Music/Direction_4.mp3 &");
		if(d_mode == 5 ) system("mpg123 /home/pi/Music/Direction_5.mp3 &");
		if(d_mode == 6 ) system("mpg123 /home/pi/Music/Direction_6.mp3 &");
		if(d_mode == 7 ) system("mpg123 /home/pi/Music/Direction_7.mp3 &");
		if(d_mode == 8 ) system("mpg123 /home/pi/Music/Direction_8.mp3 &");
		if(d_mode == 9 ) system("mpg123 /home/pi/Music/Direction_9.mp3 &");
		if(d_mode == 10 ) system("mpg123 /home/pi/Music/Direction_10.mp3 &");
		if(d_mode == 11 ) system("mpg123 /home/pi/Music/Direction_11.mp3 &");
		if(d_mode == 12 ) system("mpg123 /home/pi/Music/Direction_12.mp3 &");
		if(d_mode == 13 ) system("mpg123 /home/pi/Music/Direction_13.mp3 &");
		if(d_mode == 14 ) system("mpg123 /home/pi/Music/Direction_14.mp3 &");
		if(d_mode == 15 ) system("mpg123 /home/pi/Music/Direction_15.mp3 &");
		if(d_mode == 16 ) system("mpg123 /home/pi/Music/Direction_16.mp3 &");
		if(d_mode == 17 ) system("mpg123 /home/pi/Music/Direction_17.mp3 &");
		if(d_mode == 18 ) system("mpg123 /home/pi/Music/Direction_18.mp3 &");
		if(d_mode == 19 ) system("mpg123 /home/pi/Music/Direction_19.mp3 &");
		if(d_mode == 20 ) system("mpg123 /home/pi/Music/Direction_20.mp3 &");
		if(d_mode == 21 ) system("mpg123 /home/pi/Music/Direction_21.mp3 &");
	};
	b_btn_upd = btn_upd;

	if((ps3dat->button[PAD_KEY_DOWN])&&(ps3dat->button[PAD_KEY_L2])) btn_downd++;
	if((!ps3dat->button[PAD_KEY_DOWN])&&(ps3dat->button[PAD_KEY_L2])) btn_downd = 0;
	if(b_btn_downd > btn_downd) {
		d_mode--; if (d_mode < 1) d_mode = 1;
		if(d_mode == 1 ) system("mpg123 /home/pi/Music/Direction_1.mp3 &");
		if(d_mode == 2 ) system("mpg123 /home/pi/Music/Direction_2.mp3 &");
		if(d_mode == 3 ) system("mpg123 /home/pi/Music/Direction_3.mp3 &");
		if(d_mode == 4 ) system("mpg123 /home/pi/Music/Direction_4.mp3 &");
		if(d_mode == 5 ) system("mpg123 /home/pi/Music/Direction_5.mp3 &");
		if(d_mode == 6 ) system("mpg123 /home/pi/Music/Direction_6.mp3 &");
		if(d_mode == 7 ) system("mpg123 /home/pi/Music/Direction_7.mp3 &");
		if(d_mode == 8 ) system("mpg123 /home/pi/Music/Direction_8.mp3 &");
		if(d_mode == 9 ) system("mpg123 /home/pi/Music/Direction_9.mp3 &");
		if(d_mode == 10 ) system("mpg123 /home/pi/Music/Direction_10.mp3 &");
		if(d_mode == 11 ) system("mpg123 /home/pi/Music/Direction_11.mp3 &");
		if(d_mode == 12 ) system("mpg123 /home/pi/Music/Direction_12.mp3 &");
		if(d_mode == 13 ) system("mpg123 /home/pi/Music/Direction_13.mp3 &");
		if(d_mode == 14 ) system("mpg123 /home/pi/Music/Direction_14.mp3 &");
		if(d_mode == 15 ) system("mpg123 /home/pi/Music/Direction_15.mp3 &");
		if(d_mode == 16 ) system("mpg123 /home/pi/Music/Direction_16.mp3 &");
		if(d_mode == 17 ) system("mpg123 /home/pi/Music/Direction_17.mp3 &");
		if(d_mode == 18 ) system("mpg123 /home/pi/Music/Direction_18.mp3 &");
		if(d_mode == 19 ) system("mpg123 /home/pi/Music/Direction_19.mp3 &");
		if(d_mode == 20 ) system("mpg123 /home/pi/Music/Direction_20.mp3 &");
		if(d_mode == 21 ) system("mpg123 /home/pi/Music/Direction_21.mp3 &");
	};
	b_btn_downd = btn_downd;

	if(d_mode == 1 ) setPCA9685Duty(fds , 11 ,  100);
	if(d_mode == 2 ) setPCA9685Duty(fds , 11 ,  90);
	if(d_mode == 3 ) setPCA9685Duty(fds , 11 ,  80);
	if(d_mode == 4 ) setPCA9685Duty(fds , 11 ,  70);
	if(d_mode == 5 ) setPCA9685Duty(fds , 11 ,  60);
	if(d_mode == 6 ) setPCA9685Duty(fds , 11 ,  50);
	if(d_mode == 7 ) setPCA9685Duty(fds , 11 ,  40);
	if(d_mode == 8 ) setPCA9685Duty(fds , 11 ,  30);
	if(d_mode == 9 ) setPCA9685Duty(fds , 11 ,  20);
	if(d_mode == 10 ) setPCA9685Duty(fds , 11 , 10);
	if(d_mode == 11 ) setPCA9685Duty(fds , 11 , 0);
	if(d_mode == 12 ) setPCA9685Duty(fds , 11 , -10);
	if(d_mode == 13 ) setPCA9685Duty(fds , 11 , -20);
	if(d_mode == 14 ) setPCA9685Duty(fds , 11 , -30);
	if(d_mode == 15 ) setPCA9685Duty(fds , 11 , -40);
	if(d_mode == 16 ) setPCA9685Duty(fds , 11 , -50);
	if(d_mode == 17 ) setPCA9685Duty(fds , 11 , -60);
	if(d_mode == 18 ) setPCA9685Duty(fds , 11 , -70);
	if(d_mode == 19 ) setPCA9685Duty(fds , 11 , -80);
	if(d_mode == 20 ) setPCA9685Duty(fds , 11 , -90);
	if(d_mode == 21 ) setPCA9685Duty(fds , 11 , -100);

	
	if(ps3dat->button[PAD_KEY_CROSS]) btn_cross++;	// 皿の握る・離す
	if(!ps3dat->button[PAD_KEY_CROSS]) btn_cross = 0;
	if(b_btn_cross > btn_cross) {
		if(l_mode_b == 100) l_mode_b = -80; else l_mode_b = 100;
		if(l_mode_b == 100 )   system("mpg123 /home/pi/Music/Open_my_left_tray.mp3 &");
		if(l_mode_b == -80 ) system("mpg123 /home/pi/Music/Close_my_left_tray.mp3 &");

	};
	b_btn_cross = btn_cross;
	setPCA9685Duty(fds , 10 ,  l_mode_b);

	if(ps3dat->button[PAD_KEY_CIRCLE]) btn_circle++;	// 皿の握る・離す
	if(!ps3dat->button[PAD_KEY_CIRCLE]) btn_circle = 0;
	if(b_btn_circle > btn_circle) {
		if(r_mode_b == +90) r_mode_b = -80; else r_mode_b = +90;
		if(r_mode_b == +90 )   system("mpg123 /home/pi/Music/Close_my_right_tray.mp3 &");
		if(r_mode_b == -80 ) system("mpg123 /home/pi/Music/Open_my_right_tray.mp3 &");

	};
	b_btn_circle = btn_circle;
	setPCA9685Duty(fds , 12 ,  r_mode_b);
	
	if((ps3dat->button[PAD_KEY_UP])&&(1-ps3dat->button[PAD_KEY_L2])) btn_up++;
	if((!ps3dat->button[PAD_KEY_UP])&&(1-ps3dat->button[PAD_KEY_L2])) btn_up = 0;
	if(b_btn_up > btn_up) {
		h_mode++; if (h_mode > 20) h_mode = 20;
		if(h_mode == 1 ) system("mpg123 /home/pi/Music/Height_1.mp3 &");
		if(h_mode == 2 ) system("mpg123 /home/pi/Music/Height_2.mp3 &");
		if(h_mode == 3 ) system("mpg123 /home/pi/Music/Height_3.mp3 &");
		if(h_mode == 4 ) system("mpg123 /home/pi/Music/Height_4.mp3 &");
		if(h_mode == 5 ) system("mpg123 /home/pi/Music/Height_5.mp3 &");
		if(h_mode == 6 ) system("mpg123 /home/pi/Music/Height_6.mp3 &");
		if(h_mode == 7 ) system("mpg123 /home/pi/Music/Height_7.mp3 &");
		if(h_mode == 8 ) system("mpg123 /home/pi/Music/Height_8.mp3 &");
		if(h_mode == 9 ) system("mpg123 /home/pi/Music/Height_9.mp3 &");
		if(h_mode == 10 ) system("mpg123 /home/pi/Music/Height_10.mp3 &");
		if(h_mode == 11 ) system("mpg123 /home/pi/Music/Height_11.mp3 &");
		if(h_mode == 12 ) system("mpg123 /home/pi/Music/Height_12.mp3 &");
		if(h_mode == 13 ) system("mpg123 /home/pi/Music/Height_13.mp3 &");
		if(h_mode == 14 ) system("mpg123 /home/pi/Music/Height_14.mp3 &");
		if(h_mode == 15 ) system("mpg123 /home/pi/Music/Height_15.mp3 &");
		if(h_mode == 16 ) system("mpg123 /home/pi/Music/Height_16.mp3 &");
		if(h_mode == 17 ) system("mpg123 /home/pi/Music/Height_17.mp3 &");
		if(h_mode == 18 ) system("mpg123 /home/pi/Music/Height_18.mp3 &");
		if(h_mode == 19 ) system("mpg123 /home/pi/Music/Height_19.mp3 &");
		if(h_mode == 20 ) system("mpg123 /home/pi/Music/Height_20.mp3 &");
	};
	b_btn_up = btn_up;

	if((ps3dat->button[PAD_KEY_DOWN])&&(1-ps3dat->button[PAD_KEY_L2])) btn_down++;
	if((!ps3dat->button[PAD_KEY_DOWN])&&(1-ps3dat->button[PAD_KEY_L2])) btn_down = 0;
	if(b_btn_down > btn_down) {
		h_mode--; if (h_mode < 1) h_mode = 1;
		if(h_mode == 1 ) system("mpg123 /home/pi/Music/Height_1.mp3 &");
		if(h_mode == 2 ) system("mpg123 /home/pi/Music/Height_2.mp3 &");
		if(h_mode == 3 ) system("mpg123 /home/pi/Music/Height_3.mp3 &");
		if(h_mode == 4 ) system("mpg123 /home/pi/Music/Height_4.mp3 &");
		if(h_mode == 5 ) system("mpg123 /home/pi/Music/Height_5.mp3 &");
		if(h_mode == 6 ) system("mpg123 /home/pi/Music/Height_6.mp3 &");
		if(h_mode == 7 ) system("mpg123 /home/pi/Music/Height_7.mp3 &");
		if(h_mode == 8 ) system("mpg123 /home/pi/Music/Height_8.mp3 &");
		if(h_mode == 9 ) system("mpg123 /home/pi/Music/Height_9.mp3 &");
		if(h_mode == 10 ) system("mpg123 /home/pi/Music/Height_10.mp3 &");
		if(h_mode == 11 ) system("mpg123 /home/pi/Music/Height_11.mp3 &");
		if(h_mode == 12 ) system("mpg123 /home/pi/Music/Height_12.mp3 &");
		if(h_mode == 13 ) system("mpg123 /home/pi/Music/Height_13.mp3 &");
		if(h_mode == 14 ) system("mpg123 /home/pi/Music/Height_14.mp3 &");
		if(h_mode == 15 ) system("mpg123 /home/pi/Music/Height_15.mp3 &");
		if(h_mode == 16 ) system("mpg123 /home/pi/Music/Height_16.mp3 &");
		if(h_mode == 17 ) system("mpg123 /home/pi/Music/Height_17.mp3 &");
		if(h_mode == 18 ) system("mpg123 /home/pi/Music/Height_18.mp3 &");
		if(h_mode == 19 ) system("mpg123 /home/pi/Music/Height_19.mp3 &");
		if(h_mode == 20 ) system("mpg123 /home/pi/Music/Height_20.mp3 &");
	};
	b_btn_down = btn_down;

	if(h_mode == 1 ) setPCA9685Duty(fds , 9 ,  -100);
	if(h_mode == 2 ) setPCA9685Duty(fds , 9 ,  -90);
	if(h_mode == 3 ) setPCA9685Duty(fds , 9 ,  -80);
	if(h_mode == 4 ) setPCA9685Duty(fds , 9 ,  -70);
	if(h_mode == 5 ) setPCA9685Duty(fds , 9 ,  -60);
	if(h_mode == 6 ) setPCA9685Duty(fds , 9 ,  -50);
	if(h_mode == 7 ) setPCA9685Duty(fds , 9 ,  -40);
	if(h_mode == 8 ) setPCA9685Duty(fds , 9 ,  -30);
	if(h_mode == 9 ) setPCA9685Duty(fds , 9 ,  -20);
	if(h_mode == 10 ) setPCA9685Duty(fds , 9 , -10);
	if(h_mode == 11 ) setPCA9685Duty(fds , 9 , 0);
	if(h_mode == 12 ) setPCA9685Duty(fds , 9 , 10);
	if(h_mode == 13 ) setPCA9685Duty(fds , 9 , 20);
	if(h_mode == 14 ) setPCA9685Duty(fds , 9 , 30);
	if(h_mode == 15 ) setPCA9685Duty(fds , 9 , 40);
	if(h_mode == 16 ) setPCA9685Duty(fds , 9 , 50);
	if(h_mode == 17 ) setPCA9685Duty(fds , 9 , 60);
	if(h_mode == 18 ) setPCA9685Duty(fds , 9 , 70);
	if(h_mode == 19 ) setPCA9685Duty(fds , 9 , 80);
	if(h_mode == 20 ) setPCA9685Duty(fds , 9 , 90);
	if(h_mode == 21 ) setPCA9685Duty(fds , 9 , 100);

	if(ps3dat->button[PAD_KEY_RIGHT]) btn_right++;
	if(!ps3dat->button[PAD_KEY_RIGHT]) btn_right = 0;
	if(b_btn_right > btn_right) {
		t_mode++; if (t_mode > 21) t_mode = 21;
		if(t_mode == 1 ) system("mpg123 /home/pi/Music/tray_number_1.mp3 &");
		if(t_mode == 2 ) system("mpg123 /home/pi/Music/tray_number_2.mp3 &");
		if(t_mode == 3 ) system("mpg123 /home/pi/Music/tray_number_3.mp3 &");
		if(t_mode == 4 ) system("mpg123 /home/pi/Music/tray_number_4.mp3 &");
		if(t_mode == 5 ) system("mpg123 /home/pi/Music/tray_number_5.mp3 &");
		if(t_mode == 6 ) system("mpg123 /home/pi/Music/tray_number_6.mp3 &");
		if(t_mode == 7 ) system("mpg123 /home/pi/Music/tray_number_7.mp3 &");
		if(t_mode == 8 ) system("mpg123 /home/pi/Music/tray_number_8.mp3 &");
		if(t_mode == 9 ) system("mpg123 /home/pi/Music/tray_number_9.mp3 &");
		if(t_mode == 10 ) system("mpg123 /home/pi/Music/tray_number_10.mp3 &");
		if(t_mode == 11 ) system("mpg123 /home/pi/Music/tray_number_11.mp3 &");
		if(t_mode == 12 ) system("mpg123 /home/pi/Music/tray_number_12.mp3 &");
		if(t_mode == 13 ) system("mpg123 /home/pi/Music/tray_number_13.mp3 &");
		if(t_mode == 14 ) system("mpg123 /home/pi/Music/tray_number_14.mp3 &");
		if(t_mode == 15 ) system("mpg123 /home/pi/Music/tray_number_15.mp3 &");
		if(t_mode == 16 ) system("mpg123 /home/pi/Music/tray_number_16.mp3 &");
		if(t_mode == 17 ) system("mpg123 /home/pi/Music/tray_number_17.mp3 &");
		if(t_mode == 18 ) system("mpg123 /home/pi/Music/tray_number_18.mp3 &");
		if(t_mode == 19 ) system("mpg123 /home/pi/Music/tray_number_19.mp3 &");
		if(t_mode == 20 ) system("mpg123 /home/pi/Music/tray_number_20.mp3 &");
		if(t_mode == 21 ) system("mpg123 /home/pi/Music/tray_number_21.mp3 &");
	};
	b_btn_right = btn_right;

	if(ps3dat->button[PAD_KEY_LEFT]) btn_left++;
	if(!ps3dat->button[PAD_KEY_LEFT]) btn_left = 0;
	if(b_btn_left > btn_left) {
		t_mode--; if (t_mode < 1) t_mode = 1;
		if(t_mode == 1 ) system("mpg123 /home/pi/Music/tray_number_1.mp3 &");
		if(t_mode == 2 ) system("mpg123 /home/pi/Music/tray_number_2.mp3 &");
		if(t_mode == 3 ) system("mpg123 /home/pi/Music/tray_number_3.mp3 &");
		if(t_mode == 4 ) system("mpg123 /home/pi/Music/tray_number_4.mp3 &");
		if(t_mode == 5 ) system("mpg123 /home/pi/Music/tray_number_5.mp3 &");
		if(t_mode == 6 ) system("mpg123 /home/pi/Music/tray_number_6.mp3 &");
		if(t_mode == 7 ) system("mpg123 /home/pi/Music/tray_number_7.mp3 &");
		if(t_mode == 8 ) system("mpg123 /home/pi/Music/tray_number_8.mp3 &");
		if(t_mode == 9 ) system("mpg123 /home/pi/Music/tray_number_9.mp3 &");
		if(t_mode == 10 ) system("mpg123 /home/pi/Music/tray_number_10.mp3 &");
		if(t_mode == 11 ) system("mpg123 /home/pi/Music/tray_number_11.mp3 &");
		if(t_mode == 12 ) system("mpg123 /home/pi/Music/tray_number_12.mp3 &");
		if(t_mode == 13 ) system("mpg123 /home/pi/Music/tray_number_13.mp3 &");
		if(t_mode == 14 ) system("mpg123 /home/pi/Music/tray_number_14.mp3 &");
		if(t_mode == 15 ) system("mpg123 /home/pi/Music/tray_number_15.mp3 &");
		if(t_mode == 16 ) system("mpg123 /home/pi/Music/tray_number_16.mp3 &");
		if(t_mode == 17 ) system("mpg123 /home/pi/Music/tray_number_17.mp3 &");
		if(t_mode == 18 ) system("mpg123 /home/pi/Music/tray_number_18.mp3 &");
		if(t_mode == 19 ) system("mpg123 /home/pi/Music/tray_number_19.mp3 &");
		if(t_mode == 20 ) system("mpg123 /home/pi/Music/tray_number_20.mp3 &");
		if(t_mode == 21 ) system("mpg123 /home/pi/Music/tray_number_21.mp3 &");
	};
	b_btn_left = btn_left;

	if(t_mode == 1 ) setPCA9685Duty(fds , 8 ,  -100);
	if(t_mode == 2 ) setPCA9685Duty(fds , 8 ,  -90);
	if(t_mode == 3 ) setPCA9685Duty(fds , 8 ,  -80);
	if(t_mode == 4 ) setPCA9685Duty(fds , 8 ,  -70);
	if(t_mode == 5 ) setPCA9685Duty(fds , 8 ,  -60);
	if(t_mode == 6 ) setPCA9685Duty(fds , 8 ,  -50);
	if(t_mode == 7 ) setPCA9685Duty(fds , 8 ,  -40);
	if(t_mode == 8 ) setPCA9685Duty(fds , 8 ,  -30);
	if(t_mode == 9 ) setPCA9685Duty(fds , 8 ,  -20);
	if(t_mode == 10 ) setPCA9685Duty(fds , 8 , -10);
	if(t_mode == 11 ) setPCA9685Duty(fds , 8 , 0);
	if(t_mode == 12 ) setPCA9685Duty(fds , 8 , 10);
	if(t_mode == 13 ) setPCA9685Duty(fds , 8 , 20);
	if(t_mode == 14 ) setPCA9685Duty(fds , 8 , 30);
	if(t_mode == 15 ) setPCA9685Duty(fds , 8 , 40);
	if(t_mode == 16 ) setPCA9685Duty(fds , 8 , 50);
	if(t_mode == 17 ) setPCA9685Duty(fds , 8 , 60);
	if(t_mode == 18 ) setPCA9685Duty(fds , 8 , 70);
	if(t_mode == 19 ) setPCA9685Duty(fds , 8 , 80);
	if(t_mode == 20 ) setPCA9685Duty(fds , 8 , 90);
	if(t_mode == 21 ) setPCA9685Duty(fds , 8 , 100);


	if(ps3dat->button[PAD_KEY_L1]) btn_l1++;
	if(!ps3dat->button[PAD_KEY_L1]) btn_l1 = 0;
	if(b_btn_l1 > btn_l1) {
		l_mode++; if (l_mode > 9) l_mode = 0;
		if(l_mode == 0 ) system("mpg123 /home/pi/Music/fire.mp3 &");
		if(l_mode == 1 ) system("mpg123 /home/pi/Music/01.mp3 &");
		if(l_mode == 2 ) system("mpg123 /home/pi/Music/Lower_my_left_arm.mp3 &");
		if(l_mode == 3 ) system("mpg123 /home/pi/Music/Raise_my_left_arm.mp3 &");
		if(l_mode == 4 ) system("mpg123 /home/pi/Music/Posted_on_my_left_arm.mp3 &");
		if(l_mode == 5 ) system("mpg123 /home/pi/Music/Release_my_left_arm.mp3 &");
		if(l_mode == 6 ) system("mpg123 /home/pi/Music/06.mp3 &");
		if(l_mode == 7 ) system("mpg123 /home/pi/Music/07.mp3 &");
		if(l_mode == 8 ) system("mpg123 /home/pi/Music/08.mp3 &");
		if(l_mode == 9 ) system("mpg123 /home/pi/Music/09.mp3 &");
	};
	b_btn_l1 = btn_l1;

	
	if(l_mode == -3) {
		setPCA9685Duty(fds , 0 ,  -60);	// 右腕　格納
		setPCA9685Duty(fds , 1 ,  -60);
		setPCA9685Duty(fds , 2 ,  -160);
	};

	if(l_mode == -2) {
		setPCA9685Duty(fds , 0 ,  0);	// 左腕 格納直前
		setPCA9685Duty(fds , 1 ,  0);
		setPCA9685Duty(fds , 2 ,  -160);
	};

	if(l_mode == -1) {
		setPCA9685Duty(fds , 0 ,  0);	// 左腕 垂直
		setPCA9685Duty(fds , 1 ,  0);
		setPCA9685Duty(fds , 2 ,  0);
	};

	if(l_mode == 2) {
		setPCA9685Duty(fds , 0 ,  106);//106	// 左腕　すくう
		setPCA9685Duty(fds , 1 ,  -60);//-60
		setPCA9685Duty(fds , 2 ,  160);//180
	};
	
	if(l_mode == 3) {
		setPCA9685Duty(fds , 0 ,  -20);	// 左腕　ちょい持ち上げ
		setPCA9685Duty(fds , 1 ,  -70);
		setPCA9685Duty(fds , 2 ,  180);
	};

	if(l_mode == 4) {
		setPCA9685Duty(fds , 0 ,  -35);	// 左腕　置く 電圧が下がったら-24 上がったら-35という変な設定
		setPCA9685Duty(fds , 1 ,  30);
		setPCA9685Duty(fds , 2 ,  90);
	};

	if(l_mode == 5) {
		setPCA9685Duty(fds , 0 ,  10);	// 左腕　ゆっくり離す
		setPCA9685Duty(fds , 1 ,  30);
		setPCA9685Duty(fds , 2 ,  90);
	};
	
	if(l_mode == 6) {
		setPCA9685Duty(fds , 0 ,  30);	// 左腕　ちょい持ち上げ
		setPCA9685Duty(fds , 1 ,  30);
		setPCA9685Duty(fds , 2 ,  90);
	};
	
// *************************************************************************** //	
	if(ps3dat->button[PAD_KEY_R1]) btn_r1++;
	if(!ps3dat->button[PAD_KEY_R1]) btn_r1 = 0;
	if(b_btn_r1 > btn_r1) {
		r_mode++; if (r_mode > 9) r_mode = 0;
		if(r_mode == 0 ) system("mpg123 /home/pi/Music/fire.mp3 &");
		if(r_mode == 1 ) system("mpg123 /home/pi/Music/01.mp3 &");
		if(r_mode == 2 ) system("mpg123 /home/pi/Music/02.mp3 &");
		if(r_mode == 3 ) system("mpg123 /home/pi/Music/03.mp3 &");
		if(r_mode == 4 ) system("mpg123 /home/pi/Music/04.mp3 &");
		if(r_mode == 5 ) system("mpg123 /home/pi/Music/05.mp3 &");
		if(r_mode == 6 ) system("mpg123 /home/pi/Music/06.mp3 &");
		if(r_mode == 7 ) system("mpg123 /home/pi/Music/07.mp3 &");
		if(r_mode == 8 ) system("mpg123 /home/pi/Music/08.mp3 &");
		if(r_mode == 9 ) system("mpg123 /home/pi/Music/09.mp3 &");
	};
	b_btn_r1 = btn_r1;

//	if(ps3dat->button[PAD_KEY_R2]) btn_r2++;
//	if(!ps3dat->button[PAD_KEY_R2]) btn_r2 = 0;
	if(b_btn_r2 > btn_r2) {
		r_mode--; if (r_mode < 0) r_mode = 9;
		if(r_mode == 0 ) system("mpg123 /home/pi/Music/fire.mp3 &");
		if(r_mode == 1 ) system("mpg123 /home/pi/Music/01.mp3 &");
		if(r_mode == 2 ) system("mpg123 /home/pi/Music/02.mp3 &");
		if(r_mode == 3 ) system("mpg123 /home/pi/Music/03.mp3 &");
		if(r_mode == 4 ) system("mpg123 /home/pi/Music/04.mp3 &");
		if(r_mode == 5 ) system("mpg123 /home/pi/Music/05.mp3 &");
		if(r_mode == 6 ) system("mpg123 /home/pi/Music/06.mp3 &");
		if(r_mode == 7 ) system("mpg123 /home/pi/Music/07.mp3 &");
		if(r_mode == 8 ) system("mpg123 /home/pi/Music/08.mp3 &");
		if(r_mode == 9 ) system("mpg123 /home/pi/Music/09.mp3 &");
	};
	b_btn_r2 = btn_r2;


	if(r_mode == -2) {
		setPCA9685Duty(fds , 0+4 ,  0);//0	// 右腕　?
		setPCA9685Duty(fds , 1+4 ,  0);
		setPCA9685Duty(fds , 2+4 ,  0);//100
	};

	if(r_mode == -1) {
		setPCA9685Duty(fds , 0+4 ,  0);//0	// 右腕　?
		setPCA9685Duty(fds , 1+4 ,  0);
		setPCA9685Duty(fds , 2+4 ,  0);//100
	};
	
	if(r_mode == 2) {
		setPCA9685Duty(fds , 0+4 ,  102);//102	// 右腕　ゲットボトル
		setPCA9685Duty(fds , 1+4 ,  -70);//-70
		setPCA9685Duty(fds , 2+4 ,  170);//180
	};
	
	if(r_mode == 3) {
		setPCA9685Duty(fds , 0+4 ,  -20);	// 右腕　ちょい持ち上げ
		setPCA9685Duty(fds , 1+4 ,  -70);
		setPCA9685Duty(fds , 2+4 ,  180);
	};

	if(r_mode == 4) {
		setPCA9685Duty(fds , 0+4 ,  -20);	// 置く 電圧が下がったら-20 上がったら-30という変な設定
		setPCA9685Duty(fds , 1+4 ,  30);
		setPCA9685Duty(fds , 2+4 ,  110);
	};
	
	if(r_mode == 5) {
		setPCA9685Duty(fds , 2+4 ,  90);
	};

	if(r_mode == 6) {
		setPCA9685Duty(fds , 0+4 ,  10);	// 右腕　ゆっくり離す
		setPCA9685Duty(fds , 1+4 ,  30);
		setPCA9685Duty(fds , 2+4 ,  90);
	};

	if(r_mode == 7) {
		setPCA9685Duty(fds , 0+4 ,  30);	// 左腕　ちょい持ち上げ
		setPCA9685Duty(fds , 1+4 ,  30);
		setPCA9685Duty(fds , 2+4 ,  110);
	};


	if(ps3dat->button[PAD_KEY_SQUARE]) btn_square++;	// 握る・離す
	if(!ps3dat->button[PAD_KEY_SQUARE]) btn_square = 0;
	if(b_btn_square > btn_square) {
		if(l_mode_a == 55) l_mode_a = -25; else l_mode_a = 55;
		if(l_mode_a == 55 )  system("mpg123 /home/pi/Music/Close_my_left_hand.mp3 &");
		if(l_mode_a == -25 ) system("mpg123 /home/pi/Music/Open_my_left_hand.mp3 &");

	};
	b_btn_square = btn_square;
	setPCA9685Duty(fds , 3 ,  l_mode_a);


	if(ps3dat->button[PAD_KEY_TRIANGLE]) btn_tri++;	// 握る・離す
	if(!ps3dat->button[PAD_KEY_TRIANGLE]) btn_tri = 0;
	if(b_btn_tri > btn_tri) {
		if(r_mode_a == 72) r_mode_a = -20; else r_mode_a = 72;
		if(r_mode_a == 72 )  system("mpg123 /home/pi/Music/Close_my_right_hand.mp3 &");
		if(r_mode_a == -20 ) system("mpg123 /home/pi/Music/Open_my_right_hand.mp3 &");
	};
	b_btn_tri = btn_tri;
	setPCA9685Duty(fds , 3+4 ,  r_mode_a);


	if(ps3dat->button[PAD_KEY_START]) {
		system("mpg123 /home/pi/Music/Moves_to_programming_mode.mp3 &");
		softPwmWrite(5,0); // motor-1 10ms
		softPwmWrite(6,0); // motor-1 10ms
		softPwmWrite(26,0); // motor-2 10ms
		softPwmWrite(27,0); // motor-2 10ms
		softPwmWrite(28,0); // motor-3 10ms
		softPwmWrite(29,0); // motor-3 10ms
		softPwmWrite(24,0); // motor-4 10ms
		softPwmWrite(25,0); // motor-4 10ms
		softPwmWrite(14,0); // motor-5 10ms // NC
		softPwmWrite(23,0); // motor-5 10ms // NC
		softPwmWrite(3,0); // beep

		delay(1000);
		return -1; // end of program
	};

	return 0;
	
}


int ps3c_input(struct ps3ctls *ps3dat) {

	int rp;
	struct js_event ev;

	do {
		rp = read(ps3dat->fd, &ev, sizeof(struct js_event));
		if (rp != sizeof(struct js_event)) {
			return -1;
		}
	} while (ev.type & JS_EVENT_INIT);

	switch (ev.type) {
		case JS_EVENT_BUTTON:
			if (ev.number < ps3dat->nr_buttons) {
				ps3dat->button[ev.number] = ev.value;
			}
			break;
		case JS_EVENT_AXIS:
			if (ev.number < ps3dat->nr_sticks) {
				ps3dat->stick[ev.number] = ev.value / 200; // 327 range -32767 ~ +32768 -> -100 ~ +100
			}
			break;
		default:
			break;
	}

	return 0;
}


int ps3c_getinfo(struct ps3ctls *ps3dat) {

	if(ioctl(ps3dat->fd , JSIOCGBUTTONS , &ps3dat->nr_buttons) < 0) return -1;
	if(ioctl(ps3dat->fd , JSIOCGAXES    , &ps3dat->nr_sticks ) < 0) return -2;

	return 0;
}


int ps3c_init(struct ps3ctls *ps3dat, const char *df) {

	unsigned char nr_btn;
	unsigned char nr_stk;
	unsigned char *p;
	int i;

	ps3dat->fd = open(df, O_RDONLY);
	if (ps3dat->fd < 0) return -1;

	if (ps3c_getinfo(ps3dat) < 0) {
		close(ps3dat->fd);
		return -2;
	}

	nr_btn = ps3dat->nr_buttons;
	nr_stk = ps3dat->nr_sticks;
//	printf("%d %d\n",nr_btn,nr_stk);

	p = calloc(nr_btn + nr_stk , sizeof(short));
	if (p == NULL) {
		close(ps3dat->fd);
		return -3;
	}
	ps3dat->button = (short *)p;
	ps3dat->stick  = (short *)&p[nr_btn * sizeof(short)];

//	for (i = 0; i<nr_btn; i++) ps3dat->button[i] = 0;
//	for (i = 0; i<nr_stk; i++) ps3dat->stick [i] = 0;
//	ps3dat->button[PAD_KEY_LEFT]=0;
//	ps3dat->button[PAD_KEY_RIGHT]=0;
//	ps3dat->button[PAD_KEY_UP]=0;;
//	ps3dat->button[PAD_KEY_DOWN]=0;
//	ps3dat->stick [PAD_LEFT_X]=0;
//	ps3dat->stick [PAD_LEFT_Y]=0;
//	ps3dat->stick [PAD_RIGHT_X]=0;
//	ps3dat->stick [PAD_RIGHT_Y]=0;

	return 0;
}

void ps3c_exit   (struct ps3ctls *ps3dat) {

	free (ps3dat->button);
	close(ps3dat->fd);
}


void main() {

	char *df = "/dev/input/js0";
	struct ps3ctls ps3dat;

	wiringPiSetup();
	softPwmCreate(5,0,20); // motor-1 10ms
	softPwmCreate(6,0,20); // motor-1 10ms
	softPwmCreate(26,0,20); // motor-2 10ms
	softPwmCreate(27,0,20); // motor-2 10ms
	softPwmCreate(28,0,20); // motor-3 10ms
	softPwmCreate(29,0,20); // motor-3 10ms
	softPwmCreate(24,0,20); // motor-4 10ms
	softPwmCreate(25,0,20); // motor-4 10ms
	softPwmCreate(14,0,20); // motor-5 10ms // NC
	softPwmCreate(23,0,20); // motor-5 10ms // NC
	softPwmCreate(3,0,20); // beep

	fds = wiringPiI2CSetup(0x40);	// PCA9685
	resetPCA9685(fds);
	setPCA9685Freq(fds,50);
	system("mpg123 /home/pi/Music/Main_system_startup.mp3 &");

	if(!(ps3c_init(&ps3dat, df))) {

		do {
			if (ps3c_test(&ps3dat) < 0) break;
		} while (!(ps3c_input(&ps3dat)));

		ps3c_exit(&ps3dat);
	}
}

