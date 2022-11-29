/*这是一个实例程序
This is a example program*/

#include <VFD.h> //使用显示驱动前请先包含该头文件

const byte image[5] = {0x7F, 0x6B, 0x5D, 0x6B, 0x7F}; // 5字节的自定义图像（由取模软件得到）
VFD_Display VFD;                                      //实例化对象

void setup()
{
}

void loop()
{

    VFD.displayStatus(true); //开启显示屏
    VFD.show(0, "12345678"); //显示字符串“12345678”

    delay(1000);
    for (size_t i = 0; i < 8; i++)
    {
        VFD.RDnum(i); //第i位数字乱码效果
        VFD.show(i, char('0' + i));
    }

    delay(1000);

    VFD.clear(1); //清除第二位的显示

    delay(1000);

    VFD.show(0, "ABCDE"); //显示字符串“ABCDE”

    delay(1000);

    VFD.standbyMode(true); //进入省电模式，临时关闭显示

    delay(1000);

    VFD.standbyMode(false); //退出省电模式

    VFD.fadeOut(3); //淡出效果

    delay(1000);

    VFD.clear();                 //清除所有位的显示
    VFD.writeCustdata(0, image); //写入自定义字符，保存为第0个自定义字符
    VFD.showCustdata(1, 0);      //在第二位显示第0个自定义字符
    VFD.show(0, String('x'));

    VFD.fadeIn(3); //淡入效果

    delay(1000);

    VFD.displayStatus(false); //关闭显示屏

    delay(1000);
}
