/**
 * @brief 		串口收发优化，环形缓冲区的实现
 * @param 		
 * @data 		20260611
 * @note 		不是自己想出来的，我只是知道有这个东西，不过我要自己独立实现
 */
#include "stm32f1xx_hal.h"
#include "string.h"
#include "ring_buffer_device.h"


static stRingBufTdf s_stRingBuf[RING_BUFFER_DEV_NUM];

/// @brief      环形缓冲区参数初始化
///
/// @note
void vRingBufParamCopy(stRingBufTdf *pststaticInit,emRingBufNumTdf emDevNum)
{
	if(NULL == pststaticInit)
	{
		return;
	}
	memcpy(&s_stRingBuf[emDevNum],pststaticInit,sizeof(stRingBufTdf)/sizeof(uint8_t));
}
/**
 * @brief 		写入一个字节
 * @param 	
 * @rev 		0-成功	1-失败
 * @note 		向缓冲区写入字节
 */
uint8_t ucWriteOnebyte(uint8_t byte)
{
	if(s_stRingBuf.ucCount >= RX_BUF_SIZE)
	{
		return 1;
	}
	
	s_stRingBuf.aucBuf[s_stRingBuf.ucWrite ++] = byte;
	if(s_stRingBuf.ucWrite >= RX_BUF_SIZE)
	{
		s_stRingBuf.ucWrite = 0;
	}
	s_stRingBuf.ucCount++;
	return 0;
}

/**
 * @brief 		读取一个字节
 * @param 		aucRxBuf：缓冲区数组
 * @rev 		0-成功	1-失败
 * @note 		从缓冲区读取字节
 */
uint8_t ucReadOnebyte(uint8_t *pucByte)
{
	if (s_stRingBuf.ucCount == 0)   // 空，无数据可读
    return 1;

	*pucByte = s_stRingBuf.aucBuf[s_stRingBuf.ucRead ++];
	if (s_stRingBuf.ucRead >= RX_BUF_SIZE) 
	{
		s_stRingBuf.ucRead = 0;
	}
	s_stRingBuf.ucCount--;
	return 0;
}
