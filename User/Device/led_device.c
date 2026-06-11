#include "stm32f1xx_hal.h"
#include "string.h"
#include "led_device.h"

/// @brief      设备实例
///
/// @note
static stLedDeviceParamTdf s_astLedDevice[LED_NUM];

/// @brief      led静态参数初始化
///
/// @note
static void s_vLedStaticParamCopy(stLedStaticParamTdf *pststaticInit,emLedDevNumTdf emDevNum)
{
	memcpy(&s_astLedDevice[emDevNum].stLedStaticParam,pststaticInit,sizeof(stLedStaticParamTdf)/sizeof(uint8_t));
}

/// @brief      Led设备参数初始化
///
/// @note
void vLedParamInit(stLedStaticParamTdf *pststaticInit,emLedDevNumTdf emDevNum)
{
	
	s_vLedStaticParamCopy(pststaticInit,emDevNum);
	
	// 先读取当前引脚电平，确定初始状态
    emLedStatusTdf init_state;
    if(pststaticInit->emLedLevel == emLedActiveLevel_Low && 
       HAL_GPIO_ReadPin(pststaticInit->pstGpioBase, pststaticInit->usGpioPin) == GPIO_PIN_RESET)
        init_state = emLedStatus_On;
    else if(pststaticInit->emLedLevel == emLedActiveLevel_High && 
            HAL_GPIO_ReadPin(pststaticInit->pstGpioBase, pststaticInit->usGpioPin) == GPIO_PIN_SET)
        init_state = emLedStatus_On;
    else
        init_state = emLedStatus_Off;
	
	//动态参数初始化
	s_astLedDevice[emDevNum].stLedRunningParam.emCurrentStatus		= init_state;
}


/**
 * @brief 		led更新
 * @param 	
 * @data 		
 * @note 		根据当前状态和有效电平设置引脚
 */
void vLed_Update(emLedDevNumTdf emDevNum)
{
	emLedStatusTdf emLedStatus	 = s_astLedDevice[emDevNum].stLedRunningParam.emCurrentStatus;
	emLedLevelTdf emLedLevel	 = s_astLedDevice[emDevNum].stLedStaticParam.emLedLevel;
	
	// 提取LED参数，简化代码
	const stLedStaticParamTdf *pParam = &s_astLedDevice[emDevNum].stLedStaticParam;
	GPIO_PinState pinState;

	// 根据点亮状态 + 有效电平 配置输出电平
	if(emLedStatus == emLedStatus_On)
	{
		// 需要点亮LED
		if(emLedLevel == emLedActiveLevel_Low)
		{
			pinState = GPIO_PIN_RESET;  // 低电平点亮
		}
		else
		{
			pinState = GPIO_PIN_SET;    // 高电平点亮
		}
	}
	else
	{
		// 需要熄灭LED
		if(emLedLevel == emLedActiveLevel_Low)
		{
			pinState = GPIO_PIN_SET;    // 低电平点亮 → 灭 = 高电平
		}
		else
		{
			pinState = GPIO_PIN_RESET;  // 高电平点亮 → 灭 = 低电平
		}
	}

	// 统一输出电平
	HAL_GPIO_WritePin(pParam->pstGpioBase, pParam->usGpioPin, pinState);
}


/**
 * @brief 		led点亮
 * @param 	
 * @data 		
 * @note 		
 */
void vLed_On(emLedDevNumTdf emDevNum)
{
	s_astLedDevice[emDevNum].stLedRunningParam.emCurrentStatus = emLedStatus_On;
	vLed_Update(emDevNum);
}	
 
 /**
 * @brief 		led熄灭
 * @param 	
 * @data 		
 * @note 		
 */
 
 void vLed_Off(emLedDevNumTdf emDevNum)
{
	s_astLedDevice[emDevNum].stLedRunningParam.emCurrentStatus = emLedStatus_Off;
	vLed_Update(emDevNum);
}
 
 /**
 * @brief 		led翻转
 * @param 	
 * @data 		
 * @note 		
 */
void vLed_Turn(emLedDevNumTdf emDevNum)
{
	if(s_astLedDevice[emDevNum].stLedRunningParam.emCurrentStatus == emLedStatus_On)
	{
		vLed_Off(emDevNum);
	}
	else
	{
		vLed_On(emDevNum);
	}
}
