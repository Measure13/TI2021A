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
static int temp, base_freq_index = 0;
static float normmax, tempnorm;
static const int n = 1 << 10;
static int freq_interval = 1000;
static int freq, further_times = 0;
static const double b = 0.07125531, k = 1.22043663;

static const uint8_t adc_bytes = 2;

static TaskHandle_t task_handle;

void First_Sample()
{
    static const char* TAG = "First_Sample";
    ADC_Wait_Results();
    ESP_LOGI(TAG, "ADC Sample done");
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
        FFT_Get_Norms(fn, norms, n);
        UARTHMI_Draw_Curve_addt(norms, n);
        TCP_Send(norms);
        FFT(fn, n);
        normmax = 0;
        for (i = 3; i < (int)(100 * 1000 / freq_interval + 1); ++i)
        {
            tempnorm = norms[i];
            if (normmax < tempnorm)
            {
                normmax = tempnorm;
                base_freq_index = i;
            }
        }
        free(fn);
        free(norms);
    }
    else if (ret == ESP_ERR_TIMEOUT) {
        ESP_LOGE(TAG, "Time out!");
    }
    ADC_Stop();
}

void Further_Sample()
{
    static const char* TAG = "Further_Sample";

    ++further_times;
    freq = (int)((base_freq_index * k + b) * freq_interval * 11);
    freq_interval = freq / n;
    ADC_Freq_Config(freq);
    ADC_Wait_Results();
    ret = ADC_Read_Raw(n, result, &ret_num);
    if (ret == ESP_OK) {
        fn = (complex*)malloc(COMPLEX_SIZE * n);
        norms = (float*)malloc(sizeof(float) * n);
        for (i = 0; i < ret_num; i += adc_bytes) {
            ESP_ERROR_CHECK(ADC_Read_Cal(&result[i], &temp));
            fn[i / 2].real = (float)temp;
            fn[i / 2].imag = 0.0f;
        }
        FFT_Hanning_Window(fn, n);
        FFT(fn, n);
        FFT_Get_Norms(fn, norms, n);
        // TCP_Send(norms);
        normmax = 0;
        for (i = 1; i < (int)(100 * 1000 / freq_interval + 1); ++i)
        {
            tempnorm = norms[i];
            if (normmax < tempnorm)
            {
                normmax = tempnorm;
                base_freq_index = i;
            }
        }
        ESP_LOGI(TAG, "time:%d, base norm:%f, base freq index:%d", further_times, normmax, base_freq_index);
        free(fn);
        free(norms);
    }
    else if (ret == ESP_ERR_TIMEOUT) {
        ESP_LOGE(TAG, "Time out!");
    }
    ADC_Stop();
}

static void test_draw(void)
{
    static const double pi2 = 2 * acos(-1.0);
    float* test_results;
    test_results = (double*)malloc(sizeof(double) * n);
    for (int i = 0; i < n; i++)
    {
        test_results[i] = 0.0f;
    }
    double coefs[5] = {1, 0.3, 0.1, 0.2, 0.05};
    double phase[5] = {0, 0.02, 0.03, 0.09, 0};
    for (int j = 0; j < 5; j++)
    {
        for (int i = 0; i < n; i++)
        {
            test_results[i] += (float)fabs(coefs[j] * sin(pi2 * i / n + phase[j]));//signal_base_freq * (j + 1) * (i / n * period)
        }
    }
    UARTHMI_Draw_Curve_addt(test_results, n);
    free(test_results);
}

void app_main(void)
{
    result = (uint8_t*)malloc(sizeof(uint8_t) * n * 2);
    result[0] = 0;
    freq = n * freq_interval;
    // freq = 1500 * 1000;
    ADC_Init(freq, n);
    
    //! UART_Init will harm wifi_sta_init, WiFi should be init first
    wifi_sta_init(n);
    TCP_Server_Start();
    UART_Init();
    First_Sample();
    
    // while (1)
    // {
    //     Further_Sample();
    //     if (further_times > 1)
    //     {
    //         break;
    //     }
        
    // }
    free(result);
    TCP_Close();
}
