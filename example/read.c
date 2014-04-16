//包含头文件
#include <conf-c/conf.h>

void print_key(char **key)
{
	int i=0;

	printf("all key is:\n");
	//遍历
	while(key[i] != NULL)
	{
		printf("key : %s\n",key[i]);
		++i;
	}
}

void print_all(CONF_VALUE **value)
{
	int i;
	int j;

	printf("all:\n");
	//遍历
	for(i=0;value[i] != NULL;++i)
	{
		j=0;
		printf("key:%s ",value[i]->key);

		while(value[i]->value[j] != NULL)
		{
			printf(" value:%s",value[i]->value[j]);
			++j;
		}

		printf("\n");
	}
}

int main(int argc,char **argv)
{
	CONF *conf; //需要的数据结构
	CONF_VALUE *value; //键/值参数数据结构
	char **key; //获取所有键
	CONF_VALUE **list; //获取所有键/值参数
	int code; //返回的错误代码

	//打开并初始化数据结构
	//出错时返回NULL
	if((conf=conf_open("configrc")) == NULL)
		return -1;

	//开始解析配置文件
	//成功时返回0
	//出错时返回错误代码
	//错误代码可使用conf_error函数打印错误信息
	if((code=conf_parse(conf)) != 0)
	{
		conf_error(code);
		return -2;
	}

	//取出一个参数
	value=conf_value_get(conf,"arg1");
	//如果有该参数时返回
	//否则返回一个NULL
	if(value)
		printf("arg1 is %s\n",value->value[0]);

	//多参数
	value=conf_value_get(conf,"arg2");
	if(value)
		printf("arg2 is %s %s\n",value->value[0],value->value[1]);
	
	//有空白符的参数
	value=conf_value_get(conf,"arg3");
	if(value)
		printf("arg3 is %s\n",value->value[0]);
	
	//特殊符号
	value=conf_value_get(conf,"arg4");
	if(value)
		printf("arg4 is %s\n",value->value[0]);

	//多参数加特殊符号
	value=conf_value_get(conf,"arg5");
	if(value)
		printf("arg5 is %s %s %s\n",value->value[0],value->value[1],value->value[2]);

	//得到当前配置文件中键的个数
	printf("key has %d\n",conf_count(conf));

	//得到所有键
	//出错时返回NULL
	key=conf_key_list(conf);
	if(key)
		print_key(key);

	//得到所有键/值参数
	//出错时返回NULL
	list=conf_value_get_all(conf);
	if(list)
		print_all(list);

	//释放内存
	conf_free(conf);

	return 0;
}
