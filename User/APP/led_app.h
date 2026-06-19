/**
 * @brief 		led相关应用层实现
 * @param 		
 * @data 		20260616
 * @note 		尝试嵌入式分层架构，这是我将目前的部分代码发给ai让她帮我生成整理的
 */

#ifndef __LED_APP_H
#define __LED_APP_H

#include "project.h"

#include STM32_IC_HAL
#include "led_device.h"
#include "ui_button.h"
#include "interpreter_device.h"

/* 外部可获取的 LED 按钮列表指针（如果需要直接在菜单中注册） */
extern stUibuttonItemTdf *const s_astLedButtons[];
extern const uint8_t s_ucLedButtonCount;

/* 初始化 LED 应用层：注册按钮回调、命令表等 */
void vLedAppInit(void);

/* 提供给命令解释器注册 LED 命令表的函数（可选） */
void vLedAppRegisterCommands(void);

#endif
