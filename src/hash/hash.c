#include "hash.h"

/*两个计算哈希值的哈希函数*/
unsigned int hash_func1(const char *key);
unsigned int hash_func2(const char *key);
/* 在hash_data中找出key
 * 如果找到则将值赋给value并返回1
 * 否则返回0 */
int hash_search(CONF_ARG *arg,char *key,CONF_VALUE **value);
//插入数据
void hash_insert(CONF_ARG *arg,CONF_VALUE *value);

void conf_value_insert(CONF_ARG *arg,CONF_VALUE *value,int len)
{
	 /* hash为最终的hash值
	 * hash2为临时hash值 */
	int hash,hash2;

	//默认使用第一个哈希函数
	hash=hash2=hash_func1(value->key)%len;

	/* 如果要插入的地方已经有一个值则使用第二个哈希函数计算一个新的哈希值
	 * 如果新的哈希值所在的位置中数据的个数比第一个哈希值所计算的位置少则使用新的值*/
	if(arg[hash].len > 0)
	{
		hash2=hash_func2(value->key)%len;
		if(arg[hash2].len < arg[hash].len)
			hash=hash2;
	}

	//当前位置数据个数加1
	++arg[hash].len;
	//插入数据
	/*while(arg[hash].next != NULL)
		arg[hash]=arg[hash].next;*/
	hash_insert(&arg[hash],value);

	//arg[hash].value=value;
}

/*BKDR 哈希算法*/
unsigned int hash_func1(const char *key)
{
	unsigned int seed=131;
	unsigned int hash=0;

	while(*key)
	{
		hash=hash*seed+*key;

		++key;
	}

	return hash&0x7FFFFFFF;
}

/*ELF 哈希算法*/
unsigned int hash_func2(const char *key)
{
	unsigned int hash=0;
	unsigned int x=0;

	while(*key)
	{
		hash=(hash << 4)+*key;
		++key;

		if((x = hash & 0xF0000000L) != 0)
		{
			hash^=(x>>24);
			hash&=~x;
		}
	}

	return hash&0x7FFFFFFF;
}

int hash_search(CONF_ARG *arg,char *key,CONF_VALUE **value)
{
	//如果该位置没有数据返回0
	if(arg->len == 0)
		return 0;
	//否则进行查找
	while(arg != NULL)
	{
		//如果找到返回
		if(strcmp(arg->value->key,key) == 0)
		{
			*value=arg->value;
			return 1;
		}

		//下一个结点
		arg=arg->next;
	}

	//如果没有找到返回0
	return 0;
}

void hash_insert(CONF_ARG *arg,CONF_VALUE *value)
{
	CONF_ARG *temp;

	if(arg->len == 1)
	{
		arg->value=value;
		arg->next=NULL;
	}
	else
	{
		temp=malloc(sizeof(CONF_ARG));
		temp->value=value;
		temp->next=NULL;

		while(arg->next != NULL)
			arg=arg->next;

		arg->next=temp;
	}
}
