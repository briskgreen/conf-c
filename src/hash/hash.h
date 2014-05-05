//哈希函数库头文件
#ifndef _HASH_H
#define _HASH_H

/*#include <stdio.h>
#include <stdlib.h>
#include <string.h>*/
#include "../conf.h"

//插入数据,arg为hash数组，value为值,len为数组长度
void conf_value_insert(CONF_ARG *arg,CONF_VALUE *value,int len);

#endif
