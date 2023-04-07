#include <stdint.h>
#include <math.h>
#include <string.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "FFT.h"
#include "WiFi.h"
#include "ADC.h"
#include "UART_HMI.h"

/*
800    500    500    264.7   164.7
*/

static esp_err_t ret;

extern int COMPLEX_SIZE;
static uint8_t* result;
static float* norms;
static int freq_interval = 1000;
static uint32_t sampling_freq, fft_base_freq;
static uint8_t further_times = 0, final_times = 2;
static float freq_amp_norm[5];          //normalized amplitude
static uint16_t* adc_value_p;

static const uint8_t HARMONIC_ORDER = 5;
static const float EXPAND_FACTOR = 1.1f;
static const int SIGNAL_NUM = 1 << 10;

void Sample()
{
    static const char* TAG = "Sample";

    float distortion_degree = 0;

    if (further_times == 0)
    {
        sampling_freq = SIGNAL_NUM * freq_interval;
    }
    else
    {
        sampling_freq = fft_base_freq * 10 * EXPAND_FACTOR;//10 = 5 *2
    }
    freq_interval = sampling_freq / SIGNAL_NUM;

    ++further_times;
    ESP_LOGI(TAG, "%d times:freq interval:%d, sampling freq:%lu", further_times, freq_interval, sampling_freq);

    memset(result, 0x00, sizeof(uint8_t) * SIGNAL_NUM * 2);
    adc_value_p = (uint16_t*)result;
    ret = UART_Read_ADC(STM_UART_NUM, sampling_freq, result, SIGNAL_NUM);
    if (ret == ESP_OK) {
        FFT_Load_Data(adc_value_p, SIGNAL_NUM, &norms);
        // FFT_Hanning_Window();

        UARTHMI_Draw_Curve_addt(norms, SIGNAL_NUM);
        if (further_times == 2)
        {
            TCP_Send(norms);//TODO: change this param for better compatibility
        }

        FFT_Start();
        FFT_Get_Norms();

        fft_base_freq = FFT_Get_Accurate_Base_Freq(freq_interval);
        ESP_LOGI(TAG, "base freq:%lu", fft_base_freq);
        FFT_Get_Normalized_Amp(freq_amp_norm, HARMONIC_ORDER, freq_interval);
        for (int i = 0; i < HARMONIC_ORDER; ++i)
        {
            ESP_LOGI(TAG, "normalized amp %d: %3f", i, freq_amp_norm[i]);
        }

        if (further_times == final_times)
        {
            for (int i = 0; i < HARMONIC_ORDER; ++i)
            {
                UARTHMI_Send_Float(i, freq_amp_norm[i]);
                if (i > 0)
                {
                    distortion_degree += freq_amp_norm[i] * freq_amp_norm[i];
                }
            }
            UARTHMI_Send_Float(HARMONIC_ORDER, sqrtf(distortion_degree));

            TCP_Close();
        }
        FFT_Release_Data();
    }
    else{
        ESP_ERROR_CHECK(ESP_FAIL);
    }
}

void app_main(void)
{
    result = (uint8_t*)(uint16_t*)malloc(sizeof(uint16_t) * SIGNAL_NUM);
    sampling_freq = SIGNAL_NUM * freq_interval;
    
    UART_Init();
    wifi_sta_init(SIGNAL_NUM);
    TCP_Server_Start();
    
    while (1)
    {
        Sample();
        if (further_times >= final_times)
        {
            break;
        }
    }
    free(result);
    ESP_LOGI("Main", "Done! Did I did a good job?");
}
