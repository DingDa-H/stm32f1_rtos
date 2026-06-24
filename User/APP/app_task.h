#ifndef __APP_TASK_H
#define __APP_TASK_H

#include "stm32f1xx_hal.h"
#include "project.h"
#include "botton.h"       // 按键中间层


extern uint8_t aucRxBuf[RX_BUF_SIZE];
extern uint8_t s_ucRxComplete;

// 按键事件消息结构体
// 用于队列传递按键完整信息：按键编号 + 按键触发事件类型
typedef struct
{
    uint8_t ucBtnId;     		// 独立物理按键ID，0xFF代表薄膜矩阵按键
    emButEventTdf eEvent;
	
    uint8_t ucMatrixKeyVal;     // 薄膜按键字符 '0'~'9' '+', '-', '*', '/', '=', 'A'
} stBtnEventMsg;

// ===================== 函数声明 =====================
void Led_Init(void);
void vOledInit(void);
void RingBuf_Init(void);
void Button_Init(void);
void vMainMenuKeyHandler(stBtnEventMsg msg);
void vTask(void);

void vProcessReceivedFrame(void);
// ===================== 任务函数声明 =====================
void vBtnScanTask(void *pvParameters);
void vDisplayTask(void *pvParameters);
void vAppTask(void *pvParameters);
void vSerialTask(void *pvParameters);
#endif
