/*这是一个实例程序
This is a example program*/
#include <VFD.h>

byte image[5] = {0x00, 0x01, 0x57, 0x88, 0x00};

void setup(){
  SPI_Init();
  VFD_Init();
}

void loop(){
  VFD_Show_str(0, "12345");
  delay(1000);
  VFD_Show_custdata(0, image);
}
