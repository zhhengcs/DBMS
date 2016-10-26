#include "stdio.h"
#include "structfile.h"
#include "memblock.h"

int main()
{

	//文件 
	createSysFile("../data/system01.dbf", 2*SIZE_PER_PAGE);
 	struct SysFile sf1;
 	readSysFile(&sf1);
	createDataFile("../data/datafile01.dbf", 2*SIZE_PER_PAGE, &sf1);
	saveSysFile(&sf1);	
	DisplayDBInfo(&sf1);

		
	//缓存初使化 
	struct MemBlock *buff;
	buff = malloc(sizeof(struct MemBlock));
	allocateBuff(buff);	  //缓冲区初使化

	//查找空闲缓冲区演示 
	int i, j;
	for(i=0; i<=40; i++)
	{
		j = queryFreeBuff(buff, &sf1);
		printf("%d, ", j);
	};
	printf("\n");
	
	//写入数据 
	writeDate(&sf1, buff, 1);
}
