//哈希函数库头文件
#ifndef _HASH_H
#define _HASH_H

#include "../conf.h"

//插入数据,arg为hash数组，value为值,len为数组长度
int conf_value_insert(CONF_ARG *arg,CONF_VALUE *value,int len);

/* 复制哈希表
 * new为新的哈表 
 * new_len为新的哈表的大小
 * old为旧的hash表
 * old_len为旧hash表的大小
 */
void conf_hash_copy(CONF_ARG *new,int new_len,CONF_ARG *old,int old_len);

//动态更新哈希表
int conf_hash_update(CONF *conf);

//置空哈希表
void conf_hash_zero(CONF_ARG *arg,int len);

#endif
