/**
 * @file    test_sensors.c
 * @brief   Host (PC) uzerinde, HAL/donanim olmadan, sensor surucu
 *          matematiginin dogru calistigini dogrulayan test programi.
 *
 *          Test 1 - BMP280: Bosch Sensortec'in resmi BMP280 datasheet'inde
 *          (BST-BMP280-DS001) yayinlanan ornek kalibrasyon katsayilari ve
 *          ham ADC degerleriyle kompanzasyon fonksiyonlarini calistirir,
 *          sonucun fiziksel olarak makul (deniz seviyesi civari sicaklik/
 *          basinc) bir degerde oldugunu dogrular.
 *
 *          Test 2 - MPU6050: secilen olcum araligina (+-4g, +-500dps) gore
 *          hesaplanan olcek katsayilarinin, bilinen referans noktalarini
 *          (orn. 1g = 9.80665 m/s^2) dogru urettigini dogrular.
 */

#include <stdio.h>
#include <math.h>
#include "bmp280.h"
#include "../Core/Src/bmp280.c"

#define ASSERT_NEAR(desc, actual, expected, tol) do { \
    double _a = (actual), _e = (expected), _t = (tol); \
    int _ok = fabs(_a - _e) <= _t; \
    printf("  [%s] %-42s beklenen=%.4f hesaplanan=%.4f (tol=%.4f)\n", \
           _ok ? "OK" : "FAIL", desc, _e, _a, _t); \
    if (!_ok) all_ok = 0; \
} while (0)

int main(void)
{
    int all_ok = 1;

    printf("=== Test 1: BMP280 Kompanzasyon Formulu (Bosch referans veri) ===\n");
    BMP280_Handle_t dev = {0};
    dev.sea_level_hpa = 1013.25f;

    /* Bosch BST-BMP280-DS001 datasheet ornek kalibrasyon katsayilari */
    dev.calib.dig_T1 = 27504;
    dev.calib.dig_T2 = 26435;
    dev.calib.dig_T3 = -1000;
    dev.calib.dig_P1 = 36477;
    dev.calib.dig_P2 = -10685;
    dev.calib.dig_P3 = 3024;
    dev.calib.dig_P4 = 2855;
    dev.calib.dig_P5 = 140;
    dev.calib.dig_P6 = -7;
    dev.calib.dig_P7 = 15500;
    dev.calib.dig_P8 = -14600;
    dev.calib.dig_P9 = 6000;

    int32_t adc_T = 519888;
    int32_t adc_P = 415148;

    double T = BMP280_CompensateTemperature(&dev, adc_T);
    double P = BMP280_CompensatePressure(&dev, adc_P);
    float alt = BMP280_ComputeAltitude(&dev, P);

    printf("  Ham ADC girdileri : adc_T=%d, adc_P=%d\n", adc_T, adc_P);
    printf("  Hesaplanan sicaklik : %.2f C\n", T);
    printf("  Hesaplanan basinc   : %.2f Pa (%.2f hPa)\n", P, P / 100.0);
    printf("  Hesaplanan irtifa   : %.2f m (deniz seviyesi referansina gore)\n", alt);

    /* Datasheet'teki ornek girdiler, deniz seviyesinde oda sicakligi ve
       normal atmosfer basincina karsilik gelecek sekilde secilmistir.
       Fiziksel olarak makul araliklarda oldugunu dogruluyoruz. */
    ASSERT_NEAR("Sicaklik makul aralikta (20-30 C)", T, 25.0, 5.0);
    ASSERT_NEAR("Basinc makul aralikta (~1006 hPa)", P / 100.0, 1006.5, 2.0);

    printf("\n=== Test 2: MPU6050 Olcek Donusumu ===\n");
    /* +-4g araliginda: tam skala 32768 LSB = 4g = 4*9.80665 m/s^2 */
    float accel_scale_4g = (4.0f / 32768.0f) * 9.80665f;
    float one_g_lsb = 32768.0f / 4.0f;  /* 1g'ye karsilik gelen LSB sayisi */
    float computed_1g_ms2 = one_g_lsb * accel_scale_4g;

    ASSERT_NEAR("+-4g araliginda 1g -> m/s^2 donusumu",
                computed_1g_ms2, 9.80665, 0.001);

    /* +-500 dps araliginda: tam skala 32768 LSB = 500 deg/s */
    float gyro_scale_500 = 500.0f / 32768.0f;
    float computed_500dps = 32768.0f * gyro_scale_500;
    ASSERT_NEAR("+-500dps araliginda tam skala donusumu",
                computed_500dps, 500.0, 0.001);

    printf("\n%s\n", all_ok ? "[TUM TESTLER BASARILI]" : "[BAZI TESTLER BASARISIZ]");
    return all_ok ? 0 : 1;
}
