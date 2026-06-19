#include "stm32f1xx_hal.h"
#include "string.h"
#include "test.h"
#include "oled_device.h"
#include "keypad4_4.h"
#include <stdint.h>

												//static 在这里的作用：限制作用域
static stFsmRunParamTdf s_stCalcParam;  		// 静态，编译器自动清零，但再次调用 Init 重置
//状态处理函数指针定义
typedef emStateTdf efsmCallbackTdf(emEventTdf event,uint8_t key);

//状态处理函数声明
static emStateTdf emHandleInput1State(emEventTdf event,uint8_t key);
static emStateTdf emProcessOperator(emEventTdf event,uint8_t key);
static emStateTdf emHandleInput2State(emEventTdf event,uint8_t key);
static emStateTdf emHandleResultState(emEventTdf event,uint8_t key);

static efsmCallbackTdf *s_apfnFsmCallback[]={emHandleInput1State,emProcessOperator,emHandleInput2State,emHandleResultState};

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
 * @brief 		状态机初始化函数
 * @param 	
 * @data 			
 * @note 		
 */

void vCalcSystemInit(void)
{
    vCalcParamInit(&s_stCalcParam);
}

/**
 * @brief  初始化/重置计算器运行参数
 * @param  pParam  指向 stFsmRunParamTdf 的指针
 */
void vCalcParamInit(stFsmRunParamTdf *pParam)
{
    if (pParam == NULL) return;

    pParam->iOperato_1   	= 0;
    pParam->iOperato_2   	= 0;
    pParam->ucOperator  	= 0;      // 0 表示无运算符
    pParam->lCurrentInput 	= 0;
	pParam->ucCurrentState	= emState_In01;
	vOledWriteOneCharToBuffer(0,EM_FONT_SIZE,'0',EM_FONT_SIZE,emOledPixelShowMode_Positive, OLED);
	vOledRefreshFromBuffer(OLED);
}

/**
 * @brief  字符串转换为有符号32位整数
 * @param  str  输入字符串，可带 '+' 或 '-' 前缀
 * @retval 转换后的有符号整数值（遇到非数字字符停止转换）
 */
int32_t lStringToInt32(char *str)
{
    int32_t lResult = 0;
    int8_t  sign    = 1;          // 默认为正数

    // 处理可选的正负号
    if (*str == '-') {
        sign = -1;
        str++;
    } else if (*str == '+') {
        str++;
    }

    // 连续数字字符转换
    while ((*str >= '0') && (*str <= '9')) {
        lResult = lResult * 10 + (*str - '0');
        str++;
    }

    return lResult * sign;
}


/**
 * @brief  将32位有符号整数转换为十进制字符串
 * @param  lNumber   要转换的整数
 * @param  pucBuf    输出缓冲区（uint8_t 数组）
 * @param  ucBufSize 缓冲区大小（字节数）
 * @retval 实际写入的字符个数（不含结尾 '\0'），缓冲区不足时返回 0
 */
uint8_t ucInt32ToStr(int32_t lNumber, uint8_t *pucBuf, uint8_t ucBufSize)
{
    uint8_t ucIdx = 0;
    uint8_t ucLen = 0;
    uint8_t ucNegative = 0;
    uint32_t ulTemp;

    if (pucBuf == NULL || ucBufSize == 0) {
        return 0;
    }

    /* 处理 0 特殊情况 */
    if (lNumber == 0) {
        pucBuf[0] = '0';
        pucBuf[1] = '\0';
        return 1;
    }

    /* 安全地取绝对值，避免 INT32_MIN 溢出 */
    if (lNumber < 0) {
        ucNegative = 1;
        if (lNumber == INT32_MIN) {
            ulTemp = 2147483648UL;          /* 直接使用其绝对值 */
        } else {
            ulTemp = (uint32_t)(-lNumber);
        }
    } else {
        ulTemp = (uint32_t)lNumber;
    }

    /* 计算所需字符数（含负号） */
    {
        uint32_t ulTmp = ulTemp;
        do {
            ucLen++;
            ulTmp /= 10;
        } while (ulTmp > 0);
    }
    if (ucNegative) {
        ucLen++;
    }

    /* 缓冲区不足，直接截断标记 */
    if (ucLen + 1 > ucBufSize) {            /* +1 为结尾 '\0' */
        pucBuf[0] = '\0';
        return 0;
    }

    /* 从后向前填充数字字符 */
    ucIdx = ucLen;
    pucBuf[ucIdx] = '\0';
    do {
        pucBuf[--ucIdx] = (ulTemp % 10) + '0';
        ulTemp /= 10;
    } while (ulTemp > 0);

    if (ucNegative) {
        pucBuf[0] = '-';
    }

    return ucLen;
}


/**
 * @brief 		返回事件函数
 * @param 	
 * @data 			
* @note 		根据按下不同的按键返回对应的事件
 */
emEventTdf emReturnEvent(uint8_t ucKey)
{
	if(ucKey == 'A')
		return emEvent_Del;
	else if(ucKey == '+' || ucKey == '-' || ucKey == '/' || ucKey == '*')
		return emEvent_Operator;
	else if(ucKey == '=')
		return emEvent_Sum;
	else
		return emEvent_Int;
}
/**
 * @brief 		显示函数
 * @param 	
 * @data 			
 * @note 		用于显示当前输入字符
 */
uint8_t ucOledShowNumber(int32_t num)
{
    uint8_t buf[16],t;
    t = ucInt32ToStr(num, buf, sizeof(buf));

    uint8_t blank[] = "        ";
	
    vOledWriteStringToBuffer(0,EM_FONT_SIZE, blank, EM_FONT_SIZE,emOledPixelShowMode_Positive,OLED);
	vOledWriteStringToBuffer(0,EM_FONT_SIZE, buf, EM_FONT_SIZE,emOledPixelShowMode_Positive,OLED);
	return t;
}

/**
 * @brief 		求和函数
 * @param 	
 * @data 			
 * @note 		用于各个状态下需要求和的事件
 */
int32_t iEventSum(void)
{
	if(s_stCalcParam.ucOperator == '+')
	{
		return s_stCalcParam.iOperato_1 + s_stCalcParam.iOperato_2;
	}
	else if(s_stCalcParam.ucOperator == '-')
	{
		return s_stCalcParam.iOperato_1 - s_stCalcParam.iOperato_2;
	}
	else if(s_stCalcParam.ucOperator == '*')
	{
		return s_stCalcParam.iOperato_1 * s_stCalcParam.iOperato_2;
	}
	else if(s_stCalcParam.ucOperator == '/')
	{
		return s_stCalcParam.iOperato_1 / s_stCalcParam.iOperato_2;
	}
	return 9999;
}


/**
 * @brief 		输入1状态的事件处理函数
 * @param 	
 * @data 			
 * @note 		输入事件：获取当前值处理后显示
 * @note 		运算符事件：跳转到运算符状态，并实现运算符事件的对应逻辑，不然需要按两次
 * @note 		退格事件：删除一个字符
 * @note 		
 * @note 		
 * @note 		
 * @note 		
 */
static emStateTdf emHandleInput1State(emEventTdf event,uint8_t key)
{	
	switch (event)
		{
			case emEvent_Int:															//输入1事件
			{
				s_stCalcParam.lCurrentInput = s_stCalcParam.lCurrentInput * 10 + (key - '0');
				ucOledShowNumber(s_stCalcParam.lCurrentInput);
			}
				break;
			case emEvent_Operator:														//输入运算符事件
			{
				s_stCalcParam.iOperato_1 = s_stCalcParam.lCurrentInput;
				s_stCalcParam.lCurrentInput = 0;
				
				s_stCalcParam.ucOperator = key;
				uint8_t t;
				t = ucOledShowNumber(s_stCalcParam.iOperato_1);				
				vOledWriteOneCharToBuffer(EM_FONT_SIZE*t/2, EM_FONT_SIZE, s_stCalcParam.ucOperator, EM_FONT_SIZE, emOledPixelShowMode_Positive, OLED);
				return emState_Operator;
			}
				break;
			case emEvent_Del:															//退格事件
			{
				s_stCalcParam.lCurrentInput /= 10;
				ucOledShowNumber(s_stCalcParam.lCurrentInput);					
			}
				break;
			default:
				break;
		}
		return s_stCalcParam.ucCurrentState;
}

/**
 * @brief 		输入运算符状态的事件处理函数
 * @param 	
 * @data 			
 * @note 		输入事件：跳转到输入状态2，并实现一次输入事件逻辑，防止按键按两次
 * @note 		运算事件：在操作数1后面显示对应运算符号
 * @note 		求和事件：回到输入状态1，清除运算符，并实现一下输入事件逻辑，显示当前值

 */
static emStateTdf emProcessOperator(emEventTdf event,uint8_t key)
{
	switch (event)
		{
			case emEvent_Int:
			{
				
				s_stCalcParam.lCurrentInput = s_stCalcParam.lCurrentInput * 10 + (key - '0');
				
				s_stCalcParam.iOperato_2 = s_stCalcParam.lCurrentInput;
				ucOledShowNumber(s_stCalcParam.lCurrentInput);		
				return emState_In02;
			}
				break;
			case emEvent_Operator:
			{
				s_stCalcParam.ucOperator = key;
				uint8_t t;
				t = ucOledShowNumber(s_stCalcParam.iOperato_1);	
						
				vOledWriteOneCharToBuffer(EM_FONT_SIZE*t/2, EM_FONT_SIZE, s_stCalcParam.ucOperator, EM_FONT_SIZE, emOledPixelShowMode_Positive, OLED);
			}
				break;
			case emEvent_Sum:
			{
				
				s_stCalcParam.lCurrentInput = s_stCalcParam.iOperato_1;
				s_stCalcParam.iOperato_1 = 0;
				ucOledShowNumber(s_stCalcParam.lCurrentInput);	
				return emState_In01;
			}
				break;
			default:
				break;
		}
		return s_stCalcParam.ucCurrentState;
}

/**
 * @brief 		输入2状态的事件处理函数
 * @param 	
 * @data 			
 * @note 		输入事件：获取当前值处理后显示
 * @note 		运算符事件：将求和的值赋值给操作数1，然后显示操作数和运算符，并且跳转到运算状态
 * @note 		退格事件：删除一个字符
 * @note 		求和事件：求和，当前值赋值给操作数2，求和值赋值给操作数1，跳转到求和状态，显示求和的值
 */
 
static emStateTdf emHandleInput2State(emEventTdf event,uint8_t key)
{
	switch (event)
		{
			case emEvent_Int:
			{
				s_stCalcParam.lCurrentInput = s_stCalcParam.lCurrentInput * 10 + (key - '0');
				ucOledShowNumber(s_stCalcParam.lCurrentInput);	
			}
				break;
			case emEvent_Operator:
			{
				s_stCalcParam.iOperato_2 = s_stCalcParam.lCurrentInput;
				
				s_stCalcParam.lCurrentInput = 0;
				
				s_stCalcParam.iOperato_1 = iEventSum();
				
				s_stCalcParam.ucOperator = key;
				uint8_t t;
				t = ucOledShowNumber(s_stCalcParam.iOperato_1);	
				
				vOledWriteOneCharToBuffer(EM_FONT_SIZE*t/2, EM_FONT_SIZE, s_stCalcParam.ucOperator, EM_FONT_SIZE, emOledPixelShowMode_Positive, OLED);

				//求和，将求和的值赋值给操作数1，然后显示操作数和运算符，并且跳转到运算状态
				return emState_Operator;
				
			}
				break;
			case emEvent_Sum:
			{
				//求和，将当前值赋值给操作数2，清零当前值
				//将求和的值赋值给操作数1，然后显示求和的值也就是操作数1，并且跳转到求和状态
				
				s_stCalcParam.iOperato_2 = s_stCalcParam.lCurrentInput;
				
				s_stCalcParam.lCurrentInput = 0;
				
				s_stCalcParam.iOperato_1 = iEventSum();
				
				ucOledShowNumber(s_stCalcParam.iOperato_1);				

				return emState_Sum;

			}
				break;
			case emEvent_Del:														//退格事件
			{
				s_stCalcParam.lCurrentInput /= 10;
				ucOledShowNumber(s_stCalcParam.lCurrentInput);			
			}
				break;
			default:
				break;
		}
		return s_stCalcParam.ucCurrentState;
}
 /**
 * @brief 		输入求和状态的事件处理函数
 * @param 		event：当前事件	key：按键的值
 * @data 			
 * @note 		输入数字的事件：  求和状态下，应该回到输入1状态，
 *				并且初始化运行参数，将当前按键的值赋值给当前输入（lCurrentInput

				输入运算符的事件：求和状态下，应该回到运算符状态，
 *				清空操作数2，将当前按键赋值给ucOperator：输入运算符
 */

static emStateTdf emHandleResultState(emEventTdf event,uint8_t key)
{
	switch (event)
		{
			case emEvent_Int:
			{
				int t = key;
				vCalcParamInit(&s_stCalcParam);
				s_stCalcParam.lCurrentInput = s_stCalcParam.lCurrentInput * 10 + (t - '0');
				ucOledShowNumber(s_stCalcParam.lCurrentInput);
				return emState_In01;
	
			}
				break;
			case emEvent_Operator:
			{	
				s_stCalcParam.lCurrentInput = 0;
				s_stCalcParam.iOperato_2 = 0;
				
				s_stCalcParam.ucOperator = key;
				uint8_t t;
				t = ucOledShowNumber(s_stCalcParam.iOperato_1);		
						
				vOledWriteOneCharToBuffer(EM_FONT_SIZE*t/2, EM_FONT_SIZE, s_stCalcParam.ucOperator, EM_FONT_SIZE, emOledPixelShowMode_Positive, OLED);
				return emState_Operator;
				
			}
				break;
			
			default:
				break;
		}
		return s_stCalcParam.ucCurrentState;
}


/**
 * @brief 		状态机函数实现
* @param 		key:按键的值
 * @data 			
 * @note 		emReturnEvent(key)返回对应事件
 */
void vState_machine_run(uint8_t key)
{
	emEventTdf event = emReturnEvent(key);
	//获取当前状态
	emStateTdf next_state = s_stCalcParam.ucCurrentState;
	
	/*通过函数指针数组调用当前状态的处理函数并返回对应状态*/
	next_state = s_apfnFsmCallback[next_state](event,key);
	
//	switch (s_stCalcParam.ucCurrentState)
//    {
//    	case emState_In01:							//处理输入1状态的对应事件
//			next_state = emHandleInput1State(event,key);	//状态处理函数
//    		break;
//    	case emState_Operator:						//处理输入运算符状态的对应事件
//			next_state = emProcessOperator(event,key);
//    		break;
//		case emState_In02:							//处理输入2状态的对应事件
//			next_state = emHandleInput2State(event,key);
//    		break;
//		case emState_Sum:							//处理输入求和状态的对应事件
//			next_state = emHandleResultState(event,key);
//    		break;
//    	default:
//    		break;
//    }
	s_stCalcParam.ucCurrentState = next_state;
}


/**
 * @brief 		测试函数待优化
 * @version 	
 * @data 			
 * @note 		实现加减乘除的思路，状态机/
 */

void vTest(void)
{		
	uint8_t ucKey;
	ucKey = ucKeypad_scan();					//调用扫描函数并返回按键
												// 检测到有按键
	if(ucKey != 0)
	{
		if(ucKey != 0)
		{
			vState_machine_run(ucKey);			//轮询状态机
			vOledRefreshFromBuffer(OLED);		//更新缓存	
			
		}		
	}
	
	
	
}
