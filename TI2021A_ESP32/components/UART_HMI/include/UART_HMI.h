#ifndef __UART_HMI_H__
#define __UART_HMI_H__

void UART_Init(void);
void UARTHMI_Draw_Curve_addt(float* pf, uint16_t num);
int UART_Write_Data(char* data_pointer, int data_len);
int UART_Read_Data(char* data_pointer);

#endif