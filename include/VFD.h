#ifndef __VFD_H
#define __VFD_H

#include <Arduino.h>
#include <SPI.h>

// Define ALTERNATE_PINS to use non-standard GPIO pins for SPI bus
#ifdef ALTERNATE_PINS
  #define VSPI_MISO   2
  #define VSPI_MOSI   4
  #define VSPI_SCLK   0
  #define VSPI_SS     33

  #define HSPI_MISO   26
  #define HSPI_MOSI   27
  #define HSPI_SCLK   25
  #define HSPI_SS     32
#else
  #define VSPI_MISO   MISO
  #define VSPI_MOSI   MOSI
  #define VSPI_SCLK   SCK
  #define VSPI_SS     SS

  #define HSPI_MISO   12
  #define HSPI_MOSI   13
  #define HSPI_SCLK   14
  #define HSPI_SS     15
#endif

#if CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32S3
#define VSPI FSPI
#endif

#define VFD_EN_PIN 16
#define VFD_RESET_PIN 17
#define VFD_DIGITS 8
#define VFD_DIMMING 255

#define DCRAM_DATA_WRITE 0x20
#define DGRAM_DATA_CLAER 0x10
#define CGRAM_DATA_WRITE 0x40
#define SET_DISPLAY_TIMING 0xE0
#define SET_DIMMING_DATA 0xE4
#define SET_DISPLAT_LIGHT_ON 0xE8
#define SET_DISPLAT_LIGHT_OFF 0xEA
#define SET_STAND_BY_MODE 0xEC

typedef struct {
    uint8_t cmd;
    uint8_t data;
} VFD_cmd_t;

void SPI_Init();
void VFD_Init();
void VFD_Clear(char bit);
void VFD_Show_str(char bit, String str);
void VFD_Set_cmd(SPIClass *spi, byte cmd, byte data);
void VFD_On();
void VFD_Off();

#endif
