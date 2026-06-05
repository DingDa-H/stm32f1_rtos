#include "stm32f1xx_hal.h"
#include "string.h"
#include <stdint.h>

#include "task.h"



/**
 * @brief 		物理按键初始化
 * @param 	
 * @data 		
 * @note 		
 */
void Button_Init(void)
{
	stBtnStaticParamTdf stBtnInit;
	stBtnInit.emButLevel					= emBtnActiveLevel_Low;
	stBtnInit.pstGpioBase					= GPIOB;
	stBtnInit.ulDebounceMs 					= 20;
	stBtnInit.ulLongPressMs					= 1000;
	stBtnInit.ulDoubleClickMs				= 300;
	stBtnInit.usGpioPin						= GPIO_PIN_1;
	
	vBtnParamInit(&stBtnInit,BUTTON_DOWN);
//	
//	stBtnStaticParamTdf btn_cfg = {
//	.pstGpioBase    = GPIOB,        // 你的按键GPIO口
//	.usGpioPin      = GPIO_PIN_1,   // 你的按键引脚
//	.emButLevel     = emBtnActiveLevel_Low,
//	.ulDebounceMs   = 20,
//	.ulLongPressMs  = 800,   // 按下≥800ms才是长按
//	.ulDoubleClickMs= 300
//	};
//	vBtnParamInit(&btn_cfg, BUTTON_DOWN);
}


/**
 * @brief 		测试函数待优化
 * @version 	
 * @data 			
 * @note 		实现加减乘除的思路，状态机/
 */

void vTask(void)
{
    vBtnExecute();

    if(emBtnGetCurEvent(BUTTON_DOWN) == emBtnEvent_Click)
    {
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
        vBtnEventClear(BUTTON_DOWN);
    }
    else if(emBtnGetCurEvent(BUTTON_DOWN) == emBtnEvent_DoubleClick)
    {
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
        vBtnEventClear(BUTTON_DOWN);
    }
	else if(emBtnGetCurEvent(BUTTON_DOWN) == emBtnEvent_LongPress)
    {

        vBtnEventClear(BUTTON_DOWN);
    }
//	else
//	{
//		vBtnEventClear(BUTTON_DOWN);
//	}
}
