#include "stdio.h"
#include "stdlib.h"
#include "memblock.h"
#include "structfile.h"
//initial a queue
/*int initQueue (struct linkQueue *q)
{
    q->front=q->rear=(QueuePtr)malloc(sizeof(Qnode));
	q->queueSize = 1;
	q->front->buffPageId = 0;
	q->front->buffPageId = 0;
    if (!q->front) exit (0);
    return 0;
}
*/

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
	
	//struct MemBlock *pofm1;	
	//pofm1 = malloc(sizeof(struct MemBlock));  //分配内存 
	/*
	struct linkQueue *Q; //初始化队列 
	Q = malloc(sizeof(struct linkQueue)); 
	initQueue(Q);
	buff->linkQ = malloc(sizeof(struct linkQueue));
	*/
	buff -> currentBuffPage = -1;
	
	int i;  //初始化buff信息 MAX_BUFF_SIZE
	for( i=0; i<MAX_BUFF_SIZE; i++ ) {
 		buff->blockno[i].fileid = -1; 
		buff->blockno[i].pageno = -1;
		buff->blockno[i].isedit = 0;
	}
	
	//buff = pofm1;
	printf("Init the buffer successfully.\n");
};

int buffSwith(struct MemBlock *buff,struct SysFile *sf)
{
/**
 * @brief 淘汰一个内存块，并从文件读入一个新块
 *
 * @param 
 * @return  int 
 *
 * @author Andy
 * @date 2016/10/16
**/
    //if(buff->linkQ->queueSize == MAX_BUFF_SIZE)
    //{
   /* 	buff->currentBuffPage++;
        if(buff->blockno[buff->currentBuffPage].isedit == 1)
        {
            //write the front page to disk; 
        }
   */
		/*
		buff->linkQ->front = buff->linkQ->front->next;
		buff->linkQ->rear = buff->linkQ->rear->next;
		*/
    //}
}

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

    buff->currentBuffPage = (buff->currentBuffPage + 1)%MAX_BUFF_SIZE;
        if(buff->blockno[buff->currentBuffPage].isedit == 1)
        {
            //write the front page to disk; 
        }
    return buff->currentBuffPage;
}

/*
struct MemBlock initMemory()
{
	return allocateBuff();
}
*/