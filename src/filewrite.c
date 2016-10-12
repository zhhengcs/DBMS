/**********************************************************************
 > File Name :filewrite.c
 > Author :Devy.Wang
 > Mail :wdw8027@gmail.com
 > Created Time : 2016年10月06日 星期四 20时27分57秒
*********************************************************************/
#define MAXLINE 40000
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int filewrite(int BUFLEN,int CRLEN,char *path,char *LM[][CRLEN])
{
    FILE *fp;
    int i=0,j=0;
    int flag=0;
    fp = fopen(path, "w");
    if(!fp)
        return -1;
	for(i=0;i<BUFLEN;i++)
	{
		for(j=0;j<CRLEN;j++)
        {
            if(flag==2)
            {
			    fprintf(fp,"%s",LM[i][j]);
                flag=0;
            }
            else
            {
			    fprintf(fp,"%s,",LM[i][j]);
                flag++;

            }
        }
        fprintf(fp,"\n");
	}
		fclose(fp);
    return 0;
}
