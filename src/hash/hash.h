//哈希函数库头文件
#ifndef _HASH_H
#define _HASH_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* conf-c的数据结构
 * fp为指向配置文件的FILE指针
 * len为键/值对个数
 * conf为存储键/值的hash函数指针
 */
typedef struct
{
	FILE *fp;
	int len;
	conf_hash *conf;
}CONF;

/*值链表数据结构*/
typedef struct node
{
	char *value;
	struct node *next;
}CONF_VALUE;

//插入数据,key为键，value为值
int conf_value_insert(const char *key,CONF_VALUE *value);

//根据一个键查找数据
int conf_value_get(CONF *conf,const char *key);

#endif
