#include "hash.h"

/*两个计算哈希值的哈希函数*/
int hash_func1(const char *key);
int hash_func2(const char *key);

int conf_value_insert(CONF_ARG *arg)
{
}

CONF_VALUE *conf_value_get(CONF *conf,const char *key)
{}

CONF_VALUE **conf_value_get_all(CONF *conf)
{}

/*BKDR 哈希算法*/
int hash_func1(const char *key)
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

/*AP 哈希算法*/
int hash_func2(const char *key)
{
	unsigned int hash=0;
	int i;

	for(i=0;key[i];++i)
	{
		if((i & 1) == 0)
		{
			hash^=((hash << 7)^*key^(hash >> 3));
			++key;
		}
		else
		{
			hash^=(~((hash << 11)^*key^(hash >> 5)));
			++key;
		}
	}

	return hash&0x7FFFFFFF;
}
