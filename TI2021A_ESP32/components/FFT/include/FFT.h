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

void FFT(complex* f, int n);

complex cadd(complex c1, complex c2);
complex csub(complex c1, complex c2);
complex cmul(complex c1, complex c2);
float norm(complex c);
void FFT_Get_Norms(complex* cp, float* fp, uint16_t num);
void FFT_Hanning_Window(complex* cp, uint16_t num);

#endif // #ifdef __cplusplus

#endif
