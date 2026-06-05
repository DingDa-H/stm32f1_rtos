/**
 * @brief 		薄膜矩阵按键4X4
 * @version 	DingDa-H	叮答_H
 * @data 		20260523	
 * @note 		........
 */

#include "keypad4_4.h"
#include "project.h"

 
uint16_t KEYPAD_Row[4] = {KEYPAD_Row01,KEYPAD_Row02,KEYPAD_Row03,KEYPAD_Row04};

uint16_t KEYPAD_Col[4] = {KEYPAD_Col01,KEYPAD_Col02,KEYPAD_Col03,KEYPAD_Col04};

/**
 * @brief   优化版4x4矩阵按键扫描（带消抖、松手检测、单键触发）
 * @retval  按下的按键字符，无按键返回0	
 * @data 
 *				        C1   C2   C3   C4   ← 列
						↓    ↓    ↓    ↓
				 R1   → 1    2    3    A
				 R2   → 4    5    6    B
				 R3   → 7    8    9    C
				 R4   → *    0    #    D
  
 * @note 		我设置引脚默认为高,A是删除单个字符，B是 + ,C是 - ,D是 / #是 =
 */
/**
 * @brief  		4x4 矩阵按键扫描（带消抖与松手检测，单次触发）
 * @retval 		返回按下按键的字符（'0'~'9', 'A', '+', '-', '*', '/', '=', 等），无按键时返回 0
 * @note   		该函数采用非阻塞状态机：检测到按键按下后立即返回键值并进入等待释放状态，  
 *         		直到所有按键完全松开并经过释放消抖后才允许再次触发，避免连击。  
 *         		需在主循环中周期性调用（建议周期 10~20ms）。
 */
uint8_t ucKeypad_scan(void)
{
    static uint8_t s_ucKeyState = 0;       // 0=等待按下，1=等待松开
    static uint8_t s_ucReleaseCnt = 0;     // 释放消抖计数
    uint8_t aucKey[4][4] = {0};
    uint8_t i, j;
    uint8_t ucReturnValue = 0;

    /* 行扫描 */
    for (i = 0; i < 4; i++) {
        HAL_GPIO_WritePin(GPIOA, KEYPAD_Row[i], GPIO_PIN_RESET);
        for (j = 0; j < 4; j++) {
            if (HAL_GPIO_ReadPin(GPIOA, KEYPAD_Col[j]) == 0)
                aucKey[i][j] = 1;
        }
        HAL_GPIO_WritePin(GPIOA, KEYPAD_Row[i], GPIO_PIN_SET);
    }

    if (s_ucKeyState == 0) {
        /* 等待按下：检测任意键 */
        if (aucKey[0][0]) ucReturnValue = '1';
        else if (aucKey[0][1]) ucReturnValue = '2';
        else if (aucKey[0][2]) ucReturnValue = '3';
        else if (aucKey[0][3]) ucReturnValue = 'A';
        else if (aucKey[1][0]) ucReturnValue = '4';
        else if (aucKey[1][1]) ucReturnValue = '5';
        else if (aucKey[1][2]) ucReturnValue = '6';
        else if (aucKey[1][3]) ucReturnValue = '+';
        else if (aucKey[2][0]) ucReturnValue = '7';
        else if (aucKey[2][1]) ucReturnValue = '8';
        else if (aucKey[2][2]) ucReturnValue = '9';
        else if (aucKey[2][3]) ucReturnValue = '-';
        else if (aucKey[3][0]) ucReturnValue = '*';
        else if (aucKey[3][1]) ucReturnValue = '0';
        else if (aucKey[3][2]) ucReturnValue = '=';
        else if (aucKey[3][3]) ucReturnValue = '/';

        if (ucReturnValue != 0) {
			//切换状态为等待松开，并且开始计数
            s_ucKeyState = 1;
            s_ucReleaseCnt = 0;
            return ucReturnValue;
        }
    } else {
        /* 等待松开：连续 N 次检测到全释放才回到等待状态 */
        uint8_t ucAllReleased = 1;
        for (i = 0; i < 4; i++) {
            for (j = 0; j < 4; j++) {
                if (aucKey[i][j] == 1) {
                    ucAllReleased = 0;
                    break;
                }
            }
        }
        if (ucAllReleased) 
		{
            s_ucReleaseCnt++;
            if (s_ucReleaseCnt >= 3) 
			{    // 根据调用周期调整，约 50ms
                s_ucKeyState = 0;
                s_ucReleaseCnt = 0;
            }
        } 
		else 
		{
            s_ucReleaseCnt = 0;
        }
    }
    return 0;
}
