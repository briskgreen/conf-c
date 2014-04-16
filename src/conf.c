#include "conf.h"

//解析对参数
//data为配置文件内容
//正确时返回0并置res为参数列表conf->len为参数个数
//错误时返回错误代码
int parse_value(CONF *conf,char *data,CONF_VALUE **res);
//得到一行的偏移量
int next_line(char *data);
//释放内存
void free_data(CONF_ARG *data);
//得到键
void get_key(char **key,int *index,CONF_ARG *arg);

//打开配置文件并初始化值
//path为配置文件路径
CONF *conf_open(const char *path)
{
	CONF *conf;

	//开辟内存空间
	conf=malloc(sizeof(CONF));
	if(conf == NULL)
	{
		printf("申请内存空间出错!\n");
		return NULL;
	}

	//打开配置文件，返回FILE指针
	if((conf->fp=fopen(path,"rb")) == NULL)
	{
		printf("打开%s出错!\n",path);
		return NULL;
	}

	//初始化键值对
	conf->len=0;
	conf->hash_data=NULL;

	return conf;
}

//解析配置文件
//解析成功返回0
//解析成功返回错误代码
int conf_parse(CONF *conf)
{
	CONF_VALUE *value=NULL; //参数对列表
	char *data; //配置文件内容
	long file_len; //配置文件长度
	int retcode;
	int i;

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

	//解析 
	retcode=parse_value(conf,data,&value);

	//开辟内存空间
	conf->hash_data=malloc(sizeof(CONF_ARG)*conf->len);
	//开辟内存空间出错
	if(conf->hash_data == NULL)
	{
		//如果已经解析了则进行释放
		if(value)
			free(value);

		//返回错误
		retcode=CONF_NO_MEM;
	}
	
	//初始化数组
	for(i=0;i != conf->len;++i)
	{
		conf->hash_data[i].len=0;
		conf->hash_data[i].next=NULL;
		conf->hash_data[i].value=NULL;
	}
	//插入数据
	for(i=0;i != conf->len;++i)
		conf_value_insert(conf->hash_data,&value[i],conf->len);

	free(data);
	//free(value);

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
		printf("申请内存空间出错!\n");
		return NULL;
	}

	if((creater->fp=fopen(path,"wb")) == NULL)
	{
		printf("创建配置文件出错!\n");
		return NULL;
	}

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

	//如果是第一个参数，则直接放入
	if(creater->key == NULL)
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
//	int i;

	if(creater->key == NULL)
		return CONF_NO_DATA;

	while(creater != NULL)
	{
		//如果有注释则加入注释以#字开头
		if(creater->note != NULL)
			fprintf(head->fp,"#%s\n",creater->note);
		if(creater->key != NULL && creater->value != NULL)
			fprintf(head->fp,"%s = %s\n",creater->key,creater->value);
		creater=creater->next;
		//如果该键有多个参数则用,分开
		/*if(creater->value->value[1] != NULL)
		{
			for(i=1;creater->value->value[i] != NULL;++i)
				fprintf(creater->fp,",%s",creater->value->value[i]);
		}

		fwrite("\n",1,1,creater->fp);*/
	}

	fclose(head->fp);

	return CONF_OK;
}

//释放CONF内存
void conf_free(CONF *conf)
{
	int i;
	int j;

	for(i=0;i != conf->len;++i)
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
		case STACK_MAX:
			printf("参数字符过多!\n");
			break;
		default:
			printf("未知错误!\n");
	}
}

//解析参数
int parse_value(CONF *conf,char *data,CONF_VALUE **res)
{
	int i=0; //字符偏移量
	int flags=0; //'与"的标记
	int key=0; //键标记
	int arg=0; //,标记
	int len=0; //键值对个数
	int count=0; //参数个数
	CONF_VALUE *value=NULL; //存放键值参数
	STACK stack; //存放参数的栈

	stack_init(&stack);

	while(data[i])
	{
		switch(data[i])
		{
			case '=': //读取键，如果第一次读到=则表示键已读完
				if(!key)
				{
					value=realloc(value,sizeof(CONF_VALUE)*(len+1));
					value[len].key=malloc(sizeof(char)*stack_length(&stack)+1);
					snprintf(value[len].key,sizeof(char)*stack_length(&stack)+1,"%s",stack.data);
					key=1;
					value[len].value=NULL;
					stack_cleanup(&stack);
				}
				else
				{
					if(stack_push(&stack,data[i]) != 0) //否则则是值压入栈
						return STACK_MAX;
				}
				break;
/* 如果第一次读到"则设置"标记
 * 如果不是第一次读入"则如果flags为1则表示读取完成
 * 如果为2则表示'标记则将"压入栈中
 */
			case '"':
				if(!flags)
					flags=1;
				else if(flags == 1)
					flags=0;
				else if(flags == 2)
				{
					if(stack_push(&stack,data[i]) != 0)
						return STACK_MAX;
				}
				break;
			case '\'': //上同
				if(!flags)
					flags=2;
				else if(flags == 2)
					flags=0;
				else if(flags == 1)
				{
					if(stack_push(&stack,data[i]) != 0)
						return STACK_MAX;
				}
				break;
/* 多参数标记
 * 如果已设置'/"标记则直接压入
 * 如果没有设置'/"标记且arg未设置则设置arg
 * 否则arg设置为0
 */
			case ',':
				if(flags)
					stack_push(&stack,data[i]);
				if(!flags && !arg)
					arg=1;
				if(arg) //存入多参数
				{
					value[len].value=realloc(value[len].value,sizeof(char *)*(count+1));
					value[len].value[count]=malloc(sizeof(char)*stack_length(&stack)+1);
					snprintf(value[len].value[count],sizeof(char)*stack_length(&stack)+1,"%s",stack.data);
					++count;
					stack_cleanup(&stack);
					arg=0;
				}
				break;
			case '#': //如果没有设置flags则是注释直接跳到下一行
				if(!flags)
					i+=next_line(data+i);
				else
				{
					if(stack_push(&stack,data[i]) != 0)
						return STACK_MAX;
				}
				break;
			case ' ':
			case '\t':
				if(flags)
				{
					if(stack_push(&stack,data[i]) != 0)
						return STACK_MAX;
				}
				break;
			case '\n': //一行数据读完
				if(stack_empty(&stack))
					break;
				value[len].value=realloc(value[len].value,sizeof(char *)*(count+2));
				value[len].value[count]=malloc(sizeof(char)*stack_length(&stack)+1);
				snprintf(value[len].value[count],sizeof(char)*stack_length(&stack)+1,"%s",stack.data);
				value[len].value[count+1]=NULL;
				count=0;
				++len;
				key=0;
				stack_cleanup(&stack);
				break;
			default: //插入字符
				if(stack_push(&stack,data[i]) != 0)
					return STACK_MAX;
		}

		++i;
	}

	*res=value;
	conf->len=len;

	return 0;
}

int next_line(char *data)
{
	int i=0;

	while(data[i] != '\n')
		++i;

	return i;
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
	for(i=0;i < conf->len;++i)
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
