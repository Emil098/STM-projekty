/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
//#include "printf/printf.h"
#include "D:\stm projekty\Sterownik_temp\libs\lcd.h"
#include "D:\stm projekty\Sterownik_temp\TrueSTUDIO\Sterownik_temp\touch.h"
#include "D:\stm projekty\Sterownik_temp\TrueSTUDIO\Sterownik_temp\MAX31865_v1.h"
#include "D:\stm projekty\Sterownik_temp\TrueSTUDIO\Sterownik_temp\TC74A0.h"
#include "D:\stm projekty\Sterownik_temp\TrueSTUDIO\Sterownik_temp\disp_irq.h"
#include "D:\stm projekty\Sterownik_temp\TrueSTUDIO\Sterownik_temp\Flash.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
//typedef struct{
//	float temp_diff[3];
//	uint8_t hysteresis[3];
//	uint16_t pwm_val[3];
//	float voltage_val[3];
//}temperature_differential_val;	// przechowuje dane o ró¿nicy tepm, wskazaniach napiêcia i PWM
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define PWM_MAX 500


#define PIEC_PWM_MIN 375	// PWM 1 piec 8,5V
#define PIEC_PWM_MAX 500	// 12 V
#define PIEC_U_MIN 8.5
#define PIEC_U_MAX 12

#define SOLAR_PWM_MIN 237 	// PWM 2 solar 6,5V
#define SOLAR_PWM_MAX 500	// 12 V
#define SOLAR_U_MIN 6.5
#define SOLAR_U_MAX 12

#define OBIEG_PWM_MIN 252 	// PWM 3 cyrkulacja 6V
#define OBIEG_PWM_MAX 211	// 5 V
#define OBIEG_U_MIN 6
#define OBIEG_U_MAX 5
#define OBIEG_TIME 3

//#define hysteresis 0.5

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c2;

RTC_HandleTypeDef hrtc;

SPI_HandleTypeDef hspi2;

TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim6;
TIM_HandleTypeDef htim7;

UART_HandleTypeDef huart3;

/* USER CODE BEGIN PV */
temperature temp = {0}; // wskazania temperatury ze wszystkch czujników

time t_last;  // przechowuje wskazania czasu z poprzedniego cyklu procesora je¿eli zostanie wykryta zmiana nadpisana zostanie tylko wskazanie które siê zmieni³o

//differntial_vals diff;

btn_location cr_screen;  // struktura przechowuj¹ca po³o¿enie wszystkich przycisków oraz wyœwietlanego menu

point ts_p;  // wskazania pkt x,y w którym wykryto dotkniêcie matrycy wyœwietlacza

time t,t_in_timer; // przechowuj¹ wskazania czasu dla g³ównej pêtli programu oraz wewn¹trz timera (przypisanie wartoœci przez urzytkownika w menu)

differntial_vals diff_vals, diff_menu; // wartoœci ró¿nic temperatur pomiêdzy zasobnikiem a innymi czujnikami s³u¿¹ jako odniesienie do wyznaczenia pwm-a poszczególnych pompek

temperature_differential_val diff_otp; // przechowuje dok³adne dane o ró¿nicy tepm, wskazaniach napiêcia i PWM

uint8_t obieg_cnt; // pompka obiegu pracuje w dwóch stanach (rozruch 5V i normalna praca 6V) po okreœlonym czasie nastêpuje prze³¹czenie z jednego na drugi do tego s³u¿y ten licznik

//int8_t temp_diff[3];

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C2_Init(void);
static void MX_SPI2_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM3_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_RTC_Init(void);
static void MX_TIM6_Init(void);
static void MX_TIM7_Init(void);
/* USER CODE BEGIN PFP */

void set_time(time t_init){
	// funkcja zapisuj¹ca godzinê do rtc wewn¹trz STM-a

	//struktury przechowujace date i czas
	RTC_TimeTypeDef sTime = {0};
	RTC_DateTypeDef sDate = {0};

	sTime.Hours = t_init.hour;
	sTime.Minutes = t_init.min;
	sTime.Seconds = 0;
	sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	sTime.StoreOperation = RTC_STOREOPERATION_RESET;
	if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK)
	{
		Error_Handler();
	}

	// przypisanie jakich kolwiek wartoœci do rejestrów daty
	// aby nadpisac RTC potrzebne jest jednoczesne zapisanie i godziny i daty
	// w programie wykorzystane bêd¹ jedynie wskazania godziny
	sDate.WeekDay = RTC_WEEKDAY_MONDAY;
	sDate.Month = RTC_MONTH_JANUARY;
	sDate.Date = 0x1;
	sDate.Year = 0x20;

	if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN) != HAL_OK)
	{
		Error_Handler();
	}
//	HAL_RTCEx_BKUPWrite(&hrtc,PTC_B)
}
time read_time(){
	//funkcja odczytuj¹ca czas z rtc STM-a

	time t= {0};  // pusta struktura czasu

	RTC_TimeTypeDef gTime = {0};
	RTC_DateTypeDef gDate = {0};

	//odczyt daty i godziny
	HAL_RTC_GetTime(&hrtc, &gTime,RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &gDate,RTC_FORMAT_BIN);


	// nadpisanie struktury póŸniej u¿ywanej w programie
	t.year = gDate.Year;
	t.mon = gDate.Month;
	t.day = gDate.Date;
	t.hour = gTime.Hours;
	t.min = gTime.Minutes;
	t.sec = gTime.Seconds;

	return t;
}

void set_pwm_otp(uint16_t val,uint8_t otp_nr){
	// funkcja za³¹czaj¹ca pwm na odpowiednim wyjœciu dla pompki (pieca, solara, obiegu)
	// val (pwm) musi byc wiêksze od 100 je¿eli pompka jest za³¹czona
	if(val > 100){
		switch(otp_nr){
		case PIEC_DIFF:
			HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_4);
			TIM2->CCR4 = val;
			break;
		case SOLAR_DIFF:
			HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_1);
			TIM3->CCR1 = val;
			break;
		case OBIEG_DIFF:
			HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_2);
			TIM3->CCR2 = val;
			break;
		}
	}else{
		// je¿eli pompka jest wy³¹czona wtedy timer jest zatrzymywany i ostawiane jest "0" na gpio
		switch(otp_nr){
		case PIEC_DIFF:
			TIM2->CCR4 = 0;
//			HAL_TIM_PWM_Stop(&htim2,TIM_CHANNEL_4);
//			HAL_TIM_
			HAL_GPIO_WritePin(PWM_1_GPIO_Port,PWM_1_Pin,0);
			break;
		case SOLAR_DIFF:
			HAL_TIM_PWM_Stop(&htim3,TIM_CHANNEL_1);
			HAL_GPIO_WritePin(PWM_2_GPIO_Port,PWM_2_Pin,0);
			break;
		case OBIEG_DIFF:
			HAL_TIM_PWM_Stop(&htim3,TIM_CHANNEL_2);
			HAL_GPIO_WritePin(PWM_3_GPIO_Port,PWM_3_Pin,0);
			break;
		}
	}
//	TIM2->CCR4 = 1430;	// PWM 1 piec 8,5V
//	  TIM3->CCR1 = 1093;	// PWM 2 solar 6,5V
//	  TIM3->CCR2 = 1010;	// PWM 3 cyrkulacja 6V
}
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
	uint8_t i,btn_name;
	if(htim->Instance == TIM7){
		// przerwanie timera 7 s³u¿y do podejmowania reakcji na klikanie przycisków przechodz¹cych miêdzy ekranami menu
//		point ts_p = get_pos();
		if(ts_p.p != NO_TOUCH){  // wejœcie do funkcji tylko gdy wykryte zostanie dotkniêcie matrycy

			switch(cr_screen.menu_state){  // sprawdzenie aktualnie wyœwietlanego ekranu

			case MAIN_SCREEN:  // ekran g³ówny
				// warunek po³o¿enia przycisku wejœcia do menu g³ównego
				if(ts_p.x > cr_screen.btn_x[MAIN_SCREEN][0] &&
						ts_p.x < (cr_screen.btn_x[cr_screen.menu_state][0] + cr_screen.btn_x_w[cr_screen.menu_state][0]) &&
						ts_p.y > cr_screen.btn_y[cr_screen.menu_state][0] &&
						ts_p.y < (cr_screen.btn_y[cr_screen.menu_state][0] + cr_screen.btn_y_h[cr_screen.menu_state][0])){
//					LCD_FillScreen(BLACK);
//					print_main_menu_screen();
					cr_screen.menu_state = MAIN_MENU;	// zmiana zmiennej stanu ekranu na menu g³ówne
					print_screen(cr_screen.menu_state);	// wyœwietlenie ekranu menu g³ównego
					// wyzerowanie zmiennych pkt x,y aby w nastêpnym cyklu nie by³y one rozpatrywane
					ts_p.x = 0xFF;
					ts_p.y = 0xFF;
//					 return;

				}
				break;

			case MAIN_MENU:  // menu g³ówne
//				uint8_t i,btn_name;
				btn_name = NO_TOUCH; // wyzerowanie zmiennej do której nast¹pi ewentualne przypisanie makra wciœniêtego przycisku

				// warunek który sprawdza wspó³rzêdne wciœniêtego pkt je¿eli jest on wewn¹trz którego kolwiek przycisku wtedy do btn_name trafi makro danego przycisku
				for(i=0;i<4;i++){
					if(ts_p.x > cr_screen.btn_x[cr_screen.menu_state][i] &&
							ts_p.x < (cr_screen.btn_x[cr_screen.menu_state][i] + cr_screen.btn_x_w[cr_screen.menu_state][i]) &&
							ts_p.y > cr_screen.btn_y[cr_screen.menu_state][i] &&
							ts_p.y < (cr_screen.btn_y[cr_screen.menu_state][i] + cr_screen.btn_y_h[cr_screen.menu_state][i])){
						btn_name = i;
					}
				}

				if(btn_name != NO_TOUCH){ // sprawdzenie czy zosta³ wciœniêty jakikolwiek przycisk
					switch(btn_name){  // sprawdzenie makra przycisku
					case EXIT_BTN: // wyjœcie do ekranu g³ównego
						cr_screen.menu_state = MAIN_SCREEN;
//						print_screen(cr_screen.menu_state);
						update_main_screen(temp,t,diff_otp);
						break;

					case SET_TIME_BTN: // menu zmiany godziny i minuty
						get_time(t); // aktualizacja struktury czasu wewn¹trz biblioteki
						cr_screen.menu_state = TIME_SCREEN;
//						print_screen(cr_screen.menu_state);
						break;

					case SET_PUMP_T_MIN_BTN: // menu zmiany ró¿nic temperatur miêdzy czujnikem a zasobnikiem przy których za³¹czana jest dana pompka
						get_diff_struct(diff_vals);  // przekazanie aktualej strytury z ró¿nicami temp do biblioteki
						cr_screen.menu_state = PUMP_T_MIN_SCREEN;
						break;

					case SET_PUMP_T_MAX_BTN: // menu zmiany ró¿nic temperatur miêdzy czujnikem a zasobnikiem przy których pompki maj¹ osi¹gn¹c max wartoœc napiêcia
						get_diff_struct(diff_vals);
						cr_screen.menu_state = PUMP_T_MAX_SCREEN;
						break;

					}
					// zale¿nie od wybranego menu nastêpuje wyczyszczenie wyœwietlacza i wyœwietlenie ekranu danego menu
					print_screen(cr_screen.menu_state);
				}
				break;

			case TIME_SCREEN:
			case PUMP_T_MIN_SCREEN:
			case PUMP_T_MAX_SCREEN:
				// wspólny zbiór warunków dla menu czasu,za³. pompek i 100% nap. pompek
//				t_in_timer = update_time_screen(ts_p);
				btn_name = NO_TOUCH;

				// sprawdzane s¹ przyciski: zatwierdzenia "ok" i wyjœcia z menu "<"
				for(i=0;i<2;i++){
					if(ts_p.x > cr_screen.btn_x[cr_screen.menu_state][i] &&
							ts_p.x < (cr_screen.btn_x[cr_screen.menu_state][i] + cr_screen.btn_x_w[cr_screen.menu_state][i]) &&
							ts_p.y > cr_screen.btn_y[cr_screen.menu_state][i] &&
							ts_p.y < (cr_screen.btn_y[cr_screen.menu_state][i] + cr_screen.btn_y_h[cr_screen.menu_state][i])){
						btn_name = i;
					}
				}

				ts_p.p = NO_TOUCH;
				if(btn_name != NO_TOUCH){
					switch(btn_name){
					case EXIT_BTN: // wyjœcie do ekranu menu g³ównego
						cr_screen.menu_state = MAIN_MENU;
						print_screen(cr_screen.menu_state); // wyœwietlane zostaje menu g³ówne
						break;

					case OK_BTN: // przycisk zatwierdzenia ustawieñ
						if(cr_screen.menu_state == TIME_SCREEN)set_time(t_in_timer);
						// je¿eli zatwierdzone zostaje menu czasu nastêpuje nadpisanie rtc STM-a
						else {
							// je¿eli zatwierdzone zostaje menu zmiany ró¿nic temperatur
							// struktura zmieniana w menu zostaje przepisana do struktury programu g³ównego
							// zmodyfikowana strutura zostaje zapisana do pamiêci flash aby przy zaniku zasilania dane nie zosta³y utracone
							diff_vals = diff_menu;
							Flash_save_diff(diff_vals);
						}
						// zmienna stanu menu zostaje zmieniona na menu g³ówne
						// wyœwietlone zostaje menu g³ówne
						cr_screen.menu_state = MAIN_MENU;
						print_screen(cr_screen.menu_state);
						break;
					}
				}
				break;

			}
		}
	}
	if(htim->Instance == TIM6){
		// przerwanie timera 6 zarz¹dza wyliczaniem ró¿nicy temperatur miêdzy zasobnikiem a czujnikami dodatkowo wyliczane s¹ wype³nienia dla poszczególnych pwm-ów oraz napiêcia jakie powinny znajdowac siê na wyjœciach pompek

		// wyliczanie ró¿nic temperatur
		diff_otp.temp_diff[PIEC_DIFF] = temp.piec_t.temp - temp.zasobnik_t.temp;
		diff_otp.temp_diff[SOLAR_DIFF] = temp.solar_t.temp - temp.zasobnik_t.temp;
		diff_otp.temp_diff[OBIEG_DIFF] = temp.obieg_t.temp - temp.zasobnik_t.temp;

		uint8_t current_val,read_ok=0;

		// pêtla w której wyliczane zostaj¹ wartoœci pwm i napiêcia
		for(current_val=0;current_val<2;current_val++){
			uint16_t pwm_min,pwm_max;
			float u_min,u_max;

			switch(current_val){

			// przypisanie wartoœci (maks,min) pwm-a i napiêcia dla wskazañ solara
			case SOLAR_DIFF:
				if(temp.solar_t.status == STATUS_OK)read_ok = 1; // sprawdzenie poprawnoœci odczytu
				pwm_min = SOLAR_PWM_MIN;
				pwm_max = SOLAR_PWM_MAX;
				u_min = SOLAR_U_MIN;
				u_max = SOLAR_U_MAX;
				break;

			// przypisanie wartoœci (maks,min) pwm-a i napiêcia dla wskazañ pieca
			case PIEC_DIFF:
				if(temp.piec_t.status == STATUS_OK)read_ok = 1;  // sprawdzenie poprawnoœci odczytu
				pwm_min = PIEC_PWM_MIN;
				pwm_max = PIEC_PWM_MAX;
				u_min = PIEC_U_MIN;
				u_max = PIEC_U_MAX;
				break;
			}

			// je¿eli odczyt jest poprawny
			if(read_ok){

				// je¿eli temp znajduje siê pomiêdzy wartoœci¹ za³. a wart maks.
				if(diff_otp.temp_diff[current_val] >= diff_vals.turn_on_val[current_val] && diff_otp.temp_diff[current_val] <= diff_vals.turn_12V_val[current_val]){
					// wyliczany jest dany pwm (solara lub pieca) na podstawie funkcji mapuj¹cej
					diff_otp.pwm_val[current_val] = map_float(diff_otp.temp_diff[current_val],diff_vals.turn_on_val[current_val],diff_vals.turn_12V_val[current_val],pwm_min,pwm_max);
					// wyliczane jest dane napiêcie (solara lub pieca)
					diff_otp.voltage_val[current_val] = map_float(diff_otp.temp_diff[current_val],diff_vals.turn_on_val[current_val],diff_vals.turn_12V_val[current_val],u_min,u_max);
//					diff_otp.hysteresis[current_val] = 1; opcjonalna histereza
					// faktyczne ustawienie pwm-a danej pompki na podstawie uprzednio wyliczonej wartoœci
					set_pwm_otp(diff_otp.pwm_val[current_val],current_val);
				}

				// je¿eli ró¿nica temperatur jest mniejsz ni¿ ró¿nica za³.
				if(diff_otp.temp_diff[current_val] < (diff_vals.turn_on_val[current_val])){
//					diff_otp.hysteresis[current_val] = 0;
					// napiêcie pompki jest równe 0V
					diff_otp.voltage_val[current_val] = 0;
					// wy³¹czenie danej pompki przez wpisanie wartoœci pwm jako 0
					set_pwm_otp(0,current_val);
				}

				// je¿eli ró¿nica temperatur jest wiêksza ni¿ ró¿nica maksymalna
				if(diff_otp.temp_diff[current_val] > diff_vals.turn_12V_val[current_val]){
					// ustawione zostaje maksymalne wype³nienie dla pwm-a
					set_pwm_otp(PWM_MAX,current_val);
					// wskazanie napiêcei zostaje ustawione na 12V
					diff_otp.voltage_val[current_val] = 12;
				}
			}
		}

		// wylczanie pwm-a i napiêcia dla pompki od obiegu
		current_val = OBIEG_DIFF;
		// je¿eli odczyt jest poprawny
		if(temp.obieg_t.status == STATUS_OK){

			// warunek dla rozruch pompki
			if(diff_otp.temp_diff[current_val] < diff_vals.turn_on_val[current_val] && obieg_cnt < OBIEG_TIME){
				// ustawienie napiêcia 6V oraz odpowiedniego wype³nienia pwm-a
				set_pwm_otp(OBIEG_PWM_MIN,current_val);
				diff_otp.voltage_val[current_val] = OBIEG_U_MIN;
				// zmienna histerezy która przyjmuje wart 1 w czasie rozruchu
				diff_otp.hysteresis[current_val] = 1;
			}

			// warunek pracy ci¹g³ej pompki
			if(diff_otp.temp_diff[current_val] < diff_vals.turn_on_val[current_val] && obieg_cnt == OBIEG_TIME){
				// ustawienie wart. maks. pwm-a pompki obiegu
				set_pwm_otp(OBIEG_PWM_MAX,current_val);
				diff_otp.voltage_val[current_val] = OBIEG_U_MAX;
			}

			// warunek dla ró¿nicy temp. mniejszej ni¿ minimalna z uwzglêdnieniem histerezy + 0,25 stopnie
			if(diff_otp.temp_diff[current_val] > (diff_vals.turn_on_val[current_val] + 0.25) && diff_otp.hysteresis[current_val] == 1){
				// wy³¹czenie pwm-a pompki obiegu
				// ustawienie nap. pompki = 0V
				// wyzerowanie licznika rozruchu pompki
				set_pwm_otp(0,current_val);
				diff_otp.voltage_val[current_val] = 0;
				obieg_cnt = 0;
			}
		}
		if(t.hour > 6 && t.hour < 22){ // podœwietlenie wyœwietlacza nie dzia³a w nocy
			HAL_GPIO_WritePin(LCD_LED_GPIO_Port,LCD_LED_Pin,1);
		}else HAL_GPIO_WritePin(LCD_LED_GPIO_Port,LCD_LED_Pin,0);
	}
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C2_Init();
  MX_SPI2_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_USART3_UART_Init();
  MX_RTC_Init();
  MX_TIM6_Init();
  MX_TIM7_Init();
  /* USER CODE BEGIN 2 */
//  HAL_TIM_Base_Start_IT(&htim7); // sprawdza przyciski
  HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_4);	// PWM 1 piec
  HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_1);  // PWM 2 solar
  HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_2);	// PWM 3 obieg
  HAL_GPIO_WritePin(LCD_LED_GPIO_Port,LCD_LED_Pin,1);  //podswietlanie

//  TIM2->CCR4 = 1430;	// PWM 1 piec 8,5V
//  TIM3->CCR1 = 1093;	// PWM 2 solar 6,5V
//  TIM3->CCR2 = 1010;	// PWM 3 cyrkulacja 6V
  // ustawienie pwm-ow pompek na 0
  set_pwm_otp(0,PIEC_DIFF);
  set_pwm_otp(0,SOLAR_DIFF);
  set_pwm_otp(0,OBIEG_DIFF);

  // inicjacja wyœwietlacza
  LCD_Init();
  LCD_FillScreen(BLACK);
  LCD_SetTextColor(WHITE,BLACK);
  LCD_SetTextScaled(0);
  LCD_SetRotation(1);
  touch_init();
  LCD_FillScreen(BLACK);

  // zainicjowanie czyjnika temp. dla obiegu (i2c)
  TC74A0_init(&hi2c2);

  // zainicjowanie pozosta³ych czujników pompek rezystancyjnych (PT100)
  max31865_gpio_init(&hspi2);
  max31865_init(ZASOBNIK);
  max31865_init(SOLAR);
  max31865_init(PIEC);

  // pierwszy odczyt temperatury ze wszystkich czujnikow
  temp.obieg_t = TC74A0_read_temp();
  temp.zasobnik_t = max31865_getTemp(ZASOBNIK);
  temp.solar_t = max31865_getTemp(SOLAR);
  temp.piec_t = max31865_getTemp(PIEC);

  // odczyt ró¿nic temperatur z pamiêci flash uC
  diff_vals = Flash_read_diff();

  // inicjacja struktury przechowuj¹cej dane o stanie pracy urz¹dzenia oraz o po³o¿eniach przycisków
  cr_screen = btn_init();
  // wyœwietlenie ekranu g³ównego
  print_screen(cr_screen.menu_state);


  HAL_TIM_Base_Start_IT(&htim6);	// wylicza ró¿nicê temp, liczy pwm i napiecia
  HAL_TIM_Base_Start_IT(&htim7);	// sprawdza przyciski


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
//	  HAL_GPIO_WritePin(LCD_LED_GPIO_Port,LCD_LED_Pin,0);

	  // cykliczny odczyt czasu, temp. wszystkich czujników, stanu wyœwietlacza dotykowego
	  t = read_time();
	  temp.obieg_t = TC74A0_read_temp();
	  temp.zasobnik_t = max31865_getTemp(ZASOBNIK);
	  temp.solar_t = max31865_getTemp(SOLAR);
	  temp.piec_t = max31865_getTemp(PIEC);
	  ts_p = get_pos();

	  // sprawdzanie stanu pracy urz¹dzenia
	  switch(cr_screen.menu_state){
	  case MAIN_SCREEN:  // dla ekranu g³ównego
	  if(t.sec != t_last.sec){  // aktualizowanie wyœwietlacza co sekundê
		  if(diff_otp.hysteresis[OBIEG_DIFF] && obieg_cnt < OBIEG_TIME)obieg_cnt++; // zwiêkszanie licznika rozruchu pompki obiegu
		  update_main_screen(temp,t,diff_otp); // aktualizacja wskazañ na ekranie g³ównym
	  }
	  break;
	  case TIME_SCREEN:  // dla ekranu ustawieñ godziny
		  t_in_timer = update_time_screen(ts_p);  // aktualizacja ekranu menu w oparciu o wspó³rzêdne dotkniêtego pkt na matrycy
		  break;

	  // dla ekranu ustawieñ ró¿nicy temp.
	  case PUMP_T_MIN_SCREEN:
	  case PUMP_T_MAX_SCREEN:
		  diff_menu = update_temp_screen(ts_p,cr_screen.menu_state);  // aktualizacja ekranu menu w oparciu o wspó³rzêdne dotkniêtego pkt na matrycy
		  break;
	  }

	  t_last = t;  // przepisanie struktury czasu (aby porównac j¹ w nastêpnym cyklu

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();
  __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL16;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART3|RCC_PERIPHCLK_I2C2
                              |RCC_PERIPHCLK_RTC;
  PeriphClkInit.Usart3ClockSelection = RCC_USART3CLKSOURCE_PCLK1;
  PeriphClkInit.I2c2ClockSelection = RCC_I2C2CLKSOURCE_HSI;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C2_Init(void)
{

  /* USER CODE BEGIN I2C2_Init 0 */

  /* USER CODE END I2C2_Init 0 */

  /* USER CODE BEGIN I2C2_Init 1 */

  /* USER CODE END I2C2_Init 1 */
  hi2c2.Instance = I2C2;
  hi2c2.Init.Timing = 0x200009FE;
  hi2c2.Init.OwnAddress1 = 0;
  hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c2.Init.OwnAddress2 = 0;
  hi2c2.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c2) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c2, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c2, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C2_Init 2 */

  /* USER CODE END I2C2_Init 2 */

}

/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  RTC_TimeTypeDef sTime = {0};
  RTC_DateTypeDef sDate = {0};

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */
  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN Check_RTC_BKUP */
//  HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
//  HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
  return;
  /* USER CODE END Check_RTC_BKUP */

  /** Initialize RTC and set the Time and Date
  */
  sTime.Hours = 0x11;
  sTime.Minutes = 0x30;
  sTime.Seconds = 0x0;
  sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sTime.StoreOperation = RTC_STOREOPERATION_RESET;
  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  sDate.WeekDay = RTC_WEEKDAY_MONDAY;
  sDate.Month = RTC_MONTH_JANUARY;
  sDate.Date = 0x1;
  sDate.Year = 0x20;

  if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */

  /* USER CODE END RTC_Init 2 */

}

/**
  * @brief SPI2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI2_Init(void)
{

  /* USER CODE BEGIN SPI2_Init 0 */

  /* USER CODE END SPI2_Init 0 */

  /* USER CODE BEGIN SPI2_Init 1 */

  /* USER CODE END SPI2_Init 1 */
  /* SPI2 parameter configuration*/
  hspi2.Instance = SPI2;
  hspi2.Init.Mode = SPI_MODE_MASTER;
  hspi2.Init.Direction = SPI_DIRECTION_2LINES;
  hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi2.Init.CLKPhase = SPI_PHASE_2EDGE;
  hspi2.Init.NSS = SPI_NSS_SOFT;
  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
  hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi2.Init.CRCPolynomial = 7;
  hspi2.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  hspi2.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;
  if (HAL_SPI_Init(&hspi2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI2_Init 2 */

  /* USER CODE END SPI2_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 0;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 500;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_PWM_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */
  HAL_TIM_MspPostInit(&htim2);

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 0;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 500;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */
  HAL_TIM_MspPostInit(&htim3);

}

/**
  * @brief TIM6 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM6_Init(void)
{

  /* USER CODE BEGIN TIM6_Init 0 */

  /* USER CODE END TIM6_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM6_Init 1 */

  /* USER CODE END TIM6_Init 1 */
  htim6.Instance = TIM6;
  htim6.Init.Prescaler = 10;
  htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim6.Init.Period = 10000;
  htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim6) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM6_Init 2 */

  /* USER CODE END TIM6_Init 2 */

}

/**
  * @brief TIM7 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM7_Init(void)
{

  /* USER CODE BEGIN TIM7_Init 0 */

  /* USER CODE END TIM7_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM7_Init 1 */

  /* USER CODE END TIM7_Init 1 */
  htim7.Instance = TIM7;
  htim7.Init.Prescaler = 10;
  htim7.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim7.Init.Period = 30000;
  htim7.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim7) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim7, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM7_Init 2 */

  /* USER CODE END TIM7_Init 2 */

}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 9600;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  huart3.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart3.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_RXOVERRUNDISABLE_INIT|UART_ADVFEATURE_DMADISABLEONERROR_INIT;
  huart3.AdvancedInit.OverrunDisable = UART_ADVFEATURE_OVERRUN_DISABLE;
  huart3.AdvancedInit.DMADisableonRxError = UART_ADVFEATURE_DMA_DISABLEONRXERROR;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOF, LCD_RD_Pin|LCD_RST_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, LCD_WR_Pin|LCD_RS_Pin|LCD_CS_Pin|LCD_LED_Pin
                          |LCD_DB7_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, CS1_Pin|CS2_Pin|CS3_Pin|LCD_DB6_Pin
                          |LCD_DB5_Pin|LCD_DB4_Pin|LCD_DB3_Pin|LCD_DB2_Pin
                          |LCD_DB1_Pin|LCD_DB0_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : LCD_RD_Pin LCD_RST_Pin */
  GPIO_InitStruct.Pin = LCD_RD_Pin|LCD_RST_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

  /*Configure GPIO pins : LCD_WR_Pin LCD_RS_Pin LCD_CS_Pin LCD_LED_Pin
                           LCD_DB7_Pin */
  GPIO_InitStruct.Pin = LCD_WR_Pin|LCD_RS_Pin|LCD_CS_Pin|LCD_LED_Pin
                          |LCD_DB7_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : CS1_Pin CS2_Pin CS3_Pin LCD_DB6_Pin
                           LCD_DB5_Pin LCD_DB4_Pin LCD_DB3_Pin LCD_DB2_Pin
                           LCD_DB1_Pin LCD_DB0_Pin */
  GPIO_InitStruct.Pin = CS1_Pin|CS2_Pin|CS3_Pin|LCD_DB6_Pin
                          |LCD_DB5_Pin|LCD_DB4_Pin|LCD_DB3_Pin|LCD_DB2_Pin
                          |LCD_DB1_Pin|LCD_DB0_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
