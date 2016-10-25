#include "stdio.h"
#include "structfile.h"
#include "memblock.h"

struct student
{
	int id;
};


int init(struct student *s1)
{
	/*
	struct student *s2;
	s1->id =2;
	*/
	
	s1->id = 2;
}

int main()
{

	//文件 
	createSysFile("../data/system01.dbf", 2*SIZE_PER_PAGE);
 	struct SysFile sf1;
 	readSysFile(&sf1);
	createDataFile("../data/datafile01.dbf", 2*SIZE_PER_PAGE, &sf1);
	saveSysFile(&sf1);	
	DisplayDBInfo(&sf1);

		
	//缓存 
	struct MemBlock *buff;
	buff = malloc(sizeof(struct MemBlock));
	
	printf("aaaa");
	allocateBuff(buff);	  //缓冲区初使化
	
	printf("ok");
	int i, j;
	for(i=0; i<=40; i++)
	{
		j = queryFreeBuff(buff, &sf1);
		printf("%d, ", j);
	};
	
	//写入数据 
	writeDate(&sf1, buff, 1);
	



/*
struct student *s1;
//s1->id = 1;
init(s1);
*/

}