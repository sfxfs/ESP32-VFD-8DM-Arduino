/*这是一个实例程序
This is a example program*/

#include <VFD.h> //使用显示驱动前请先包含该头文件

const byte image[5] = {0x7F, 0x6B, 0x5D, 0x6B, 0x7F}; //自定义图像（由取模软件得到）
VFD_Display VFD;  //实例化对象

void setup()
{
  VFD.VFD_Init(); //显示屏初始化
}

void loop()
{
  VFD.VFD_Display_status(true); //开启显示屏
  VFD.VFD_Show_str(0, "12345"); //显示字符串“12345”

  delay(1000);

  VFD.VFD_Clear(1); //清除第二位的显示

  delay(1000);

  VFD.VFD_Show_str(0, "ABCDE"); //显示字符串“ABCDE”

  delay(1000);

  VFD.VFD_Standby_mode(true); //进入省电模式，临时关闭显示

  delay(1000);

  VFD.VFD_Standby_mode(false); //退出省电模式
  
  VFD.VFD_FadeOut(3); //淡出效果
  
  VFD.VFD_Clear(-1);                //清除所有位的显示
  VFD.VFD_Write_custdata(0, image); //写入自定义字符，保存为第0个自定义字符
  VFD.VFD_Show_custdata(1, 0);      //在第二位显示第0个自定义字符
  
  delay(400);
  
  VFD.VFD_FadeIn(3);  //淡入效果

  delay(1000);

  VFD.VFD_Display_status(false); //关闭显示屏

  delay(1000);
}
