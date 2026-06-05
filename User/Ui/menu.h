/**
 * @brief 		二级菜单主页面导航
 * @version 	DingDa-H	
 * @data 		20260530	
 * @note 		........
 */

#ifndef __MENU_H
#define __MENU_H

#include "stm32f1xx_hal.h"
#include "project.h"

/**
 * @brief 		菜单选项的选中状态
 * @param 	
 * @data 			
 * @note 		
 */
typedef enum
{
    emMenutextSelectStatus_Flase     = 0,                       // 0 未选中
    emMenutextSelectStatus_True    	 = 1,                       // 1 选中
}
emMenutextSelectStatusTdf;

/**
 * @brief 		单行选择框数据结构
 * @param 		
 * @data 			
 * @note 		
 */
typedef struct
{
	uint32_t 					x;							//起始x坐标
	uint32_t					y;							//起始y坐标
	uint8_t 					*ucData;					//要显示的字符串
	emOledFontSizeTdf			emFontSize;					//字号大小
	emMenutextSelectStatusTdf	emSelectStatus;				//选中状态	
}
stMenuItemTdf;


void vMenu(void);
void vMenuInit(void);
void vShowWenuTextAll(void);
#endif
