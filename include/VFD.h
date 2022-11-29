#ifndef _VFD_H
#define _VFD_H

#include <Arduino.h>

#define VFD_EN_PIN 16
#define VFD_RESET_PIN 17

#define VFD_DIGITS 8
#define VFD_DIMMING 255

struct VFD_cmd_t
{
  byte cmd;
  byte data;
};

class VFD_Display
{
public:
  VFD_Display(
      byte spi = VSPI,
      byte en = VFD_EN_PIN,
      byte reset = VFD_RESET_PIN,
      byte dig = VFD_DIGITS,
      byte dim = VFD_DIMMING);
  ~VFD_Display();

  bool isInit() const;
  void clear() const;
  void clear(char bit) const;
  void show(char bit, char chr) const;
  void show(char bit, String str) const;
  void showCustdata(char bit, char flag) const;
  void writeCustdata(char flag, const byte *data) const;
  void setDimming(byte dimming);
  void standbyMode(bool mode) const;
  void displayStatus(bool status) const;
  void fadeIn(byte pertime);
  void fadeOut(byte pertime);
  void RDnum(char bit) const;

private:
  byte spi_num;
  byte en_pin;
  byte reset_pin;
  byte digits;
  byte dimming;

  void init() const;
  void spiInit() const;
  void setCmd(byte cmd, byte data) const;
};

#endif
