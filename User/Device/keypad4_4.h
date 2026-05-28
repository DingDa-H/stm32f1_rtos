/**
 * @brief 		薄膜矩阵按键4X4
 * @version 	DingDa-H	叮答_H
 * @data 		20260523	
 * @note 		........
 */

#ifndef __KEYPAD4_4_H
#define __KEYPAD4_4_H

#include "stm32f1xx_hal.h"
//行：Row

//列：Column

#define KEYPAD_Row01		GPIO_PIN_0		//行1，默认为GPIOA
#define KEYPAD_Row02		GPIO_PIN_1
#define KEYPAD_Row03		GPIO_PIN_2
#define KEYPAD_Row04		GPIO_PIN_3
                                  
#define KEYPAD_Col01		GPIO_PIN_4		//列1，默认为GPIOA
#define KEYPAD_Col02		GPIO_PIN_5
#define KEYPAD_Col03		GPIO_PIN_6
#define KEYPAD_Col04		GPIO_PIN_7

extern uint16_t KEYPAD_Row[4];

extern uint16_t KEYPAD_Col[4];

uint8_t ucKeypad_scan(void);
#endif
