#include <stdio.h>
#include <math.h>
#include "FFT.h"

static const float pi2 = 2 * acos(-1.0);
const int COMPLEX_SIZE = 8;
static complex omega0, omegank;

void FFT(complex* f, int n)
{
    if (n == 1)
    {
        return;
    }
    int mid = n / 2;
    complex* f1, *f2;
    f1 = (complex*)malloc(COMPLEX_SIZE * mid);
    f2 = (complex*)malloc(COMPLEX_SIZE * mid);
    for (int i = 0; i < n; i += 2)
    {
        f1[i / 2] = f[i];
        f2[i / 2] = f[i + 1];
    }
    FFT(f1, mid);
    FFT(f2, mid);
    omega0.real = 1;
    omega0.imag = 0;
    omegank.real = cos(pi2 / n);
    omegank.imag = sin(pi2 / n);
    for (int i = 0; i < mid; ++i, omega0 = cmul(omega0, omegank))
    {
        f[i] = cadd(f1[i], cmul(omega0, f2[i]));
        f[i + n / 2] = csub(f1[i], cmul(omega0, f2[i]));
    }
    free(f1);
    free(f2);
}

complex cadd(complex c1, complex c2)
{
    static complex cr;
    cr.real = c1.real + c2.real;
    cr.imag = c1.imag + c2.imag;
    return cr;
}

complex csub(complex c1, complex c2)
{
    static complex cr;
    cr.real = c1.real - c2.real;
    cr.imag = c1.imag - c2.imag;
    return cr;
}

complex cmul(complex c1, complex c2)
{
    static complex cr;
    cr.real = c1.real * c2.real - c1.imag * c2.imag;
    cr.imag = c1.real * c2.imag + c1.imag * c2.real;
    return cr;
}

float norm(complex c)
{
    return sqrtf(powf(c.real, 2.0f) + powf(c.imag, 2.0f));
}

void FFT_Get_Norms(complex* cp, float* fp, uint16_t num)
{
    for (int i = 0; i < num; ++i)
    {
        fp[i] = norm(cp[i]);
    }
}

void FFT_Hanning_Window(complex* cp, uint16_t num)
{
    for (int i = 0; i < num; ++i)
    {
        cp[i].real *= (1 - cosf(pi2 * i / num)) / 2;
    }
}