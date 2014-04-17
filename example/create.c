//包含头文件
#include <conf-c/conf.h>

int main(int argc,char **argv)
{
	CONF_CREATER *conf; //需要的数据结构

	//初始化数据结构
	//出错则返回NULL
	if((conf=conf_creater_new("newrc")) == NULL)
		return -1;

	//写配置
	//条二个参数为键
	//第三个参数为参数，这里注意格式
	//第四个参数为注释
	//成功时返回0，出错时返回错误代码
	//使用conf_error函数可打印错误代码的信息
	
	conf_insert(conf,NULL,NULL,"这是一个测试文件\n");
	conf_insert(conf,"arg1","abc","单个参数");
	conf_insert(conf,"arg2","abc,def","多个参数");
	conf_insert(conf,"arg3","\'abc def\'","空白符");
	conf_insert(conf,"arg4","\"abc \' def\"","特殊符号");
	conf_insert(conf,"arg5","\"abc,def\",\"#this is value\",hello\n","多参数，特殊符号");
	conf_insert(conf,NULL,NULL,"文件结束");

	//保存配置文件
	conf_save(conf);
	//释放内存
	conf_creater_free(conf);

	return 0;
}
