/**
 * @file    mpu6050.c
 * @brief   MPU6050 surucu implementasyonu - I2C okuma/yazma, WHO_AM_I
 *          dogrulama, ham veriyi fiziksel birime (m/s^2, deg/s) cevirme.
 */

#include "mpu6050.h"

#define I2C_TIMEOUT_MS   100
#define G_TO_MS2         9.80665f

HAL_StatusTypeDef MPU6050_Init(MPU6050_Handle_t *dev, I2C_HandleTypeDef *hi2c)
{
    dev->hi2c = hi2c;

    /* 1) Cihaz dogru adreste ve calisir durumda mi kontrol et */
    uint8_t who_am_i = 0;
    HAL_StatusTypeDef status = HAL_I2C_Mem_Read(
        dev->hi2c, MPU6050_I2C_ADDR, MPU6050_REG_WHO_AM_I,
        I2C_MEMADD_SIZE_8BIT, &who_am_i, 1, I2C_TIMEOUT_MS);
    if (status != HAL_OK || who_am_i != MPU6050_WHO_AM_I_VAL) {
        return HAL_ERROR;
    }

    /* 2) Sensoru uyandir (reset sonrasi varsayilan uyku modundadir) */
    uint8_t pwr_mgmt_1 = 0x00;
    status = HAL_I2C_Mem_Write(dev->hi2c, MPU6050_I2C_ADDR, MPU6050_REG_PWR_MGMT_1,
                                I2C_MEMADD_SIZE_8BIT, &pwr_mgmt_1, 1, I2C_TIMEOUT_MS);
    if (status != HAL_OK) return status;

    /* 3) Ivmeolcer +-4g, jiroskop +-500 dps araligi secildi */
    uint8_t accel_cfg = 0x08;  /* AFS_SEL=1 -> +-4g */
    status = HAL_I2C_Mem_Write(dev->hi2c, MPU6050_I2C_ADDR, MPU6050_REG_ACCEL_CONFIG,
                                I2C_MEMADD_SIZE_8BIT, &accel_cfg, 1, I2C_TIMEOUT_MS);
    if (status != HAL_OK) return status;

    uint8_t gyro_cfg = 0x08;   /* FS_SEL=1 -> +-500 dps */
    status = HAL_I2C_Mem_Write(dev->hi2c, MPU6050_I2C_ADDR, MPU6050_REG_GYRO_CONFIG,
                                I2C_MEMADD_SIZE_8BIT, &gyro_cfg, 1, I2C_TIMEOUT_MS);
    if (status != HAL_OK) return status;

    /* Secilen olcum araligina gore LSB -> fiziksel birim katsayilari
       (MPU6050 datasheet Register Map, bolum 4.4 / 4.5) */
    dev->accel_scale = (4.0f / 32768.0f) * G_TO_MS2;  /* +-4g -> m/s^2 */
    dev->gyro_scale  = (500.0f / 32768.0f);            /* +-500dps -> deg/s */

    return HAL_OK;
}

HAL_StatusTypeDef MPU6050_Read(MPU6050_Handle_t *dev, MPU6050_Data_t *out)
{
    uint8_t buf[14];
    HAL_StatusTypeDef status = HAL_I2C_Mem_Read(
        dev->hi2c, MPU6050_I2C_ADDR, MPU6050_REG_ACCEL_XOUT_H,
        I2C_MEMADD_SIZE_8BIT, buf, sizeof(buf), I2C_TIMEOUT_MS);
    if (status != HAL_OK) {
        return status;
    }

    int16_t raw_ax = (int16_t)((buf[0] << 8) | buf[1]);
    int16_t raw_ay = (int16_t)((buf[2] << 8) | buf[3]);
    int16_t raw_az = (int16_t)((buf[4] << 8) | buf[5]);
    int16_t raw_t  = (int16_t)((buf[6] << 8) | buf[7]);
    int16_t raw_gx = (int16_t)((buf[8] << 8) | buf[9]);
    int16_t raw_gy = (int16_t)((buf[10] << 8) | buf[11]);
    int16_t raw_gz = (int16_t)((buf[12] << 8) | buf[13]);

    out->ax = raw_ax * dev->accel_scale;
    out->ay = raw_ay * dev->accel_scale;
    out->az = raw_az * dev->accel_scale;
    out->gx = raw_gx * dev->gyro_scale;
    out->gy = raw_gy * dev->gyro_scale;
    out->gz = raw_gz * dev->gyro_scale;
    out->temp_c = (raw_t / 340.0f) + 36.53f;  /* datasheet sicaklik formulu */

    return HAL_OK;
}
