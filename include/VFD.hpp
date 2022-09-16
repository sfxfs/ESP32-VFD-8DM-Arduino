#ifndef _VFD_H
#define _VFD_H

#include <Arduino.h>
#include <SPI.h>
#include <esp_log.h>

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
#define EMPTY_DATA 0x00

struct VFD_cmd_t
{
    uint8_t cmd;
    uint8_t data;
};

const char *VFD_TAG = "VFD";
const int spiClk = 5000000; // 5 MHz
SPIClass *spi = NULL;
VFD_cmd_t VFD_initcmd[] = {{SET_DISPLAY_TIMING, EMPTY_DATA},    //设置显示位数
                           {SET_DIMMING_DATA, EMPTY_DATA},      //设置显示亮度
                           {SET_DISPLAT_LIGHT_ON, EMPTY_DATA}}; //设置开启显示

class VFD_Display
{
private:
    void SPI_Init()
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
    };
    void VFD_Set_cmd(byte cmd, byte data)
    {
        spi->beginTransaction(SPISettings(spiClk, LSBFIRST, SPI_MODE0)); //设置SPI时钟，高低位优先，SPI模式
        digitalWrite(spi->pinSS(), LOW);                                 //拉低片选，代表开始传输
        spi->transfer(cmd);                                              //写入数据
        spi->transfer(data);
        digitalWrite(spi->pinSS(), HIGH); //拉高片选，结束传输
        spi->endTransaction();
    }

public:
    byte vfd_spi_num;
    byte vfd_en_pin;
    byte vfd_reset_pin;
    byte vfd_digits;
    byte vfd_dimming;

    VFD_Display(
        byte vfd_spi = VSPI,
        byte vfd_en = VFD_EN_PIN,
        byte vfd_reset = VFD_RESET_PIN,
        byte vfd_dig = VFD_DIGITS,
        byte vfd_dim = VFD_DIMMING) : vfd_spi_num(vfd_spi),
                                      vfd_en_pin(vfd_en),
                                      vfd_reset_pin(vfd_reset),
                                      vfd_digits(vfd_dig),
                                      vfd_dimming(vfd_dim) {}
    ~VFD_Display() { spi = NULL; }

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
    void VFD_Show_custdata(char bit, char flag)
    {
        if (flag >= 0 && flag <= 17)
        {
            VFD_Set_cmd(DCRAM_DATA_WRITE | bit, flag);
        }
        else
            ESP_LOGE(VFD_TAG, "beyond DCRAM storage space!");
    }
    void VFD_Write_custdata(char flag, const byte *data)
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
    void VFD_Set_dimming(byte dimming) { VFD_Set_cmd(SET_DIMMING_DATA, dimming); }
    void VFD_Standby_mode(bool mode) { VFD_Set_cmd(SET_STAND_BY_MODE | mode, EMPTY_DATA); }
    void VFD_Display_status(bool status)
    {
        if (status == true)
            VFD_Set_cmd(SET_DISPLAT_LIGHT_ON, EMPTY_DATA);
        else
            VFD_Set_cmd(SET_DISPLAT_LIGHT_OFF, EMPTY_DATA);
    }
    void VFD_FadeIn(byte pertime)
    {
        for (size_t i = 0; i <= vfd_dimming; i++)
        {
            VFD_Set_dimming(i); //字符淡入效果
            delay(pertime);
        }
    }
    void VFD_FadeOut(byte pertime)
    {
        for (size_t i = vfd_dimming; i >= 0; i--)
        {
            VFD_Set_dimming(i); //字符淡出效果
            delay(pertime);
        }
    }
};

#endif
