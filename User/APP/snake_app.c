/**
 * @brief 		贪吃蛇应用层实现
 * @param 		
 * @data 		20260618
 * @data 		20260620--------（改为静态链表）
 * @note 		已经用环形缓冲区搞定了，后面计划用链表重构（单链表可以优化为双链表）
 * @note 		这样可以实现更多功能了，例如“蛇身中间插入”“删除任意一段”，不过我只重构，其他不做了
 */

#include "snake_app.h"
#include "string.h"
#include "button_mid.h"
#include "oled_device.h"
#include "ring_buffer_device.h"
#include "ui_text.h"
#include <stdlib.h>

static stSnakeSegment s_dir;  						// 蛇头朝向
static stSnakeSegment s_Food; 						// 食物
static emSnakeGameStuTdf emSnakeGameStu;			// 当前游戏状态

/* ========================== 链表结构对应的变量 ========================== */

static stSnakeNode s_astPool[SNAKE_MAX_LEN]; 		// 节点池
static int16_t s_sHead = -1;  						// 蛇头索引,-1为特殊值表示没有节点
static int16_t s_sTail = -1;  						// 蛇尾索引,-1为特殊值表示蛇尾
static int16_t s_sFree = 0;   						// 空闲链表头索引
/* ========================== 环形结构对应的变量 ========================== */
static stSnakeSegment s_astSnake[SNAKE_MAX_LEN];  	// 蛇身数组
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
	(void)pCtx;
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
* @note 		链表方向：蛇尾 <- 蛇身 <- 蛇头
 */
//static void s_vResetSnakeBody(void)
//{
///* ========================== 环形结构对应的代码 ========================== */

////    s_ucHead = 0;
////    s_ucTail = 0;
////    s_ucLen = 3;

////    s_dir.x = 1;    // 初始向右
////    s_dir.y = 0;

////    // 按尾->头的顺序写入
////    s_astSnake[0].x = 3;  s_astSnake[0].y = 3;
////    s_astSnake[1].x = 4;  s_astSnake[1].y = 3;
////    s_astSnake[2].x = 5;  s_astSnake[2].y = 3;
////    s_ucHead = 3;
////    s_ucTail = 0;
//	
///* ========================== 链表结构对应的代码 ========================== */

//	//1.初始化空闲链表
//	uint8_t i;
//	for(i = 0;i < SNAKE_MAX_LEN - 1;i++)
//    {
//		s_astPool[i].next = i + 1;
//    }
//	s_astPool[SNAKE_MAX_LEN - 1].next = -1;			//特殊值就行，表示表尾
//	s_sFree = 0;
//	//2.初始化蛇身，假设长度3，向右）
//	
//	s_dir.x = 1;    								// 初始向右
//    s_dir.y = 0;

//	for (int i = 0; i < 3; i++) {
//		/*重点：第0次newIdx是头节点，第一次是1节点，要当作节点来看 */
//		int16_t newIdx = s_sFree;          				// 取出空闲链表头
//		s_sFree = s_astPool[newIdx].next;  				// 空闲链表后移

//		s_astPool[newIdx].x = 3 + i;
//		s_astPool[newIdx].y = 3;
//		s_astPool[newIdx].next = -1;       				// 新节点作为当前蛇头

//		if (s_sHead == -1) {
//			s_sHead = s_sTail = newIdx;    				// 第一个节点，头尾都是它
//		} else {
//			s_astPool[newIdx].next = s_sHead;   		// 新节点指向旧蛇头
//			s_sHead = newIdx;							
//		}
//	}
//}

static void s_vResetSnakeBody(void)
{
	uint8_t i;
	for(i = 0;i < SNAKE_MAX_LEN - 1;i++)
    {
		s_astPool[i].next = i + 1;
        s_astPool[i].prev = -1;
    }
	s_astPool[SNAKE_MAX_LEN - 1].next = -1;
    s_astPool[SNAKE_MAX_LEN - 1].prev = -1;
	s_sFree = 0;

	s_dir.x = 1;
    s_dir.y = 0;

	s_sHead = -1;
    s_sTail = -1;

	for (int i = 0; i < 3; i++) {
		int16_t newIdx = s_sFree;
		s_sFree = s_astPool[newIdx].next;

		s_astPool[newIdx].x = 20 + i;
		s_astPool[newIdx].y = 30;
		s_astPool[newIdx].next = -1;
        s_astPool[newIdx].prev = -1;

		if (s_sHead == -1) {
			s_sHead = s_sTail = newIdx;
		} else {
            // 双向绑定
			s_astPool[newIdx].next = s_sHead;			// 新节点next指向旧头
            s_astPool[s_sHead].prev = newIdx;			// 旧头prev指向新节点
			s_sHead = newIdx;							// 更新头为新节点
		}
	}
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
 * @param		x,y:新蛇头索引的x,y坐标（计算出来的当前将要移动的那一个位置）
 * @ret			返回0：没有碰撞，1：碰撞了
 * @note 		检测，蛇头是否撞到蛇身或者墙壁
 */
static uint8_t s_ucIsCollision(int8_t x,int8_t y)
{
//	int8_t i,end;
//	i = s_ucTail;
//	end = s_ucHead;
//	if (x < 0 || (uint8_t)x >= OLED_POINT_WIDTH || y < 0 || y >= OLED_POINT_HIGH)
//		return 1;
//	while(i!=end)
//	{
//		if(s_astSnake[i].x == x 
//			&& s_astSnake[i].y == y)
//		{
//			return 1;
//		}
//		i++;
//		if (i >= SNAKE_MAX_LEN) {
//			i = 0;
//		}
//	}
//	return 0;
	
/* ========================== 链表结构对应的代码 ========================== */
	if (x < 0 || (uint8_t)x >= OLED_POINT_WIDTH || y < 0 || y >= OLED_POINT_HIGH)
		return 1;
	int16_t idx = s_sHead;
    while (idx != -1) {
//		if (idx == s_sTail) {
//            idx = s_astPool[idx].next;
//            continue;   						// 跳过尾节点
//        }
        if (s_astPool[idx].x == x && s_astPool[idx].y == y) return 1;
        idx = s_astPool[idx].next;
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
//void vSnakeMove(void)
//{
///* ========================== 环形结构对应的代码 ========================== */

//    // 1. 获取当前蛇头索引并计算新坐标
////    uint8_t ucHeadIdx = (s_ucHead == 0) ? (SNAKE_MAX_LEN - 1) : (s_ucHead - 1);
////	
////    stSnakeSegment stNewHead;
////    stNewHead.x = s_astSnake[ucHeadIdx].x + s_dir.x;
////    stNewHead.y = s_astSnake[ucHeadIdx].y + s_dir.y;

////    // 碰撞检测
////	if (s_ucIsCollision(stNewHead.x, stNewHead.y)) {
////		emSnakeGameStu = emSnakeGameStu_Fail;
////    // 游戏结束，例如设置状态标志或停止移动
////		return;
////	}
////	
////    // 2. 写入新蛇头
////    if (ucWriteOnestSnakeParm(stNewHead) != 0) {
////        // 缓冲区满，长到极限了（可当作胜利或什么也不做）
////		emSnakeGameStu = emSnakeGameStu_Win;
////        return;
////    }

////    // 3. 食物判断
////	// 3.1 吃到食物
////    if (stNewHead.x == s_Food.x && stNewHead.y == s_Food.y)
////	{
////		s_vFoodGenerate();
////	}
////    else { stSnakeSegment stDummy; ucReadOnestSnakeParm(&stDummy); }
///* ========================== 链表结构对应的代码 ========================== */
//	int16_t newIdx = s_sFree;          				// 取出空闲链表头
//	s_sFree = s_astPool[newIdx].next;  				// 空闲链表后移
//	
//	if (newIdx == -1)
//	{
//		emSnakeGameStu = emSnakeGameStu_Win;
//		return;										// 节点用尽（游戏胜利）
//	}
//	s_astPool[newIdx].x = s_astPool[s_sHead].x + s_dir.x;
//	s_astPool[newIdx].y = s_astPool[s_sHead].y + s_dir.y;
//	// 碰撞检测
//	if (s_ucIsCollision(s_astPool[newIdx].x, s_astPool[newIdx].y)) {
//		emSnakeGameStu = emSnakeGameStu_Fail;
//		// 归还新节点
//		s_astPool[newIdx].next = s_sFree;
//		s_sFree = newIdx;
//		return;
//	}
//	s_astPool[newIdx].next = s_sHead;				// 检测通过，新节点指向旧头
//	s_sHead = newIdx;
//	
//	
//	// 3. 食物判断
//	// 3.1 吃到食物
//    if (s_astPool[newIdx].x == s_Food.x && s_astPool[newIdx].y == s_Food.y)
//	{
//		s_vFoodGenerate();
//	}
//    else
//	{
//        // 没吃到：删除尾节点，归还空闲链表
//        int16_t oldTail = s_sTail;
//        // 遍历寻找到尾节点的前一个节点
//        if (s_sHead == s_sTail) {        			// 只剩一节（一般不会在正常移动中发生）
//            s_sHead = s_sTail = -1;
//        } else {
//            int16_t cur = s_sHead;					// 因为是单向，所以从头开始
//            while (s_astPool[cur].next != s_sTail) {
//                cur = s_astPool[cur].next;
//            }
//            s_astPool[cur].next = -1;   	 		// 倒数第二个节点指向-1成为新尾
//            s_sTail = cur;
//        }
//        // 回收旧尾节点到空闲链表（头插法回收）
//        s_astPool[oldTail].next = s_sFree;
//        s_sFree = oldTail;
//		 // 无食物，直接用缓存的前驱节点更新尾巴，删除while遍历
//    }
//}

void vSnakeMove(void)
{
	int16_t newIdx = s_sFree;
	s_sFree = s_astPool[newIdx].next;

	if (newIdx == -1)
	{
		emSnakeGameStu = emSnakeGameStu_Win;
		return;
	}
	s_astPool[newIdx].x = s_astPool[s_sHead].x + s_dir.x;
	s_astPool[newIdx].y = s_astPool[s_sHead].y + s_dir.y;

	if (s_ucIsCollision(s_astPool[newIdx].x, s_astPool[newIdx].y)) {
		emSnakeGameStu = emSnakeGameStu_Fail;
		s_astPool[newIdx].next = s_sFree;
		s_sFree = newIdx;
		return;
	}

    // 新节点插入头部，双向绑定
	s_astPool[newIdx].next = s_sHead;
    s_astPool[s_sHead].prev = newIdx;
    s_astPool[newIdx].prev = -1;
	s_sHead = newIdx;

    if (s_astPool[newIdx].x == s_Food.x && s_astPool[newIdx].y == s_Food.y)
	{
		s_vFoodGenerate();
	}
    else
	{
        // 直接删尾，无任何循环
        int16_t oldTail = s_sTail;
        int16_t newTail = s_astPool[oldTail].prev;

        if(newTail == -1)
        {
            s_sHead = s_sTail = -1;
        }
        else
        {
            s_astPool[newTail].next = -1;
            s_sTail = newTail;
        }
        // 回收旧尾
        s_astPool[oldTail].next = s_sFree;
        s_astPool[oldTail].prev = -1;
        s_sFree = oldTail;
    }
}
/**
 * @brief 		绘制函数---游戏实现函数
 * @param 	
 * @retval 		
 * @note 		绘制蛇、食物,以及定时移动蛇（设置蛇移动速度）
 */
void DrawSnakeGame()
{
/* ==========================链表和环形结构通用代码 ========================== */
	static uint32_t s_ulLastMoveTick = 0;
	uint32_t t = HAL_GetTick();
	if((t - s_ulLastMoveTick) >= SNAKE_MOVE_SPEED)
	{
		vSnakeMove();											//蛇的移动
		s_ulLastMoveTick = t;
	}
	vOledDrawOnePointToBuffer(s_Food.x,s_Food.y,OLED);			//绘制食物
/* ========================== 环形结构对应的代码 ========================== */
//	uint8_t ucIdx = s_ucTail;
//	while (ucIdx != s_ucHead) {									//绘制蛇身
//		vOledDrawOnePointToBuffer(s_astSnake[ucIdx].x, s_astSnake[ucIdx].y, OLED);
//		ucIdx++;
//		if (ucIdx >= SNAKE_MAX_LEN) {
//			ucIdx = 0;
//		}
//	}
/* ========================== 链表结构对应的代码 ========================== */

	int16_t ucIdx = s_sHead;
	while (ucIdx != -1) {										//绘制蛇身
		vOledDrawOnePointToBuffer(s_astPool[ucIdx].x, s_astPool[ucIdx].y, OLED);
		ucIdx = s_astPool[ucIdx].next;
	}
	// 更新缓存
}
