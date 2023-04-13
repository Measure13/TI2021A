#include <stdio.h>
#include <string.h>
#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "esp_log.h"
#include "esp_err.h"

#include "UART_HMI.h"

#define BUF_SIZE 2048
const int special_cmd_len = 4;

void UART_Init(void)
{
    uart_config_t uart_config_hmi = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };

    //Install UART driver.
    ESP_ERROR_CHECK(uart_driver_install(HMI_UART_NUM, BUF_SIZE * 2, BUF_SIZE * 2, 0, NULL, 0));
    ESP_ERROR_CHECK(uart_param_config(HMI_UART_NUM, &uart_config_hmi));

    //Set UART pins (using UART0 default pins ie no changes.)
    ESP_ERROR_CHECK(uart_set_pin(HMI_UART_NUM, 17, 16, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

    uart_config_t uart_config_stm = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };

    //Install UART driver.
    ESP_ERROR_CHECK(uart_driver_install(STM_UART_NUM, BUF_SIZE * 2, BUF_SIZE * 2, 0, NULL, 0));
    ESP_ERROR_CHECK(uart_param_config(STM_UART_NUM, &uart_config_stm));

    //Set UART pins (using UART0 default pins ie no changes.)
    ESP_ERROR_CHECK(uart_set_pin(STM_UART_NUM, 25, 26, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
}

int UART_Write_Data(uint8_t uart_num, uint8_t* data_pointer, int data_len)
{
    static const char* TAG = "UART Write";
    int len;

    while (1)
    {
        len = uart_write_bytes(uart_num, (const uint8_t*) data_pointer, data_len);
        if (len == -1)
        {
            ESP_LOGE(TAG, "Fail to write");
            ESP_ERROR_CHECK(ESP_FAIL);
        }
        else{
            return len;
        }
    }
}

int UART_Read_Data(uint8_t uart_num, uint8_t* data_pointer)
{
    static const char* TAG = "UART Read";

    int read_data_size = 0;
    while (1) {
        // Read data from the UART
        read_data_size = uart_read_bytes(uart_num, data_pointer, BUF_SIZE + 1, 5000 / portTICK_PERIOD_MS);
        if (read_data_size != -1) {
            if (read_data_size > 0)
            {
                // data_pointer[read_data_size] = '\0';
                // ESP_LOGI(TAG, "Recv str: %s", (uint8_t *) data_pointer);
                return read_data_size;
            }
            else{
                ESP_LOGE(TAG, "Time out!");
                return -1;
            }
        }
    }
}

static int UARTHMI_Append_Ending(uint8_t* dest)
{
    const int ending_len = 3;
    const uint8_t ending[3] = {0xFF, 0xFF, 0xFF};
    int dest_end_index = strlen((char*)dest);

    for (int i = 0; i < ending_len; i++)
    {
        dest[dest_end_index++] = ending[i];
    }
    return dest_end_index;
}

static esp_err_t UARTHMI_Check_Return_Data(const uint8_t* cmp, uint8_t* data_tmp_read)
{
    static const char* TAG = "UARTHMI_Check";
    if (!strncmp((const char*)cmp, (const char*)data_tmp_read, special_cmd_len))
    {
        ESP_LOGI(TAG, "Check OK!");
        return ESP_OK;
    }
    else{
        ESP_LOGE(TAG, "%x %x %x %x", data_tmp_read[0], data_tmp_read[1], data_tmp_read[2], data_tmp_read[3]);
        return ESP_FAIL;
    }
}

void UARTHMI_Draw_Curve_addt(float* pf, uint16_t num)
{
    static const char* TAG = "UART_Draw";
    uint8_t START_TRANS[30] = "addt s0.id,0,";
    uint8_t numstr[7];
    const uint8_t ready[4] = {0xFE, 0xFF, 0xFF, 0xFF};
    const uint8_t drawing[4] = {0xFD, 0xFF, 0xFF, 0xFF};
    static const int MAX_SEND_LEN = 256;
    static const int MAX_SEND_DATA = 240;
    uint8_t* data_tmp_read = (uint8_t*)malloc(BUF_SIZE + 1); // for read
    uint8_t data_tmp_write; //for write
    int i, data_len, total_num, send_num, interval_num, temp_data_len = -1;
    float max_vol = pf[0], coef = 0, min_vol = pf[0];
    esp_err_t err_rcv = ESP_FAIL;

    total_num = num;
    interval_num = num / MAX_SEND_LEN;
    send_num = num / interval_num;

    //max num:256
    sprintf((char*)numstr, "%d", send_num);
    strcat((char*)START_TRANS, (const char*)numstr);
    data_len = UARTHMI_Append_Ending(START_TRANS);
    while ((temp_data_len == -1) || (err_rcv != ESP_OK))
    {
        data_len = UART_Write_Data(HMI_UART_NUM, START_TRANS, data_len);
        ESP_LOGI(TAG, "write done, size:%d", data_len);
        temp_data_len = UART_Read_Data(HMI_UART_NUM, data_tmp_read);
        err_rcv = UARTHMI_Check_Return_Data(ready, data_tmp_read);
    }
    ESP_LOGI(TAG, "start send points:%d", send_num);

    for (i = 0; i < total_num; i += interval_num)
    {
        if (max_vol < pf[i])
        {
            max_vol = pf[i];
        }
        if (min_vol > pf[i])
        {
            min_vol = pf[i];
        }
    }
    coef = MAX_SEND_DATA / max_vol;
    for (i = 0; i < total_num; i += interval_num)
    {
        data_tmp_write = (uint8_t)((pf[i] - min_vol) * coef);
        UART_Write_Data(HMI_UART_NUM, &data_tmp_write, 1);
    }
    ESP_LOGI(TAG, "Send done");
    data_len = UART_Read_Data(HMI_UART_NUM, data_tmp_read);
    ESP_ERROR_CHECK(UARTHMI_Check_Return_Data(drawing, data_tmp_read));
    free(data_tmp_read);
}

static uint8_t UARTHMI_Get_Integer_Digits(int integer)
{
    int temp;
    uint8_t cnt = 0;
    if (integer > 0)
    {
        temp = integer;
        while (temp)
        {
            temp /= 10;
            ++cnt;
        }
        return cnt;
    }
    else if (integer == 0)
    {
        return 0;
    }
    else
    {
        //can't handle negative number, return 0 for convenience.
        return 0;
    }
}

static int UARTHMI_Float_Adjust(float float_num, uint8_t digits_for_integer, uint8_t digits_for_decimals)
{
    uint8_t integer_len = UARTHMI_Get_Integer_Digits((int)float_num);
    int adjusted_float = 0;
    integer_len = digits_for_integer + digits_for_decimals - integer_len;
    float_num *= powf(10.0f, integer_len - 1);//must minus 1 to get correct result
    adjusted_float = (int)float_num;
    return adjusted_float;
}

static void UARTHMI_Set_Float(int index, float float_num, uint8_t digits_for_integer, uint8_t digits_for_decimals)
{
    static const char* TAG = "Set Float";

    uint8_t data_len;
    uint8_t* send_str;
    uint8_t len = UARTHMI_Get_Integer_Digits(index) + 11 + digits_for_integer + digits_for_decimals;
    send_str = (uint8_t*)malloc(sizeof(uint8_t) * (len));
    if (!send_str)ESP_ERROR_CHECK(ESP_ERR_NO_MEM);
    memset(send_str, 0, sizeof(uint8_t) * (len));
    sprintf((char*)send_str, "x%d.val=%d", index, UARTHMI_Float_Adjust(float_num, digits_for_integer, digits_for_decimals));
    data_len = UARTHMI_Append_Ending(send_str);
    data_len = UART_Write_Data(HMI_UART_NUM, send_str, data_len);
    ESP_LOGI(TAG, "write done, size:%d", data_len);
    free(send_str);
}

void UARTHMI_Send_Float(int index, float float_num)
{
    if (index == 0)
    {
        //do nothing because U_0 is always 1
    }
    else
    {
        UARTHMI_Set_Float(index, float_num, 1, 2);
    }
}

esp_err_t UART_Read_ADC(uint8_t uart_num, uint32_t adc_freq, uint8_t* data_pointer, uint16_t data_len)
{
    static const char* TAG = "UART_ADC";
    uint32_t temp_freq = adc_freq;
    uint8_t temp[7];
    int size = -1;
    memset(temp, 0x00, sizeof(temp));
    for (int i = 0; i < 7; ++i)
    {
        if (i < 4)
        {
            temp[i] = (temp_freq >> (8 * i)) & 0xff;
        }
        else
        {
            temp[i] = 0xff;
        }
    }
    while (size == -1)
    {
        UART_Write_Data(STM_UART_NUM, temp, 7);
        size = UART_Read_Data(STM_UART_NUM, (uint8_t*)data_pointer);
    }
    if (size == data_len * 2)
    {
        return ESP_OK;
    }
    else
    {
        ESP_LOGE(TAG, "real adc size:%d", size);
        return ESP_FAIL;
    }
}
