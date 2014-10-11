#include "hash.h"

/*两个计算哈希值的哈希函数*/
unsigned int conf_hash_func1(const char *key);
unsigned int conf_hash_func2(const char *key);
/* 在hash_data中找出key
 * 如果找到则将值赋给value并返回1
 * 否则返回0 */
int conf_hash_search(CONF_ARG *arg,char *key,CONF_VALUE **value);
//插入数据
void conf_hash_insert(CONF_ARG *arg,CONF_VALUE *value);
//复制哈希表
void _conf_hash_copy(CONF_ARG *arg,CONF_ARG *new,int len);
//检测数据是否在表中
int conf_hash_in(CONF_ARG *arg,char *key,int len);

int conf_value_insert(CONF_ARG *arg,CONF_VALUE *value,int len)
{
	 /* hash为最终的hash值
	 * hash2为临时hash值 */
	int hash,hash2;

	if(conf_hash_in(arg,value->key,len))
	{
		free(value->key);
		while(*value->value)
		{
			free(*value->value);
			++value->value;
		}
		free(value);

		return 0;
	}
	//默认使用第一个哈希函数
	hash=hash2=conf_hash_func1(value->key)%len;

	/* 如果要插入的地方已经有一个值则使用第二个哈希函数计算一个新的哈希值
	 * 如果新的哈希值所在的位置中数据的个数比第一个哈希值所计算的位置少则使用新的值*/
	if(arg[hash].len > 0)
	{
		hash2=conf_hash_func2(value->key)%len;
		if(arg[hash2].len < arg[hash].len)
			hash=hash2;
	}

	//当前位置数据个数加1
	++arg[hash].len;
	//插入数据
	/*while(arg[hash].next != NULL)
		arg[hash]=arg[hash].next;*/
	conf_hash_insert(&arg[hash],value);

	return 1;
	//arg[hash].value=value;
}

//复制哈希表
void conf_hash_copy(CONF_ARG *new,int new_len,CONF_ARG *old,int old_len)
{
	int i;

	for(i=0;i < old_len;++i)
	{
		if(old[i].len)
			_conf_hash_copy(&old[i],new,new_len);
	}
}

//动态更新哈希表
int conf_hash_update(CONF *conf)
{
	CONF_ARG *new;
	int len;

	len=conf->size+conf->size*HASH_SP;
	new=malloc(sizeof(CONF_ARG)*len);
	if(new == NULL) return CONF_NO_MEM;
	conf_hash_zero(new,len);
	conf_hash_copy(new,len,conf->hash_data,conf->size);
	conf->size=len;
	free(conf->hash_data);
	conf->hash_data=new;

	return CONF_OK;
}

//置空哈希表
void conf_hash_zero(CONF_ARG *arg,int len)
{
	int i;

	for(i=0;i < len;++i)
	{
		arg[i].len=0;
		arg[i].value=NULL;
		arg[i].next=NULL;
	}
}

/*BKDR 哈希算法*/
unsigned int conf_hash_func1(const char *key)
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
unsigned int conf_hash_func2(const char *key)
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

int conf_hash_search(CONF_ARG *arg,char *key,CONF_VALUE **value)
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

void conf_hash_insert(CONF_ARG *arg,CONF_VALUE *value)
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

void _conf_hash_copy(CONF_ARG *arg,CONF_ARG *new,int len)
{
	while(arg != NULL)
	{
		conf_value_insert(new,arg->value,len);

		arg=arg->next;
	}
}

int conf_hash_in(CONF_ARG *arg,char *key,int len)
{
	CONF_VALUE *value;
	int hash,hash2;

	hash=hash2=conf_hash_func1(key)%len;
	if(arg[hash].len == 0)
	{
		hash2=conf_hash_func2(key)%len;
		if(arg[hash2].len == 0)
			return 0;
		else if(conf_hash_search(&arg[hash2],key,&value))
			return 1;
		else
			return 0;
	}
	else if(conf_hash_search(&arg[hash],key,&value))
		return 1;
	else
		return 0;
}
