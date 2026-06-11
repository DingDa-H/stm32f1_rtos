#ifndef __LED_DEVICE_H
#define __LED_DEVICE_H


#include "project.h"
#include STM32_IC_HAL

/**
 * @brief 		按钮设备数量枚举
 * @param 	
 * @data 			
 * @note 		
 */
typedef enum
{
    emLedDevNum0    	 = 0,
    emLedDevNum1,
	emLedDevNum2,
	emLedDevNum3,
}
emLedDevNumTdf;


/**
 * @brief 		Led有效电平枚举
 * @param 	
 * @data 			
 * @note 		Led点亮时的电平状态
 */
typedef enum
{
    emLedActiveLevel_Low                               = 0,                // 0  Led点亮时低电平
    emLedActiveLevel_High                              = 1,                // 1  Led点亮时高电平
}
emLedLevelTdf;

/// @brief      Led状态枚举定义
///
/// @note
typedef enum
{
    emLedStatus_Off                               = 0,                 		// 0        熄灭
    emLedStatus_On                                = 1,                 		// 1        点亮
}
emLedStatusTdf;


/// @brief      设备运行参数结构定义
///
/// @note       
typedef struct
{
    emLedStatusTdf                                     emCurrentStatus;               // 当前状态
}
stLedRunningParamTdf;


/// @brief      设备静态参数结构定义
/// @note       通常情况下也可以理解为硬件参数
typedef struct
{
    GPIO_TypeDef                *pstGpioBase;                  // 使用的 GPIOx
    uint16_t                    usGpioPin;                     // 使用的 PINx
    emLedLevelTdf		        emLedLevel;           	       // Led点亮时电平
}
stLedStaticParamTdf;


/// @brief      Led设备参数结构定义
///
/// @note       含静态参数和动态参数
typedef struct
{
    stLedStaticParamTdf         stLedStaticParam;                 // 静态参数
    stLedRunningParamTdf        stLedRunningParam;                // 运行参数
} 
stLedDeviceParamTdf;

void vLedParamInit(stLedStaticParamTdf *pststaticInit,emLedDevNumTdf emDevNum);
void vLed_On(emLedDevNumTdf emDevNum);
void vLed_Off(emLedDevNumTdf emDevNum);
void vLed_Turn(emLedDevNumTdf emDevNum);

#endif
