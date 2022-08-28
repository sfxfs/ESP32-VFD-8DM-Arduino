/*这是一个实例程序
This is a example program*/
#include <VFD.h>

void setup(){
  SPI_Init();
  VFD_Init();
}

void loop(){
  VFD_Show_str(0, "12345");
}
