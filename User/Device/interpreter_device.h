#ifndef __INTERPRETER_DEVICE_H
#define __INTERPRETER_DEVICE_H


#include "project.h"
#include STM32_IC_HAL

/*	回调函数定义（参数待完善）
 *
 */
typedef void vfCmdInterpreterCallbackTdf(char *pcParam);
/**
 * @brief 		缓冲区设备数量枚举
 * @param 	
 * @data 			
 * @note 		
 */
typedef enum
{
    emInterpreterNum0    	 = 0,
    emInterpreterNum1,
	emInterpreterNum2,
	emInterpreterNum3,
}
emInterpreterNumTdf;

/**
 * @brief 		解释器数据结构
* @note 		未拓展，实际应该可以根据参数实现对应功能，例如led闪烁或者呼吸
 */
typedef struct {
    const char  						*c_pcObject;   					// 对象："LED"、"DHT11"、"TIMER"……
    const char  						*c_pcCmd;      					// 命令："ON"、"OFF"、"READ"……
    const char  						*c_pcParam;    					// 参数说明（例如：1号led）
    uint8_t     						 c_pcParamNum; 					// 参数个数
    vfCmdInterpreterCallbackTdf 		*pvfCallBack;					// 回调函数
} stInterpreterTdf;

uint8_t ucFenCi(char *pcString, char *apsTokens[], uint8_t ucMaxTokens);
void vBianliCmdList(char *apsTokens[]);
void vInterpreterRegisterCmdTable(const stInterpreterTdf *pCmdTable, uint8_t ucTableSize);
void vInterpreterInit(void);
#endif
