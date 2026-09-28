/**
 * Host (PC) testleri icin minimal STM32 HAL taklidi (stub).
 * Gercek donanimda bu dosya yerine ST'nin kendi HAL kutuphanesi kullanilir.
 * Sadece derleme tipi uyumlulugu icin gerekli tanimlari icerir; gercek
 * I2C/SPI islemi yapmaz. Boylece surucu dosyalari (.c) donanim olmadan
 * PC'de derlenip, salt matematik/parse fonksiyonlari test edilebilir.
 */
#ifndef STM32F4XX_HAL_STUB_H
#define STM32F4XX_HAL_STUB_H

#include <stdint.h>
#include <stddef.h>

typedef enum { HAL_OK = 0, HAL_ERROR = 1, HAL_BUSY = 2, HAL_TIMEOUT = 3 } HAL_StatusTypeDef;
typedef enum { I2C_MEMADD_SIZE_8BIT = 1, I2C_MEMADD_SIZE_16BIT = 2 } I2C_MemAddSize_t;
#define I2C_MEMADD_SIZE_8BIT  1
#define I2C_MEMADD_SIZE_16BIT 2

typedef struct { int dummy; } I2C_HandleTypeDef;
typedef struct { int dummy; } SPI_HandleTypeDef;
typedef struct { int dummy; } UART_HandleTypeDef;
typedef struct { int dummy; } GPIO_TypeDef;

#define GPIO_PIN_RESET 0
#define GPIO_PIN_SET   1

static inline HAL_StatusTypeDef HAL_I2C_Mem_Read(I2C_HandleTypeDef *h, uint16_t addr,
    uint16_t reg, uint16_t sz, uint8_t *buf, uint16_t len, uint32_t timeout)
{ (void)h;(void)addr;(void)reg;(void)sz;(void)buf;(void)len;(void)timeout; return HAL_ERROR; }

static inline HAL_StatusTypeDef HAL_I2C_Mem_Write(I2C_HandleTypeDef *h, uint16_t addr,
    uint16_t reg, uint16_t sz, uint8_t *buf, uint16_t len, uint32_t timeout)
{ (void)h;(void)addr;(void)reg;(void)sz;(void)buf;(void)len;(void)timeout; return HAL_ERROR; }

#endif
