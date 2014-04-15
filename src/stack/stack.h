/*插入字符的栈*/
#ifndef _STACK_H
#define _STACK_H

//bzero函数，如果使用的是win32平台则使用memset函数替代
#include <string.h>
#ifdef _WIN32
#define bzero(ptr,size) memset(ptr,0,size)
#endif
//键和参数的最大值
#define MAX_ARG_SIZE 512
#define STACK_MAX -6 //过大的参数
/*栈数据结构*/
typedef struct
{
	int len; //当前插入字符的数量
	char data[MAX_ARG_SIZE]; //插入的字符数据
}STACK;

//初始化栈结构
void stack_init(STACK *stack);

//插入字符
int stack_push(STACK *stack,char data);

//得到当前字符长度
int stack_length(STACK *stack);

//判断当前本是否为空
int stack_empty(STACK *stack);

//清理栈数据结构
void stack_cleanup(STACK *stack);
#endif
