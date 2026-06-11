
#include "stm32f1xx_hal.h"
#include "string.h"
#include <stdint.h>
#include <ui_text.h>
	
static uint8_t s_aucCalc[]   = "Calculator";			//计算器

static uint8_t s_aucLed[]   = "Led";					//Led

/**
 * @brief 		[Led单行文本]
 * @version 	
 * @data 			
 * @note 		
 */
static stTextItemTdf s_stSingleLineTest_Led = 
{
	//-1是因为字符串末尾的'\0'
	//(sizeof(s_aucLed)字符串数组总长， sizeof(s_aucLed[0])单个字符的长度
	//(sizeof(s_aucLed) / sizeof(s_aucLed[0]) - 1) 字符个数
	//EM_FONT_SIZE / 2,每个字符占像素宽度
	.x			= 64 - (sizeof(s_aucLed) / sizeof(s_aucLed[0]) - 1) / 2 * EM_FONT_SIZE / 2,
	.y			= 3,
	.ucData		= s_aucLed,
	.emFontSize = EM_FONT_SIZE,
};

static stTextItemTdf s_stSingleLineTest_s_aucCalc = 
{
	//-1是因为字符串末尾的'\0'
	//(sizeof(s_aucLed)字符串数组总长， sizeof(s_aucLed[0])单个字符的长度
	//(sizeof(s_aucLed) / sizeof(s_aucLed[0]) - 1) 字符个数
	//emOledFontSize_8x16 / 2,每个字符占像素宽度
	.x			= 64 - (sizeof(s_aucCalc) / sizeof(s_aucCalc[0]) - 1) / 2 * EM_FONT_SIZE / 2,
	.y			= 0,
	.ucData		= s_aucCalc,
	.emFontSize = EM_FONT_SIZE,
};

// 子菜单文本项 结构体指针数组
static stTextItemTdf *s_astTextLines[] = {
	&s_stSingleLineTest_Led,
//	&s_stSingleLineTest_s_aucCalc;
};



/**
 * @brief 		显示所有单行文本
 * @param 	
 * @data 			
* @note 		根据不同页面显示不同文本，这里后续需要修改
 */
void vShowSingleLineAll(void)
{
	uint8_t i;
	for(i = 0;i < sizeof(s_astTextLines)/sizeof(s_astTextLines[0]);i++)
    {
		vOledWriteStringToBuffer(s_astTextLines[i]->x,
								 s_astTextLines[i]->y,
								 s_astTextLines[i]->ucData,
								 s_astTextLines[i]->emFontSize,
								 emOledPixelShowMode_Positive,OLED);
    }
}
///**
// * @brief 		菜单测试函数待优化
// * @version 	
// * @data 			
// * @note 		实现主菜单页面思路，先显示文字吧/
// */

//void vText(void)
//{	    
//	vOledClearBuffer();                	 	//清屏
//	vShowSingleLineAll();					//显示单行文本
//	vOledRefreshFromBuffer(OLED);			//更新缓存	
//}