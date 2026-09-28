/**
 * @file    bmp280.h
 * @brief   BMP280 barometrik basinc/sicaklik sensoru surucusu - I2C1
 *
 *          Kart semantigine bagimli degildir: MPU6050 ile ayni I2C1
 *          hattinda, farkli sabit adreste calisir.
 *
 * @author  Aysegul
 */

#ifndef BMP280_H
#define BMP280_H

#include "stm32f4xx_hal.h"
#include <stdint.h>

/* SDO pini GND'ye bagliysa 0x76, VCC'ye bagliysa 0x77 */
#define BMP280_I2C_ADDR       (0x76 << 1)

#define BMP280_REG_CHIP_ID    0xD0
#define BMP280_CHIP_ID_VAL    0x58

#define BMP280_REG_CALIB00    0x88   /* kalibrasyon katsayilari basi (24 bayt) */
#define BMP280_REG_CTRL_MEAS  0xF4
#define BMP280_REG_CONFIG     0xF5
#define BMP280_REG_PRESS_MSB  0xF7   /* basinc(3) + sicaklik(3) = 6 bayt */

typedef struct {
    uint16_t dig_T1;
    int16_t  dig_T2, dig_T3;
    uint16_t dig_P1;
    int16_t  dig_P2, dig_P3, dig_P4, dig_P5, dig_P6, dig_P7, dig_P8, dig_P9;
    int32_t  t_fine;  /* sicaklik-basinc kompanzasyonu arasi ortak deger */
} BMP280_Calib_t;

typedef struct {
    I2C_HandleTypeDef *hi2c;
    BMP280_Calib_t calib;
    float sea_level_hpa;
} BMP280_Handle_t;

HAL_StatusTypeDef BMP280_Init(BMP280_Handle_t *dev, I2C_HandleTypeDef *hi2c);
HAL_StatusTypeDef BMP280_ReadRaw(BMP280_Handle_t *dev, int32_t *raw_press, int32_t *raw_temp);
double BMP280_CompensateTemperature(BMP280_Handle_t *dev, int32_t raw_temp);
double BMP280_CompensatePressure(BMP280_Handle_t *dev, int32_t raw_press);
float  BMP280_ComputeAltitude(BMP280_Handle_t *dev, double pressure_pa);

#endif /* BMP280_H */
