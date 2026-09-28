/**
 * @file    mpu6050.h
 * @brief   MPU6050 IMU (ivmeolcer + jiroskop) surucusu - I2C1
 *
 *          Kart semantigine bagimli degildir: standart I2C1 hatti
 *          (STM32 Nucleo/Discovery kartlarinda varsayilan olarak
 *          PB6=SCL, PB7=SDA) uzerinden calisir, ozel kart uretimi
 *          beklenmeden herhangi bir gelistirme kartinda test edilebilir.
 *
 * @author  Aysegul
 */

#ifndef MPU6050_H
#define MPU6050_H

#include "stm32f4xx_hal.h"
#include <stdint.h>

/* AD0 pini GND'ye bagliysa 0x68, VCC'ye bagliysa 0x69 (7-bit adres << 1) */
#define MPU6050_I2C_ADDR      (0x68 << 1)

#define MPU6050_REG_WHO_AM_I     0x75
#define MPU6050_WHO_AM_I_VAL     0x68

#define MPU6050_REG_PWR_MGMT_1   0x6B
#define MPU6050_REG_ACCEL_CONFIG 0x1C
#define MPU6050_REG_GYRO_CONFIG  0x1B
#define MPU6050_REG_ACCEL_XOUT_H 0x3B  /* 14 baytlik blok: accel(6)+temp(2)+gyro(6) */

typedef struct {
    float ax, ay, az;   /* m/s^2 */
    float gx, gy, gz;   /* deg/s */
    float temp_c;
} MPU6050_Data_t;

typedef struct {
    I2C_HandleTypeDef *hi2c;
    float accel_scale;  /* LSB -> m/s^2 */
    float gyro_scale;   /* LSB -> deg/s */
} MPU6050_Handle_t;

HAL_StatusTypeDef MPU6050_Init(MPU6050_Handle_t *dev, I2C_HandleTypeDef *hi2c);
HAL_StatusTypeDef MPU6050_Read(MPU6050_Handle_t *dev, MPU6050_Data_t *out);

#endif /* MPU6050_H */
