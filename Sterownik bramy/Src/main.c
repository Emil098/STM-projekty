
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2019 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f0xx_hal.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc;

TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim14;
TIM_HandleTypeDef htim16;
TIM_HandleTypeDef htim17;

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
#define time_ms_for_50_pr 340  // oczekiwany czas do timera do 50%
#define time_ms_for_10_pr 1700 // oczekiwany czas do timera do 10%
#define expected_val_for_50_pr (time_ms_for_50_pr * 2)
#define expected_val_for_10_pr (time_ms_for_10_pr * 2)
#define tim_50_pr_max 120  // wartoœc max ocr dla pwm dla pwm przy 50%
#define tim_10_pr_max 34   // wartoœc max ocr dla pwm dla pwm przy 10%
uint8_t timer_50_pr_val = tim_50_pr_max;
uint8_t timer_10_pr_val = tim_10_pr_max;
uint32_t val_1;


#define FAST 1
#define SLOW 0
#define LEFT 0
#define RIGHT 1
#define ADD 1
#define DONT_ADD 0
#define FULL_OPEN 58	    // iloœc kroków do pe³nego otwarcia bramy
#define HALF_OPEN 37        // iloœc kroków do po³owy
uint16_t adc_val = 100;		// wartoœc chwilowa adc
uint8_t motor_steps = 0;	// licznik kroków (na magnesach)
uint8_t wait_for_fast = 0;	// licznki który po pewnym czasie za³¹cza szybkie otwieranie zamykanie
uint8_t direction;			// okreœla kierunek czy kroki maj¹ siê zwiêkszac czy zmniejszac
uint8_t adding_actived = 0;	// zlicza kroki tylko w czasie pracy silnika
uint16_t time_between = 0;	// okreœla czas miêdzy poszczególnymi krokami opcjonalnie na jej podstawie ustalanie bedzie wype³nienie miedzy trybem szybkim a wolnym 1 setunda = wartoœc zmiennej 2000
uint8_t last_step = 0;		// zmienna pomocnicza do tim14 i time_between s³u¿y do okreœlenia zmiany kroku
uint8_t current_step = 0;	// zmienna pomocnicza do tim14 i time_between s³u¿y do okreœlenia zmiany kroku
uint8_t speed_mode =SLOW;   // okreœla prêdkoœc silnika
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC_Init(void);
static void MX_TIM3_Init(void);
static void MX_TIM16_Init(void);
static void MX_TIM17_Init(void);
static void MX_TIM14_Init(void);
                                    
void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);
                                

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
	if(htim->Instance == TIM14){
		// timer 2 kHz odpowiada za przeliczenie wype³nienia 44 - 50% na wyjœciu pwm w zale¿noœci od czasu miedzy zliczonymi krokami

		current_step = motor_steps;
		if(current_step != last_step){ // je¿eli licznik kroków w³aœnie siê zwiêkszy³ i w³¹czony jest tryb szybki to ...
			if(speed_mode == FAST){
			int val;
			val = expected_val_for_50_pr - time_between;
			if(val<0){ // czas pomiedzy nie mo¿e byc krótszy ni¿ oczekiwany je¿eli jest to podaj wype³nienie maksymalne
				timer_50_pr_val = tim_50_pr_max;
			}
			else if(val > 425){ // czas pomiedzy nie mo¿e byc za d³ugi wype³nienie minimalne 44%
				timer_50_pr_val = 105;
			}
			else{
				val_1 = ((expected_val_for_50_pr - time_between)*10000) / expected_val_for_50_pr; // przeliczanie czasu pomiedzy na wype³nienie (ko¿ysta z funkcji map z arduino)
				val_1 = (50 * 1000) - val_1;
				val_1 = (val_1 - 0) * ((tim_50_pr_max * 1000) - 0) / (5000 - 0) + 0;
				val_1 /= 1000;
				timer_50_pr_val = val_1;
			}
			}
			time_between = 0;  // po zmianie wype³nienia zeruje licznki
		}
		last_step = current_step;
		time_between++;  // zwiêkszanie licznika czasu pomiedzy
	}
	if(htim->Instance == TIM16){
		    // timer z przerwaniem na 10 Hz ma ustalic sta³¹ czasow¹ do prze³¹czania wype³nienia z 10 do 50% ( wolny start silnika )
			if(HAL_GPIO_ReadPin(SW1_GPIO_Port,SW1_Pin)==0){ // przy przycisku resetuje program na wszeli wypadek
				NVIC_SystemReset();
			}
			if(HAL_GPIO_ReadPin(FAULT_GPIO_Port,FAULT_Pin)==0){ // przy przycisku resetuje program na wszeli wypadek
				NVIC_SystemReset();
			}
			wait_for_fast++;
		}
	if(htim->Instance == TIM17){
		     // timer który zlicza kroki silnika ustawiony na 1 kHz
		if(HAL_GPIO_ReadPin(COUNT_GPIO_Port,COUNT_Pin)==0 && adding_actived){
			while(!HAL_GPIO_ReadPin(COUNT_GPIO_Port,COUNT_Pin));
			switch(direction){
			case LEFT:
				motor_steps--;
				break;
			case RIGHT:
				motor_steps++;
				break;
			}
		}
	}
}


//uint32_t map(uint32_t x, uint32_t in_min, uint32_t in_max, uint32_t out_min, uint32_t out_max) {   // mapuje wartoœc adc by przeliczyc j¹ na wolty
//	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
//}
void change_speed(uint8_t channel,uint8_t speed){
	speed_mode = speed;
	if(channel == 4){
		if(speed==FAST)TIM3->CCR4 = timer_50_pr_val;
		if(speed==SLOW)TIM3->CCR4 = 34;
	}
	if(channel == 2){
		if(speed==FAST)TIM3->CCR2 = timer_50_pr_val;
		if(speed==SLOW)TIM3->CCR2 = 34;
	}
}
void go_left(uint8_t adding){ // silnik skrêca w lewo
	direction = LEFT;  // zmienna u¿ywana w timerze (wie czy zwiêkszac czy zmniejszac licznik kroków)
	adding_actived = adding;  // zmienna która daje info do timera czy zliczac kroki
	HAL_GPIO_WritePin(BOT_L_GPIO_Port,BOT_L_Pin,0);
	HAL_GPIO_WritePin(BOT_R_GPIO_Port,BOT_R_Pin,1);
	HAL_TIM_PWM_Stop(&htim3,TIM_CHANNEL_4);
	HAL_GPIO_WritePin(PWM_R_GPIO_Port,PWM_R_Pin,0);
	HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_2);
//	adding_actived = 0;
}
void go_right(uint8_t adding){ // silnik skrêca w prawo
	direction = RIGHT;  // zmienna u¿ywana w timerze (wie czy zwiêkszac czy zmniejszac licznik kroków)
	adding_actived = adding; // zmienna która daje info do timera czy zliczac kroki
	HAL_GPIO_WritePin(BOT_L_GPIO_Port,BOT_L_Pin,1);
	HAL_GPIO_WritePin(BOT_R_GPIO_Port,BOT_R_Pin,0);
	HAL_TIM_PWM_Stop(&htim3,TIM_CHANNEL_2);
	HAL_GPIO_WritePin(PWM_L_GPIO_Port,PWM_L_Pin,0);
	HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_4);
//	adding_actived = 0;
}
void open_full(void){  // pe³ne otwieranie bramy
	uint8_t last_speed=0;
	wait_for_fast=0;
	go_right(ADD);
	while(motor_steps<FULL_OPEN){
//		go_right();
		if(wait_for_fast==50 && motor_steps < FULL_OPEN-4)last_speed=1;  // wype³nienie 50% po 5 sekundach
		if(motor_steps >= FULL_OPEN-3){  // wype³nienie 10% je¿eli brama ma 3 kroki do zakoñczenia otwierania
			last_speed=0;
			change_speed(4,SLOW);
		}
		if(last_speed)change_speed(4,FAST);
	}
}
void open_half(void){  // otwieranie bramy do po³owy
	uint8_t last_speed=0;
	wait_for_fast=0;
//	adding_actived = 1;
	go_right(ADD);
	while(motor_steps<HALF_OPEN){
//		go_right();
		if(wait_for_fast==50 && motor_steps < HALF_OPEN-4)last_speed=1; // wype³nienie 50% po 5 sekundach
		if(motor_steps >= HALF_OPEN-3){  // wype³nienie 10% je¿eli brama ma 3 kroki do zakoñczenia otwierania
			last_speed=0;
			change_speed(4,SLOW);

		}
		if(last_speed)change_speed(4,FAST);
	}

}
void close(void){
	uint8_t last_speed=0;
	wait_for_fast=0;
	go_left(ADD);
	while(motor_steps!=0){
//		go_left();
		if(HAL_GPIO_ReadPin(REM2_GPIO_Port,REM2_Pin)==0){  // je¿eli w trakcie zamykania pilot zostanie klikniêty otwiera bramê
			stop_motor();
			TIM3->CCR2 = 34;
			HAL_Delay(50);
			if(motor_steps < HALF_OPEN)open_half();
			else open_full();
			break;
		}
		if(wait_for_fast==50 && motor_steps > 4)last_speed=1; // wype³nienie 50% po 5 sekundach
		if(motor_steps <= 3){  // wype³nienie 10% je¿eli brama ma 3 kroki do zakoñczenia zamykania
			last_speed=0;
			change_speed(2,SLOW);
		}
		if(last_speed)change_speed(2,FAST);
	}
}
void stop_motor(void){ // hamowanie silnika
	adding_actived = 0;  // wy³¹cza zliczanie kroków
	HAL_GPIO_WritePin(BOT_L_GPIO_Port,BOT_L_Pin,1);
	HAL_GPIO_WritePin(BOT_R_GPIO_Port,BOT_R_Pin,1);
	HAL_TIM_PWM_Stop(&htim3,TIM_CHANNEL_4);
	HAL_GPIO_WritePin(PWM_R_GPIO_Port,PWM_R_Pin,0);
	HAL_TIM_PWM_Stop(&htim3,TIM_CHANNEL_2);
	HAL_GPIO_WritePin(PWM_L_GPIO_Port,PWM_L_Pin,0);
}
void gpio_init(void){  // "mryganie" diod na starcie i zahamowanie silnika
	stop_motor();
	HAL_GPIO_WritePin(LED1_GPIO_Port,LED1_Pin,1);
	HAL_GPIO_WritePin(LED2_GPIO_Port,LED2_Pin,0);
	HAL_GPIO_WritePin(LED3_GPIO_Port,LED3_Pin,1);
	HAL_Delay(1000);
	HAL_GPIO_WritePin(LED1_GPIO_Port,LED1_Pin,0);
	HAL_GPIO_WritePin(LED2_GPIO_Port,LED2_Pin,1);
	HAL_GPIO_WritePin(LED3_GPIO_Port,LED3_Pin,0);
	HAL_Delay(1000);
	HAL_GPIO_WritePin(LED1_GPIO_Port,LED1_Pin,1);
	HAL_GPIO_WritePin(LED2_GPIO_Port,LED2_Pin,1);
	HAL_GPIO_WritePin(LED3_GPIO_Port,LED3_Pin,1);
}
void reset_pos(void){
	adding_actived = 0;
	while(HAL_GPIO_ReadPin(REM1_GPIO_Port,REM1_Pin)){ // przycisk na p³ytce po prawej z brzegu
		// wykonuje siê dopuki u¿ytkownik nie zatwierdzi pozycji "zero"
		// za pomoc¹ przycisków podnoszenie opuszczanie bramy
		// przy nie wciœniêtych przyciskach hamowanie silnika


		if(HAL_GPIO_ReadPin(REM3_GPIO_Port,REM3_Pin)==0){  // przycisk na p³ytce po œrodku
			HAL_Delay(100);
			while(!HAL_GPIO_ReadPin(REM3_GPIO_Port,REM3_Pin)){
				go_left(DONT_ADD); // w dó³
				adding_actived = 0;
				HAL_GPIO_WritePin(LED3_GPIO_Port,LED3_Pin,0);
			}
		}
		if(HAL_GPIO_ReadPin(REM2_GPIO_Port,REM2_Pin)==0){  // przycisk na p³ytce po lewej z brzegu
			HAL_Delay(100);
			while(!HAL_GPIO_ReadPin(REM2_GPIO_Port,REM2_Pin)){
				go_right(DONT_ADD);  // do góry
				adding_actived = 0;
				HAL_GPIO_WritePin(LED1_GPIO_Port,LED1_Pin,0);
			}
		}
		HAL_GPIO_WritePin(LED1_GPIO_Port,LED1_Pin,1);
		HAL_GPIO_WritePin(LED3_GPIO_Port,LED3_Pin,1);
		stop_motor();

	}
//	HAL_Delay(2000);
	gpio_init();
	HAL_Delay(2000);
	motor_steps = 0; // zeruje licznik kroków
}
/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  *
  * @retval None
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

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
  MX_ADC_Init();
  MX_TIM3_Init();
  MX_TIM16_Init();
  MX_TIM17_Init();
  MX_TIM14_Init();
  /* USER CODE BEGIN 2 */
	gpio_init();
	HAL_ADC_Start(&hadc);
	HAL_TIM_Base_Start_IT(&htim14);
	HAL_TIM_Base_Start_IT(&htim16);
	HAL_TIM_Base_Start_IT(&htim17);
	reset_pos();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1)
	{

  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */


		//		/*
		if (HAL_ADC_PollForConversion(&hadc, 10) == HAL_OK) {
			adc_val = HAL_ADC_GetValue(&hadc);
			HAL_ADC_Start(&hadc);
		}

		if(HAL_GPIO_ReadPin(REM2_GPIO_Port,REM2_Pin)==0){  // po wciœnieciu lewego przycisku otwiera do po³owy lub zamyka bramê zale¿y w jakiej jest pozycji
			HAL_Delay(100);
			while(!HAL_GPIO_ReadPin(REM2_GPIO_Port,REM2_Pin)){
//				HAL_Delay(500);
				switch(motor_steps){
				case 0:
					HAL_GPIO_WritePin(LED3_GPIO_Port,LED3_Pin,0);
					open_half();
					break;
				case HALF_OPEN:
					HAL_GPIO_WritePin(LED1_GPIO_Port,LED1_Pin,0);
					HAL_Delay(200);
					close();
					break;
				case FULL_OPEN:
					HAL_GPIO_WritePin(LED1_GPIO_Port,LED1_Pin,0);
					HAL_Delay(200);
					close();
					break;
				}
			}
		}
		if(HAL_GPIO_ReadPin(REM3_GPIO_Port,REM3_Pin)==0 && motor_steps < FULL_OPEN){  // po wciœnieciu œrodkowego przycisku otwiera ca³kowicie bramê
			HAL_Delay(100);
			while(!HAL_GPIO_ReadPin(REM3_GPIO_Port,REM3_Pin)){
				HAL_GPIO_WritePin(LED2_GPIO_Port,LED2_Pin,0);
				open_full();
			}
		}
		HAL_GPIO_WritePin(LED1_GPIO_Port,LED1_Pin,1);
		HAL_GPIO_WritePin(LED2_GPIO_Port,LED2_Pin,1);
		HAL_GPIO_WritePin(LED3_GPIO_Port,LED3_Pin,1);
		stop_motor();  // przy wy³¹czeniu hamuje silnik
		//		*/

	}
  /* USER CODE END 3 */

}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_HSI14;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSI14State = RCC_HSI14_ON;
  RCC_OscInitStruct.HSICalibrationValue = 16;
  RCC_OscInitStruct.HSI14CalibrationValue = 16;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL12;
  RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* ADC init function */
static void MX_ADC_Init(void)
{

  ADC_ChannelConfTypeDef sConfig;

    /**Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion) 
    */
  hadc.Instance = ADC1;
  hadc.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
  hadc.Init.Resolution = ADC_RESOLUTION_12B;
  hadc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc.Init.ScanConvMode = ADC_SCAN_DIRECTION_FORWARD;
  hadc.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc.Init.LowPowerAutoWait = DISABLE;
  hadc.Init.LowPowerAutoPowerOff = DISABLE;
  hadc.Init.ContinuousConvMode = ENABLE;
  hadc.Init.DiscontinuousConvMode = DISABLE;
  hadc.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc.Init.DMAContinuousRequests = DISABLE;
  hadc.Init.Overrun = ADC_OVR_DATA_OVERWRITTEN;
  if (HAL_ADC_Init(&hadc) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure for the selected ADC regular channel to be converted. 
    */
  sConfig.Channel = ADC_CHANNEL_3;
  sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
  sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* TIM3 init function */
static void MX_TIM3_Init(void)
{

  TIM_MasterConfigTypeDef sMasterConfig;
  TIM_OC_InitTypeDef sConfigOC;

  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 9;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 239;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 34;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  HAL_TIM_MspPostInit(&htim3);

}

/* TIM14 init function */
static void MX_TIM14_Init(void)
{

  htim14.Instance = TIM14;
  htim14.Init.Prescaler = 999;
  htim14.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim14.Init.Period = 23;
  htim14.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim14.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim14) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* TIM16 init function */
static void MX_TIM16_Init(void)
{

  htim16.Instance = TIM16;
  htim16.Init.Prescaler = 9999;
  htim16.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim16.Init.Period = 479;
  htim16.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim16.Init.RepetitionCounter = 0;
  htim16.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim16) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* TIM17 init function */
static void MX_TIM17_Init(void)
{

  htim17.Instance = TIM17;
  htim17.Init.Prescaler = 999;
  htim17.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim17.Init.Period = 47;
  htim17.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim17.Init.RepetitionCounter = 0;
  htim17.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim17) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/** Configure pins as 
        * Analog 
        * Input 
        * Output
        * EVENT_OUT
        * EXTI
*/
static void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, LED1_Pin|LED2_Pin|LED3_Pin|BOT_L_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(BOT_R_GPIO_Port, BOT_R_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : SW1_Pin SW2_Pin */
  GPIO_InitStruct.Pin = SW1_Pin|SW2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : LED1_Pin LED2_Pin LED3_Pin BOT_L_Pin */
  GPIO_InitStruct.Pin = LED1_Pin|LED2_Pin|LED3_Pin|BOT_L_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : FAULT_Pin COUNT_Pin REM1_Pin REM3_Pin 
                           REM2_Pin */
  GPIO_InitStruct.Pin = FAULT_Pin|COUNT_Pin|REM1_Pin|REM3_Pin 
                          |REM2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : BOT_R_Pin */
  GPIO_InitStruct.Pin = BOT_R_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(BOT_R_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  file: The file name as string.
  * @param  line: The line in file as a number.
  * @retval None
  */
void _Error_Handler(char *file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	while(1)
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
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
	/* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
