/*
 * touch.c
 *
 *  Created on: 02.06.2021
 *      Author: Emil
 */

#include "touch.h"

// funkcje mapuj¹ce przeliczaj¹ wartoœci zmiennych

int32_t map(int32_t x, int32_t in_min, int32_t in_max, int32_t out_min, int32_t out_max){
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

float map_float(float x, float in_min, float in_max, float out_min, float out_max){
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void check_version(){
	uint8_t tab_s[3] = { 0x55 , 0x01 , 0x10 }, tab_r[8] = {};
	HAL_UART_Transmit(&uart, tab_s, 3, 100);
	HAL_UART_Receive(&uart, tab_r, 8, 100);
	if(tab_r[0] > 0){
		uint8_t i;
		for(i=0;i<8;i++){
		LCD_SetCursor(40,(i*20));
		LCD_Printf("bit %d : %#x", i, tab_r[i]);
		}
	}else{
		LCD_SetCursor(120,160);
		LCD_Printf("pusto");
	}
}

void send_comand(uint8_t cmd, uint8_t size, uint8_t *rcv){
	uint8_t tab_s[3] = { 0x55 , 0x01 , cmd };
	uint8_t tab_r[size];
	HAL_UART_Transmit(&uart, tab_s, 3, 100);
	HAL_UART_Receive(&uart, tab_r, size, 100);
	memcpy(rcv,tab_r,size);
}

void send_comand_n_data(uint8_t size_cmd, uint8_t* cmd, uint8_t size_rcv, uint8_t *rcv){
	uint8_t tab_s[size_cmd];
	uint8_t tab_r[size_rcv];
	memcpy(tab_s,cmd,size_cmd);
	HAL_UART_Transmit(&uart, tab_s, size_cmd, 100);
	HAL_UART_Receive(&uart, tab_r, size_rcv, 100);
	memcpy(rcv,tab_r,size_rcv);
}

void disp_status(uint8_t* tab, uint8_t size){
	uint8_t i;
	uint8_t recv[size];
	memcpy(recv,tab,size);
	for(i=0;i<size;i++){
			LCD_SetCursor(100,((i+1)*20)+60);
			LCD_Printf("byte %d : %#x", i, recv[i]);
		}
}

void disable_touch(){  // command 0x13
	uint8_t len = 4 , command = 0x13;
	uint8_t successful_tab[4] = { 0x55, 0x02, 0x00, 0x13};
	uint8_t recv[len];
	send_comand(command,len,recv);
	LCD_FillScreen(BLACK);
	LCD_SetCursor(100,40);
	LCD_Printf("DISABLE TOUCH");
	disp_status(recv,len);
	LCD_SetCursor(100,200);
	if(*successful_tab == *recv){
		LCD_Printf(".. OK ..");
	}else{
		LCD_Printf(".. ERROR ..");
	}

}

void reg_start(){  // command 0x22  REGISTER_START_ADDRESS_REQUEST
	uint8_t len = 5 , command[3] = {0x55, 0x01, 0x22};
	uint8_t successful_tab[5] = { 0x55, 0x02, 0x00, 0x22, 0x20};
		uint8_t recv[len];
		send_comand_n_data(3,command,len,recv);
		LCD_FillScreen(BLACK);
		LCD_SetCursor(100,40);
		LCD_Printf("REG START");
		disp_status(recv,len);
		LCD_SetCursor(100,200);
		if(*successful_tab == *recv){
			LCD_Printf(".. OK ..");
		}else{
			LCD_Printf(".. ERROR ..");
		}
}

void calibration_inset(){   // command 0x21  REGISTER_WRITE
	uint8_t len_cmd = 7;
	uint8_t len_recv = 4;
	// datasheet strona 39
//	uint8_t command[7] = {0x55, 0x05, 0x21, 0x00, 0x2E, 0x01, 0x19};
	// wartoœci po kalibracji manualnej
	uint8_t command[7] = {0x55, 0x05, 0x21, 0x00, 0x0A, 0x01, 0x00};

	uint8_t successful_tab[4] = { 0x55, 0x02, 0x00, 0x21};
	uint8_t recv[len_recv];
	send_comand_n_data(len_cmd,command,len_recv,recv);
	LCD_FillScreen(BLACK);
	LCD_SetCursor(100,40);
	LCD_Printf("CALIBRATION INSET");
	disp_status(recv,len_recv);

	LCD_SetCursor(100,200);
	if(*successful_tab == *recv){
		LCD_Printf(".. OK ..");
	}else{
		LCD_Printf(".. ERROR ..");
	}
}

void register_read(){    // command 0x20  REGISTER_READ
	uint8_t len_cmd = 7;
	uint8_t len_recv = 5;
	uint8_t command[7] = {0x55, 0x04, 0x20, 0x00, 0x0D, 0x01};
	uint8_t successful_tab[4] = { 0x55, 0x02, 0x00, 0x20};
	uint8_t recv[len_recv];
	send_comand_n_data(len_cmd,command,len_recv,recv);
	LCD_FillScreen(BLACK);
	LCD_SetCursor(100,40);
	LCD_Printf("REGISTER_READ");
	disp_status(recv,len_recv);

	LCD_SetCursor(100,200);
	if(successful_tab[2] == recv[2]){
		LCD_Printf(".. OK ..");
	}else{
		LCD_Printf(".. ERROR ..");
	}
}

void calibration_mode(){    // command 0x14
	uint8_t len_cmd = 4;
	uint8_t len_recv = 4;
	uint8_t command[4] = {0x55, 0x02, 0x14, 0x04}, recv_tab[4],i;
	uint8_t successful_tab[4] = { 0x55, 0x02, 0x00, 0x14};
	uint8_t recv[len_recv];
	send_comand_n_data(len_cmd,command,len_recv,recv);
	LCD_FillScreen(BLACK);
	LCD_SetCursor(80,40);
	LCD_Printf("CALIBRATION POINTS ... touch them");
	disp_status(recv,len_recv);
	HAL_Delay(1000);
	for(i=0;i<4;i++){
		uint16_t x,y;
		switch(i){
		case 0:x=20; y=20; break;
		case 1:x=300; y=20; break;
		case 2:x=300; y=220; break;
		case 3:x=20; y=220; break;
		}
		LCD_FillScreen(BLACK);
		LCD_FillCircle(x,y,20,MAGENTA);
		while(HAL_UART_Receive(&uart,recv_tab,4,1000) != HAL_OK);
		disp_status(recv_tab,len_recv);
		LCD_SetCursor(100,200);
		if(successful_tab[2] == recv[2]){
			LCD_Printf(".. OK ..");
		}else{
			LCD_Printf(".. ERROR ..");
		}
		HAL_Delay(1000);
	}
}


void enable_touch(){    // command 0x12
	uint8_t len = 4 , command = 0x12;
	uint8_t recv[len], successful_tab[4] = {0x55, 0x02, 0x00, 0x12};
	send_comand(command,len,recv);
	LCD_FillScreen(BLACK);
	LCD_SetCursor(100,40);
	LCD_Printf("ENABLE TOUCH");
	disp_status(recv,len);
	LCD_SetCursor(100,200);
	if(successful_tab[3] == recv[3]){
		LCD_Printf(".. OK ..");
	}else{
		LCD_Printf(".. ERROR ..");
	}
}

point get_pos(){


	uint8_t p,rec[10] = {0},i;
	uint16_t x,y,x_out,y_out;
	int ret = -1;
	point ts;



	HAL_UART_Receive(&uart,rec,1,10);
	if(rec[0] == 128 || rec[0] ==129){
			HAL_UART_Receive(&uart,rec,4,10);
			ret = 1;
		}

	if (ret >= 0) {

//		for(o=0;o<5;o++){
//			rec[o] = reciv[ret+o];
//		}
//		TOUCH_ADC_Y_MIN,TOUCH_ADC_Y_MAX,0,TFT_WIDTH
		x = ((rec[0] | (rec[1] << 8)) >> 2);
		y = ((rec[2] | (rec[3] << 8)) >> 2);
#ifdef DO_DOMKU
		x_out = map(y,TOUCH_ADC_Y_MIN,TOUCH_ADC_Y_MAX,0,TFT_HEIGHT);
		y_out = map(x,TOUCH_ADC_Y_MAX,TOUCH_ADC_Y_MIN,0,TFT_WIDTH);
//		y_out = map(y,TOUCH_ADC_Y_MIN,TOUCH_ADC_Y_MAX,0,TFT_HEIGHT);
//		x_out = map(x,TOUCH_ADC_Y_MAX,TOUCH_ADC_Y_MIN,0,TFT_WIDTH);
#endif
#ifdef DO_DOMU
		x_out = map(x,TOUCH_ADC_Y_MIN,TOUCH_ADC_Y_MAX,0,TFT_HEIGHT);
		y_out = map(y,TOUCH_ADC_Y_MIN,TOUCH_ADC_Y_MAX,0,TFT_WIDTH);
#endif

		p = (rec[0] & 0b1);

		ts.x = x_out;
		ts.y = y_out;
		ts.p = p;

	}else{
		ts.p = NO_TOUCH;
	}

	return ts;
}

void touch_init(){
	disable_touch();
	reg_start();
	calibration_inset(); // kalibracja automatyczna dane z datasheeta
	enable_touch();
//	HAL_UART_Receive(&uart,recive,20,50);


}
