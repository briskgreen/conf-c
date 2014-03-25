//哈希函数库头文件
#ifndef _HASH_H
#define _HASH_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*值链表数据结构*/
typedef struct node
{
	char *key
	char *value;
	struct node *next;
}CONF_VALUE;

/* 键/值对数据结构
 * hash为使用的hash函数：0为第一个哈希算法 1：为第二个哈希算法
 * len为当前结点个数
 * next为冲突时的链表
 */
typedef struct arg
{
	int hash;
	int len;
	CONF_VALUE *value;

	struct arg *next;
}CONF_ARG;

/* conf-c的数据结构
 * fp为指向配置文件的FILE指针
 * len为键/值对个数
 * hash_data为存储键/值的数据结构
 */
typedef struct
{
	FILE *fp;
	int len;
	CONF_ARG *hash_data;
}CONF;

//插入数据,key为键，value为值
int conf_value_insert(CONF_ARG *arg);

//根据一个键查找数据
CONF_VALUE *conf_value_get(CONF *conf,const char *key);

//得到所有键值对
CONF_VALUE **conf_value_get_all(CONF *conf); 

#endif
