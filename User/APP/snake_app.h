/**
 * @brief 		led相关应用层实现
 * @param 		
 * @data 		20260616
 * @note 		尝试嵌入式分层架构，这是我将目前的部分代码发给ai让她帮我生成整理的
 */

#ifndef __SNAKE_APP_H
#define __SNAKE_APP_H

#include "project.h"

#include STM32_IC_HAL

#include "ui_button.h"

/* 外部可获取的 贪吃蛇 按钮列表指针（如果需要直接在菜单中注册） */
extern stUibuttonItemTdf *const c_astSnakeButtons[];
extern const uint8_t c_sucSnakeButtonCount;


/**
 * @brief 		贪吃蛇蛇身体坐标
 * @note 		
 */
typedef struct {
    int8_t x;
    int8_t y;
} stSnakeSegment;

/* 初始化 Snake 应用层：注册按钮回调、设置蛇身位置、长度、食物等 */
void vSnakeInit(void);

void vSetDirUp(void);
void vSetDirDown(void);
void vSetDirLeft(void);
void vSetDirRight(void);

void DrawSnakeGame(void);				//游戏实现函数
emSnakeGameStuTdf emGetSnakeGameCurrentStu(void);
void vSnakeSetStu(emSnakeGameStuTdf emStu);
#endif
