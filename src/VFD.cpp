#include <VFD.h>

static const char *VFD_TAG = "VFD";
static const int spiClk = 5000000; // 5 MHz
SPIClass *spi = NULL;
VFD_cmd_t VFD_initcmd[] = {{SET_DISPLAY_TIMING, EMPTY_DATA},    //设置显示位数
                           {SET_DIMMING_DATA, EMPTY_DATA},      //设置显示亮度
                           {SET_DISPLAT_LIGHT_ON, EMPTY_DATA}}; //设置开启显示

VFD_Display::VFD_Display(byte vfd_spi, byte vfd_en, byte vfd_reset, byte vfd_dig, byte vfd_dim)
    : vfd_spi_num(vfd_spi), vfd_en_pin(vfd_en), vfd_reset_pin(vfd_reset), vfd_digits(vfd_dig), vfd_dimming(vfd_dim)
{
}

VFD_Display::~VFD_Display()
{
    spi = NULL;
}

void VFD_Display::SPI_Init()
{
    //初始化SPI
    spi = new SPIClass(vfd_spi_num);
    if (spi == NULL)
        ESP_LOGE(VFD_TAG, "SPI init fail!");
    spi->begin();
    pinMode(spi->pinSS(), OUTPUT); // VSPI SS

    //使能VFD
    pinMode(vfd_en_pin, OUTPUT);
    digitalWrite(vfd_en_pin, HIGH);

    //重置VFD
    pinMode(vfd_reset_pin, OUTPUT);
    digitalWrite(vfd_reset_pin, HIGH);
    delay(1);
    digitalWrite(vfd_reset_pin, LOW);
    delay(10);
    digitalWrite(vfd_reset_pin, HIGH);
    delay(3);

    //初始化配置值
    VFD_initcmd[0].data = vfd_digits - 1;
    VFD_initcmd[1].data = vfd_dimming;
}

void VFD_Display::VFD_Init()
{
    SPI_Init();
    for (size_t i = 0; i < 3; i++)
    {
        VFD_Set_cmd(VFD_initcmd[i].cmd, VFD_initcmd[i].data);
    }
}

void VFD_Display::VFD_Clear(char bit)
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

void VFD_Display::VFD_Show_str(char bit, String str)
{
    for (size_t i = 0; i < str.length(); i++)
    {
        VFD_Set_cmd(DCRAM_DATA_WRITE | bit, (int)str.charAt(i));
        if (bit < vfd_digits - 1)
        {
            bit += 1;
        }
        else
        {
            break;
        }
    }
}

void VFD_Display::VFD_Set_dimming(byte dimming) // 0 <= dimming <= 255
{
    VFD_Set_cmd(SET_DIMMING_DATA, dimming);
}

void VFD_Display::VFD_FadeIn(byte pertime)
{
    for (size_t i = 0; i <= 255; i++)
    {
        VFD_Set_dimming(i); //字符淡入效果
        delay(pertime);
    }
}

void VFD_Display::VFD_FadeOut(byte pertime)
{
    for (size_t i = 255; i > 0; i--)
    {
        VFD_Set_dimming(i); //字符淡出效果
        delay(pertime);
    }
}

void VFD_Display::VFD_Display_status(bool status)
{
    if (status == true)
        VFD_Set_cmd(SET_DISPLAT_LIGHT_ON, EMPTY_DATA);
    else
        VFD_Set_cmd(SET_DISPLAT_LIGHT_OFF, EMPTY_DATA);
}

void VFD_Display::VFD_Standby_mode(bool mode)
{
    VFD_Set_cmd(SET_STAND_BY_MODE | mode, EMPTY_DATA);
}

void VFD_Display::VFD_Show_custdata(char bit, char flag)
{
    if (flag >= 0 && flag <= 17)
    {
        VFD_Set_cmd(DCRAM_DATA_WRITE | bit, flag);
    }
    else
        ESP_LOGE(VFD_TAG, "beyond DCRAM storage space!");
}

void VFD_Display::VFD_Write_custdata(char flag, const byte *data) // data为5个字节，CGRAM最多能存8个自定义字符
{
    if (flag >= 0 && flag <= 17)
    {
        spi->beginTransaction(SPISettings(spiClk, LSBFIRST, SPI_MODE0));
        digitalWrite(spi->pinSS(), LOW);
        spi->transfer(CGRAM_DATA_WRITE | flag); //指定存放位置
        for (size_t i = 0; i < 5; i++)
        {
            spi->transfer(data[i]); //写入图形数据
        }
        digitalWrite(spi->pinSS(), HIGH);
        spi->endTransaction();
    }
    else
        ESP_LOGE(VFD_TAG, "beyond DCRAM storage space!");
}

void VFD_Display::VFD_Set_cmd(byte cmd, byte data)
{
    spi->beginTransaction(SPISettings(spiClk, LSBFIRST, SPI_MODE0)); //设置SPI时钟，高低位优先，SPI模式
    digitalWrite(spi->pinSS(), LOW);                                 //拉低片选，代表开始传输
    spi->transfer(cmd);                                              //写入数据
    spi->transfer(data);
    digitalWrite(spi->pinSS(), HIGH); //拉高片选，结束传输
    spi->endTransaction();
}
