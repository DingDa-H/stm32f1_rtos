/**
 * @brief 		贪吃蛇应用层实现
 * @param 		
 * @data 		20260618
 * @note 		
 */

#include "snake_app.h"
#include "string.h"
#include "button_mid.h"
#include "oled_device.h"
#include "ring_buffer_device.h"
#include "ui_text.h"
#include <stdlib.h>

static uint8_t  s_ucStartPending = 0;          		// 1 表示等待开始
static uint32_t s_ulStartPendingTick = 0;      		// 记录请求开始时的 tick

static stSnakeSegment s_astSnake[SNAKE_MAX_LEN];  	// 蛇身数组
static stSnakeSegment s_dir;  						// 蛇头朝向
static stSnakeSegment s_Food;  						// 食物
static emSnakeGameStuTdf emSnakeGameStu;			// 当前游戏状态
static uint8_t s_ucHead = 0;  						// 蛇头写入位置
static uint8_t s_ucTail = 0;  						// 蛇尾读取位置
static uint8_t s_ucLen  = 0;  						// 当前蛇身长度

/* ========================== 静态函数声明 ========================== */
static void s_vSnakeStarCallback(void *pCtx);
static void s_vSnakeBackCallback(void *pCtx);
static void s_vResetSnakeBody(void);
static void s_vFoodGenerate(void);

/* ========================== LED 按钮定义 ========================== */
// 按钮显示的字符串（从 ui_button.c 移过来）
static uint8_t s_aucSnakeStart[]   	= "Start";
static uint8_t s_aucSnakeBack[]  	= "Back";

// LED 按钮控件实例（从 ui_button.c 移过来，回调指向本文件静态函数）
static stUibuttonItemTdf s_stBtnSnakeStart = {
    .x                     = 64 - (sizeof(s_aucSnakeStart) / sizeof(s_aucSnakeStart[0]) - 1) / 2 * emOledFontSize_6x12 / 2,
    .y                     = 20,
    .ucData                = s_aucSnakeStart,
    .emFontSize            = emOledFontSize_6x12,
    .emSelectStatus        = emUibuttonSelectStatus_True,
    .emUibutIsPressStatus  = emUibuttonIsPress_Flase,
    .emButtonPressStatusLast = emUibuttonIsPress_Flase,
    .uCnt                  = 0,
    .uCntThreshold         = 10,
    .vpfCallback           = s_vSnakeStarCallback,
};

static stUibuttonItemTdf s_stSnakeBack = {
    .x                     = 64 - (sizeof(s_aucSnakeBack) / sizeof(s_aucSnakeBack[0]) - 1) / 2 * emOledFontSize_6x12 / 2,
    .y                     = 44,
    .ucData                = s_aucSnakeBack,
    .emFontSize            = emOledFontSize_6x12,
    .emSelectStatus        = emUibuttonSelectStatus_Flase,
    .emUibutIsPressStatus  = emUibuttonIsPress_Flase,
    .emButtonPressStatusLast = emUibuttonIsPress_Flase,
    .uCnt                  = 0,
    .uCntThreshold         = 10,
    .vpfCallback           = s_vSnakeBackCallback,
};


// 子菜单按钮控件 结构体指针数组（外部可用）
stUibuttonItemTdf *const c_astSnakeButtons[] = {
    &s_stBtnSnakeStart,
    &s_stSnakeBack,
};
const uint8_t c_sucSnakeButtonCount = sizeof(c_astSnakeButtons) / sizeof(c_astSnakeButtons[0]);

/* ========================== UI 按钮回调实现 ========================== */
static void s_vSnakeStarCallback(void *pCtx)
{
    s_vResetSnakeBody();          // 只重置蛇身，保留按钮注册
    s_vFoodGenerate();            // 重新随机食物
    emSnakeGameStu = emSnakeGameStu_Running;
}

static void s_vSnakeBackCallback(void *pCtx)
{
    (void)pCtx;
    emSnakeGameStu = emBackMenu;
}

/**
 * @brief 		返回游戏当前状态函数
 * @retval 		emSnakeGameStuTdf：贪吃蛇状态
 */
emSnakeGameStuTdf emGetSnakeGameCurrentStu(void)
{
	return emSnakeGameStu;
}
/**
 * @brief 		设置游戏当前状态函数
 * @retval 		emSnakeGameStuTdf：贪吃蛇状态
 */
void vSnakeSetStu(emSnakeGameStuTdf emStu) {
    emSnakeGameStu = emStu;
}
/**
 * @brief 		蛇身重置函数
 * @retval 		用于仅重置蛇身的情况
 */
static void s_vResetSnakeBody(void)
{
    s_ucHead = 0;
    s_ucTail = 0;
    s_ucLen = 3;

    s_dir.x = 1;    // 初始向右
    s_dir.y = 0;

    // 按尾->头的顺序写入
    s_astSnake[0].x = 3;  s_astSnake[0].y = 3;
    s_astSnake[1].x = 4;  s_astSnake[1].y = 3;
    s_astSnake[2].x = 5;  s_astSnake[2].y = 3;
    s_ucHead = 3;
    s_ucTail = 0;
}
/**
 * @brief 		贪吃蛇初始化函数
 * @param 	
 * @retval 		
 * @note 		
 */
void vSnakeInit(void)
{
	emSnakeGameStu = emSnakeGameStu_Idle;						// 初始化游戏s状态
	srand(HAL_GetTick());                               		// 只调用一次，一般在游戏初始化时
	s_vFoodGenerate();								    		// 食物生成
	vUibuttonInit(c_astSnakeButtons, c_sucSnakeButtonCount);	// 注册游戏ui按钮
	
    s_vResetSnakeBody();										// 蛇身重置
}

/**
 * @brief 		写入一个字节
 * @param 	
 * @rev 		0-成功	1-失败
 * @note 		向缓冲区写入字节
 */
uint8_t ucWriteOnestSnakeParm(stSnakeSegment stSnakeParm)
{
	if(s_ucLen >= SNAKE_MAX_LEN)
	{
		return 1;
	}
	
	s_astSnake[s_ucHead ++] = stSnakeParm;
	if(s_ucHead >= SNAKE_MAX_LEN)
	{
		s_ucHead = 0;
	}
	s_ucLen++;
	return 0;
}

/**
 * @brief 		读取一个字节
 * @param 		pucByte：用于存储读取元素缓冲区数组
 * @rev 		0-成功	1-失败
 * @note 		从缓冲区读取字节
 */
uint8_t ucReadOnestSnakeParm(stSnakeSegment *pucByte)
{
	if (s_ucLen == 0)   // 空，无数据可读
    return 1;

	*pucByte = s_astSnake[s_ucTail ++];
	if (s_ucTail >= SNAKE_MAX_LEN) 
	{
		s_ucTail = 0;
	}
	s_ucLen--;
	return 0;
}

/**
 * @brief 		简单方向函数
 * @param 	
 * @note 		待优化
 */
void vSetDirUp(void)    { s_dir.x = 0;  s_dir.y = -1; }
void vSetDirDown(void)  { s_dir.x = 0;  s_dir.y = 1; }
void vSetDirLeft(void)  { s_dir.x = -1; s_dir.y = 0; }
void vSetDirRight(void) { s_dir.x = 1;  s_dir.y = 0; }

/**
 * @brief 		碰撞检测函数
 * @param		x,y:当前蛇头索引的x,y坐标
 * @ret			返回0：没有碰撞，1：碰撞了
 * @note 		检测，蛇头是否撞到蛇身或者墙壁
 */
static uint8_t s_ucIsCollision(int8_t x,int8_t y)
{
	int8_t i,end;
	i = s_ucTail;
	end = s_ucHead;
	if (x < 0 || (uint8_t)x >= OLED_POINT_WIDTH || y < 0 || y >= OLED_POINT_HIGH)
		return 1;
	while(i!=end)
	{
		if(s_astSnake[i].x == x 
			&& s_astSnake[i].y == y)
		{
			return 1;
		}
		i++;
		if (i >= SNAKE_MAX_LEN) {
			i = 0;
		}
	}
	return 0;
}
/**
 * @brief 		食物生成函数
 * @note 		
 */
static void s_vFoodGenerate(void)
{
	s_Food.x = rand() % OLED_POINT_WIDTH;       // 生成食物列坐标
	s_Food.y = rand() % OLED_POINT_HIGH;      	// 生成食物行坐标
	//如果食物和蛇身重合
	while(s_ucIsCollision(s_Food.x, s_Food.y))
	{
		s_Food.x = rand() % OLED_POINT_WIDTH;       // 生成食物列坐标
		s_Food.y = rand() % OLED_POINT_HIGH;      	// 生成食物行坐标
	}
}
/**
 * @brief 		蛇移动函数
 * @note 		
 */
void vSnakeMove(void)
{
    // 1. 获取当前蛇头索引并计算新坐标
    uint8_t ucHeadIdx = (s_ucHead == 0) ? (SNAKE_MAX_LEN - 1) : (s_ucHead - 1);
	
    stSnakeSegment stNewHead;
    stNewHead.x = s_astSnake[ucHeadIdx].x + s_dir.x;
    stNewHead.y = s_astSnake[ucHeadIdx].y + s_dir.y;

    // 碰撞检测
	if (s_ucIsCollision(stNewHead.x, stNewHead.y)) {
		emSnakeGameStu = emSnakeGameStu_Fail;
    // 游戏结束，例如设置状态标志或停止移动
		return;
	}
	
    // 2. 写入新蛇头
    if (ucWriteOnestSnakeParm(stNewHead) != 0) {
        // 缓冲区满，长到极限了（可当作胜利或什么也不做）
		emSnakeGameStu = emSnakeGameStu_Win;
        return;
    }

    // 3. 食物判断
	// 3.1 吃到食物
    if (stNewHead.x == s_Food.x && stNewHead.y == s_Food.y)
	{
		s_vFoodGenerate();
	}
    else { stSnakeSegment stDummy; ucReadOnestSnakeParm(&stDummy); }
}
/**
 * @brief 		绘制函数---游戏实现函数
 * @param 	
 * @retval 		
 * @note 		绘制蛇、食物,以及定时移动蛇（设置蛇移动速度）
 */
void DrawSnakeGame()
{
	static uint32_t s_ulLastMoveTick = 0;
	uint32_t t = HAL_GetTick();
	if((t - s_ulLastMoveTick) >= SNAKE_MOVE_SPEED)
	{
		vSnakeMove();											//蛇的移动
		s_ulLastMoveTick = t;
	}
	vOledDrawOnePointToBuffer(s_Food.x,s_Food.y,OLED);			//绘制食物
	
	uint8_t ucIdx = s_ucTail;
	while (ucIdx != s_ucHead) {									//绘制蛇身
		vOledDrawOnePointToBuffer(s_astSnake[ucIdx].x, s_astSnake[ucIdx].y, OLED);
		ucIdx++;
		if (ucIdx >= SNAKE_MAX_LEN) {
			ucIdx = 0;
		}
	}
}
