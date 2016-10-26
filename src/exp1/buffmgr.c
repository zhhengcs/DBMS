#include "stdio.h"
#include "stdlib.h"
#include "memblock.h"
#include "structfile.h"


int allocateBuff(struct MemBlock *buff)
{
/**
 * @brief  分配一大块空闲内存，该内存包含很多内存page
 *
 * @param buffSize: 分配的buff大小
 * @return  int 
 *
 * @author Andy
 * @date 2016/10/16
**/
	printf("Init the buffer.\n");
	
	buff -> currentBuffPage = -1;
	
	int i;  //初始化buff信息 MAX_BUFF_SIZE
	for( i=0; i<MAX_BUFF_SIZE; i++ ) {
 		buff->blockno[i].fileid = -1; 
		buff->blockno[i].pageno = -1;
		buff->blockno[i].isedit = 0;
	}
	
	printf("Init the buffer successfully.\n");
};


int queryFreeBuff(struct MemBlock *buff,struct SysFile *sf)
{
/**
 * @brief 查找一个空闲内存块
 *
 * @param
 * @return  int
 *
 * @author Andy
 * @date 2016/10/16
 **/

    buff->currentBuffPage = (buff->currentBuffPage + 1)%MAX_BUFF_SIZE;	//查找空闲内存块时，游标先向前走一位 
        if(buff->blockno[buff->currentBuffPage].isedit == 1)
        {
        	//如果走之后的游标指向的块被使用的话，则把当前块写回文件。 
            //write the front page to disk; 
        }
    //如果走之后的游标指向的块未被使用的话，则可以直接使用，把当前块号直接返回。
    return buff->currentBuffPage;
}

/*
struct MemBlock initMemory()
{
	return allocateBuff();
}
*/
