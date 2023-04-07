#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include "FFT.h"
#include "esp_log.h"
#include "esp_err.h"

static const float pi2 = 2 * acos(-1.0);
const int COMPLEX_SIZE = 8;
static complex omega0, omegank;
static complex* fft_data;
static float* norms;
static float dc_vol;
static float vol_sum = 0;
static int signal_num;
static int fft_base_freq = 0;
static int base_freq_index;
static int base_freq_upper_limit = 100 * 1000;

static const uint16_t VCC_VOL = 3290;
static const uint16_t MAX_16B = (1 << 16) - 1;

static complex cadd(complex c1, complex c2)
{
    static complex cr;
    cr.real = c1.real + c2.real;
    cr.imag = c1.imag + c2.imag;
    return cr;
}

static complex csub(complex c1, complex c2)
{
    static complex cr;
    cr.real = c1.real - c2.real;
    cr.imag = c1.imag - c2.imag;
    return cr;
}

static complex cmul(complex c1, complex c2)
{
    static complex cr;
    cr.real = c1.real * c2.real - c1.imag * c2.imag;
    cr.imag = c1.real * c2.imag + c1.imag * c2.real;
    return cr;
}

static float norm(complex c)
{
    return sqrtf(powf(c.real, 2.0f) + powf(c.imag, 2.0f));
}

void FFT_Load_Data(uint16_t* adc_value_p, int n, float** norm_p)
{
    signal_num = n;
    vol_sum = 0;
    fft_data = (complex*)malloc(COMPLEX_SIZE * signal_num);
    if (!fft_data)ESP_ERROR_CHECK(ESP_ERR_NO_MEM);
    norms = (float*)malloc(sizeof(float) * signal_num);
    if (!norms)ESP_ERROR_CHECK(ESP_ERR_NO_MEM);
    *norm_p = norms;
    for (int i = 0 ; i < signal_num; ++i)
    {
        norms[i] = VCC_VOL * adc_value_p[i] / MAX_16B;
        fft_data[i].imag = 0;
        fft_data[i].real = norms[i];
        vol_sum += norms[i];
    }
    dc_vol = vol_sum / signal_num;
    for (int i = 0; i < signal_num; ++i)
    {
        fft_data[i].real -= dc_vol;
        norms[i] -= dc_vol;
    }
}

void FFT_Release_Data()
{
    ESP_LOGI("FFT Release", "OK");
    free(fft_data);
    free(norms);
}

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
    omegank.imag = -sin(pi2 / n);
    for (int i = 0; i < mid; ++i, omega0 = cmul(omega0, omegank))
    {
        f[i] = cadd(f1[i], cmul(omega0, f2[i]));
        f[i + n / 2] = csub(f1[i], cmul(omega0, f2[i]));
    }
    free(f1);
    free(f2);
}

void FFT_Start()
{
    FFT(fft_data, signal_num);
}

void FFT_Get_Norms()
{
    for (int i = 0; i < signal_num; ++i)
    {
        norms[i] = norm(fft_data[i]);
    }
}

void FFT_Hanning_Window()
{
    for (int i = 0; i < signal_num; ++i)
    {
        fft_data[i].real *= (1 - cosf(pi2 * i / signal_num)) / 2;
    }
}

static void FFT_Base_Freq_Upper_Limit_Adjust(int freq_interval)
{
    if (((100 * 1000 / freq_interval) + 1) < (signal_num / 2))
    {
        base_freq_upper_limit = (100 * 1000 / freq_interval) + 1;
    }
    else
    {
        base_freq_upper_limit = (signal_num / 2);
    }
}

int FFT_Get_Approximate_Base_Freq(int freq_interval)
{
    float normmax, tempnorm;
    base_freq_index = 0;
    normmax = 0;
    FFT_Base_Freq_Upper_Limit_Adjust(freq_interval);
    for (int i = 0; i < base_freq_upper_limit; ++i) //upper limit is 100kHz
    {
        tempnorm = norms[i];
        if (normmax < tempnorm)
        {
            normmax = tempnorm;
            base_freq_index = i;
        }
    }
    fft_base_freq = base_freq_index * freq_interval;
    return fft_base_freq;
}

int FFT_Get_Accurate_Base_Freq(int freq_interval)
{
    static const char* TAG = "Accurate";

    float normmax, tempnorm, tempnorm_sum;
    base_freq_index = 0;
    bool left = true, right = true;
    normmax = 0.0f;
    FFT_Base_Freq_Upper_Limit_Adjust(freq_interval);
    for (int i = 0; i < base_freq_upper_limit; ++i) //upper limit is 100kHz
    {
        if (normmax < norms[i])
        {
            normmax = norms[i];
            base_freq_index = i;
        }
    }
    ESP_LOGI(TAG, "max norm:%1f, index:%d", normmax, base_freq_index);
    tempnorm = normmax * base_freq_index;
    tempnorm_sum = normmax;
    for (int i = 1; ;++i)
    {
        ESP_LOGI(TAG, "search range:%d to %d", base_freq_index - i, base_freq_index + i);
        if ((norms[base_freq_index - i] > (normmax / 2)) && left)
        {
            tempnorm += norms[base_freq_index - i] * (base_freq_index - i);
            tempnorm_sum += norms[base_freq_index - i];
        }
        else
        {
            left = false;
        }
        if ((norms[base_freq_index + i] > (normmax / 2)) && right)
        {
            if (i == (signal_num / 2 - base_freq_index - 1))
            {
                break;
            }
            tempnorm += norms[base_freq_index + i] * (base_freq_index + i);
            tempnorm_sum += norms[base_freq_index + i];
        }
        else
        {
            right = false;
        }
        if ((!left) && (!right))
        {
            break;
        }
    }
    fft_base_freq = (int)(tempnorm / tempnorm_sum * freq_interval + 0.5);
    return fft_base_freq; //plus 0.5 to round the decimal
}

void FFT_Get_Normalized_Amp(float* freq_amp_norm, uint8_t order, int freq_interval)
{
    bool left = true, right = true;
    int real_index = 0, temp_index;
    float temp_norm_max = 0.0f;
    float temp_norm_square_sum = 0.0f;
    for (int i = 0; i < order; ++i)
    {
        left = true;
        right = true;
        real_index = base_freq_index * (i + 1);
        temp_index = real_index;
        temp_norm_max = norms[real_index];
        // find the maximum
        for (int j = real_index - 3; j > real_index + 3; ++j)
        {
            if (norms[j] > temp_norm_max)
            {
                temp_index = j;
                temp_norm_max = norms[j];
            }
        }
        if (real_index != temp_index)
        {
            ESP_LOGI("Normalize", "real:%d vs temp:%d", real_index, temp_index);
        }
        real_index = temp_index;
        temp_norm_square_sum = temp_norm_max * temp_norm_max;
        // sum up the point with 20% norms
        for (int j = 1; j <= 3; ++j)
        {
            if ((norms[real_index - j] > (temp_norm_max / 5)) && left)
            {
                temp_norm_square_sum += norms[real_index - j] * norms[real_index - j];
            }
            else
            {
                left = false;
            }
            if ((norms[real_index + j] > (temp_norm_max / 5)) && right)
            {
                temp_norm_square_sum += norms[real_index + j] * norms[real_index + j];
            }
            else
            {
                right = false;
            }
            if ((!left) && (!right))
            {
                break;
            }
        }
        freq_amp_norm[i] = sqrtf(temp_norm_square_sum);
    }
    for (int i = order - 1; i >= 0; --i)
    {
        freq_amp_norm[i] /= freq_amp_norm[0];
    }
}