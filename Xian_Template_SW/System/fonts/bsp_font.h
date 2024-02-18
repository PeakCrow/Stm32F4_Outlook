#ifndef __BSP_FONT_H_
#define __BSP_FONT_H_
//常用ASCII表
//偏移量32 
//ASCII字符集: !"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}~
//PC2LCD2002取模方式设置：阴码+逐列式+顺向+C51格式
//总共：3个字符集（12*12、16*16、24*24和32*32），用户可以自行新增其他分辨率的字符集。
//每个字符所占用的字节数为:(size/8+((size%8)?1:0))*(size/2),其中size:是字库生成时的点阵大小(12/16/24/32...)

//12*12 ASCII字符集点阵
extern const unsigned char asc2_1206_lcd[95][12];
//16*16 ASCII字符集点阵
extern const unsigned char asc2_1608_lcd[95][16];
//24*24 ASICII字符集点阵
extern const unsigned char asc2_2412_lcd[95][36];
//32*32 ASCII字符集点阵
extern const unsigned char asc2_3216_lcd[95][128];

#endif
