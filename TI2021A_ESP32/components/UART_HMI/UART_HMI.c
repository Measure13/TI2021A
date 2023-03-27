#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/uart.h"
#include "esp_log.h"
#include "esp_err.h"

#include "UART_HMI.h"


#define EX_UART_NUM UART_NUM_2
#define BUF_SIZE (1024)
#define RD_BUF_SIZE (BUF_SIZE)
const int special_cmd_len = 4;

void UART_Init(void)
{
    uart_config_t uart_config = {
        .baud_rate = 9600,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };

    //Install UART driver.
    ESP_ERROR_CHECK(uart_driver_install(EX_UART_NUM, BUF_SIZE * 2, BUF_SIZE * 2, 0, NULL, 0));
    ESP_ERROR_CHECK(uart_param_config(EX_UART_NUM, &uart_config));

    //Set UART pins (using UART0 default pins ie no changes.)
    ESP_ERROR_CHECK(uart_set_pin(EX_UART_NUM, 17, 16, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

}

int UART_Read_Data(char* data_pointer)
{
    static const char* TAG = "UART Read";

    int read_data_size = 0;
    while (1) {
        // Read data from the UART
        read_data_size = uart_read_bytes(EX_UART_NUM, data_pointer, (BUF_SIZE - 1), 200 / portTICK_PERIOD_MS);
        if (read_data_size != -1) {
            if (read_data_size > 0)
            {
                data_pointer[read_data_size] = '\0';
                ESP_LOGI(TAG, "Recv str: %s", (char *) data_pointer);
                return read_data_size;
            }
            else{
                ESP_LOGE(TAG, "Time out!");
                ESP_ERROR_CHECK(ESP_FAIL);
            }
        }
    }
}

int UART_Write_Data(char* data_pointer, int data_len)
{
    static const char* TAG = "UART Write";

    int len = uart_write_bytes(EX_UART_NUM, (const char*) data_pointer, data_len);
    if (len == -1)
    {
        return ESP_FAIL;
        ESP_ERROR_CHECK(ESP_FAIL);
    }
    else{
        return len;
    }
}

static int UARTHMI_Append_Ending(char* dest)
{
    const int ending_len = 3;
    const char ending[3] = {0xFF, 0xFF, 0xFF};
    int dest_end_index = strlen(dest);

    for (int i = 0; i < ending_len; i++)
    {
        dest[dest_end_index++] = ending[i];
    }
    return dest_end_index;
}

static esp_err_t UARTHMI_Check_Return_Data(char* cmp, char* data_tmp_read)
{
    static const char* TAG = "UARTHMI_Check";
    if (!strncmp(cmp, (const char*)data_tmp_read, special_cmd_len))
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
    char START_TRANS[30] = "addt s0.id,0,";
    char numstr[7];
    const char ready[4] = {0xFE, 0xFF, 0xFF, 0xFF};
    const char drawing[4] = {0xFD, 0xFF, 0xFF, 0xFF};
    static const int MAX_SEND_LEN = 255;
    static const int MAX_SEND_DATA = 240;
    char* data_tmp_read = (char*)malloc(RD_BUF_SIZE + 1); // for read
    char data_tmp_write; //for write
    int i, data_len, total_num, send_num, interval_num;
    float max_vol = pf[0], coef = 0, min_vol = pf[0];

    total_num = num;
    interval_num = num / MAX_SEND_LEN;
    send_num = num / interval_num;

    //max num:256
    sprintf(numstr, "%d", send_num);
    strcat(START_TRANS, numstr);
    data_len = UARTHMI_Append_Ending(START_TRANS);
    data_len = UART_Write_Data(START_TRANS, data_len);
    ESP_LOGI(TAG, "write done, size:%d", data_len);
    data_len = UART_Read_Data(data_tmp_read);
    ESP_ERROR_CHECK(UARTHMI_Check_Return_Data(ready, data_tmp_read));
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
        UART_Write_Data(&data_tmp_write, 1);
    }
    ESP_LOGI(TAG, "Send done");
    data_len = UART_Read_Data(data_tmp_read);
    ESP_ERROR_CHECK(UARTHMI_Check_Return_Data(drawing, data_tmp_read));
}