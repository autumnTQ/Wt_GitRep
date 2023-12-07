#ifndef __LCD_TEST_H
#define __LCD_TEST_H

#include "stm32f4xx.h"
#include "lcd_spi_154.h"
#include <stdint.h>
#include <stdio.h>
#include "delay.h"
#include "lcd_fonts.h"		// 图片和字库文件不是必须，用户可自行删减
#include "lcd_image.h" 


// LCD测试函数，函数定义在底部
void LCD_Test_Clear(void);			   // 清屏测试
void LCD_Test_Text(void);			   // 文本测试
void LCD_Test_Variable (void);	       // 变量显示，包括整数和小数
void LCD_Test_Color(void);			   // 矩形填充测试
void LCD_Test_Grahic(void);		       // 2D图形绘制
void LCD_Test_Image(void);			   // 图片显示
void LCD_Test_Direction(void);	       // 更换显示方向

#endif 


