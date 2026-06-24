#ifndef __TEST_H
#define __TEST_H

#include "stm32f1xx_hal.h"


typedef enum
{
	emEvent_Int			= 0,			//输入数字的事件
	emEvent_Operator,					//输入加减乘除的事件 
	emEvent_Sum,						//输入求和运算的事件
	emEvent_Del,						//删除单个字符事件
}
emEventTdf;

typedef enum
{
	emState_In01			= 0,		//输入状态1
	emState_Operator,					//输入运算的状态
	emState_In02,						//输入状态2
	emState_Sum,						//求结果的状态
}
emStateTdf;


/**
* @brief 		状态机的运行参数
 * @param 	
 * @data 			
 * @note 		
 */
typedef struct
{
	int32_t		iOperato_1;			//输入数1
	int32_t		iOperato_2;			//输入数2
	uint8_t		ucOperator;			//输入运算符
	int32_t		lCurrentInput; 		//当前输入
	
	emStateTdf 	ucCurrentState;  	// 当前状态（对应你的状态枚举）
}
stFsmRunParamTdf;

void vTest(void);
void vState_machine_run(uint8_t key);
void vCalcUiResetDraw(void);
void vCalcParamInit(stFsmRunParamTdf *pParam);
void vCalcSystemInit(void);
#endif
