/**
 * @file    bmp280.c
 * @brief   BMP280 surucu implementasyonu.
 *
 *          Kompanzasyon formulleri Bosch Sensortec'in resmi BMP280
 *          datasheet'inde (BST-BMP280-DS001) yayinlanan cift hassasiyetli
 *          (double) referans formulasyonun birebir uygulamasidir.
 */

#include "bmp280.h"
#include <math.h>

#define I2C_TIMEOUT_MS   100

HAL_StatusTypeDef BMP280_Init(BMP280_Handle_t *dev, I2C_HandleTypeDef *hi2c)
{
    dev->hi2c = hi2c;
    dev->sea_level_hpa = 1013.25f;

    uint8_t chip_id = 0;
    HAL_StatusTypeDef status = HAL_I2C_Mem_Read(
        dev->hi2c, BMP280_I2C_ADDR, BMP280_REG_CHIP_ID,
        I2C_MEMADD_SIZE_8BIT, &chip_id, 1, I2C_TIMEOUT_MS);
    if (status != HAL_OK || chip_id != BMP280_CHIP_ID_VAL) {
        return HAL_ERROR;
    }

    uint8_t calib[24];
    status = HAL_I2C_Mem_Read(dev->hi2c, BMP280_I2C_ADDR, BMP280_REG_CALIB00,
                               I2C_MEMADD_SIZE_8BIT, calib, sizeof(calib), I2C_TIMEOUT_MS);
    if (status != HAL_OK) return status;

    dev->calib.dig_T1 = (uint16_t)(calib[1]  << 8 | calib[0]);
    dev->calib.dig_T2 = (int16_t) (calib[3]  << 8 | calib[2]);
    dev->calib.dig_T3 = (int16_t) (calib[5]  << 8 | calib[4]);
    dev->calib.dig_P1 = (uint16_t)(calib[7]  << 8 | calib[6]);
    dev->calib.dig_P2 = (int16_t) (calib[9]  << 8 | calib[8]);
    dev->calib.dig_P3 = (int16_t) (calib[11] << 8 | calib[10]);
    dev->calib.dig_P4 = (int16_t) (calib[13] << 8 | calib[12]);
    dev->calib.dig_P5 = (int16_t) (calib[15] << 8 | calib[14]);
    dev->calib.dig_P6 = (int16_t) (calib[17] << 8 | calib[16]);
    dev->calib.dig_P7 = (int16_t) (calib[19] << 8 | calib[18]);
    dev->calib.dig_P8 = (int16_t) (calib[21] << 8 | calib[20]);
    dev->calib.dig_P9 = (int16_t) (calib[23] << 8 | calib[22]);

    /* ctrl_meas: sicaklik x1, basinc x4 oversampling, normal mod */
    uint8_t ctrl_meas = 0x2B;
    status = HAL_I2C_Mem_Write(dev->hi2c, BMP280_I2C_ADDR, BMP280_REG_CTRL_MEAS,
                                I2C_MEMADD_SIZE_8BIT, &ctrl_meas, 1, I2C_TIMEOUT_MS);
    if (status != HAL_OK) return status;

    /* config: IIR filtre katsayisi 4, standby 0.5ms */
    uint8_t config = 0x04 << 2;
    status = HAL_I2C_Mem_Write(dev->hi2c, BMP280_I2C_ADDR, BMP280_REG_CONFIG,
                                I2C_MEMADD_SIZE_8BIT, &config, 1, I2C_TIMEOUT_MS);
    return status;
}

HAL_StatusTypeDef BMP280_ReadRaw(BMP280_Handle_t *dev, int32_t *raw_press, int32_t *raw_temp)
{
    uint8_t buf[6];
    HAL_StatusTypeDef status = HAL_I2C_Mem_Read(
        dev->hi2c, BMP280_I2C_ADDR, BMP280_REG_PRESS_MSB,
        I2C_MEMADD_SIZE_8BIT, buf, sizeof(buf), I2C_TIMEOUT_MS);
    if (status != HAL_OK) {
        return status;
    }

    *raw_press = ((int32_t)buf[0] << 12) | ((int32_t)buf[1] << 4) | (buf[2] >> 4);
    *raw_temp  = ((int32_t)buf[3] << 12) | ((int32_t)buf[4] << 4) | (buf[5] >> 4);
    return HAL_OK;
}

/* Bosch BST-BMP280-DS001, bolum 8.2 - cift hassasiyetli referans formul */
double BMP280_CompensateTemperature(BMP280_Handle_t *dev, int32_t raw_temp)
{
    double var1, var2, T;
    var1 = (((double)raw_temp) / 16384.0 - ((double)dev->calib.dig_T1) / 1024.0)
           * ((double)dev->calib.dig_T2);
    var2 = ((((double)raw_temp) / 131072.0 - ((double)dev->calib.dig_T1) / 8192.0)
           * (((double)raw_temp) / 131072.0 - ((double)dev->calib.dig_T1) / 8192.0))
           * ((double)dev->calib.dig_T3);
    dev->calib.t_fine = (int32_t)(var1 + var2);
    T = (var1 + var2) / 5120.0;
    return T;  /* Celsius */
}

double BMP280_CompensatePressure(BMP280_Handle_t *dev, int32_t raw_press)
{
    double var1, var2, p;
    var1 = ((double)dev->calib.t_fine / 2.0) - 64000.0;
    var2 = var1 * var1 * ((double)dev->calib.dig_P6) / 32768.0;
    var2 = var2 + var1 * ((double)dev->calib.dig_P5) * 2.0;
    var2 = (var2 / 4.0) + (((double)dev->calib.dig_P4) * 65536.0);
    var1 = (((double)dev->calib.dig_P3) * var1 * var1 / 524288.0
           + ((double)dev->calib.dig_P2) * var1) / 524288.0;
    var1 = (1.0 + var1 / 32768.0) * ((double)dev->calib.dig_P1);
    if (var1 == 0.0) {
        return 0;  /* sifira bolme koruma - fiziksel olarak gecersiz durum */
    }
    p = 1048576.0 - (double)raw_press;
    p = (p - (var2 / 4096.0)) * 6250.0 / var1;
    var1 = ((double)dev->calib.dig_P9) * p * p / 2147483648.0;
    var2 = p * ((double)dev->calib.dig_P8) / 32768.0;
    p = p + (var1 + var2 + ((double)dev->calib.dig_P7)) / 16.0;
    return p;  /* Pascal */
}

float BMP280_ComputeAltitude(BMP280_Handle_t *dev, double pressure_pa)
{
    double pressure_hpa = pressure_pa / 100.0;
    return (float)(44330.0 * (1.0 - pow(pressure_hpa / dev->sea_level_hpa, 0.190294957)));
}
