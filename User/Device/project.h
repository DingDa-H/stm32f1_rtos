/**
 * @file 	project.h
 * @author	
 * @version 
 * @data 	20260507
 * @brief 	工程配置文件
 */
 
#ifndef __PROJECT_H_
#define __PROJECT_H_

#define TRUE				1
#define FALSE				0
	
#define STM32_IC_HAL		"stm32f1xx_hal.h"


#define BTN_DEV_NUM				4						//按钮设备数量
#define BUTTON_UP				emBtnDevNum0
#define BUTTON_DOWN				emBtnDevNum1
#define BUTTON_ENTER			emBtnDevNum2
#define BUTTON_CANCEL			emBtnDevNum3


#define LED_DEV_NUM			5						//led设备数量
#define LED_BanZai			emLedDevNum0
#define LED1				emLedDevNum1
#define LED2				emLedDevNum2
#define LED3				emLedDevNum3
#define LED4				emLedDevNum4

#define DHT11_DEV_NUM		1							//dht11设备数量
#define DHT11				emDht11DevNum0

#define RING_BUFFER_DEV_NUM			1					//环形缓冲区设备数量
#define UART_RX_BUFFER				emRingBufferDevNum0


#define INTERPRETER_DEV_NUM			1					//命令解释器设备数量
#define CMD_INTERPRETER_DILIVERY_NUM_MAX			5		//命令被分割最大段数
#define CMD_INTERPRETER_DILIVERY_STRING				" "		//空格


#define OLED_DEV_NUM				1					// OLED设备数量
#define OLED                        emOledDevNum0		// OLED设备号
#define OLED_POINT_WIDTH			128					//像素宽度
#define OLED_POINT_HIGH				64					//像素高度
#define OLED_BUFFER_WIDTH			OLED_POINT_WIDTH	//OLED一帧数据的行宽度，等于屏幕一行的像素数量
#define OLED_BUFFER_HIGH			OLED_POINT_HIGH/8	//OLED一帧数据的列高度，等于屏幕一列的像素数量/8


#endif