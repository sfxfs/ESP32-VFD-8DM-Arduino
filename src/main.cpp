/*这是一个实例程序
This is a example program*/
#include <VFD.h>

byte image[5] = {0x7F, 0x6B, 0x5D, 0x6B, 0x7F};

void setup(){
  VFD_Init();
}

void loop(){
  VFD_Display_status(true);
  VFD_Show_str(0, "12345");
  delay(1000);
  VFD_Clear(1);
  delay(1000);
  VFD_Show_str(0, "ABCDE");
  delay(1000);
  VFD_Standby_mode(true);
  delay(1000);
  VFD_Standby_mode(false);
  for (size_t i = 255; i >= 0; i--)
  {
    VFD_Set_dimming(i);
    delay(10);
  }
  VFD_Clear(-1);
  VFD_Show_custdata(0, image);
  for (size_t i = 0; i <= 255; i++)
  {
    VFD_Set_dimming(i);
    delay(10);
  }
  delay(1000);
  VFD_Display_status(false);
  delay(1000);
}
