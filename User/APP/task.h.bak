#ifndef __TASK_H
#define __TASK_H

#include "stm32f1xx_hal.h"
#include "project.h"

extern uint8_t aucRxBuf[RX_BUF_SIZE];
extern uint8_t s_ucRxComplete;



// ===================== 函数声明 =====================
void Led_Init(void);
void RingBuf_Init(void);
void Button_Init(void);
void vMainMenuKeyHandler(void);
void vTask(void);

void vProcessReceivedFrame(void);

#endif
