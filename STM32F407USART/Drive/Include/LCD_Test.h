#ifndef __LCD_TEST_H
#define __LCD_TEST_H

#include "stm32f4xx.h"
#include "lcd_spi_154.h"
#include <stdint.h>
#include <stdio.h>
#include "delay.h"
#include "lcd_fonts.h"		// ͼƬ���ֿ��ļ����Ǳ��룬�û�������ɾ��
#include "lcd_image.h" 


// LCD���Ժ��������������ڵײ�
void LCD_Test_Clear(void);			   // ��������
void LCD_Test_Text(void);			   // �ı�����
void LCD_Test_Variable (void);	       // ������ʾ������������С��
void LCD_Test_Color(void);			   // ����������
void LCD_Test_Grahic(void);		       // 2Dͼ�λ���
void LCD_Test_Image(void);			   // ͼƬ��ʾ
void LCD_Test_Direction(void);	       // ������ʾ����

#endif 


