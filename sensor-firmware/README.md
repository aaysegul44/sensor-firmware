# Sensör Sürücüleri — MPU6050 (IMU) + BMP280 (Barometre)

TUSMEC Aday Mühendis Programı 2026 — Track 2 (Gömülü & Donanım) internship
kapsamında yazılan, **I2C1 hattı üzerinden çalışan iki sensör sürücüsü**.

## Projenin Kapsamı ve Durumu

Ekibin özel uçuş kontrol kartının (STM32F427VIT6) şematiği henüz
tamamlanmamıştır. Bu nedenle bu iki sürücü, kartın son pin yerleşimine
bağımlı olmayacak şekilde, **standart I2C1 hattı** (STM32 Nucleo/Discovery
kartlarında varsayılan PB6=SCL, PB7=SDA) üzerinden, yaygın ve ucuz breakout
modülleriyle (MPU6050, BMP280) herhangi bir geliştirme kartında test
edilebilecek şekilde yazılmıştır. Kart üretimi tamamlandığında aynı
sürücüler, sadece I2C handle'ı değiştirilerek doğrudan kullanılabilir.

Bu, **tam bitmiş bir proje değil**; staj sürecinin bu aşamasında
tamamlanan, sensör verisini ham register değerinden anlamlı fiziksel
büyüklüklere (m/s², deg/s, °C, Pa, metre) çeviren temel sürücü katmanıdır.

## İçerik

| Sensör | Arayüz | Dosya |
|---|---|---|
| MPU6050 (ivmeölçer + jiroskop) | I2C1, adres 0x68 | `Core/Src/mpu6050.c` |
| BMP280 (basınç + sıcaklık) | I2C1, adres 0x76 | `Core/Src/bmp280.c` |

## Host Doğrulama Testi

STM32 donanımı olmadan, sürücülerin matematiksel doğruluğunu PC üzerinde
kanıtlayan bağımsız bir test:

```bash
cd tests
make run
```

**Test 1 (BMP280):** Bosch Sensortec'in resmi BMP280 datasheet'inde
(BST-BMP280-DS001) yayınlanan örnek kalibrasyon katsayıları ve ham ADC
değerleri kullanılarak kompanzasyon fonksiyonu çalıştırıldı. Sonuç
**25.08°C** çıktı — bu, datasheet'teki referans örnek sonucuyla birebir
örtüşüyor ve kompanzasyon formülünün doğru uygulandığını kanıtlıyor.

**Test 2 (MPU6050):** Seçilen ölçüm aralıklarına (±4g, ±500°/s) göre
hesaplanan ölçek katsayılarının, bilinen referans noktalarını (1g =
9.80665 m/s²) doğru ürettiği doğrulandı.

```
=== Test 1: BMP280 Kompanzasyon Formulu (Bosch referans veri) ===
  Hesaplanan sicaklik : 25.08 C
  Hesaplanan basinc   : 100653.26 Pa (1006.53 hPa)
  [OK] Sicaklik makul aralikta (20-30 C)
  [OK] Basinc makul aralikta (~1006 hPa)

=== Test 2: MPU6050 Olcek Donusumu ===
  [OK] +-4g araliginda 1g -> m/s^2 donusumu
  [OK] +-500dps araliginda tam skala donusumu

[TUM TESTLER BASARILI]
```

## Dizin Yapısı

```
Core/
  Inc/    - header dosyalari (mpu6050.h, bmp280.h)
  Src/    - implementasyon dosyalari (mpu6050.c, bmp280.c)
tests/
  test_sensors.c        - host (PC) dogrulama testi
  stubs/stm32f4xx_hal.h - HAL'in tip-uyumlu taklidi (gercek I2C yapmaz)
  Makefile
```

> Not: `Core/Inc` ve `Core/Src` altındaki dosyalar STM32CubeIDE projesine
> eklenmek üzere gerçek HAL fonksiyonlarını (`HAL_I2C_Mem_Read` vb.)
> kullanır. `tests/` klasöründeki host testi ise bu HAL çağrılarını hiç
> yapmadan, sadece register-değerinden-fiziksel-birime dönüşüm
> matematiğini (`BMP280_CompensateTemperature`, `BMP280_CompensatePressure`
> ve MPU6050 ölçek katsayıları) doğrular.

## Yazar

Ayşegül — Bilgisayar Mühendisliği, TUSMEC AMP-2026 Track 2 stajyeri
