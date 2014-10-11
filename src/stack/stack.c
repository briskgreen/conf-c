#include "stack.h"
#include "../conf.h"

#define CONF_STACK_UPDATE(stack) \
{\
	stack->data=realloc(stack->data,sizeof(char)*(stack->size+MAX_ARG_SIZE));\
	if(stack->data == NULL)\
		return CONF_NO_MEM;\
	stack->size+=MAX_ARG_SIZE;\
}

//初始化栈
int conf_stack_init(STACK *stack)
{
	//栈置空
	stack->len=0;
	stack->size=MAX_ARG_SIZE;
	stack->data=malloc(sizeof(char)*MAX_ARG_SIZE);
	if(stack->data == NULL) return CONF_NO_MEM;

	return CONF_OK;
}

//插入字符到栈中,data为插入的字符
//成功时返回0，出错时返回错误代码
int conf_stack_push(STACK *stack,char data)
{
	if(stack->len >= stack->size-1)
		CONF_STACK_UPDATE(stack);

	stack->data[stack->len]=data;
	++stack->len;
	stack->data[stack->len]='\0';

	return CONF_OK;
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
	stack->len=0;
}

//销毁栈
void conf_stack_destroy(STACK *stack)
{
	if(stack->data)
		free(stack->data);
}

//获取栈中字符串
char *conf_stack_get_data(STACK *stack)
{
	if(stack->len)
		return stack->data;
	else
		return NULL;
}
