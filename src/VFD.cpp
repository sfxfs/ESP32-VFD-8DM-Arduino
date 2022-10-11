#include <VFD.h>

static const char *VFD_TAG = "VFD";
static const int spiClk = 5000000; // 5 MHz
SPIClass *spi = NULL;
static VFD_cmd_t VFD_initcmd[] = {{SET_DISPLAY_TIMING, EMPTY_DATA},    //设置显示位数
                                  {SET_DIMMING_DATA, EMPTY_DATA},      //设置显示亮度
                                  {SET_DISPLAT_LIGHT_ON, EMPTY_DATA}}; //设置开启显示

/// @brief VFD显示屏的构造函数
/// @param vfd_spi VFD连接的SPI总线名称
/// @param vfd_en VFD的EN引脚的连接脚
/// @param vfd_reset VFD的RESET引脚的连接脚
/// @param vfd_dig VFD的显示位数
/// @param vfd_dim VFD的初始亮度
VFD_Display::VFD_Display(
                        byte vfd_spi,
                        byte vfd_en,
                        byte vfd_reset,
                        byte vfd_dig,
                        byte vfd_dim
                        )
    : vfd_spi_num(vfd_spi),
    vfd_en_pin(vfd_en),
    vfd_reset_pin(vfd_reset),
    vfd_digits(vfd_dig),
    vfd_dimming(vfd_dim)
{
}

/// @brief VFD的析构函数（释放SPI指针）
VFD_Display::~VFD_Display()
{
    delete spi;
}

/// @brief SPI初始化函数
void VFD_Display::SPI_Init() const
{
    //初始化SPI
    spi = new SPIClass(vfd_spi_num);
    if (spi == NULL)
    {
        ESP_LOGE(VFD_TAG, "SPI init fail!");
        return;
    }
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

/// @brief VFD初始化函数
void VFD_Display::VFD_Init() const
{
    SPI_Init();
    for (size_t i = 0; i < 3; i++)
    {
        VFD_Set_cmd(VFD_initcmd[i].cmd, VFD_initcmd[i].data);
    }
}

/// @brief VFD清屏函数
/// @param bit 要清除的显示位（-1则清除所有位）
void VFD_Display::VFD_Clear(char bit) const
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
        }else
        ESP_LOGE(VFD_TAG, "Out of bits!");
    }
}

/// @brief VFD显示单个字符函数
/// @param bit 要显示的位
/// @param chr 要显示的字符
void VFD_Display::VFD_Show(char bit, char chr) const
{
    VFD_Set_cmd(DCRAM_DATA_WRITE | bit, (byte)chr);
}

/// @brief VFD显示字符串
/// @param bit 第一个字符显示的位
/// @param str 要显示的字符串
void VFD_Display::VFD_Show(char bit, String str) const
{
    for (size_t i = 0; i < str.length(); i++)
    {
        VFD_Show(bit, str.charAt(i));
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

/// @brief 设置VFD的显示亮度
/// @param dimming VFD的亮度（最小为0，最大为255）
void VFD_Display::VFD_Set_dimming(byte dimming) // 0 <= dimming <= 255
{
    vfd_dimming = constrain(dimming, 0, 255);
    VFD_Set_cmd(SET_DIMMING_DATA, vfd_dimming);
}

/// @brief VFD淡入效果
/// @param pertime 控制每次调整亮度的间隔（调整淡入快慢）
void VFD_Display::VFD_FadeIn(byte pertime)
{
    for (int i = 0; i < vfd_dimming; i++)
    {
        VFD_Set_dimming(i); //字符淡入效果
        delay(pertime);
    }
}

/// @brief VFD淡出效果
/// @param pertime 控制每次调整亮度的间隔（调整淡出快慢）
void VFD_Display::VFD_FadeOut(byte pertime)
{
    for (int i = vfd_dimming; i >= 0; i--)
    {
        VFD_Set_dimming(i); //字符淡出效果
        delay(pertime);
    }
}

/// @brief VFD乱码效果
/// @param bit 要乱码的位
void VFD_Display::VFD_RDnum(char bit) const
{
    for (size_t i = 0; i < 15; i++)
    {
        VFD_Show(bit, String(i + 48));
        delay(8);
    }
}

/// @brief 设置VFD的开关
/// @param status VFD的开关
void VFD_Display::VFD_Display_status(bool status) const
{
    if (status == true)
        VFD_Set_cmd(SET_DISPLAT_LIGHT_ON, EMPTY_DATA);
    else
        VFD_Set_cmd(SET_DISPLAT_LIGHT_OFF, EMPTY_DATA);
}

/// @brief VFD待机模式
/// @param mode 待机模式开启关闭
void VFD_Display::VFD_Standby_mode(bool mode) const
{
    VFD_Set_cmd(SET_STAND_BY_MODE | mode, EMPTY_DATA);
}

/// @brief VFD显示自定义字符
/// @param bit 要显示的位
/// @param flag 存储的自定义字符的标志
void VFD_Display::VFD_Show_custdata(char bit, char flag) const
{
    if (flag >= 0 && flag <= 17)
    {
        VFD_Set_cmd(DCRAM_DATA_WRITE | bit, flag);
    }
    else
        ESP_LOGE(VFD_TAG, "Out of DCRAM storage space!");
}

/// @brief 向VFD寄存器写入自定义字符
/// @param flag 存储的自定义字符的标志（调用时要用）
/// @param data 写入的自定义字符数据
void VFD_Display::VFD_Write_custdata(char flag, const byte *data) const // data为5个字节，CGRAM最多能存8个自定义字符
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
        ESP_LOGE(VFD_TAG, "Out of DCRAM storage space!");
}

/// @brief 向VFD发送命令或者数据
/// @param cmd 命令
/// @param data 数据
void VFD_Display::VFD_Set_cmd(byte cmd, byte data) const
{
    spi->beginTransaction(SPISettings(spiClk, LSBFIRST, SPI_MODE0)); //设置SPI时钟，高低位优先，SPI模式
    digitalWrite(spi->pinSS(), LOW);                                 //拉低片选，代表开始传输
    spi->transfer(cmd);                                              //写入数据
    spi->transfer(data);
    digitalWrite(spi->pinSS(), HIGH); //拉高片选，结束传输
    spi->endTransaction();
}
