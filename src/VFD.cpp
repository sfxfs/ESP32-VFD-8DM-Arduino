#include <SPI.h>
#include <esp_log.h>

#include "VFD.h"

// 定义 ALTERNATE_PINS 来指定非标准 GPIO 引脚用于 SPI 总线通讯
#ifdef ALTERNATE_PINS
#define VSPI_MISO 2
#define VSPI_MOSI 4
#define VSPI_SCLK 0
#define VSPI_SS 33

#define HSPI_MISO 26
#define HSPI_MOSI 27
#define HSPI_SCLK 25
#define HSPI_SS 32
#else
#define VSPI_MISO MISO
#define VSPI_MOSI MOSI
#define VSPI_SCLK SCK
#define VSPI_SS SS

#define HSPI_MISO 12
#define HSPI_MOSI 13
#define HSPI_SCLK 14
#define HSPI_SS 15
#endif

#if CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32S3
#define VSPI FSPI
#endif

#define DCRAM_DATA_WRITE 0x20
#define DGRAM_DATA_CLAER 0x10
#define CGRAM_DATA_WRITE 0x40
#define SET_DISPLAY_TIMING 0xE0
#define SET_DIMMING_DATA 0xE4
#define SET_DISPLAT_LIGHT_ON 0xE8
#define SET_DISPLAT_LIGHT_OFF 0xEA
#define SET_STAND_BY_MODE 0xEC
#define EMPTY_DATA 0x00

static const char *VFD_TAG = "VFD";
static const int spiClk = 5000000; // 5 MHz

SPIClass *spi = NULL;

/// @brief VFD显示屏的构造函数
/// @param spi VFD连接的SPI总线名称
/// @param en VFD的EN引脚的连接脚
/// @param reset VFD的RESET引脚的连接脚
/// @param dig VFD的显示位数
/// @param dim VFD的初始亮度
VFD_Display::VFD_Display(
    byte spi,
    byte en,
    byte reset,
    byte dig,
    byte dim)
    : spi_num(spi),
      en_pin(en),
      reset_pin(reset),
      digits(dig),
      dimming(dim)
{
}

/// @brief VFD的析构函数（释放SPI指针）
VFD_Display::~VFD_Display()
{
    delete spi;
}

/// @brief SPI初始化函数
void VFD_Display::spiInit() const
{
    //初始化SPI
    spi = new SPIClass(spi_num);
    if (spi == NULL)
    {
        ESP_LOGE(VFD_TAG, "SPI init fail!");
        return;
    }
    spi->begin();
    pinMode(spi->pinSS(), OUTPUT);

    //使能VFD
    pinMode(en_pin, OUTPUT);
    digitalWrite(en_pin, HIGH);

    //重置VFD
    pinMode(reset_pin, OUTPUT);
    digitalWrite(reset_pin, HIGH);
    delay(1);
    digitalWrite(reset_pin, LOW);
    delay(10);
    digitalWrite(reset_pin, HIGH);
    delay(3);
}

/// @brief VFD初始化函数
void VFD_Display::init() const
{
    spiInit();
    VFD_cmd_t VFD_initcmd[] = {{SET_DISPLAY_TIMING, byte(digits - 1)}, //设置显示位数
                               {SET_DIMMING_DATA, dimming},            //设置显示亮度
                               {SET_DISPLAT_LIGHT_ON, EMPTY_DATA}};    //设置开启显示
    for (size_t i = 0; i < 3; i++)
    {
        setCmd(VFD_initcmd[i].cmd, VFD_initcmd[i].data);
    }
}

/// @brief VFD清屏函数
void VFD_Display::clear() const
{
    for (size_t i = 0; i < 8; i++)
    {
        setCmd(DCRAM_DATA_WRITE | i, DGRAM_DATA_CLAER);
    }
}

/// @brief VFD清屏函数
/// @param bit 要清除的显示位
void VFD_Display::clear(char bit) const
{
    if (bit > -1 && bit < 8)
    {
        setCmd(DCRAM_DATA_WRITE | bit, DGRAM_DATA_CLAER);
    }
    else
        ESP_LOGE(VFD_TAG, "Out of bits!");
}

/// @brief VFD显示单个字符函数
/// @param bit 要显示的位
/// @param chr 要显示的字符
void VFD_Display::show(char bit, char chr) const
{
    setCmd(DCRAM_DATA_WRITE | bit, (byte)chr);
}

/// @brief VFD显示字符串
/// @param bit 第一个字符显示的位
/// @param str 要显示的字符串
void VFD_Display::show(char bit, String str) const
{
    for (size_t i = 0; i < str.length(); i++)
    {
        show(bit, str.charAt(i));
        if (bit < digits - 1)
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
void VFD_Display::setDimming(byte dimming) // 0 <= dimming <= 255
{
    setCmd(SET_DIMMING_DATA, dimming);
}

/// @brief VFD淡入效果
/// @param pertime 控制每次调整亮度的间隔（调整淡入快慢）
void VFD_Display::fadeIn(byte pertime)
{
    for (int i = 0; i < dimming; i++)
    {
        setCmd(SET_DIMMING_DATA, i); //字符淡入效果
        delay(pertime);
    }
}

/// @brief VFD淡出效果
/// @param pertime 控制每次调整亮度的间隔（调整淡出快慢）
void VFD_Display::fadeOut(byte pertime)
{
    for (int i = dimming; i >= 0; i--)
    {
        setCmd(SET_DIMMING_DATA, i); //字符淡出效果
        delay(pertime);
    }
}

/// @brief VFD乱码效果
/// @param bit 要乱码的位
void VFD_Display::RDnum(char bit) const
{
    for (size_t i = 0; i < 15; i++)
    {
        show(bit, String(i + 48));
        delay(8);
    }
}

/// @brief 设置VFD的开关
/// @param status VFD的开关
void VFD_Display::displayStatus(bool status) const
{
    if (status == true)
        setCmd(SET_DISPLAT_LIGHT_ON, EMPTY_DATA);
    else
        setCmd(SET_DISPLAT_LIGHT_OFF, EMPTY_DATA);
}

/// @brief VFD待机模式
/// @param mode 待机模式开启关闭
void VFD_Display::standbyMode(bool mode) const
{
    setCmd(SET_STAND_BY_MODE | mode, EMPTY_DATA);
}

/// @brief VFD显示自定义字符
/// @param bit 要显示的位
/// @param flag 存储的自定义字符的标志
void VFD_Display::showCustdata(char bit, char flag) const
{
    if (flag >= 0 && flag <= 17)
    {
        setCmd(DCRAM_DATA_WRITE | bit, flag);
    }
    else
        ESP_LOGE(VFD_TAG, "Out of DCRAM storage space!");
}

/// @brief 向VFD寄存器写入自定义字符
/// @param flag 存储的自定义字符的标志（调用时要用）
/// @param data 写入的自定义字符数据
void VFD_Display::writeCustdata(char flag, const byte *data) const // data为5个字节，CGRAM最多能存8个自定义字符
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
void VFD_Display::setCmd(byte cmd, byte data) const
{
    spi->beginTransaction(SPISettings(spiClk, LSBFIRST, SPI_MODE0)); //设置SPI时钟，高低位优先，SPI模式
    digitalWrite(spi->pinSS(), LOW);                                 //拉低片选，代表开始传输
    spi->transfer(cmd);                                              //写入数据
    spi->transfer(data);
    digitalWrite(spi->pinSS(), HIGH); //拉高片选，结束传输
    spi->endTransaction();
}
