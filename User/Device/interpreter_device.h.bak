#ifndef __RING_BUFFER_DEVICE_H
#define __RING_BUFFER_DEVICE_H


#include "project.h"
#include STM32_IC_HAL

/**
 * @brief 		缓冲区设备数量枚举
 * @param 	
 * @data 			
 * @note 		
 */
typedef enum
{
    emRingBufNum0    	 = 0,
    emRingBufNum1,
	emRingBufNum2,
	emRingBufNum3,
}
emRingBufNumTdf;

/**
 * @brief 		缓冲区数据结构
 * @note 		
 */
typedef struct
{
	uint8_t aucBuf[RX_BUF_SIZE];
	uint8_t ucRead;
	uint8_t ucWrite;

	uint8_t ucCount;							// 当前存储字节数
}
stRingBufTdf;

void vRingBufParamCopy(stRingBufTdf *pststaticInit,emRingBufNumTdf emDevNum);
uint8_t ucReadOnebyte(uint8_t *pucByte);
uint8_t ucWriteOnebyte(uint8_t byte);
#endif
