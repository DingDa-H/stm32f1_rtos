/**
 * @brief 		应用层管理层实现
 * @param 		
 * @data 		20260622
 * @note 		已经实现全部功能，正在进行freertos重构。
 * @note 		当前0623实现了真正的-物理按键事件驱动-
 * @note 		记录一个小要点，出了主函数外，menu文件的vMenuEnter也有部分初始化
 */


#include "stm32f1xx_hal.h"
#include "string.h"
#include <stdint.h>
#include "oled_device.h"
#include "keypad4_4.h"
#include "led_device.h"
#include "menu.h"
#include "test.h"
#include "ring_buffer_device.h"
#include "interpreter_device.h"
#include "led_app.h"
#include "snake_app.h"
#include "button_mid.h"
#include "app_task.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

extern QueueHandle_t s_xBtnQueue;  			// 全局队列句柄

extern uint8_t s_ucRxComplete;
extern uint8_t s_ucFrameIndex;

static char *s_apsArgv[10];   				// 分词结果指针数组
/**
 * @brief 		LED初始化
 * @param 	
 * @data 		
 * @note 		
 */
void Led_Init(void)
{
	stLedStaticParamTdf stLedInit;
	stLedInit.emLedLevel					= emLedActiveLevel_Low;
	stLedInit.pstGpioBase					= GPIOC;
	stLedInit.usGpioPin						= GPIO_PIN_13;
	vLedParamInit(&stLedInit,LED0);
	
	stLedInit.emLedLevel					= emLedActiveLevel_Low;
	stLedInit.pstGpioBase					= GPIOB;
	stLedInit.usGpioPin						= GPIO_PIN_0;
	vLedParamInit(&stLedInit,LED1);
	
	stLedInit.emLedLevel					= emLedActiveLevel_Low;
	stLedInit.pstGpioBase					= GPIOB;
	stLedInit.usGpioPin						= GPIO_PIN_10;
	vLedParamInit(&stLedInit,LED2);
	
	stLedInit.emLedLevel					= emLedActiveLevel_High;
	stLedInit.pstGpioBase					= GPIOA;
	stLedInit.usGpioPin						= GPIO_PIN_15;
	vLedParamInit(&stLedInit,LED3);
}
/**
 * @brief 		OLED初始化函数
 * @version 	
 * @data 			
 * @note 		
 */
void vOledInit(void)
{
	stOledStaticParamTdf stInit;
	stInit.pstSclGpioBase	= GPIOB;
	stInit.pstSdaGpioBase	= GPIOB;
	stInit.usSclGpioPin		= GPIO_PIN_8;
	stInit.usSdaGpioPin		= GPIO_PIN_9;
	
	vOledDeviceInit(&stInit, OLED);
}

/**
 * @brief 		物理按键初始化
 * @param 	
 * @data 		
 * @note 		
 */
void Button_Init(void)
{
	stBtnStaticParamTdf stBtnInit;
	stBtnInit.emButLevel					= emBtnActiveLevel_Low;
	stBtnInit.pstGpioBase					= GPIOB;
	stBtnInit.ulDebounceMs 					= 20;
	stBtnInit.ulLongPressMs					= 100000;			//不允许长按和双击
	stBtnInit.ulDoubleClickMs				= 10;
	stBtnInit.usGpioPin						= GPIO_PIN_1;
	
	vBtnParamInit(&stBtnInit,BUTTON_DOWN);
	
	stBtnInit.emButLevel					= emBtnActiveLevel_Low;
	stBtnInit.pstGpioBase					= GPIOB;
	stBtnInit.ulDebounceMs 					= 20;
	stBtnInit.ulLongPressMs					= 100000;
	stBtnInit.ulDoubleClickMs				= 10;
	stBtnInit.usGpioPin						= GPIO_PIN_11;
	
	vBtnParamInit(&stBtnInit,BUTTON_UP);
	
	stBtnInit.emButLevel					= emBtnActiveLevel_High;
	stBtnInit.pstGpioBase					= GPIOB;
	stBtnInit.ulDebounceMs 					= 20;
	stBtnInit.ulLongPressMs					= 100000;
	stBtnInit.ulDoubleClickMs				= 10;
	stBtnInit.usGpioPin						= GPIO_PIN_12;
	
	vBtnParamInit(&stBtnInit,BUTTON_ENABLE);
	
	stBtnInit.emButLevel					= emBtnActiveLevel_High;
	stBtnInit.pstGpioBase					= GPIOB;
	stBtnInit.ulDebounceMs 					= 20;
	stBtnInit.ulLongPressMs					= 100000;
	stBtnInit.ulDoubleClickMs				= 200;
	stBtnInit.usGpioPin						= GPIO_PIN_13;
	
	vBtnParamInit(&stBtnInit,BUTTON_CANCEL);
}

/**
 * @brief 		环形缓冲区初始化
 * @note 		
 */
void RingBuf_Init(void)
{
//	stRingBufTdf stRingBuf;
//	stRingBuf.aucBuf			 			= {0};
//	stRingBuf.ucCount						= 0;
//	stRingBuf.ucRead						= 0;
//	stRingBuf.ucWrite						= 0;
	
	stRingBufTdf stRingBuf = {0};
	
	vRingBufParamCopy(&stRingBuf,UART_RX_BUFFER);
}


/**
 * @brief 		贪吃蛇按键事件处理函数
 * @param 		msg:接收的按钮编号以及对应事件
 * @note 		贪吃蛇页面的按键事件处理逻辑函数
 */
void vSnakeGame(stBtnEventMsg msg)
{
	emSnakeGameStuTdf emSnakeGameStu = emGetSnakeGameCurrentStu();

	if(emSnakeGameStu == emSnakeGameStu_Running)
	{
		if (msg.eEvent == emBtnEvent_Click)
			{
                switch (msg.ucBtnId) {
                    case BUTTON_UP:    	vSetDirUp(); 		break;
                    case BUTTON_DOWN:  	vSetDirDown(); 		break;
                    case BUTTON_ENABLE: vSetDirLeft(); 		break;
					case BUTTON_CANCEL: vSetDirRight();		break;
                    default: break;
                }
            }
		
		if (msg.eEvent == emBtnEvent_DoubleClick && msg.ucBtnId == BUTTON_CANCEL)
		{
			vSnakeSetStu(emSnakeGameStu_Idle);
			vSnakeInit();
			vMenuCancel();
		}
	}
	else
	{
		if (msg.eEvent == emBtnEvent_Click)
		{
                switch (msg.ucBtnId) {
                    case BUTTON_UP:    	vUibuttonSelectUp(); 		break;
                    case BUTTON_DOWN:  	vUibuttonSelectDown(); 		break;
                    case BUTTON_ENABLE: vUibuttonEnter(); 			break;
					case BUTTON_CANCEL: 
						vSnakeSetStu(emSnakeGameStu_Idle);
						vSnakeInit();
						vMenuCancel();								break;
                    default: break;
                }
         }
	}
}


/**
 * @brief 		物理按键事件驱动
 * @param 		msg:接收的按钮编号以及对应事件
 * @data 		
 * @note 		目前计划实现选择箭头上下移动
 * @note 		目前根据当前页面的不同实现对应的效果
 * @note 		目前会随着页面增多逐渐变得臃肿，考虑优化
 */
// task.c - vMainMenuKeyHandler() 函数修改
void vMainMenuKeyHandler(stBtnEventMsg msg)
{
    // 调试：打印事件状态（可通过串口输出）
    // printf("UP Event: %d, DOWN Event: %d\r\n", emEventUp, emEventDown);	
    // 未触发事件的按键，无需清除（避免干扰状态机）
	
	emAllMenuTdf ePage = emGetCurrentPage();
	uint8_t ucCalcKey;							//提取薄膜按键字符
    switch (ePage) {
        case emMenu:
            if (msg.eEvent == emBtnEvent_Click) {
                switch (msg.ucBtnId) {
                    case BUTTON_UP:    	vMenuSelectUp(); 		break;
                    case BUTTON_DOWN:  	vMenuSelectDown(); 		break;
                    case BUTTON_ENABLE: vMenuEnter(); 			break;
					case BUTTON_CANCEL:
					// 主菜单下无操作（或留空）
					break;
                    default: break;
                }
            }
            break;

        case emMenu_LED:
            if (msg.eEvent == emBtnEvent_Click) {
                switch (msg.ucBtnId) {
                    case BUTTON_UP:    	vUibuttonSelectUp(); 	break;
                    case BUTTON_DOWN:  	vUibuttonSelectDown(); 	break;
                    case BUTTON_ENABLE: vUibuttonEnter(); 		break;
					case BUTTON_CANCEL: vMenuCancel(); 			break;
                    default: break;
                }
            }
            break;

        case emMenu_SnakeGame:
            vSnakeGame(msg);
            break;
		case emMenu_SerialTest:
			if (msg.eEvent == emBtnEvent_Click && msg.ucBtnId == BUTTON_CANCEL)
			{
				//返回主菜单
				vMenuCancel();	
			}
            break;
		case emMenu_Calculator:
			// ==========计算器页面核心改造==========
            // 分支1：薄膜矩阵按键（数字、运算符、等于、删除A）
			if (msg.ucBtnId == 0xFF)
			{
				ucCalcKey = msg.ucMatrixKeyVal;
				vState_machine_run(ucCalcKey); // 送入计算器状态机
			}
			// 分支2：独立取消键，退出计算器
			else if (msg.eEvent == emBtnEvent_Click && msg.ucBtnId == BUTTON_CANCEL)
			{
				//返回主菜单
				vMenuCancel();	
			}
            break;
    }
}

/**
 * @brief 		串口收发
 * @param 		缓存数组，状态标志位
 * @data 		
 * @note 		临时写在这里，先实现
 */
static uint8_t s_aucFrameBuf[RX_BUF_SIZE];  /* 独立的帧缓冲区 */

static uint8_t s_ucFrameIndex;

//接收标志位
uint8_t s_ucRxComplete = 0;

const uint8_t c_aucCmdLedOn[]    		= "LED On 0\r\n";				//led控制
const uint8_t c_aucCmdLedOff[]    		= "LED Off 0\r\n";				//led控制
const uint8_t c_aucCmdLedTurn[]    		= "LED Turn 0\r\n";				//led控制
const uint8_t c_aucCmdCalculator[]    	= "CULATOE STAR\r\n";			//打开计算器


const uint8_t c_aucAckLed[] 			= "LED success\r\n";
extern UART_HandleTypeDef huart1;

/**
 * @brief  UART frame process function
 *         串口数据帧解析/处理
 *         vUartFrameProcess就是用来组装一帧数据的，我规定帧结尾是'\n'
 */
void vUartFrameProcess(void)
{
    uint8_t ucCh;
    while (ucReadOnebyte(&ucCh,UART_RX_BUFFER) == 0) {       /* 环形缓冲区有数据 */
        if (ucCh == '\n') 
		{
            if (s_ucFrameIndex < RX_BUF_SIZE - 1)
            {
                s_aucFrameBuf[s_ucFrameIndex++] = ucCh;
            }
			// 2. 补字符串结束符 \0
            s_aucFrameBuf[s_ucFrameIndex] = '\0';

            s_ucRxComplete = 1;
            s_ucFrameIndex = 0;
            break;            /* 一次处理一帧 */
        } 
		else 
		{
            if (s_ucFrameIndex < RX_BUF_SIZE - 1) {
                s_aucFrameBuf[s_ucFrameIndex++] = ucCh;
            } else {
                /* 单帧过长，丢弃并重置 */
                s_ucFrameIndex = 0;
            }
        }
    }
}










/**
 * @brief  按键扫描后台任务函数
 * @param  pvParameters：RTOS任务入口参数，本任务无入参，未使用
 * @retval 无
 * @note   1. 任务固定10ms周期执行一次按键底层扫描，内置硬件消抖逻辑，维护每个按键状态机
 *         3. 遍历所有按键，若产生有效按键事件，则组装 stBtnEventMsg 消息结构体
 *         4. 通过 xQueueSend 将对应按钮的事件送入全局按键队列 s_xBtnQueue
 *         5. 入队超时设为0（非阻塞发送）：队列满时直接丢弃本次按键，不阻塞按键扫描周期
 *         6. 消息入队后立即清除当前按键事件
 *         7. 上层UI、菜单、贪吃蛇任务通过 xQueueReceive 阻塞读取队列，响应按键操作
 */
void vBtnScanTask(void *pvParameters)
{
    while (1) {
		// 扫描所有物理按键，更新按键事件
        vBtnExecute(); 

        // 遍历全部按键，检查是否产生有效触发事件
        for (uint8_t i = 0; i < BUTTON_NUM; i++) {
            emButEventTdf eEvent = emBtnGetCurEvent((emButDevNumTdf)i);
            // 存在有效按键事件（非无操作）
            if (eEvent != emBtnEvent_None) {
                // 组装按键消息：按键编号 + 事件类型（短按/长按等）
                stBtnEventMsg msg = { .ucBtnId = (emButDevNumTdf)i, .eEvent = eEvent};
                // 非阻塞入队，超时0：队列满不等待，保证10ms扫描周期不被拉长
                xQueueSend(s_xBtnQueue, &msg, 0);
                // 清除当前按键事件，避免循环重复上报同一按键动作
                vBtnEventClear((emButDevNumTdf)i);
            }
        }
		
		// 2. 新增：薄膜4x4矩阵按键扫描（计算器专用数字/符号）
        	uint8_t ucKey;
			ucKey = ucKeypad_scan();						//调用扫描函数并返回按键

        if (ucKey != 0) 									// 有按键按下，A/+/-/*/=/0~9
        {
			stBtnEventMsg msg;								// 复用消息结构体，新增枚举区分矩阵按键
            msg.ucBtnId = 0xFF; 							// 自定义标记：代表薄膜矩阵按键
            msg.ucMatrixKeyVal = ucKey; 					// 扩展stBtnEventMsg结构体，存放字符
            msg.eEvent = emBtnEvent_Click;
            xQueueSend(s_xBtnQueue, &msg, 0);
        }
		
		// 获取剩余栈空间--可用串口打印出来
//		volatile UBaseType_t uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
		
        // 延时10ms，控制按键扫描刷新率，兼顾消抖与CPU占用
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

// 新建一个显示任务，优先级可以低一些，比如 1
void vDisplayTask(void *pvParameters)
{
    while (1) {
        vCurrentPageShow();                			// 每30ms刷新一次屏幕
        vTaskDelay(pdMS_TO_TICKS(30));     			// 周期性让出CPU
    }
}


void vAppTask(void *pvParameters)
{
    stBtnEventMsg msg;  						 	// 用于接收事件
    while (1) {
        if (xQueueReceive(s_xBtnQueue, &msg, portMAX_DELAY) == pdTRUE) {
            // 根据 msg.ucBtnId 和 msg.eEvent 处理
            vMainMenuKeyHandler(msg);      			// 可能需要传入 msg 参数
        }
    }
}

void vSerialTask(void *pvParameters) 
{
    while (1) {
        // 阻塞等待通知（0 开销）
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
		
		// 唤醒后立刻清空缓冲区，丢弃进入页面前堆积的数据
        vRingBufClear(UART_RX_BUFFER);
        s_ucRxComplete = 0;
        s_ucFrameIndex = 0;
		
        // 收到通知后，持续轮询，直到页面切换
        while (emGetCurrentPage() == emMenu_SerialTest)
		{
            vUartFrameProcess();
           if (s_ucRxComplete) {
			/* 1. 分词 */
			uint8_t ucTokenCount = ucFenCi((char *)s_aucFrameBuf, s_apsArgv, 10);
			
			/* 2. 查表 + 3. 回调（使用全局注册的命令表） */
			vBianliCmdList(s_apsArgv);
			
			s_ucRxComplete = 0;
			}
		   
            vTaskDelay(pdMS_TO_TICKS(20));
        }
    }
}





/**
 * @brief 		执行函数函数待优化
 * @version 	
 * @data 			
 * @note 		实现加减乘除的思路，状态机/
 */
//void vTask(void)
//{
//	if(emGetCurrentPage() == emMenu_Calculator)
//		vTest();								// 薄膜按键执行函数
//	if(emGetCurrentPage() == emMenu_SerialTest)
//	{
//		vUartFrameProcess();					// 串口数据帧解析/处理
//		if (s_ucRxComplete) {
//			/* 1. 分词 */
//			uint8_t ucTokenCount = ucFenCi((char *)s_aucFrameBuf, s_apsArgv, 10);
//			
//			/* 2. 查表 + 3. 回调（使用全局注册的命令表） */
//			vBianliCmdList(s_apsArgv);
//			
//			s_ucRxComplete = 0;
//		}
//	}


//    vBtnExecute();	   							// 先扫描物理按键,更新事件
//    vMainMenuKeyHandler(); 						// 处理事件
//    vCurrentPageShow();           				// 显示当前页面
//}
