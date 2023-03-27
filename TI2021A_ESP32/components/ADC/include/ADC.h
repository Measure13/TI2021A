#ifndef __ADC_H__
#define __ADC_H__

#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void ADC_Init(int freq, int num);
void ADC_Wait_Results();
esp_err_t ADC_Deinit();
void ADC_Start();
void ADC_Stop();
esp_err_t ADC_Read_Raw(int n, uint8_t* result, uint32_t* ret_num);
esp_err_t ADC_Read_Cal(uint8_t* raw, int* result);
void ADC_Freq_Config(int freq);

#endif