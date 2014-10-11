/* VERSION : 1.1
 * Date :2014-10-12
 * conf-c一个基于c语言的轻量级读取/创建配置文件的库函数。
 * 该库使用MIT许可协议，被授权人有权利有使用、复制、修改、合并、出版发布、散布、再授权和/或贩售软件及软件的副本，及授予被供应人同等权利，惟服从以下义务。
 * 在软件和软件的所有副本中都必须包含以上版权声明和本许可声明。
 */

#ifndef _CONF_H
#define _CONF_H

/*配置文件中健/值以hash的方式存储在内存中*/
//#include "hash/hash.h"
//#include "stack/stack.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

//定义错误代码
#define CONF_OK 0 //操作成功
#define CONF_NO_DATA -1 //没有数据
#define CONF_NO_INIT -2 //未初始化
#define CONF_NO_MEM -3 //申请内存出错
#define CONF_KEY_ERR -4 //键错误
#define CONF_VALUE_ERR -5 //值错误

#define HASH_DEFAULT 100 //默认哈希表大小
#define HASH_SP 0.7 //当前哈希表中数据个数大于等于70%时表大小动态增长70%

/*值数据结构*/
typedef struct conf_value
{
	char *key;
	char **value;
}CONF_VALUE;

/* 键/值对数据结构
 * len为当前结点个数
 * next为冲突时的链表
 */
typedef struct arg
{
	int len;
	CONF_VALUE *value;

	struct arg *next;
}CONF_ARG;

/* conf-c的数据结构
 * fp为指向配置文件的FILE指针
 * len为键/值对个数
 * size为哈希表大小
 * hash_data为存储键/值的数据结构
 */
typedef struct
{
	FILE *fp;
	size_t len;
	size_t size;
	CONF_ARG *hash_data;
}CONF;

//保存配置文件数据结构
//len为当前参数个数
//key为键
//value值
//note为注释
typedef struct creater
{
	FILE *fp;
	int len;

	char *key;
	char *value;
	char *note;

	struct creater *next;
}CONF_CREATER;
//打开并初始化，path为配置文件的路经，返回CONF数据
CONF *conf_open(const char *path);

//解析配置文件，解析正确返回0，出错返回小于0的错误代码
int conf_parse(CONF *conf);

//获取当前配置文件中键值对个数
int conf_count(CONF *conf);

//得到所有键
char **conf_key_list(CONF *conf);

//创建一个配置文件，path为配置文件的路经
CONF_CREATER *conf_creater_new(const char *path);

/* 添加一个配置键/值对
 * value为键值对
 * note为注释内容
 * 文件中以#开头
 * 返回值:
 * 正确时返回0
 * 错误时返回小于0的错误代码
 */
int conf_insert(CONF_CREATER *creater,char *key,char *value,char *note);

//保存配置文件,正确时返回0，错误时返回小于0的错误代码
int conf_save(CONF_CREATER *creater);

//释放内存
void conf_free(CONF *conf);

//释放CONF_CREATER内存
void conf_creater_free(CONF_CREATER *creater);

//打印错误信息函数,errcode为错误代码
void conf_error(int errcode);

//根据一个键查找数据
CONF_VALUE *conf_value_get(CONF *conf,const char *key);

//得到所有键值对
CONF_VALUE **conf_value_get_all(CONF *conf);

#endif
