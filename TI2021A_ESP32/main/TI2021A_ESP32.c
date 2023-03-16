#include <stdint.h>
#include <math.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "FFT.h"
#include "WiFi.h"
#include "ADC.h"
#include "UART_HMI.h"

static esp_err_t ret;

extern int COMPLEX_SIZE;
static int i;
static uint8_t* result;
static complex* fn;
static float* norms;
static uint32_t ret_num = 0;
static int temp, argmax = 0;
static float normmax, tempnorm;
static const int n = 1 << 10;
static int freq_interval = 1100;
static int freq = n * freq_interval;
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
        norms = (float*)malloc(sizeof(float) * n);
        for (i = 0; i < ret_num; i += adc_bytes) {
            ESP_ERROR_CHECK(ADC_Read_Cal(&result[i], &temp));
            fn[i / 2].real = (float)temp;
            fn[i / 2].imag = 0.0f;
            // if (i == 0)
            // {
                // ESP_LOGI("ADC", "Value: %fmV", fn[i / 2].real); // , %x, p->type1.data
                // vTaskDelay(1);
            // }
        }
        // FFT_Hanning_Window(fn, n);
        FFT(fn, n);
        FFT_Get_Norms(fn, norms, n);
        // normmax = 0;
        // for (i = 0; i < n; ++i)
        // {
        //     tempnorm = norms[i];
        //     // ESP_LOGI(TAG, "FFT:%f", tempnorm);
        //     if (normmax < tempnorm)
        //     {
        //         normmax = tempnorm;
        //         argmax = i;
        //     }
        //     if (i == n >> 2)
        //     {
        //         vTaskDelay(1);
        //     }
        // }
        // ESP_LOGI(TAG, "max norm:%f, max arg:%d", normmax, argmax);
        TCP_Send(norms);
        free(fn);
        free(norms);
    }
    else if (ret == ESP_ERR_TIMEOUT) {
        ESP_LOGE(TAG, "Time out!");
    }
}

static void test_draw(void)
{
    static const float pi2 = 2 * acos(-1.0);
    float* test_results;
    test_results = (float*)malloc(sizeof(float) * n);
    for (int i = 0; i < n; i++)
    {
        test_results[i] = 0.0f;
    }
    uint32_t signal_base_freq = 20 * 1000;
    double period = 1e-6;
    float coefs[5] = {1, 0.3, 0.1, 0.2, 0.05};
    float phase[5] = {0, 0.02, 0.03, 0.09, 0};
    for (int j = 0; j < 5; j++)
    {
        for (int i = 0; i < n; i++)
        {
            test_results[i] += coefs[j] * sin(pi2 * signal_base_freq * (j + 1) * (i / n * period) + phase[j]);
        }
    }
    UART_Draw_Curve(test_results, n);
    free(test_results);
}

void app_main(void)
{
    result = (uint8_t*)malloc(sizeof(uint8_t) * n * 2);
    result[0] = 0;
    ADC_Init(freq, n);
    wifi_sta_init(n);
    TCP_Server_Start();
    ADC_Start();
    // UART_Init();
    // while (1)
    // {
        First_Sample();
        // test_draw();
        // vTaskDelay(8000);
    // }
    free(result);
    TCP_Close();
    ADC_Stop();
}
