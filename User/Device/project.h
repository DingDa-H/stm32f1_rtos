/**
 * @file 	project.h
 * @author	
 * @version 
 * @data 	20260507
 * @brief 	工程配置文件
 */
 
#ifndef __PROJECT_H_
#define __PROJECT_H_

//行：Row		薄膜按键，还需要优化

//列：Column

#define KEYPAD_Row01				GPIO_PIN_0		//行1，默认为GPIOA
#define KEYPAD_Row02				GPIO_PIN_1
#define KEYPAD_Row03				GPIO_PIN_2
#define KEYPAD_Row04				GPIO_PIN_3
										
#define KEYPAD_Col01				GPIO_PIN_4		//列1，默认为GPIOA
#define KEYPAD_Col02				GPIO_PIN_5
#define KEYPAD_Col03				GPIO_PIN_6
#define KEYPAD_Col04				GPIO_PIN_7
			
#define STM32_IC_HAL				"stm32f1xx_hal.h"
		
#define BUTTON_NUM 					4								//按钮设备数量
#define BUTTON_UP 					emBtnDevNum0					//按钮设备对应功能
#define BUTTON_DOWN 				emBtnDevNum1					
#define BUTTON_ENABLE 				emBtnDevNum2					
#define BUTTON_CANCEL 				emBtnDevNum3					

#define LED_NUM 					4								//led设备数量

#define LED0						emLedDevNum0
#define LED1						emLedDevNum1
#define LED2						emLedDevNum2
#define LED3						emLedDevNum3

#define EM_FONT_SIZE 				emOledFontSize_8x16			//字号大小

#define OLED_DEV_NUM				1					// OLED设备数量
#define OLED                        emOledDevNum0		// OLED设备号
#define OLED_POINT_WIDTH			128					//像素宽度
#define OLED_POINT_HIGH				64					//像素高度
#define OLED_BUFFER_WIDTH			OLED_POINT_WIDTH	//OLED一帧数据的行宽度，等于屏幕一行的像素数量
#define OLED_BUFFER_HIGH			OLED_POINT_HIGH/8	//OLED一帧数据的列高度，等于屏幕一列的像素数量/8

#define RX_BUF_SIZE 				50					//串口接收缓存大小
#define RING_BUFFER_DEV_NUM 		1					//环形缓冲区设备数
#define UART_RX_BUFFER 				emRingBufNum0		//环形缓冲区设备号


#define INTERPRETER_CMD_MAX_NUM     20					/* 全局命令表最大容量（根据项目需要调整） */
#endif
