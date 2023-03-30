/* USER CODE BEGIN Header */
/**
	******************************************************************************
	* @file    usart.c
	* @brief   This file provides code for the configuration
	*          of the USART instances.
	******************************************************************************
	* @attention
	*
	* Copyright (c) 2023 STMicroelectronics.
	* All rights reserved.
	*
	* This software is licensed under terms that can be found in the LICENSE file
	* in the root directory of this software component.
	* If no LICENSE file comes with this software, it is provided AS-IS.
	*
	******************************************************************************
	*/
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "usart.h"

/* USER CODE BEGIN 0 */
#include "adc.h"
#include <stdio.h>
#define UART_RX_BUF_SIZE 1024
extern int adc_freq;

static char uart1_rx_bp[UART_RX_BUF_SIZE * 2];
static char uart1_tx_bp[UART_RX_BUF_SIZE * 2];
static uint8_t uart1_rx_cnt = 0;
static uint8_t uart1_tx_cnt = 0;
static uint8_t uart1_rx_buf = 0;
/* USER CODE END 0 */

UART_HandleTypeDef huart1;

/* USART1 init function */

void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 9600;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */
	HAL_UART_Receive_IT(&huart1, (uint8_t *)&uart1_rx_buf, 1);
  /* USER CODE END USART1_Init 2 */

}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspInit 0 */

  /* USER CODE END USART1_MspInit 0 */
    /* USART1 clock enable */
    __HAL_RCC_USART1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_9|GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* USART1 interrupt Init */
    HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspInit 1 */

  /* USER CODE END USART1_MspInit 1 */
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspDeInit 0 */

  /* USER CODE END USART1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART1_CLK_DISABLE();

    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9|GPIO_PIN_10);

    /* USART1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspDeInit 1 */

  /* USER CODE END USART1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
int fputc(int ch, FILE *stream)
{
	while((USART1->SR & 0X40) == 0);

	USART1->DR = (uint8_t) ch;

	return ch;
}

void UART_RX_Data_Parse(char* p, uint8_t cnt)
{
	adc_freq = 0;
	for (int i = 0; i < 4; ++i)
	{
		adc_freq += ((p[i] & 0xff) << (8 * i));
	}
	memset(uart1_tx_bp, 0x00, sizeof(uart1_tx_bp));
	uart1_tx_cnt = 7;
	for (int i = 0; i < sizeof(int); ++i)
	{
		uart1_tx_bp[i] = ((adc_freq >> (i * 8))) & 0xff;
	}
	for (int i = 4; i < uart1_tx_cnt; ++i)
	{
		uart1_tx_bp[i] = 0xff;
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	/* Prevent unused argument(s) compilation warning */
	UNUSED(huart);
 
	if(uart1_rx_cnt >= UART_RX_BUF_SIZE)  //????????¡è???
	{
		uart1_rx_cnt = 0;
		memset(uart1_rx_bp, 0x00, sizeof(uart1_rx_bp));
		HAL_UART_Transmit(&huart1, (uint8_t *)"OVERFLOW!!!", 12, 0xFFFF); 	
				
	}
	else
	{
		uart1_rx_bp[uart1_rx_cnt++] = uart1_rx_buf;
	
		if((uart1_rx_cnt > 3)&&(uart1_rx_bp[uart1_rx_cnt-1] == 0xFF)&&(uart1_rx_bp[uart1_rx_cnt-2] == 0xFF)&&(uart1_rx_bp[uart1_rx_cnt-3] == 0xFF)) //??¡è?????????????
		{
			UART_RX_Data_Parse(uart1_rx_bp, uart1_rx_cnt);
			// HAL_UART_Transmit(&huart1, (uint8_t *)&uart1_tx_bp, uart1_tx_cnt, 0xFFFF); //?¡ã??????¡ã??????????????????????????
			while(HAL_UART_GetState(&huart1) == HAL_UART_STATE_BUSY_TX);
			printf("\nusart adc freq:%d\n", adc_freq);
			uart1_rx_cnt = 0;
			memset(uart1_rx_bp, 0x00, sizeof(uart1_rx_bp));
			NVIC_EnableIRQ(ADC_IRQn);
		}
	}
	
	HAL_UART_Receive_IT(&huart1, (uint8_t *)&uart1_rx_buf, 1);
}

void USART_Conv_Data(uint16_t* adc_data_p, uint16_t length)
{
	uart1_tx_cnt = length;
	for (int i = 0; i < length; ++i)
	{
		uart1_tx_bp[i * 2] = ((uint8_t)(adc_data_p[i])) & 0xff;
		uart1_tx_bp[i * 2 + 1] = ((uint8_t)(adc_data_p[i] >> 8)) & 0x0f;
	}
	memset(adc_data_p, 0, sizeof(uint16_t) * length);
	// printf("here\n");
	HAL_UART_Transmit(&huart1, (uint8_t *)&uart1_tx_bp, uart1_tx_cnt, 0xFFFF);
	// printf("here!!!\n");
	memset(uart1_rx_bp, 0x00, sizeof(uart1_rx_bp));
}
/* USER CODE END 1 */
