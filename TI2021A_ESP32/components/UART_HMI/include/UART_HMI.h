#ifndef __UART_HMI_H__
#define __UART_HMI_H__

#include "esp_err.h"
#include "driver/uart.h"

#define STM_UART_NUM UART_NUM_1
#define HMI_UART_NUM UART_NUM_2

void UART_Init(void);
void UARTHMI_Draw_Curve_addt(float* pf, uint16_t num);
void UARTHMI_Send_Float(int index, float float_num);
int UART_Write_Data(uint8_t uart_num, uint8_t* data_pointer, int data_len);
int UART_Read_Data(uint8_t uart_num, uint8_t* data_pointer);
esp_err_t UART_Read_ADC(uint8_t uart_num, uint32_t adc_freq, uint8_t* data_pointer, uint16_t data_len);

#endif