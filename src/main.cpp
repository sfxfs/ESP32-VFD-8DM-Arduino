/*这是一个实例程序
This is a example program*/
#include <VFD.h>

byte image[5] = {0x7F, 0x6B, 0x5D, 0x6B, 0x7F};

void setup(){
  VFD_Init();
}

void loop(){
  VFD_On();
  VFD_Show_str(0, "12345");
  delay(1000);
  VFD_Show_str(0, "ABCDE");
  delay(1000);
  VFD_Show_custdata(0, image);
  delay(1000);
  VFD_Off();
  delay(1000);
}
