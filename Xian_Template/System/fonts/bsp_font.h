#ifndef __BSP_FONT_H_
#define __BSP_FONT_H_
//����ASCII��
//ƫ����32 
//ASCII�ַ���: !"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}~
//PC2LCD2002ȡģ��ʽ���ã�����+����ʽ+˳��+C51��ʽ
//�ܹ���3���ַ�����12*12��16*16��24*24��32*32�����û������������������ֱ��ʵ��ַ�����
//ÿ���ַ���ռ�õ��ֽ���Ϊ:(size/8+((size%8)?1:0))*(size/2),����size:���ֿ�����ʱ�ĵ����С(12/16/24/32...)

//12*12 ASCII�ַ�������
extern const unsigned char asc2_1206_lcd[95][12];
//16*16 ASCII�ַ�������
extern const unsigned char asc2_1608_lcd[95][16];
//24*24 ASICII�ַ�������
extern const unsigned char asc2_2412_lcd[95][36];
//32*32 ASCII�ַ�������
extern const unsigned char asc2_3216_lcd[95][128];

#endif
