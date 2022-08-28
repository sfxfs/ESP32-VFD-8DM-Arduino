#include <VFD.h>

static const int spiClk = 5000000; // 5 MHz
SPIClass *vspi = NULL;
const VFD_cmd_t VFD_initcmd[] = {{SET_DISPLAY_TIMING, VFD_DIGITS - 1},
                                 {SET_DIMMING_DATA, VFD_DIMMING},
                                 {SET_DISPLAT_LIGHT_ON, 0x00}};

void SPI_Init()
{
    //初始化SPI
    vspi = new SPIClass(VSPI);
    vspi->begin();
    pinMode(vspi->pinSS(), OUTPUT); // VSPI SS

    //使能VFD
    pinMode(VFD_EN_PIN, OUTPUT);
    digitalWrite(VFD_EN_PIN, HIGH);

    //重置VFD
    pinMode(VFD_RESET_PIN, OUTPUT);
    digitalWrite(VFD_RESET_PIN, HIGH);
    delay(1);
    digitalWrite(VFD_RESET_PIN, LOW);
    delay(10);
    digitalWrite(VFD_RESET_PIN, HIGH);
    delay(3);
}

void VFD_Init()
{
    for (size_t i = 0; i < 3; i++)
    {
        VFD_Set_cmd(vspi, VFD_initcmd[i].cmd, VFD_initcmd[i].data);
    }
}

void VFD_Clear(char bit)
{
    if (bit = -1)
    {
        for (size_t i = 0; i < 8; i++)
        {
            VFD_Set_cmd(vspi, DCRAM_DATA_WRITE | i, DGRAM_DATA_CLAER);
        }
    }
    else
    {
        if (bit > -1 && bit < 8)
        {
            VFD_Set_cmd(vspi, DCRAM_DATA_WRITE | bit, DGRAM_DATA_CLAER);
        }
    }
}

void VFD_Show_str(char bit, String str)
{
    for (size_t i = 0; i < str.length(); i++)
    {
        VFD_Set_cmd(vspi, DCRAM_DATA_WRITE | bit, (int)str.charAt(i));
        if (bit < VFD_DIGITS - 1)
        {
            bit += 1;
        }
        else
        {
            break;
        }
    }
}

void VFD_On()
{
    VFD_Set_cmd(vspi, SET_STAND_BY_MODE, 0x00);
}

void VFD_Off()
{
    VFD_Set_cmd(vspi, SET_STAND_BY_MODE | 1, 0x00);
}

void VFD_Write_custdata()
{
    
}

void VFD_Set_cmd(SPIClass *spi, byte cmd, byte data)
{
    spi->beginTransaction(SPISettings(spiClk, LSBFIRST, SPI_MODE0));
    digitalWrite(spi->pinSS(), LOW); // pull SS slow to prep other end for transfer
    spi->transfer(cmd);
    spi->transfer(data);
    digitalWrite(spi->pinSS(), HIGH); // pull ss high to signify end of data transfer
    spi->endTransaction();
}
