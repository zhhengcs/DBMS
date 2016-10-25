#include "stdio.h"
#include "structfile.h"
#include "memblock.h"

int writeDate(struct SysFile *sf1, struct MemBlock *buff, long fid)
{

	char *filename;
	filename=sf1->files[fid].fileName;
	
	FILE *fp;
 	fp=fopen(filename, "wb+");
 	
 	int freePageNumber = queryFreeBuff(buff, sf1);
 	
 	struct Page *page1 = (struct Page*)&(buff->data[freePageNumber][0]);
 	struct DataFileHead *dfh;
 	fread(&dfh, sizeof(struct DataFileHead), 1,fp);//把文件头信息读入到缓存

	int j = 0, k = 0;
		for(k=0; k<=246; k++)
		{
			page1->emp[k].eno = k;
			page1->emp[k].weight = 65.43;
			strcpy(page1->emp[k].ename, "Andy");
			
			printf("ENO: %d", page1->emp[k].eno);
			printf("\tWeight : %f\t", page1->emp[k].weight);
			puts(page1->emp[k].ename);
		}
	
	printf("Write a page to file.\n");
 	rewind(fp);
 	fseek(fp, SIZE_PER_PAGE, SEEK_SET);
	fwrite(page1, SIZE_PER_PAGE, 1, fp ); //把数据写回文件 
	fclose(fp);
	//writeNewPageToFile(page1, 1, &sf1, fp);
}