/**
 * @brief 		薄膜矩阵按键4X4
 * @version 	DingDa-H	叮答_H
 * @data 		20260523	
 * @note 		........
 */

#ifndef __KEYPAD4_4_H
#define __KEYPAD4_4_H

#include "stm32f1xx_hal.h"

typedef enum
{
    emKeypadDevNum0    	 = 0,
    emKeypadDevNum1,
	emKeypadDevNum2,
	emKeypadDevNum3,
	emKeypadDevNum4,
	emKeypadDevNum5,
	emKeypadDevNum6,
	emKeypadDevNum7,
}
emKeypadDevNumTdf;



extern uint16_t KEYPAD_Row[4];

extern uint16_t KEYPAD_Col[4];

uint8_t ucKeypad_scan(void);
#endif
