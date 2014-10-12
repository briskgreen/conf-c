#include "hash/hash.h"
#include "stack/stack.h"
#include <ctype.h>

//解析过程的各状态
enum conf_stat
{
	conf_stat_start,
	conf_stat_normal,
	conf_stat_note,
	conf_stat_key,
	conf_stat_more_key,
	conf_stat_quote,
	conf_stat_done
};

#define POP (*data)
#define ADVANCE (++data,++offset)
#define BACK (--data,--offset)

#define CONF_GET_VALUE(value,stack) \
{\
	if(conf_stack_get_data(stack) == NULL)\
	{\
		retcode=CONF_KEY_ERR;\
		if(conferr)\
			printf("行:%d 列:%d %c",line,offset,POP);\
		goto end;\
	}\
	value=realloc(value,sizeof(char*)*(value_len+2));\
	if(value == NULL)\
	{\
		retcode=CONF_NO_MEM;\
		goto end;\
	}\
	value[value_len]=strdup(conf_stack_get_data(stack));\
	if(value[value_len] == NULL)\
	{\
		retcode=CONF_NO_MEM;\
		free(value);\
		goto end;\
	}\
	value[value_len+1]=NULL;\
	++value_len;\
}

int line=1; //行
int offset=1; //列
int conferr=0;

//解析对参数
//data为配置文件内容
//正确时返回0并置res为参数列表conf->len为参数个数
//错误时返回错误代码
//int parse_value(CONF *conf,char *data,CONF_VALUE **res);
int parse_value(CONF *conf,char *data);
//得到一行的偏移量
int next_line(char *data);
//释放内存
void free_data(CONF_ARG *data);
//得到键
void get_key(char **key,int *index,CONF_ARG *arg);
//读取键值
int _conf_read_key(char *data,STACK *stack);
//获取值状态
enum conf_stat _conf_value_status(char c);
//读取值
int _conf_read_value(char *data,STACK *stack);
//读取带有引号的值
int _conf_read_value_with_quote(char *data,STACK *stack,char quote);
//读取带有引号的键
int _conf_read_key_with_quote(char *data,STACK *stack,char quote);

//打开配置文件并初始化值
//path为配置文件路径
CONF *conf_open(const char *path)
{
	CONF *conf;

	//开辟内存空间
	conf=malloc(sizeof(CONF));
	if(conf == NULL)
	{
		if(conferr)
			printf("申请内存空间出错!\n");
		return NULL;
	}

	//打开配置文件，返回FILE指针
	if((conf->fp=fopen(path,"rb")) == NULL)
	{
		if(conferr)
			printf("打开%s出错!\n",path);
		return NULL;
	}

	//初始化键值对
	conf->len=0;
	conf->size=HASH_DEFAULT;
	conf->hash_data=NULL;

	return conf;
}

//解析配置文件
//解析成功返回0
//解析成功返回错误代码
int conf_parse(CONF *conf)
{
	char *data; //配置文件内容
	long file_len; //配置文件长度
	int retcode;

	//加载配置文件内容到内存
	if(fseek(conf->fp,0L,SEEK_END) != 0)
		return CONF_NO_INIT;
	//得到长度
	file_len=ftell(conf->fp);
	rewind(conf->fp);
	//加载 
	data=malloc(sizeof(char)*file_len+1);
	//最后一个字符为0
	data[file_len]='\0';
	fread(data,file_len,1,conf->fp);
	//关闭文件
	fclose(conf->fp);

	//创建表
	conf->hash_data=malloc(sizeof(CONF_ARG)*conf->size);
	if(conf->hash_data == NULL) return CONF_NO_MEM;
	//初始化键值对
	conf_hash_zero(conf->hash_data,conf->size);
	//解析
	retcode=parse_value(conf,data);
	free(data);
	
	return retcode;
}

//返回键值对个数
int conf_count(CONF *conf)
{
	if(conf->len == 0)
		return CONF_NO_DATA;

	return conf->len;
}

//创建一个配置文件
//path为配置文件路径
CONF_CREATER *conf_creater_new(const char *path)
{
	CONF_CREATER *creater;

	creater=malloc(sizeof(CONF_CREATER));
	if(creater == NULL)
	{
		if(conferr)
			printf("申请内存空间出错!\n");
		return NULL;
	}

	if((creater->fp=fopen(path,"wb")) == NULL)
	{
		if(conferr)
			printf("创建配置文件出错!\n");
		return NULL;
	}

	creater->len=0;
	creater->key=NULL;
	creater->value=NULL;
	creater->note=NULL;
	creater->next=NULL;
}

//插入一个参数到内存
int conf_insert(CONF_CREATER *creater,char *key,char *value,char *note)
{
	//结点
	CONF_CREATER *temp;

	++creater->len;
	//如果是第一个参数，则直接放入
	if(creater->len == 1)
	{
		creater->key=key;
		creater->value=value;
		creater->note=note;
		creater->next=NULL;
		
		return CONF_OK;
	}

	//否则拉下链表
	while(creater->next != NULL)
		creater=creater->next;

	temp=malloc(sizeof(CONF_CREATER));
	if(temp == NULL)
		return CONF_NO_MEM;

	temp->key=key;
	temp->value=value;
	temp->note=note;
	temp->next=NULL;
	creater->next=temp;

	return CONF_OK;
}

//保存内存中的参数到文件
int conf_save(CONF_CREATER *creater)
{
	CONF_CREATER *head=creater;

	if(creater->len == 0)
		return CONF_NO_DATA;

	while(creater != NULL)
	{
		//如果有注释则加入注释以#字开头
		if(creater->note != NULL)
			fprintf(head->fp,"#%s\n",creater->note);
		if(creater->key != NULL && creater->value != NULL)
			fprintf(head->fp,"%s = %s\n",creater->key,creater->value);
		creater=creater->next;
	}

	fclose(head->fp);

	return CONF_OK;
}

//释放CONF内存
void conf_free(CONF *conf)
{
	int i;
	int j;

	for(i=0;i != conf->size;++i)
	{
		if(conf->hash_data[i].len != 0)
			free_data(&conf->hash_data[i]);
	}

	free(conf->hash_data);
	free(conf);
	conf=NULL;
}

//释放CONF_CREATER内存
void conf_creater_free(CONF_CREATER *creater)
{
	CONF_CREATER *head;

	while(creater)
	{
		head=creater;
		creater=creater->next;

		free(head);
	}
}

//根据错误代码打印错误信息
void conf_error(int errcode)
{
	switch(errcode)
	{
		case CONF_OK:
			printf("成功!\n");
			break;
		case CONF_NO_DATA:
			printf("错误:没有数据!\n");
			break;
		case CONF_NO_INIT:
			printf("错误:数据未初始化!\n");
			break;
		case CONF_NO_MEM:
			printf("错误:申请内存空间出错!\n");
			break;
		case CONF_KEY_ERR:
			printf("错误:错误的键名!\n");
			break;
		case CONF_VALUE_ERR:
			printf("错误:错误的值!\n");
			break;
		case CONF_NO_KEY:
			printf("错误:没有键名!\n");
			break;
		case CONF_NO_VALUE:
			printf("错误:没有值!\n");
			break;
		default:
			printf("未知错误!\n");
	}
}

//解析参数
int parse_value(CONF *conf,char *data)
{
/*	//保存解析字符串的首地址
	char *str=data;*/
	//状态
	int status;
	int save_status;
	int end_status;
	//引号
	char quote;
	//存储字符串的栈
	STACK stack;
	CONF_VALUE *value=NULL;
	int retcode=CONF_OK;
	//多参数个数
	int value_len=0;
	int index;

	//初始化栈和状态
	conf_stack_init(&stack);
	status=conf_stat_start;
	save_status=conf_stat_normal;
	end_status=conf_stat_done;

redo:
	while(POP)
	{
		switch(POP)
		{
			//注释
			case '#':
				if(save_status == conf_stat_normal)
					status=conf_stat_note;
				else if(save_status == conf_stat_key)
				{
					retcode=CONF_NO_VALUE;
					if(conferr)
						printf("行:%d 列:%d %c",line,offset,POP);
					free(value->key);
					free(value);
					conf_free(conf);
					goto end;
				}
				break;
			//读取值
			case '=':
				if(save_status == conf_stat_key)
					status=conf_stat_key;
				else
				{
					retcode=CONF_NO_KEY;
					if(conferr)
						printf("行:%d 列:%d =",line,offset);
					if(value)
					{
						if(value->key)
							free(value->key);
						free(value);
					}
					conf_free(conf);
					goto end;
				}
				break;
			//过滤空白符
			case ' ':
			case '\t':
				if(end_status == conf_stat_done)
					while(isspace(POP))
						ADVANCE;
				goto redo;
			case '\n':
				if(save_status == conf_stat_normal)
					ADVANCE;
				else
				{
					retcode=CONF_NO_VALUE;
					if(conferr)
						printf("行:%d 列:%d %s",line,offset,value->key);
					free(value->key);
					free(value);
					conf_free(conf);
					goto end;
				}
				++line;
				offset=1;
				goto redo;
			case '\0':
				if(status == conf_stat_normal)
				{
					retcode=CONF_NO_VALUE;
					if(conferr)
						printf("行:%d 列:%d %s",line,offset,value->key);
					free(value->key);
					free(value);
					conf_free(conf);
					goto end;
				}
				status=conf_stat_done;
				break;
			case '\'':
			case '"':
				if(status == conf_stat_start && save_status != conf_stat_key)
					status=conf_stat_quote;
				else
				{
					retcode=CONF_KEY_ERR;
					if(conferr)
						printf("行:%d 列:%d %c",line,offset,POP);
					goto end;
				}
				break;
			case ',':
				if(save_status == conf_stat_key || status != conf_stat_quote)
				{
					retcode=CONF_KEY_ERR;
					if(conferr)
						printf("行:%d 列:%d %c",line,offset,POP);
					if(save_status == conf_stat_key)
					{
						free(value->key);
						free(value);
					}
					goto end;
				}
			default:
				if(save_status == conf_stat_key)
				{
					retcode=CONF_NO_VALUE;
					if(conferr)
						printf("行:%d 列:%d %c",line,offset,POP);
					free(value->key);
					free(value);
					conf_free(conf);
					goto end;
				}
				save_status=conf_stat_normal;
				status=conf_stat_start;
		}

		switch(status)
		{
			//过滤注释
			case conf_stat_note:
				data+=next_line(data);
				status=conf_stat_start;
				save_status=conf_stat_normal;
				++line;
				offset=1;
				goto redo;
			//读取键
			case conf_stat_start:
				index=_conf_read_key(data,&stack);
				if(index == -1)
				{
					retcode=CONF_KEY_ERR;
					goto end;
				}
				data+=index;
				value=malloc(sizeof(CONF_VALUE));
				if(value == NULL)
				{
					retcode=CONF_NO_MEM;
					conf_free(conf);
					goto end;
				}
				if(conf_stack_get_data(&stack) == NULL)
				{
					retcode=CONF_NO_KEY;
					if(conferr)
						printf("行:%d 列:%d",line,offset);
					free(value);
					conf_free(conf);
					goto end;
				}
				value->key=strdup(conf_stack_get_data(&stack));
				if(value->key == NULL)
				{
					retcode=CONF_NO_MEM;
					free(value);
					conf_free(conf);
					goto end;
				}
				conf_stack_cleanup(&stack);
				save_status=conf_stat_key;
				end_status=conf_stat_done;
				value->value=NULL;
				goto redo;
			//设置值状态
			case conf_stat_key:
				ADVANCE;
				while((POP == ' ') || (POP == '\t'))
					ADVANCE;
				save_status=conf_stat_key;
				end_status=_conf_value_status(POP);
				break;
			case conf_stat_quote:
				quote=POP;
				ADVANCE;
				index=_conf_read_key_with_quote(data,&stack,quote);
				if(index == -1)
				{
					retcode=CONF_KEY_ERR;
					if(conferr)
						printf("行:%d 列:%d %c",line,offset,POP);
					goto end;
				}
				data+=index;
				value=malloc(sizeof(CONF_VALUE));
				if(value == NULL)
				{
					retcode=CONF_NO_MEM;
					conf_free(conf);
					goto end;
				}
				if(conf_stack_get_data(&stack) == NULL)
				{
					retcode=CONF_NO_KEY;
					if(conferr)
						printf("行:%d 列:%d",line,offset);
					free(value);
					conf_free(conf);
					goto end;
				}
				value->key=strdup(conf_stack_get_data(&stack));
				if(value->key == NULL)
				{
					retcode=CONF_NO_MEM;
					free(value);
					conf_free(conf);
					goto end;
				}
				conf_stack_cleanup(&stack);
				save_status=conf_stat_key;
				end_status=conf_stat_done;
				value->value=NULL;
				ADVANCE;
				goto redo;
			case conf_stat_done:
				retcode=CONF_OK;
				goto end;
		}

value_redo:
		switch(end_status)
		{
			//读取完成，保存数据到表中
			case conf_stat_done:
				if((save_status != conf_stat_key) ||
						(status == conf_stat_key))
				{
					retcode=CONF_NO_VALUE;
					if(conferr)
						printf("行:%d 列:%d %c",line,offset,POP);
					free(value->key);
					free(value);
					conf_free(conf);
					goto end;
				}
				//重置状态
				status=conf_stat_start;
				save_status=conf_stat_normal;
				value_len=0;
				if(conf->len/conf->size >= HASH_SP)
					conf_hash_update(conf);
				if(conf_value_insert(conf->hash_data,value,conf->size))
					++conf->len;
				BACK;
				value=NULL;
				conf_stack_cleanup(&stack);
				break;
			//读取值
			case conf_stat_normal:
				index=_conf_read_value(data,&stack);
				if(index == -1)
				{
					retcode=CONF_VALUE_ERR;
					goto end;
				}
				data+=index;
				CONF_GET_VALUE(value->value,&stack);
				while(isspace(POP) && (POP != '\n'))
					ADVANCE;
				end_status=_conf_value_status(POP);
				if(end_status == conf_stat_normal || end_status == conf_stat_quote)
				{
					retcode=CONF_VALUE_ERR;
					if(conferr)
						printf("行:%d 列:%d %c",line,offset,POP);
					goto end;
				}
				status=conf_stat_start;
				goto value_redo;
			//读取带有引号的值
			case conf_stat_quote:
				quote=POP;
				ADVANCE;
				index=_conf_read_value_with_quote(data,&stack,quote);
				if(index == -1)
				{
					retcode=CONF_VALUE_ERR;
					goto end;
				}
				data+=index;
				CONF_GET_VALUE(value->value,&stack);
				ADVANCE;
				while(isspace(POP) && (POP != '\n'))
					ADVANCE;
				end_status=_conf_value_status(POP);
				status=conf_stat_start;
				if(end_status == conf_stat_normal)
				{
					retcode=CONF_VALUE_ERR;
					if(conferr)
						printf("行:%d 列:%d %c",line,offset,POP);
					goto end;
				}
				goto value_redo;
			//多参数值读取
			case conf_stat_more_key:
				conf_stack_cleanup(&stack);
				ADVANCE;
				end_status=_conf_value_status(POP);
				goto value_redo;
		}

		ADVANCE;
	}
end:
	conf_stack_destroy(&stack);
	return retcode;
}

int next_line(char *data)
{
	int i=0;

	while((data[i] != '\n') && (data[i] != '\0'))
		++i;

	return i+1;
}

void free_data(CONF_ARG *data)
{
	int i;

	while(data)
	{
		free(data->value->key);
		for(i=0;data->value->value[i] != NULL;++i)
			free(data->value->value[i]);
		free(data->value->value);
		//free(data->value);

		data=data->next;
	}
}

char **conf_key_list(CONF *conf)
{
	char **key;
	int i;
	int index=0;

	//开辟足够的内存空间
	key=malloc(sizeof(char *)*(conf->len+1));
	if(key == NULL)
		return NULL;

	key[conf->len]=NULL;

	//读取出所有键
	for(i=0;i < conf->size;++i)
		if(conf->hash_data[i].len > 0)
			get_key(key,&index,&conf->hash_data[i]);

	return key;
}

void get_key(char **key,int *index,CONF_ARG *arg)
{
	while(arg != NULL)
	{
		key[*index]=arg->value->key;

		++(*index);
		arg=arg->next;
	}
}

CONF_VALUE *conf_value_get(CONF *conf,const char *key)
{
	int hash;
	CONF_VALUE *value;

	/* 首先使用第一个哈希函数计算出哈希值，如果找到匹配的key则返回
	 * 否则使用第二个哈希函数计算值并比对key
	 * 如果都未能匹配，则返回NULL */
	hash=conf_hash_func1(key)%conf->size;
	if(conf_hash_search(&conf->hash_data[hash],key,&value))
		return value;
	else
	{
		hash=conf_hash_func2(key)%conf->size;
		if(conf_hash_search(&conf->hash_data[hash],key,&value))
			return value;
	}

	return NULL;
}

CONF_VALUE **conf_value_get_all(CONF *conf)
{
	CONF_VALUE **value;
	int len=0;
	int index=0;
	int i;

	if(conf->len == 0)
		return NULL;

	//第一次扫描数组中所有存在数据的地方并计算长度
	for(i=0;i != conf->size;++i)
		if(conf->hash_data[i].len > 0)
			++len;

	//动态申请内存存储返回的数据
	value=malloc(sizeof(CONF_VALUE *)*(len+1));
	if(value == NULL)
		return NULL;
	value[len]=NULL;
	//第二次扫描，返回数组中所有有数据的值
	for(i=0;i != conf->size;++i)
	{
		if(conf->hash_data[i].len > 0)
		{
			value[index]=conf->hash_data[i].value;
			++index;
		}
	}

	return value;
}

void conf_pos_msg_err(int on)
{
	conferr=on;
}

int _conf_read_key(char *data,STACK *stack)
{
	int i=0;
	char c;

	while(isspace(POP) && (POP != '\n'))
	{
		ADVANCE;
		++i;
	}
	if(POP == '\n')
	{
		if(conferr)
			printf("行:%d 列:%d 解析出错!",line,offset);
		return -1;
	}

	do
	{
		c=POP;
		if(isspace(c) || c == '=' || c == '#' || c == ',' || c == '\'' || c == '"')
			break;
		conf_stack_push(stack,c);
		++i;
		ADVANCE;
	}while(c);

	return i;
}

enum conf_stat _conf_value_status(char c)
{
	switch(c)
	{
		case ',':
			return conf_stat_more_key;
		case '\'':
		case '"':
			return conf_stat_quote;
		case '#':
		case '\n':
			return conf_stat_done;
		default:
			return conf_stat_normal;
	}
}

int _conf_read_value(char *data,STACK *stack)
{
	char c;
	int i=0;

	while(isspace(POP) && (POP != '\n'))
	{
		ADVANCE;
		++i;
	}
	if(POP == '\n')
	{
		if(conferr)
			printf("行:%d 列:%d 解析出错!",line,offset);
		return -1;
	}

	do
	{
		c=POP;
		if(isspace(c) || c == ',' || c == '#' || c == '\'' || c == '"')
			break;

		conf_stack_push(stack,c);
		++i;
		ADVANCE;
	}while(c);

	return i;
}

int _conf_read_value_with_quote(char *data,STACK *stack,char quote)
{
	char c;
	int i=0;

	if(POP == '\n')
	{
		if(conferr)
			printf("行:%d 列:%d 解析出错!",line,offset);
		return -1;
	}

	do
	{
		c=POP;
		if(c == quote || c == '\n')
			break;

		conf_stack_push(stack,c);
		++i;
		ADVANCE;
	}while(c);

	if(c == '\0'|| c == '\n')
		return -1;

	return i;
}

int _conf_read_key_with_quote(char *data,STACK *stack,char quote)
{
	char c;
	int i=0;

	if(POP == '\n')
	{
		if(conferr)
			printf("行:%d 列:%d 解析出错!",line,offset);
		return -1;
	}

	do
	{
		c=POP;
		if(c == quote || c == '\n')
			break;

		conf_stack_push(stack,c);
		++i;
		ADVANCE;
	}while(c);

	if(c == '\n' || c == '\0')
		return -1;

	return i;
}
