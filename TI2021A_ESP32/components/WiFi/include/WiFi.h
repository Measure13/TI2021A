#ifndef __WIFI_H__
#define __WIFI_H__

// #define AP__
#define STA__

#include "esp_err.h"
#include "FFT.h"

esp_err_t wifi_ap_init(int n);
esp_err_t wifi_sta_init(int n);
void TCP_Server_Start();
esp_err_t tcp_client_write();
void TCP_Close();
void TCP_Send(float* p);

#endif