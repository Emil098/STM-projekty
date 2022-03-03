#ifndef _DISP_IRQ_H
#define _DISP_IRQ_H

#include "stm32f3xx.h"
#include "D:\stm projekty\Sterownik_temp\libs\lcd.h"
#include "D:\stm projekty\Sterownik_temp\TrueSTUDIO\Sterownik_temp\MAX31865_v1.h"
#include "D:\stm projekty\Sterownik_temp\TrueSTUDIO\Sterownik_temp\TC74A0.h"
#include "D:\stm projekty\Sterownik_temp\TrueSTUDIO\Sterownik_temp\touch.h"

#define MAIN_SCREEN 0 // makra wyœwietlanych ekranów
#define MAIN_MENU 1
#define TIME_SCREEN 2
#define PUMP_T_MIN_SCREEN 3
#define PUMP_T_MAX_SCREEN 4
#define SCREEN_QUANTITY 5

#define SETTING_BTN 0 // makro przycisków ekranu g³ównego

//#define EXIT_BTN 0xFF	// makro przycisków ekranu menu g³ównego
#define EXIT_BTN 0
#define SET_TIME_BTN 1
#define SET_PUMP_T_MIN_BTN 2
#define SET_PUMP_T_MAX_BTN 3

// makra menu zmiany godziny
// EXIT juz zdefiniowany
#define OK_BTN 1
#define INC_HOUR_BTN 2
#define DEC_HOUR_BTN 3
#define INC_MIN_BTN 4
#define DEC_MIN_BTN 5

// makra menu zmiany ró¿nicy temperatur (te same dla t. min. i t. max.)
// EXIT juz zdefiniowany
// OK juz zdefiniowany
#define SOLAR_BTN 2
#define PIEC_BTN 3
#define OBIEG_BTN 4
#define INC_TEMP_BTN 5
#define DEC_TEMP_BTN 6

#define SOLAR_DIFF 0
#define PIEC_DIFF 1
#define OBIEG_DIFF 2


typedef struct{
	uint16_t btn_x[SCREEN_QUANTITY][10];
	uint16_t btn_x_w[SCREEN_QUANTITY][10];
	uint16_t btn_y[SCREEN_QUANTITY][10];
	uint16_t btn_y_h[SCREEN_QUANTITY][10];

	uint8_t menu_state;
}btn_location;

typedef struct{
	float temp_diff[3];
	uint8_t hysteresis[3];
	uint16_t pwm_val[3];
	float voltage_val[3];
}temperature_differential_val;	// przechowuje dane o ró¿nicy tepm, wskazaniach napiêcia i PWM


typedef struct{
	Max_read zasobnik_t;
	Max_read solar_t;
	Max_read piec_t;
	tc obieg_t;
	temperature_differential_val diff;
}temperature;

typedef struct{
	int8_t turn_on_val[3];
	int8_t turn_12V_val[3];
//	uint8_t diff_in_use;
}differntial_vals;

typedef struct{
	uint8_t sec;
	uint8_t min;
	uint8_t hour;
	uint8_t day;
	uint8_t mon;
	uint8_t year;
}time;

btn_location btn;

temperature temp_last;

time t0;

differntial_vals diff;

uint8_t diff_in_use;

int8_t menu_vals[3];





void print_screen(uint8_t scr);

differntial_vals diff_val_init();

btn_location btn_init();

void get_diff_struct(differntial_vals c);

void exit_sign(uint16_t x0, uint16_t y0);

void ok_sign(uint16_t x0, uint16_t y0);

void update_main_screen(temperature temp, time tim ,temperature_differential_val diff_in);

differntial_vals update_temp_screen(point ts_p,uint8_t scr);

time update_time_screen(point ts_p);

void get_time(time c);

temperature temp_reset();

void up_sign(uint16_t x0, uint16_t y0,uint16_t bg_colour ,uint16_t colour);

void down_sign(uint16_t x0, uint16_t y0,uint16_t bg_colour ,uint16_t colour);

void setting_sign(int16_t x0, int16_t y0,uint16_t colour);

#endif
