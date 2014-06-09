#include "stack.h"

//初始化栈
void conf_stack_init(STACK *stack)
{
	//栈置空
	stack->len=0;
	bzero(stack->data,MAX_ARG_SIZE);
}

//插入字符到栈中,data为插入的字符
//成功时返回0，出错时返回错误代码
int conf_stack_push(STACK *stack,char data)
{
	if(stack->len >= MAX_ARG_SIZE)
		return STACK_MAX;

	stack->data[stack->len]=data;
	++stack->len;

	return 0;
}

//获取当前栈中字符个数
int conf_stack_length(STACK *stack)
{
	return stack->len;
}

//判断当前栈是否为空
int conf_stack_empty(STACK *stack)
{
	if(stack->len == 0)
		return 1;
	else
		return 0;
}

//清理栈数据
void conf_stack_cleanup(STACK *stack)
{
	conf_stack_init(stack);
}
