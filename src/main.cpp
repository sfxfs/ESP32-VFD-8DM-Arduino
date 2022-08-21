#include <Arduino.h>
#include <SPI.h>

#define VSPI_MISO MISO
#define VSPI_MOSI MOSI
#define VSPI_SCLK SCK
#define VSPI_SS SS

void spiCommand(SPIClass *spi, byte data);

static const int spiClk = 5000000; // 1 MHz
SPIClass * vspi = NULL;

void setup()
{
  vspi = new SPIClass(VSPI);
  vspi->begin();
  pinMode(vspi->pinSS(), OUTPUT); //VSPI SS
}

void loop()
{
  spiCommand(vspi, 0b01010101);
}

void spiCommand(SPIClass *spi, byte data) {
  spi->beginTransaction(SPISettings(spiClk, LSBFIRST, SPI_MODE0));
  digitalWrite(spi->pinSS(), LOW); //pull SS slow to prep other end for transfer
  spi->transfer(data);
  digitalWrite(spi->pinSS(), HIGH); //pull ss high to signify end of data transfer
  spi->endTransaction();
}
