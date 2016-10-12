/**********************************************************************
 > File Name :support.c
 > Author :Devy.Wang
 > Mail :wdw8027@gmail.com
 > Created Time : 2016年10月06日 星期四 20时27分57秒
*********************************************************************/
#define MAXLINE 40000
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int fileread(int BUFLEN,int CRLEN,char *path,char *sinc[][CRLEN])
{
    FILE *fp;
    char buf[BUFLEN];
    char delims[]=",";
    char *array[MAXLINE];
    char *result=NULL;
    int  len=0,i=0,j=0,t=0;
    int h=0;
    fp=fopen(path,"r");
    if(!fp)return -1;
    while(fgets(buf,BUFLEN,fp))//read the strings of txt
    {
         len=strlen(buf);
         array[i]=(char*)malloc(len+1);
         if(!array[i])break;
         strcpy(array[i],buf);
         //erase the line feed
         t=strlen(array[i]);
         array[i++][t-1]=0;
    }
    fclose(fp);
    h=i--;
    while(i>=0)
    {
        result = strtok(array[i],delims);
        j=0;
        while(result != NULL)
        {
            len=strlen(result);
            sinc[i][j]=(char*)malloc(len+1);
            if(!sinc[i][j])break;
            strcpy( sinc[i][j],result);
            result = strtok(NULL,delims);
            j++;
        }
        free(array[i--]);
    }
    return h;
}
