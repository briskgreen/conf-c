/* conf-c一个基于c语言的轻量级读取/创建配置文件的库函数。
 * 该库使用MIT许可协议，被授权人有权利有使用、复制、修改、合并、出版发布、散布、再授权和/或贩售软件及软件的副本，及授予被供应人同等权利，惟服从以下义务。
 * 在软件和软件的所有副本中都必须包含以上版权声明和本许可声明。
 */

#ifndef _CONF_H
#define _CONF_H

/*配置文件中健/值以hash的方式存储在内存中*/
#include "hash/hash.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

//打开并初始化，path为配置文件的路经，返回CONF数据
CONF *conf_open(const char *path);

//解析配置文件，解析正确返回0，出错返回小于0的错误代码
int conf_parse(CONF *conf);

//获取当前配置文件中键值对个数
int conf_count(CONF *conf);

//创建一个配置文件，path为配置文件的路经
CONF *conf_creater_new(const char *path);

/* 添加一个配置键/值对,key为键
 * value为值
 * note为注释内容
 * 文件中以#开头
 * 返回值:
 * 正确时返回0
 * 错误时返回小于0的错误代码
 */
int conf_insert(CONF *conf,const char *key,CONF_VALUE *value,const char *note);

//保存配置文件,正确时返回0，错误时返回小于0的错误代码
int conf_save(CONF *conf);

//释放内存
void conf_free(CONF *conf);

#endif
