/*插入字符的栈*/
#ifndef _STACK_H
#define _STACK_H

#include <string.h>
//键和参数的默认最大值
#define MAX_ARG_SIZE 512
/*栈数据结构*/
typedef struct
{
	size_t len; //当前插入字符的数量
	size_t size; //当前栈大小
	char *data; //插入的字符数据
}STACK;

//初始化栈结构
int conf_stack_init(STACK *stack);

//插入字符
int conf_stack_push(STACK *stack,char data);

//得到当前字符长度
int conf_stack_length(STACK *stack);

//判断当前本是否为空
int conf_stack_empty(STACK *stack);

//清理栈数据结构
void conf_stack_cleanup(STACK *stack);

//销毁栈
void conf_stack_destroy(STACK *stack);

//获取栈中字符串
char *conf_stack_get_data(STACK *stack);

#endif
