#include <VFD.h>

static const int spiClk = 5000000; // 5 MHz
SPIClass *vspi = NULL;
const VFD_cmd_t VFD_initcmd[] = {{SET_DISPLAY_TIMING, VFD_DIGITS - 1},  //设置显示位数
                                 {SET_DIMMING_DATA, VFD_DIMMING},   //设置显示亮度
                                 {SET_DISPLAT_LIGHT_ON, 0x00}}; //设置开启显示

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

void VFD_Show_custdata(char bit, char flag)
{
    if (flag >= 0 && flag <= 17)
    {
        VFD_Set_cmd(DCRAM_DATA_WRITE | bit, flag);
    }
    else
        VFD_Show_str(0, "error0");  //error0代表写入字符超出存储空间
}

void VFD_Write_custdata(char flag, byte *data) // data为5个字节，CGRAM最多能存8个自定义字符
{
    if (flag >= 0 && flag <= 17)
    {
        vspi->beginTransaction(SPISettings(spiClk, LSBFIRST, SPI_MODE0));
        digitalWrite(vspi->pinSS(), LOW);
        vspi->transfer(CGRAM_DATA_WRITE | flag);    //指定存放位置
        for (size_t i = 0; i < 5; i++)
        {
            vspi->transfer(data[i]);        //写入图形数据
        }
        digitalWrite(vspi->pinSS(), HIGH);
        vspi->endTransaction();
    }
    else
        VFD_Show_str(0, "error0");
}

void VFD_Set_cmd(byte cmd, byte data)
{
    vspi->beginTransaction(SPISettings(spiClk, LSBFIRST, SPI_MODE0));   //设置SPI时钟，高低位优先，SPI模式
    digitalWrite(vspi->pinSS(), LOW);   //拉低片选，代表开始传输
    vspi->transfer(cmd);    //写入数据
    vspi->transfer(data);
    digitalWrite(vspi->pinSS(), HIGH);  //拉高片选，结束传输
    vspi->endTransaction();
}
