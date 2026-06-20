#include "stm32f1xx_hal.h"
#include "string.h"
#include <stdint.h>
#include "oled_device.h"
#include "keypad4_4.h"
#include "button_mid.h"       // 按键中间层
#include "led_device.h"
#include "menu.h"
#include "test.h"
#include "ring_buffer_device.h"
#include "interpreter_device.h"
#include "led_app.h"
#include "snake_app.h"

#include "task.h"


static char *s_apsArgv[10];   // 分词结果指针数组
static uint32_t s_ulLastDispTick = 0;
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
 * @note 		贪吃蛇页面的按键事件处理逻辑函数
 */
void vSnakeGame(void)
{
	emButEventTdf emEventUp  		 = emBtnGetCurEvent(BUTTON_UP);
    emButEventTdf emEventDown		 = emBtnGetCurEvent(BUTTON_DOWN);
	emButEventTdf emEventEnable 	 = emBtnGetCurEvent(BUTTON_ENABLE);
	emButEventTdf emEventCancel   	 = emBtnGetCurEvent(BUTTON_CANCEL);

	emSnakeGameStuTdf emSnakeGameStu = emGetSnakeGameCurrentStu();
	if(emSnakeGameStu == emSnakeGameStu_Running)
	{
		if (emEventUp == emBtnEvent_Click) {
			vSetDirUp();
			// 仅在触发事件后清除，避免提前清除
			vBtnEventClear(BUTTON_UP);
		}
		if (emEventDown == emBtnEvent_Click) {
			vSetDirDown();
			vBtnEventClear(BUTTON_DOWN);
		}
		if (emEventEnable == emBtnEvent_Click) {
			vSetDirLeft();
			vBtnEventClear(BUTTON_ENABLE);
		}
		if (emEventCancel == emBtnEvent_Click) {
			vSetDirRight();
			vBtnEventClear(BUTTON_CANCEL);
		}
		if (emEventCancel == emBtnEvent_DoubleClick) {
			vSnakeSetStu(emSnakeGameStu_Idle);
			vSnakeInit();
			vMenuCancel();
			vBtnEventClear(BUTTON_CANCEL);
		}
	}
	else
	{
		if (emEventUp == emBtnEvent_Click) {
			
			vUibuttonSelectUp();
			// 仅在触发事件后清除，避免提前清除
			vBtnEventClear(BUTTON_UP);
		}
		if (emEventDown == emBtnEvent_Click) {
			vUibuttonSelectDown();
			vBtnEventClear(BUTTON_DOWN);
		}
		if (emEventEnable == emBtnEvent_Click) {
			vBtnEventClear(BUTTON_ENABLE);
			vUibuttonEnter();
		}
		if (emEventCancel == emBtnEvent_Click) {
			//返回主菜单
			vSnakeSetStu(emSnakeGameStu_Idle);
			vSnakeInit();
			vMenuCancel();	
			vBtnEventClear(BUTTON_CANCEL);
		}
		
	}
	
}


/**
 * @brief 		物理按键事件驱动
 * @param 	
 * @data 		
 * @note 		目前计划实现选择箭头上下移动
* @note 		目前根据当前页面的不同实现对应的效果
 * @note 		目前会随着页面增多逐渐变得臃肿，考虑优化
 */
// task.c - vMainMenuKeyHandler() 函数修改
void vMainMenuKeyHandler(void)
{
    emButEventTdf emEventUp  		 = emBtnGetCurEvent(BUTTON_UP);
    emButEventTdf emEventDown		 = emBtnGetCurEvent(BUTTON_DOWN);
	emButEventTdf emEventEnable 	 = emBtnGetCurEvent(BUTTON_ENABLE);
	emButEventTdf emEventCancel   	 = emBtnGetCurEvent(BUTTON_CANCEL);
    // 调试：打印事件状态（可通过串口输出）
    // printf("UP Event: %d, DOWN Event: %d\r\n", emEventUp, emEventDown);
	emAllMenuTdf ePage = emGetCurrentPage();
	
	if(ePage == emMenu)
	{
		if (emEventUp == emBtnEvent_Click) {
			vMenuSelectUp();
			// 仅在触发事件后清除，避免提前清除
			vBtnEventClear(BUTTON_UP);
		}
		if (emEventDown == emBtnEvent_Click) {
			vMenuSelectDown();
			vBtnEventClear(BUTTON_DOWN);
		}
		if (emEventEnable == emBtnEvent_Click) {
			vMenuEnter();
			vBtnEventClear(BUTTON_ENABLE);
		}
		if (emEventCancel == emBtnEvent_Click) {
			//返回主菜单
			vMenuCancel();
			vBtnEventClear(BUTTON_CANCEL);
		}
	}
	else if(ePage == emMenu_LED)
	{
		if (emEventUp == emBtnEvent_Click) {
			
			vUibuttonSelectUp();
			// 仅在触发事件后清除，避免提前清除
			vBtnEventClear(BUTTON_UP);
		}
		if (emEventDown == emBtnEvent_Click) {
			vUibuttonSelectDown();
			vBtnEventClear(BUTTON_DOWN);
		}
		if (emEventEnable == emBtnEvent_Click) {
			vBtnEventClear(BUTTON_ENABLE);
			vUibuttonEnter();
		}
		if (emEventCancel == emBtnEvent_Click) {
			//返回主菜单
			vMenuCancel();	
			vBtnEventClear(BUTTON_CANCEL);
		}
	}
	else if(ePage == emMenu_SnakeGame)
	{
		vSnakeGame();
	}
	else if(ePage == emMenu_SerialTest)
	{
		if (emEventCancel == emBtnEvent_Click) {
			//返回主菜单
			vMenuCancel();	
			vBtnEventClear(BUTTON_CANCEL);
		}
	}
	else if(ePage == emMenu_Calculator)
	{
		if (emEventCancel == emBtnEvent_Click) {
			//返回主菜单
			vMenuCancel();	
			vBtnEventClear(BUTTON_CANCEL);
		}
	}
    // 未触发事件的按键，无需清除（避免干扰状态机）
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
 *         vUartFrameProcess就是用来组装一帧数据的
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
 * @brief 		执行函数函数待优化
 * @version 	
 * @data 			
 * @note 		实现加减乘除的思路，状态机/
 */
void vTask(void)
{
	if(emGetCurrentPage() == emMenu_Calculator)
		vTest();								// 薄膜按键执行函数
	if(emGetCurrentPage() == emMenu_SerialTest)
	{
	//	vCalcAppRegisterCommands();   			// 如果你有计算器应用层
		vUartFrameProcess();
		if (s_ucRxComplete) {
			/* 1. 分词 */
			uint8_t ucTokenCount = ucFenCi((char *)s_aucFrameBuf, s_apsArgv, 10);
			
			/* 2. 查表 + 3. 回调（使用全局注册的命令表） */
			vBianliCmdList(s_apsArgv);
			
			s_ucRxComplete = 0;
		}
	}
	if(emGetCurrentPage() == emMenu_LED)		// 执行led相关初始化
	{
		vLedAppInit();							// 目前只有led页面的ui按钮注册
	}

    vBtnExecute();	   							// 先扫描物理按键
    vMainMenuKeyHandler(); 						// 处理事件
    vCurrentPageShow();           				// 显示当前页面
}
//void vTask(void)
//{
//	uint32_t tNow = HAL_GetTick();

//	if(emGetCurrentPage() == emMenu_Calculator)
//		vTest();							
//	if(emGetCurrentPage() == emMenu_SerialTest)
//	{
//		vUartFrameProcess();
//		if (s_ucRxComplete) {
//			uint8_t ucTokenCount = ucFenCi((char *)s_apsArgv, s_apsArgv, 10);
//			vBianliCmdList(s_apsArgv);
//			s_ucRxComplete = 0;
//		}
//	}
//	if(emGetCurrentPage() == emMenu_LED)		
//	{
//		vLedAppInit();							
//	}

//    // 1. 按键扫描永远最先执行，保证按键不丢
//    vBtnExecute();	   							
//    vMainMenuKeyHandler(); 					

//    // 2. 刷新节流：120ms刷新一次屏幕，和蛇移动速度150ms匹配
//    if(tNow - s_ulLastDispTick >= 120)
//    {
//        vCurrentPageShow();           
//        s_ulLastDispTick = tNow;
//    }
//}