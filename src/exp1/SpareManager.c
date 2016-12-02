#include "stdio.h"
#include "structfile.h"
#include "string.h"


int getFreePage(char *filename)
{
	long freePageAddress; 
	struct DataFile filehead;    //文件头 
	
	FILE *fp;
	fp=fopen(filename,"rb");
	fread(&filehead,sizeof(struct DataFile),1,fp);
	
    int Num=filehead.pageAvail;
    printf("文件的页数%d",Num);
    
    if (Num<=0)
    {
    	return -1;		//如果没有空闲空间可以分配，返回-1 
    }
    else
	{
    		freePageAddress = filehead.pagefreeHead;
			filehead.pageHead=filehead.pagefreeHead;
			filehead.freeCount--;
			filehead.pagefreeTail++;
			filehead.pagefreeHead++;  
    }    
 	fwrite(&filehead, sizeof(struct DataFile), 1, fp); //把数据写回文件 
	fclose(fp);
	return freePageAddress;
	
};
