#include "stdio.h"
#include "string.h" 
long getConfig(char *name);
long Find(char *name, char *data);
int Search(char *name, char *data);
long GetNum(char* cnum);
int main()
{
/*	char name[50];
	scanf("%s",name);
	long n = match(name);
	printf("%ld",n); */
	return 0;
}

long getConfig(char *name){  //通过输入字符串，去配置文件查找对应的数值 
	char data[1024], *buf ,*p;
	long num = -1;
	FILE *fp;
	fp = fopen("config.txt","r"); //打开文档，位置 D:\Project\RDBMS\DBMS\mingw2.95 
	if(!fp){
		printf("can't open config file!");
		return -1;
	}
	while(1){
		p = fgets(data,sizeof(data),fp);		
		if(!p){//全部查询完毕没有找到 
			break;
		}
	//	printf("%s",data);  //获得数字 
		if(Search(name,data)){ //是否在该行存在，不存在 则进行下行寻找，存在则处理 
			num = Find(name,data); 
			break;
		}
	}
	fclose(fp);
	return num;
}
long Find(char *name, char *data){ //返回具体对应配置名的数值 
	long sum = 1;
//	printf("%s\n",name);
//	printf("%s   length = %d\n",data,strlen(name));
	char *p = data+strlen(name)+1;//p 跨越'='号指向数值 
//	printf("%s\n",p);
	char *q;//q指向*号 或者末尾 
//	printf("len:%d\n",q-p);
	while(1){        // 
		char cnum[5] = {'\0'};
		q = strchr(p,'*');
		if(!q)
			q = p+strlen(p)-1;
		strncpy(cnum,p,q-p);
		sum = sum * GetNum(cnum);
		if(*q == '\n') //字符串结束 
			break;
		p = q + 1;
		q = strchr(p,'*');
		
		}
	return sum;
} 
int Search(char *name, char *data){ //查找是否在该行数据中 
	if(strstr(data,name) != NULL && *(data + strlen(name)) == '=')
		return 1;
	else
		return 0;
} 
long GetNum(char* cnum){//数字字符串转换成数字 
	long sum = 0;
	while(*cnum != '\0'){
		sum = sum * 10 + *cnum - '0';
		cnum = cnum + 1;
	}
	return sum;
}

 