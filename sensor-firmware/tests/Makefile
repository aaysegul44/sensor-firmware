# Sensor surucu matematiginin host (PC) uzerinde dogrulanmasi
# STM32 HAL'a gercekten bagli degildir; stubs/stm32f4xx_hal.h ile
# sadece tip uyumlulugu saglanir, gercek I2C hicbir zaman cagrilmaz.

CC = gcc
CFLAGS = -O2 -Wall -Istubs -I../Core/Inc

test_sensors: test_sensors.c ../Core/Src/bmp280.c ../Core/Inc/bmp280.h stubs/stm32f4xx_hal.h
	$(CC) $(CFLAGS) -o $@ test_sensors.c -lm

run: test_sensors
	./test_sensors

clean:
	rm -f test_sensors

.PHONY: run clean
