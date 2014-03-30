#include "conf.h"

//解析对数
//正确时返回0并置value为参数列表conf->len为参数个数
//错误时返回错误代码
int parse_value(CONF *conf,CONF_VALUE **value);

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
	CONF_VALUE **value=NULL; //参数对列表
	char *data; //配置文件内容
	long file_len; //配置文件长度
	int retcode;
	int i;

	//加载配置文件内容到内存
	if(fseek(conf->fp,0L,SEEK_END) == 0)
		return CONF_NO_INIT;
	//得到长度
	file_len=ftell(conf->fp);
	rewind(conf->fp);
	//加载 
	data=malloc(sizeof(char)*file_len+1);
	//最后一个字符为0
	data[file_len]='\0';
	fread(data,len,1,conf->fp);
	//关闭文件
	fclose(fp);

	//解析 
	retcode=parse_value(conf,value);

	//开辟内存空间
	conf->hash_data=malloc(sizeof(CONF_ARG));
	if(conf->hash_data == NULL)
	{
		if(value)
		{
			for(i=0;i != len;++i)
				free(value[i]);

			free(value);
		}
		retcode=CONF_NO_MEM;
	}
	//插入数据
	for(i=0;i != len;++i)
		conf_value_insert(conf->hash_data,&value[i],conf->len);

	free(data);
	free(value);

	return retcode;
}

//返回键值对个数
int conf_count(CONF *conf)
{
	if(conf->conf == 0)
		return CONF_NO_DATA;

	return conf->len;
}

//创建一个配置文件
//path为配置文件路径
CONF_CREATER *conf_creater_new(const char *path)
{
	CONF *creater;

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

	creater->note=NULL;
	creater->next=NULL;
	creater->value=NULL;
}

//插入一个参数到内存
int conf_insert(CONF_CREATER *creater,CONF_VALUE *value,const char *note)
{
	//结点
	CONF_CREATER *temp;

	//如果是第一个参数，则直接放入
	if(creater->next == NULL)
	{
		creater->value=value;
		creater->note=note;
		
		return CONF_OK;
	}

	//否则拉下链表
	while(creater->next != NULL)
		creater=creater->next;

	temp=malloc(sizeof(CONF_CREATER));
	if(temp == NULL)
		return CONF_NO_MEM;
	temp->value=value;
	temp->note=note;
	temp->next=NULL;
	creater->next=temp;

	return CONF_OK;
}

//保存内存中的参数到文件
int conf_save(CONF_CREATER *creater)
{
	int i;

	if(creater->next == NULL)
		return CONF_NO_DATA;

	while(creater->next != NULL)
	{
		//如果有注释则加入注释以#字开头
		if(creater->note != NULL)
			fprintf(creater->fp,"#%s\n",creater->note);
		fprintf(creater->fp,"%s = %s",creater->value->key,creater->value->value[0]);
		//如果该键有多个参数则用,分开
		if(creater->value->value[1] != NULL)
		{
			for(i=1;creater->value->value[i] != NULL;++i)
				fprintf(creater->fp,",%s",creater->value->value[i]);
		}

		fwrite("\n",1,1,creater->fp);
	}

	fclose(creater->fp);

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
		{
			do
			{
				free(conf->hash_data[i].value);

				hash_data[i]=hash_data[i].next;
			}while(hash_data[i].next != NULL);
			
			free(conf->hash_data[i]);
		}
	}

	free(conf->hash_data);
	free(conf);
	conf=NULL;
}

//释放CONF_CREATER内存
void conf_creater_free(CONF_CREATER *creater)
{
	do
	{
		free(creater->value);

		creater=creater->next;
	}while(creater->next != NULL);

	free(creater);
	creater=NULL;
}

//根据错误代码打印错误信息
void conf_error(int errcode)
{
	switch(errcode)
	{
		case CONF_OK:
			printf("成功!\n");
			break;
		case CONF_NO_DATA;
			printf("错误:没有数据!\n");
			break;
		case CONF_NO_INIT:
			printf("错误:数据未初始化!\n");
			break;
		case CONF_NO_MEM:
			printf("错误:申请内存空间出错!\n");
			break;
		default:
			printf("未知错误!\n");
	}
}
