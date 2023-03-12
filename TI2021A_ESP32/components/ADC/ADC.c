#include "ADC.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "esp_adc/adc_continuous.h"

// static esp_err_t ret;

static const int ADC_UNIT       = ADC_UNIT_1;
static const int ADC_ATTEN      = ADC_ATTEN_DB_11;
static const int ADC_BIT_WIDTH  = ADC_BITWIDTH_12;
static int n = 1 << 10;
static bool flag_done = false;

static adc_continuous_handle_cfg_t adc_cfg_handle;
static adc_continuous_handle_t handle = NULL;
// static TaskHandle_t* s_task_handle;
static adc_digi_output_data_t *p = NULL;
static adc_cali_handle_t cali_handle;

// static bool IRAM_ATTR s_conv_done_cb(adc_continuous_handle_t handle, const adc_continuous_evt_data_t *edata, void *user_data)
// {
//     BaseType_t mustYield = pdFALSE;
//     //Notify that ADC continuous driver has done enough number of conversions
//     vTaskNotifyGiveFromISR(*s_task_handle, &mustYield);

//     return (mustYield == pdTRUE);
// }

static bool ADC_Read_Done(adc_continuous_handle_t handle, const adc_continuous_evt_data_t *edata, void *user_data)
{
    flag_done = true;
    return (flag_done == false);
}

bool ADC_Get_Flag_Done()
{
    return flag_done;
}

void ADC_Set_Flag_Done()
{
    flag_done = false;
}

void ADC_Init(int freq, int num)
{
    const char* TAG = "ADCInit";

    n = num;
    
    adc_cfg_handle.max_store_buf_size = n * SOC_ADC_DIGI_RESULT_BYTES;
    adc_cfg_handle.conv_frame_size = n * SOC_ADC_DIGI_RESULT_BYTES;
    ESP_ERROR_CHECK(adc_continuous_new_handle(&adc_cfg_handle, &handle));
    ESP_LOGI(TAG, "max size:%lu, frame size:%lu", adc_cfg_handle.max_store_buf_size, adc_cfg_handle.conv_frame_size);

    adc_digi_pattern_config_t adc_pattern_cfg = {
        .atten = ADC_ATTEN,
        .channel = ADC_CHANNEL_7,
        .unit = ADC_UNIT,
        .bit_width = ADC_BIT_WIDTH,
    };
    adc_continuous_config_t adc_cfg = {
        .pattern_num = 1,
        .adc_pattern = &adc_pattern_cfg,
        .sample_freq_hz = freq,
        .conv_mode = ADC_CONV_SINGLE_UNIT_1,
        .format = ADC_DIGI_OUTPUT_FORMAT_TYPE1,
    };
    ESP_ERROR_CHECK(adc_continuous_config(handle, &adc_cfg));
    ESP_LOGI(TAG, "sample freq hz:%lu", adc_cfg.sample_freq_hz);

    //out_handle = &adc_cfg_handle;

    adc_continuous_evt_cbs_t cbs = {
        .on_conv_done = ADC_Read_Done,
    };
    ESP_ERROR_CHECK(adc_continuous_register_event_callbacks(handle, &cbs, NULL));
    ESP_LOGI(TAG, "s_conv_done_cb:Done");

    adc_cali_line_fitting_config_t cali_config = {
        .unit_id = ADC_UNIT,
        .atten = ADC_ATTEN,
        .bitwidth = ADC_BIT_WIDTH,
    };
    ESP_ERROR_CHECK(adc_cali_create_scheme_line_fitting(&cali_config, &cali_handle));
    ESP_LOGI(TAG, "unit:%d, attenuation:%d, bit width:%d", cali_config.unit_id, cali_config.atten, cali_config.bitwidth);
}

esp_err_t ADC_Deinit()
{
    ESP_ERROR_CHECK(adc_continuous_deinit(handle));
    ESP_LOGI("ADC", "ADC deinit done");
    return ESP_OK;
}

void ADC_Start()
{
    ESP_ERROR_CHECK(adc_continuous_start(handle));
    ESP_LOGI("ADC", "ADC Start done");
}

void ADC_Stop()
{
    ESP_ERROR_CHECK(adc_continuous_stop(handle));
    ESP_LOGI("ADC", "ADC stop done");
}

esp_err_t ADC_Read_Raw(int n, uint8_t* result, uint32_t* ret_num)
{
    return adc_continuous_read(handle, result, n * SOC_ADC_DIGI_RESULT_BYTES , ret_num, 0); //ret_num changed
}

esp_err_t ADC_Read_Cal(uint8_t* raw, int* result)
{
    p = (void*)raw;
    return adc_cali_raw_to_voltage(cali_handle, (int)(p->type1.data), result);
}