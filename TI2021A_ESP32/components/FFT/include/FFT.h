#ifndef __FFT_H__
#define __FFT_H__

#ifdef __cplusplus
#include <complex>
#include <cmath>
using std::complex;

void FFT(complex<float>* f, int n);
// void FFT(complex<float>* f, complex<float>* F, int n);

#else
typedef struct
{
    float real;
    float imag;
}complex;

void FFT_Start();
void FFT_Load_Data(uint16_t* adc_value_p, int n, float** norm_p);
void FFT_Release_Data();
void FFT_Get_Norms();
void FFT_Hanning_Window();
int FFT_Get_Approximate_Base_Freq(int freq_interval);
int FFT_Get_Accurate_Base_Freq(int freq_interval);
void FFT_Get_Normalized_Amp(float* freq_amp_norm, uint8_t order, int freq_interval);

#endif // #ifdef __cplusplus

#endif
