
#include "stm32f1xx_hal.h"
#include "string.h"
#include "oled_device.h"
#include <stdint.h>
#include <menu.h>

static uint8_t s_aucCalc[]   = "1.Calculator";		//计算器
static uint8_t s_aucLed[]    = "2.LED Control";		//led控制
static uint8_t s_aucSnake[]  = "3.Snake Game";		//贪吃蛇
static uint8_t s_aucSerial[] = "4.Serial Test";		//串口收发

// 菜单项数组
static stMenuItemTdf s_astMenuLines[] = {
    { 10,  0, s_aucCalc,   emOledFontSize_8x16 },
    { 10, 16, s_aucLed,    emOledFontSize_8x16 },
    { 10, 32, s_aucSnake,  emOledFontSize_8x16 },
    { 10, 48, s_aucSerial, emOledFontSize_8x16 },
};
/**
 * @brief 		初始化函数待完善
 * @param 	
 * @data 			
 * @note 		
 */
void vMenuInit(void)
{
	s_astMenuLines->emSelectStatus = emMenutextSelectStatus_True;
}
/**
 * @brief 		显示所有主页面选择框
 * @param 	
 * @data 			
 * @note 		
 */
void vShowWenuTextAll(void)
{
	uint8_t i,j;
	for(i = 0;i < sizeof(s_astMenuLines) / sizeof(s_astMenuLines[0]);i++)
    {
		//如果被选中就显示'>'字符
		if(s_astMenuLines[i].emSelectStatus == emMenutextSelectStatus_True)
		{
			vOledWriteOneCharToBuffer(0,
									  s_astMenuLines[i].y,
									  '>',
									  EM_FONT_SIZE,
									  emOledPixelShowMode_Positive,
									  OLED);
		}
		vOledWriteStringToBuffer(s_astMenuLines[i].x,
								 s_astMenuLines[i].y,
								 s_astMenuLines[i].ucData,
								 EM_FONT_SIZE,
								 emOledPixelShowMode_Positive,
								 OLED);
    }
}
/**
 * @brief 		菜单测试函数待优化
 * @version 	
 * @data 			
 * @note 		实现主菜单页面思路，先显示文字吧/
 */

void vMenu(void)
{	    
	vOledClearBuffer();                	 	//清屏
	vShowWenuTextAll();						//显示所有选择框
	vOledRefreshFromBuffer(OLED);			//更新缓存	
}