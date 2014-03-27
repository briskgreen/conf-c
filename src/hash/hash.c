#include "hash.h"

/*两个计算哈希值的哈希函数*/
unsigned int hash_func1(const char *key);
unsigned int hash_func2(const char *key);

int conf_value_insert(CONF_ARG *arg)
{
}

CONF_VALUE *conf_value_get(CONF *conf,const char *key)
{}

CONF_VALUE **conf_value_get_all(CONF *conf)
{}

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
