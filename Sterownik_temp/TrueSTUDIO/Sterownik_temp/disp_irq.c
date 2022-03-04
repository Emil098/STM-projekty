//#include <math.h>
#include "disp_irq.h"


void get_time(time c){
	// funkcja nadpisuj�ca struktur� lokaln� czasu
	t0 = c;
}

static void print_diff_optins(uint8_t opt, uint8_t scr){
	// funkcja nadpisuj�ca pola do wyboru ustawie� czujnika w menu wyboru r�nic temperatur min. i maks.

	// aktualnie wybrany czujnik posiada zielony kolor t�a, pozosta�e s� w kolorze czerwonym
	uint16_t bg_colour[3];
	uint8_t i;
	for(i=0;i<3;i++)bg_colour[i] = RED;
	bg_colour[opt] = GREEN;

	// nadpisanie przycisku ustawie� solara
	LCD_FillRoundRect(220,5,100,35,5,bg_colour[SOLAR_DIFF]);
	LCD_DrawRoundRect(220,5,100,35,5,WHITE);
	LCD_SetCursor(235,10);
	LCD_SetTextColor(BLACK,bg_colour[SOLAR_DIFF]);
	LCD_Printf("solar");

	// nadpisanie przycisku ustawie� pieca
	LCD_FillRoundRect(220,45,100,35,5,bg_colour[PIEC_DIFF]);
	LCD_DrawRoundRect(220,45,100,35,5,WHITE);
	LCD_SetCursor(245,50);
	LCD_SetTextColor(BLACK,bg_colour[PIEC_DIFF]);
	LCD_Printf("piec");

	// nadpisanie przycisku ustawie� obiegu (tylko w wersji do domu)
#ifdef DO_DOMU
	if(scr == PUMP_T_MIN_SCREEN){
	LCD_FillRoundRect(220,85,100,35,5,bg_colour[OBIEG_DIFF]);
	LCD_DrawRoundRect(220,85,100,35,5,WHITE);
	LCD_SetCursor(235,90);
	LCD_SetTextColor(BLACK,bg_colour[OBIEG_DIFF]);
	LCD_Printf("obieg");
	}
#endif

	// w przypadku temp obiegu pompka uruchamia si� gdy wskazanie z czujnika jest mniejsze ni� w zasobniku
	// pozosta�e czujniki za��czaj� si� przy dodatnich r�nicach temperatur
	LCD_SetCursor(120,105);
	LCD_SetTextColor(WHITE,BLACK);
	if(opt != OBIEG_DIFF)LCD_Printf("+");
	else LCD_Printf("-");

}
differntial_vals diff_val_init(){
	// funkcja ustawiaj�ca domy�lne ustawienia r�nic temperatur

	diff.turn_on_val[SOLAR_DIFF] = 2;
	diff.turn_on_val[PIEC_DIFF] = 2;
	diff.turn_on_val[OBIEG_DIFF] = -2;

	diff.turn_12V_val[SOLAR_DIFF] = 5;
	diff.turn_12V_val[PIEC_DIFF] = 5;

	return diff;
}

void get_diff_struct(differntial_vals c){
	// funkcja aktualizuj�ca struktur� r�nic temp. wew. biblioteki
	diff = c;
}

btn_location btn_init(){
	// funkcja inicjuj�ca po�o�enie i wymiary wszystkich przycisk�w

	//przy inicjacji ustawiany jest ekran g��wny jako aktualnie wy�wietlany
	btn.menu_state = MAIN_SCREEN;
	temp_last = temp_reset();
	uint8_t btn_name,screen_name;

	// ekran g��wny
	screen_name = MAIN_SCREEN;
	btn_name = SETTING_BTN;
	btn.btn_x[screen_name][btn_name] = 150;	// wej�cie do menu g��wnego
	btn.btn_y[screen_name][btn_name] = 10;
	btn.btn_x_w[screen_name][btn_name] = 160;
	btn.btn_y_h[screen_name][btn_name] = 40;

	// menu g��wne
	screen_name = MAIN_MENU;
	btn_name = EXIT_BTN;
	btn.btn_x[screen_name][btn_name] = 260; // wyj�cie do ekranu g��wnego
	btn.btn_y[screen_name][btn_name] = 180;
	btn.btn_x_w[screen_name][btn_name]= 50;
	btn.btn_y_h[screen_name][btn_name] = 50;

	btn_name = SET_TIME_BTN;
	btn.btn_x[screen_name][btn_name] = 5; // wej�cie do menu czasu
	btn.btn_y[screen_name][btn_name] = 5;
	btn.btn_x_w[screen_name][btn_name]= 310;
	btn.btn_y_h[screen_name][btn_name] = 35;

	btn_name = SET_PUMP_T_MIN_BTN;
	btn.btn_x[screen_name][btn_name] = 5; // wej�cie do menu temperatury min pompek
	btn.btn_y[screen_name][btn_name] = 45;
	btn.btn_x_w[screen_name][btn_name]= 310;
	btn.btn_y_h[screen_name][btn_name] = 35;

	btn_name = SET_PUMP_T_MAX_BTN;
	btn.btn_x[screen_name][btn_name] = 5; // wej�cie do menu temperatury max pompek
	btn.btn_y[screen_name][btn_name] = 85;
	btn.btn_x_w[screen_name][btn_name]= 310;
	btn.btn_y_h[screen_name][btn_name] = 35;

	// menu zmiany czasu zalaczania pompki
	screen_name = TIME_SCREEN;
	btn_name = EXIT_BTN;
	btn.btn_x[screen_name][btn_name] = 260; // wyj�cie do ekranu g��wnego
	btn.btn_y[screen_name][btn_name] = 180;
	btn.btn_x_w[screen_name][btn_name]= 50;
	btn.btn_y_h[screen_name][btn_name] = 50;

	btn_name = OK_BTN;
	btn.btn_x[screen_name][btn_name] = 10; // zatwierdzenie ustawien i wyj�cie do ekranu g��wnego
	btn.btn_y[screen_name][btn_name] = 180;
	btn.btn_x_w[screen_name][btn_name]= 50;
	btn.btn_y_h[screen_name][btn_name] = 50;

	btn_name = INC_HOUR_BTN;
	btn.btn_x[screen_name][btn_name] = 105; // zwi�kszenie licznika godziny
	btn.btn_y[screen_name][btn_name] = 50;
	btn.btn_x_w[screen_name][btn_name]= 50;
	btn.btn_y_h[screen_name][btn_name] = 50;

	btn_name = DEC_HOUR_BTN;
	btn.btn_x[screen_name][btn_name] = 105; // zmniejszenie licznika godziny
	btn.btn_y[screen_name][btn_name] = 160;
	btn.btn_x_w[screen_name][btn_name]= 50;
	btn.btn_y_h[screen_name][btn_name] = 50;

	btn_name = INC_MIN_BTN;
	btn.btn_x[screen_name][btn_name] = 165; // zwi�kszenie licznika minuty
	btn.btn_y[screen_name][btn_name] = 50;
	btn.btn_x_w[screen_name][btn_name]= 50;
	btn.btn_y_h[screen_name][btn_name] = 50;

	btn_name = DEC_MIN_BTN;
	btn.btn_x[screen_name][btn_name] = 165; // zmniejszenie licznika minuty
	btn.btn_y[screen_name][btn_name] = 160;
	btn.btn_x_w[screen_name][btn_name]= 50;
	btn.btn_y_h[screen_name][btn_name] = 50;

	// menu ustawienia temp zal pompek
	screen_name = PUMP_T_MIN_SCREEN;
	btn_name = EXIT_BTN;
	btn.btn_x[screen_name][btn_name] = 260; // wyj�cie do ekranu g��wnego
	btn.btn_y[screen_name][btn_name] = 180;
	btn.btn_x_w[screen_name][btn_name]= 50;
	btn.btn_y_h[screen_name][btn_name] = 50;

	btn_name = OK_BTN;
	btn.btn_x[screen_name][btn_name] = 10; // zatwierdzenie ustawien i wyj�cie do ekranu g��wnego
	btn.btn_y[screen_name][btn_name] = 180;
	btn.btn_x_w[screen_name][btn_name]= 50;
	btn.btn_y_h[screen_name][btn_name] = 50;

	btn_name = SOLAR_BTN;
	btn.btn_x[screen_name][btn_name] = 220; // wyb�r ustawie� dla solara
	btn.btn_y[screen_name][btn_name] = 5;
	btn.btn_x_w[screen_name][btn_name]= 100;
	btn.btn_y_h[screen_name][btn_name] = 35;

	btn_name = PIEC_BTN;
	btn.btn_x[screen_name][btn_name] = 220; // wyb�r ustawie� dla pieca
	btn.btn_y[screen_name][btn_name] = 45;
	btn.btn_x_w[screen_name][btn_name]= 100;
	btn.btn_y_h[screen_name][btn_name] = 35;

#ifdef DO_DOMU
	btn_name = OBIEG_BTN;
	btn.btn_x[screen_name][btn_name] = 220; // wyb�r ustawie� dla obiegu
	btn.btn_y[screen_name][btn_name] = 85;
	btn.btn_x_w[screen_name][btn_name]= 100;
	btn.btn_y_h[screen_name][btn_name] = 35;
#endif
#ifdef DO_DOMKU
	btn_name = OBIEG_BTN;
	btn.btn_x[screen_name][btn_name] = 400; // wyb�r ustawie� dla obiegu
	btn.btn_y[screen_name][btn_name] = 400;
	btn.btn_x_w[screen_name][btn_name]= 100;
	btn.btn_y_h[screen_name][btn_name] = 35;
#endif

	btn_name = INC_TEMP_BTN;
	btn.btn_x[screen_name][btn_name] = 140; // zwiekszanie licznika temp
	btn.btn_y[screen_name][btn_name] = 30;
	btn.btn_x_w[screen_name][btn_name]= 50;
	btn.btn_y_h[screen_name][btn_name] = 50;

	btn_name = DEC_TEMP_BTN;
	btn.btn_x[screen_name][btn_name] = 140; // zmniejszanie licznika temp
	btn.btn_y[screen_name][btn_name] = 150;
	btn.btn_x_w[screen_name][btn_name]= 50;
	btn.btn_y_h[screen_name][btn_name] = 50;

	// menu ustawienia temp maks pompek (te same co zal. pompek)
	screen_name = PUMP_T_MAX_SCREEN;
	btn_name = EXIT_BTN;
	for(btn_name=0;btn_name<(DEC_TEMP_BTN+1);btn_name++){
		btn.btn_x[screen_name][btn_name] = btn.btn_x[PUMP_T_MIN_SCREEN][btn_name];
		btn.btn_y[screen_name][btn_name] = btn.btn_y[PUMP_T_MIN_SCREEN][btn_name];
		btn.btn_x_w[screen_name][btn_name]= btn.btn_x_w[PUMP_T_MIN_SCREEN][btn_name];
		btn.btn_y_h[screen_name][btn_name] = btn.btn_y_h[PUMP_T_MIN_SCREEN][btn_name];
	}

	return btn;
}

void print_screen(uint8_t scr){
	//funkcja rysuj�ca ekrany menu oraz ekran g��wny w zale�no�ci od zmiennej stanu pracy urz�dzenia

	switch(scr){
	case MAIN_SCREEN:  // dla ekranu g��wnego

		LCD_FillScreen(BLACK);
		LCD_SetTextScaled(2);
		LCD_SetCursor(160,20);
		LCD_SetTextColor(WHITE,BLACK);
		LCD_Printf("Ustawienia");
		LCD_DrawRoundRect(150,10,160,40,10,WHITE);
		LCD_SetCursor(0,60);
		LCD_SetTextColor(BLUE,BLACK);
		LCD_Printf("Zasobnik");
		LCD_SetCursor(0,90);
		LCD_SetTextColor(GREEN,BLACK);
		LCD_Printf("Solar");
		LCD_SetCursor(0,120);
		LCD_SetTextColor(RED,BLACK);
		LCD_Printf("Piec");
		LCD_SetCursor(0,150);
		LCD_SetTextColor(MAGENTA,BLACK);
#ifdef DO_DOMU
		LCD_Printf("Obieg");
#endif
#ifdef DO_DOMKU
		LCD_Printf("Domek");
#endif
		LCD_SetCursor(50,180);
		LCD_SetTextScaled(0);
		LCD_SetTextColor(CYAN,BLACK);
		LCD_Printf("Napiecie pompki");
		LCD_SetTextScaled(2);

//		LCD_SetTextScaled(0);
		LCD_SetCursor(120,210);
		LCD_SetTextColor(GREEN,BLACK);
		LCD_Printf("0.0V");
		LCD_SetCursor(0,210);
		LCD_SetTextColor(RED,BLACK);
		LCD_Printf("0.0V");
		LCD_SetCursor(230,210);
		LCD_SetTextColor(MAGENTA,BLACK);
		LCD_Printf("0.0V");
//		LCD_SetTextScaled(2);

		btn.menu_state = MAIN_SCREEN;
		temp_last = temp_reset();

		break;
	case MAIN_MENU:  // dla ekranu menu g��wnego

		// w menu s� 3 opcje: ustawienie czasu, r�nicy temperatur za��czenia pompek, r�nicy temperatur U max pompek

		LCD_FillScreen(BLACK);
		LCD_SetTextScaled(2);
		LCD_SetCursor(70,10);
		LCD_SetTextColor(WHITE,BLACK);
		LCD_Printf("zmiana czasu");
		LCD_DrawRoundRect(5,5,310,35,10,WHITE);

		LCD_SetCursor(40,50);
		LCD_Printf("temp. zal. pompek");
		LCD_DrawRoundRect(5,45,310,35,10,WHITE);

		LCD_SetCursor(40,90);
		LCD_Printf("temp. 12V pompek");
		LCD_DrawRoundRect(5,85,310,35,10,WHITE);

		exit_sign(260,180);

		break;
	case TIME_SCREEN:  // ekran ustawie� czasu

		LCD_FillScreen(BLACK);
		LCD_SetTextScaled(2);
		LCD_FillRect(0,0,320,30,CYAN);
		LCD_SetCursor(5,5);
		LCD_SetTextColor(BLACK,CYAN);
		LCD_Printf("ustaw godzine i minute");

		LCD_FillRoundRect(95,110,130,40,20,WHITE);
		LCD_SetCursor(60,10);
		LCD_SetTextColor(BLACK,WHITE);

		LCD_SetTextSize(1);
		LCD_SetTextScaled(20);
		LCD_SetCursor(127,120);
		if(t0.hour < 10)LCD_Printf("0");
		LCD_Printf("%d:",t0.hour);
		if(t0.min < 10)LCD_Printf("0");
		LCD_Printf("%d",t0.min);
		LCD_SetTextScaled(2);

		// przyciski zwi�kszania i zmniejaszania
		up_sign(105,50,WHITE,BLACK);
		up_sign(165,50,WHITE,BLACK);
		down_sign(105,160,WHITE,BLACK);
		down_sign(165,160,WHITE,BLACK);

		// przyciski zatwierdzenia i wyj�cia z menu
		exit_sign(260,180);
		ok_sign(10,180);

		break;

	// wsp�lny warunek dla ekranu ustawie� obu r�nic temp.
	case PUMP_T_MIN_SCREEN:
	case PUMP_T_MAX_SCREEN:

		LCD_FillScreen(BLACK);
		LCD_SetTextScaled(0);
		LCD_FillRect(0,0,200,20,CYAN);
		LCD_SetCursor(5,5);
		LCD_SetTextColor(BLACK,CYAN);
		LCD_Printf("ustaw temp. ");
		// rozr�nienie zale�ne od wybranego menu
		if(scr == PUMP_T_MIN_SCREEN)LCD_Printf("zal.");
		else LCD_Printf("12V");
		LCD_Printf(" pompki :");
		LCD_SetTextScaled(2);

		// domy�lnie ustawiane dostaje wskazanie dla czujnika solara
		print_diff_optins(SOLAR_DIFF,scr);

		LCD_FillRoundRect(140,90,50,50,10,WHITE);
		LCD_SetCursor(150,105);
		LCD_SetTextColor(BLACK,WHITE);

		uint8_t i;
		if(scr == PUMP_T_MIN_SCREEN){ // przepisanie warto�ci r�nic temp to tablicy lokalnej dla temp. min.
			for(i=0;i<3;i++)menu_vals[i] = diff.turn_on_val[i];
		if(diff.turn_on_val[SOLAR_DIFF] < 10){
			// "sztuczna" spacja gdy r�nica temp. < 10
			LCD_Printf(" %d",diff.turn_on_val[SOLAR_DIFF]);
		}
		else LCD_Printf("%d",diff.turn_on_val[SOLAR_DIFF]); // brak spacji je�eli jest to liczba 2-cyfrowa
		}

		else{ // przepisanie warto�ci r�nic temp to tablicy lokalnej dla temp. maks.
			for(i=0;i<3;i++)menu_vals[i] = diff.turn_12V_val[i];
			if(diff.turn_12V_val[SOLAR_DIFF] < 10){
				LCD_Printf(" %d",diff.turn_12V_val[SOLAR_DIFF]);
			}
			else LCD_Printf("%d",diff.turn_12V_val[SOLAR_DIFF]);
		}

		up_sign(140,30,WHITE,BLACK);
		down_sign(140,150,WHITE,BLACK);
		exit_sign(260,180);
		ok_sign(10,180);

		diff_in_use = SOLAR_DIFF;

		break;
	}
}

differntial_vals update_temp_screen(point ts_p,uint8_t scr){
	// funkcja aktualizuj�ca wskazania na ekranie zale�nie od stanu pracy urz�dzenia (menu r�nic temp)


	uint8_t i,btn_name;
	btn_name = NO_TOUCH;
	btn.menu_state = scr;

	// p�tla sprawdzaj�ca wszystkie przyciski zale�nie od przekazacych wsp�rz�dnych x,y
	// gdy pkt x,y znajduje si� wewn�trz kt�regokolwiek przycisku zmienna btn_name przypisane b�dzie mia�a makro tego przycisku
	for(i=2;i<7;i++){
		if(ts_p.x > btn.btn_x[btn.menu_state][i] &&
				ts_p.x < (btn.btn_x[btn.menu_state][i] + btn.btn_x_w[btn.menu_state][i]) &&
				ts_p.y > btn.btn_y[btn.menu_state][i] &&
				ts_p.y < (btn.btn_y[btn.menu_state][i] + btn.btn_y_h[btn.menu_state][i])){
			btn_name = i;
		}
	}

	// je�eli jakikolwiek przycisk zosta� dotkni�ty
	if(btn_name != NO_TOUCH){
		switch(btn_name){

		case SOLAR_BTN:	// przycisk zmiany ustawie� na zmienn� r�nicy temp. solara
			diff_in_use = SOLAR_DIFF;
			print_diff_optins(diff_in_use,scr);
			break;

		case PIEC_BTN:  // przycisk zmiany ustawie� na zmienn� r�nicy temp. pieca
			diff_in_use = PIEC_DIFF;
			print_diff_optins(diff_in_use,scr);
			break;
#ifdef DO_DOMU  // tylko w wersji do domu
		case OBIEG_BTN:  // przycisk zmiany ustawie� na zmienn� r�nicy temp. obiegu
			if(scr == PUMP_T_MIN_SCREEN){
			diff_in_use = OBIEG_DIFF;
			print_diff_optins(diff_in_use,scr);
			}
			break;
#endif

		case INC_TEMP_BTN:  // przycisk zwi�kszania licznika temp
			switch(diff_in_use){  // zale�nie od aktualnie wybranej zmiennej
			case OBIEG:  // dla obiegu
				if(menu_vals[diff_in_use] < -1)menu_vals[diff_in_use]++; // zwi�kszanie licznika gdy jest mniejszy ni� -1
				break;
			default:  // dla pozosta�ych przypadk�w gdy r�nica temp jest dodatnia
				if(scr == PUMP_T_MIN_SCREEN){
					if(menu_vals[diff_in_use] < 20 && menu_vals[diff_in_use] < (diff.turn_12V_val[diff_in_use] - 1))
						menu_vals[diff_in_use]++;  // zwi�kszanie licznika temp. gdy jest ona mniejsza ni� 20 i mniejsza ni� temp maks.
				}else{
					if(menu_vals[diff_in_use] < 20)menu_vals[diff_in_use]++;
				}
				break;
			}
			break;

			case DEC_TEMP_BTN:  // przycisk zmniejszania licznkia
				switch(diff_in_use){
				case OBIEG: // zmniejszanie licznika obiegu gdy jest wi�ksza ni� -20
					if(menu_vals[diff_in_use] > -20)menu_vals[diff_in_use]--;
					break;
				default: // dla pozosta�ych przypadk�w gdy r�nica temp jest dodatnia
					if(scr == PUMP_T_MAX_SCREEN){
					if(menu_vals[diff_in_use] > 1 && menu_vals[diff_in_use] > (diff.turn_on_val[diff_in_use] + 1))
						menu_vals[diff_in_use]--; // zwi�kszanie licznika temp. gdy jest ona wi�ksza ni� 1 i wi�ksza ni� temp min.
					}else{
						if(menu_vals[diff_in_use] > 1)menu_vals[diff_in_use]--;
					}
					break;
				}
				break;
		}

		if(btn_name == INC_TEMP_BTN || DEC_TEMP_BTN){
			// nadpisywanie warto�ci licznika na wy�wietlaczu

			LCD_SetCursor(150,105);
			LCD_SetTextColor(BLACK,WHITE);
			if(menu_vals[diff_in_use] < 10 && menu_vals[diff_in_use] > -10){
				if(diff_in_use != OBIEG_DIFF)LCD_Printf(" %d",menu_vals[diff_in_use]);
				else LCD_Printf(" %d",abs(menu_vals[diff_in_use]));
			}
			else{
				if(diff_in_use != OBIEG_DIFF)LCD_Printf("%d",menu_vals[diff_in_use]);
				else LCD_Printf("%d",abs(menu_vals[diff_in_use]));
			}
			HAL_Delay(100);
		}

		// nadpisywanie struktury lokalnej zmiennymi aktualnie modyfikowanej r�nicy temp.
		uint8_t i;
		if(scr == PUMP_T_MIN_SCREEN){
			for(i=0;i<3;i++)diff.turn_on_val[i] = menu_vals[i];
		}
		else{
			for(i=0;i<3;i++)diff.turn_12V_val[i] = menu_vals[i];
		}

	}

	// zwracanie struktury po przyj�ciu wszystkich warunk�w
	return diff;
}

time update_time_screen(point ts_p){
//	funkcja aktualizuj�ca ekran menu ustawie� godziny

	uint8_t i,btn_name;
	btn_name = NO_TOUCH;
	btn.menu_state = TIME_SCREEN;


	// p�tla sprawdzaj�ca wszystkie przyciski zale�nie od przekazacych wsp�rz�dnych x,y
	// gdy pkt x,y znajduje si� wewn�trz kt�regokolwiek przycisku zmienna btn_name przypisane b�dzie mia�a makro tego przycisku
	for(i=2;i<6;i++){
		if(ts_p.x > btn.btn_x[btn.menu_state][i] &&
				ts_p.x < (btn.btn_x[btn.menu_state][i] + btn.btn_x_w[btn.menu_state][i]) &&
				ts_p.y > btn.btn_y[btn.menu_state][i] &&
				ts_p.y < (btn.btn_y[btn.menu_state][i] + btn.btn_y_h[btn.menu_state][i])){
			btn_name = i;
		}
	}

	// je�eli jakikolwiek przycisk zosta� dotkni�ty
	if(btn_name != NO_TOUCH){
		switch(btn_name){
		case INC_HOUR_BTN:  // przycisk zwi�kszania licznika godziny
			t0.hour++;
			if(t0.hour == 24)t0.hour = 0;
			break;
		case DEC_HOUR_BTN:  // przycisk zmniejszania licznika godziny
			if(t0.hour == 0)t0.hour = 24;
			t0.hour--;
			break;
		case INC_MIN_BTN:  // przycisk zwi�kszania licznika minuty
			t0.min++;
			if(t0.min == 60)t0.min = 0;
			break;
		case DEC_MIN_BTN:  // przycisk zmniejszania licznika minuty
			if(t0.min == 0)t0.min = 60;
			t0.min--;
			break;
		}

		// aktualizacja licznik�w na wy�wietlaczu
		LCD_SetTextColor(BLACK,WHITE);
		LCD_SetCursor(127,120);
		if(t0.hour < 10)LCD_Printf("0");
		LCD_Printf("%d:",t0.hour);
		if(t0.min < 10)LCD_Printf("0");
		LCD_Printf("%d",t0.min);
		HAL_Delay(100);
	}
	return t0;
}

void update_main_screen(temperature temp,time tim ,temperature_differential_val diff_in){
	// funkcja aktualizuj�ca wskazania na ekranie g��wnym

	// wy�wietlanie czasu
	LCD_SetCursor(10,20);
	LCD_SetTextColor(WHITE,BLACK);
	LCD_Printf("%d:",tim.hour);
	if(tim.min < 10)LCD_Printf("0");
	LCD_Printf("%d:",tim.min);
	if(tim.sec < 10)LCD_Printf("0");
	LCD_Printf("%d",tim.sec);

	// wy�wietlanie temperatury w zasobniku
	if(temp.zasobnik_t.temp != temp_last.zasobnik_t.temp){
		LCD_FillRect(120,60,100,30,BLACK);
		LCD_SetCursor(120,60);
		LCD_SetTextColor(BLUE,BLACK);
		if(temp.zasobnik_t.status == STATUS_OK){
			LCD_Printf("%3.1f$C", temp.zasobnik_t.temp);  //$ zmodyfikowany znak stopnia
		}else LCD_Printf("error");
	}
	// wy�wietlanie temperatury w solarze
	if(temp.solar_t.temp != temp_last.solar_t.temp){
		LCD_FillRect(120,90,150,30,BLACK);
		LCD_SetCursor(120,90);
		LCD_SetTextColor(GREEN,BLACK);
		if(temp.solar_t.status == STATUS_OK){
			LCD_Printf("%3.1f$C", temp.solar_t.temp);
		}else LCD_Printf("error");
	}
	// wy�wietlanie temperatury w piecu
	if(temp.piec_t.temp != temp_last.piec_t.temp){
		LCD_FillRect(120,120,150,30,BLACK);
		LCD_SetCursor(120,120);
		LCD_SetTextColor(RED,BLACK);
		if(temp.piec_t.status == STATUS_OK){
			LCD_Printf("%3.1f$C", temp.piec_t.temp);
		}else LCD_Printf("error");
	}
	// wy�wietlanie temperatury obiegu
	if(temp.obieg_t.temp != temp_last.obieg_t.temp){
		LCD_FillRect(120,150,150,30,BLACK);
		LCD_SetCursor(120,150);
		LCD_SetTextColor(MAGENTA,BLACK);
		if(temp.obieg_t.status == STATUS_OK){
			LCD_Printf("%2.0f$C", temp.obieg_t.temp);
		}else LCD_Printf("error");
	}


	// // wy�wietlanie napi�cia pompki solara
	if(diff_in.voltage_val[SOLAR_DIFF] != temp_last.diff.voltage_val[SOLAR_DIFF]){
		LCD_SetTextScaled(2);
		LCD_FillRect(120,210,100,20,BLACK);
		LCD_SetTextColor(GREEN,BLACK);
		LCD_SetCursor(120,210);
		LCD_Printf("%2.2fV",diff_in.voltage_val[SOLAR_DIFF]); // dok�adno�c do 2 miejsc po przecinku
//		LCD_SetTextScaled(2);
	}
	// wy�wietlanie napi�cia pompki pieca
	if(diff_in.voltage_val[PIEC_DIFF] != temp_last.diff.voltage_val[PIEC_DIFF]){
		LCD_SetTextScaled(2);
		LCD_FillRect(0,210,100,20,BLACK);
		LCD_SetTextColor(RED,BLACK);
		LCD_SetCursor(0,210);
		LCD_Printf("%2.2fV",diff_in.voltage_val[PIEC_DIFF]);
//		LCD_SetTextScaled(2);
	}
	// wy�wietlanie napi�cia pompki obiegu
	if(diff_in.voltage_val[OBIEG_DIFF] != temp_last.diff.voltage_val[OBIEG_DIFF]){
		LCD_SetTextScaled(2);
		LCD_FillRect(230,210,100,20,BLACK);
		LCD_SetTextColor(MAGENTA,BLACK);
		LCD_SetCursor(230,210);
		LCD_Printf("%2.2fV",diff_in.voltage_val[OBIEG_DIFF]);
//		LCD_SetTextScaled(2);
	}


//	LCD_SetCursor(0,210);
//			LCD_SetTextColor(RED,BLACK);
//			LCD_Printf("solar 12.0V");
//			LCD_SetCursor(120,210);
//			LCD_SetTextColor(GREEN,BLACK);
//			LCD_Printf("piec 12.0V");
//			LCD_SetCursor(240,210);
//			LCD_SetTextColor(MAGENTA,BLACK);
//			LCD_Printf("obieg 12.0V");

	// nadpisanie struktur temperatury i napi�c z poprzedniego cyklu
	temp_last = temp;
	temp_last.diff = diff_in;
}

temperature temp_reset(){
	// funkcja resetuj�ca wskazania temp.
	temperature t0;
	t0.zasobnik_t.temp = 0xFF;
	t0.solar_t.temp = 0xFF;
	t0.piec_t.temp = 0xFF;
	t0.obieg_t.temp = 0xFF;
	return t0;
}

void exit_sign(uint16_t x0, uint16_t y0){
	// funkcja wy�wieltaj�ca przycisk wyj�cia z menu
	LCD_SetTextSize(3);
	LCD_FillRoundRect(x0,y0,50,50,10,RED);
	LCD_SetCursor(x0+10,y0+10);
	LCD_SetTextColor(WHITE,RED);
	LCD_Printf("<");
}
void ok_sign(uint16_t x0, uint16_t y0){
	// funkcja wy�wieltaj�ca przycisk zatwierdzenia
	LCD_SetTextSize(3);
	LCD_FillRoundRect(x0,y0,50,50,10,GREEN);
	LCD_SetCursor(x0+10,y0+15);
	LCD_SetTextColor(WHITE,GREEN);
	LCD_Printf("ok");
}
void up_sign(uint16_t x0, uint16_t y0,uint16_t bg_colour ,uint16_t colour){
	// funkcja wy�wieltaj�ca przycisk zwi�kszania licznika (strza�ka w g�r�)
	uint8_t w = 50;
	LCD_FillRoundRect(x0,y0,50,50,10,bg_colour);
//	LCD_FillRoundRect(55,150,50,50,10,WHITE);
//	LCD_FillRoundRect(125,10,50,50,10,WHITE);
//	LCD_FillRoundRect(125,150,50,50,10,WHITE);

	uint8_t i;
	for(i=0;i<3;i++){
		LCD_DrawLine(x0+10 + i, y0+w-10 + i, x0+23 + i, y0+10 + i,colour);
		LCD_DrawLine(x0+10 + 1 + i, y0+w-10 + i, x0+23 + i, y0+10 + 1 + i,colour);

		LCD_DrawLine(x0+w-10 - i ,y0+w-10 + i ,x0+25 - i ,y0+10 + i, colour);
		LCD_DrawLine(x0+w-10 - 1 - i ,y0+w-10 + i ,x0+25 - 1 - i ,y0+10 + i, colour);
	}
//	tf
}
void down_sign(uint16_t x0, uint16_t y0,uint16_t bg_colour ,uint16_t colour){
	// funkcja wy�wieltaj�ca przycisk zmniejszania licznika (strza�ka w d�)
	uint8_t w = 50;
	LCD_FillRoundRect(x0,y0,50,50,10,bg_colour);
	uint8_t i;
	for(i=0;i<3;i++){
		LCD_DrawLine(x0+10 +i, y0+10 -i, x0+23+i, y0+w-10-i,colour);
		LCD_DrawLine(x0+10 +i, y0+10+1 -i, x0+23+i, y0+w-10+1-i,colour);
		//
		LCD_DrawLine(x0+w-10 - i ,y0+10 - i ,x0+25-i ,y0+w-10-i , colour);
		LCD_DrawLine(x0+w-10-1 - i ,y0+10 - i ,x0+25-1-i ,y0+w-10-i , colour);
	}
}

void setting_sign(int16_t x0, int16_t y0,uint16_t colour){
	// funkcja wy�wieltaj�ca ikonk� ustawie�
	int16_t w = 32;
	LCD_FillCircle(x0+16, y0+16, 10, colour);
	LCD_FillRect(x0+13, y0, 6, w, colour);
	LCD_FillRect(x0, y0+13, w, 6, colour);
	LCD_DrawRect(x0-5,y0-5,w+10,w+10,colour);

	uint8_t i;

	for (i = 0; i < 4; i++) {
		LCD_DrawLine(x0 + 2 + i, (y0+w) - 2 - 4 + i, (x0+w) - 2 - 4 + i, y0 + 2 + i, colour);

		LCD_DrawLine(x0 + 2 + 1 + i, (y0+w) - 2 - 4 + i, (x0+w) - 2 - 4 + i, y0 + 2 + 1 + i, colour);

		LCD_DrawLine(x0 + 2 + i, y0 + 2 + 4 - i, (x0+w) - 2 - 4 + i, (y0+w) - 2 - i, colour);

		LCD_DrawLine(x0 + 2 + i, y0 + 2 + 4 - 1 - i, (x0+w) - 2 - 4 - 1 + i, (y0+w) - 2 - i, colour);
	}
}
