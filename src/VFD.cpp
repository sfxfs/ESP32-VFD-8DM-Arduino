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
    SPI_Init();
    for (size_t i = 0; i < 3; i++)
    {
        VFD_Set_cmd(VFD_initcmd[i].cmd, VFD_initcmd[i].data);
    }
}

void VFD_Clear(char bit)
{
    if (bit = -1)
    {
        for (size_t i = 0; i < 8; i++)
        {
            VFD_Set_cmd(DCRAM_DATA_WRITE | i, DGRAM_DATA_CLAER);
        }
    }
    else
    {
        if (bit > -1 && bit < 8)
        {
            VFD_Set_cmd(DCRAM_DATA_WRITE | bit, DGRAM_DATA_CLAER);
        }
    }
}

void VFD_Show_str(char bit, String str)
{
    for (size_t i = 0; i < str.length(); i++)
    {
        VFD_Set_cmd(DCRAM_DATA_WRITE | bit, (int)str.charAt(i));
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

void VFD_Set_dimming(byte dimming) // 0 <= dimming <= 255
{
    VFD_Set_cmd(SET_DIMMING_DATA, dimming);
}

void VFD_Display_status(bool status)
{
    if (status == true)
        VFD_Set_cmd(SET_DISPLAT_LIGHT_ON, 0x00);
    else
        VFD_Set_cmd(SET_DISPLAT_LIGHT_OFF, 0x00);
}

void VFD_Standby_mode(bool mode)
{
    VFD_Set_cmd(SET_STAND_BY_MODE | mode, 0x00);
}

void VFD_Show_custdata(char bit, byte *data) // data为5个字节
{
    vspi->beginTransaction(SPISettings(spiClk, LSBFIRST, SPI_MODE0));
    digitalWrite(vspi->pinSS(), LOW);
    vspi->transfer(CGRAM_DATA_WRITE | bit);
    for (size_t i = 0; i < 5; i++)
    {
        vspi->transfer(data[i]);
    }
    digitalWrite(vspi->pinSS(), HIGH);
    vspi->endTransaction();
    VFD_Set_cmd(DCRAM_DATA_WRITE | bit, bit);
}

void VFD_Set_cmd(byte cmd, byte data)
{
    vspi->beginTransaction(SPISettings(spiClk, LSBFIRST, SPI_MODE0));
    digitalWrite(vspi->pinSS(), LOW);
    vspi->transfer(cmd);
    vspi->transfer(data);
    digitalWrite(vspi->pinSS(), HIGH);
    vspi->endTransaction();
}
