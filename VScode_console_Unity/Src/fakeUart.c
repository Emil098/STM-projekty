#include "fakeUart.h"

UART_HandleTypeDef test_huart;


void get_huart(UART_HandleTypeDef *huart)
{
    huart = &test_huart;
}

void __HAL_UART_CLEAR_OREFLAG(UART_HandleTypeDef *huart)
{

}

void __HAL_UART_CLEAR_FEFLAG(UART_HandleTypeDef *huart)
{

}

void __HAL_UART_ENABLE_IT(UART_HandleTypeDef *huart, uint32_t it)
{

}

void __HAL_UART_DISABLE_IT(UART_HandleTypeDef *huart, uint32_t it)
{

}