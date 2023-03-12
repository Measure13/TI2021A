#include <stdint.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "FFT.h"
#include "WiFi.h"
#include "ADC.h"

static esp_err_t ret;

extern int COMPLEX_SIZE;
static int i;
static uint8_t* result;
static complex* fn;
static uint32_t ret_num = 0;
static int temp, argmax = 0;
static float normmax, tempnorm;
static const int n = 1 << 10;
static int freq = n * 1000;
static const uint8_t adc_bytes = 2;

// static TaskHandle_t s_task_handle;

void First_Sample()
{
    static const char* TAG = "First_Sample";

    while (!ADC_Get_Flag_Done())
    {
        
    }
    ADC_Set_Flag_Done();
    ret = ADC_Read_Raw(n, result, &ret_num);
    if (ret == ESP_OK) {
        fn = (complex*)malloc(COMPLEX_SIZE * n);
        for (i = 0; i < ret_num; i += adc_bytes) {
            ESP_ERROR_CHECK(ADC_Read_Cal(&result[i], &temp));
            fn[i / 2].real = (float)temp;
            fn[i / 2].imag = 0.0f;
            // if (i == 0)
            // {
            //     ESP_LOGI("ADC", "Value: %fmV, %x", fn[i].real, p->type1.data);
            //     vTaskDelay(1);
            // }
        }
        FFT(fn, n);
        normmax = 0;
        for (i = 1; i < n; ++i)
        {
            tempnorm = norm(fn[i]);
            if (normmax < tempnorm)
            {
                normmax = tempnorm;
                argmax = i;
            }
            if (i == 1 << 9)
            {
                vTaskDelay(1);
            }
        }
        free(fn);
        ESP_LOGI(TAG, "max norm:%f, max arg:%d", normmax, argmax);
        TCP_Send(fn);
    }
    else if (ret == ESP_ERR_TIMEOUT) {
        ESP_LOGE(TAG, "Time out!");
    }
}

void app_main(void)
{
    result = (uint8_t*)malloc(sizeof(uint16_t) * n);
    result[0] = 0;
    ADC_Init(freq, n);
    wifi_sta_init(n);
    TCP_Server_Start();
    ADC_Start();
    while (1)
    {
        First_Sample();
        vTaskDelay(8000);
    }
    free(result);
    TCP_Close();
    ADC_Stop();
}
