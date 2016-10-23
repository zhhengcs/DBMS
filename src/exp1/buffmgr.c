#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "memblock.h"
#include "structfile.h"
//initial a queue
int initQueue (linkQueue *q)
{
    printf("1");
    
	q -> front = q -> rear= (QueuePtr)malloc(sizeof(Qnode));
    printf("2");
    if (!q->front) exit (0);
    printf("3");
    q->front = q;
    printf("4");
    return 0;
}
//push a element into queue
/*int EnterQueue (linkQnode *q , struct Memtag item)
{
    QueuePtr p;
    p = (QueuePtr) malloc (sizeof (Queue));
    if (!p) exit (0);
    p -> data = item;
    p -> next = NULL;
    q -> rear ->- next = p;
    q -> rear = p;
}
//delete of a element
int DelQueue (linkQnode *q , int *item)
{
    QueuePtr p;
    if (q -> front = q -> rear) return;
    p = q -> front -> next;
    *item = p -> data.pageno;
    q -> front -> next = p -> next;
    if (q -> rear == p)
    q -> rear = q -> front;
    free (p);
}*/


struct MemBlock * allocateBuff(long buffSize)
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
	linkQueue *q;
 	initQueue (q);
	struct MemBlock pofm;
    //pofm = malloc(buffSize+1);
    //initQueue(qofm);
    return &pofm;
 //buffPageNum = buffSize/
}   

int queryFreeBuff(struct MenBlcok *pofm,struct SysFile *sf,linkQueue *q,long fileid,long pageno)
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
    long i=0;
    if(q->buffsize<32)
    {
        q->rear->next=(QueuePtr) malloc (sizeof (Qnode));
        return q->rear->next;
    }
    else
        buffSwith(pofm,sf,q,fileid,pageno);
} 

int buffSwith(struct MemBlock *pofm,struct SysFile *sf,linkQueue *q,long fileid,long pageno)
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
    long tmppageno= q->front->data.pageno;
    if(q->buffsize==32)
    {
        if(q->front->data.isedit == 1)
        {
            char *filename = sf->files[q->front->data.fileid].fileName;
            FILE *fp=fopen(filename, "wb");
            fseek(fp, q->front->data.pageno*SIZE_PER_PAGE, SEEK_SET);
            fwrite(pofm->data, sizeof(pofm->data), 1, fp );
            //ch = '\0';
            fclose(fp);
        }
        readPageFromFile(pageno,fileid,pofm,sf);
    }
    else
    {
        readPageFromFile(pageno,fileid,q->rear,sf);

    }
    //QueuePtr p;
    //if (q -> front = q -> rear) return;
    //p = q -> front -> next;
    //*item = p -> data;
    //q -> front -> next = p -> next;
    //if (q -> rear == p)
     //   q -> rear = q -> front;
    //free (p);

} 


int initMemory()
{
	printf("Initial the buffer!\n");
	//allocateBuff(); 
	struct MemBlock *pofm;
	//struct linkQnode *qofm;
	pofm = allocateBuff(32*SIZE_PER_PAGE);	
	//queryFreeBuff(struct MenBlcok *pofm,struct SysFile *sf,linkQnode *q,long fileid,long pageno)
	/*
	printf("Read a page from file.\n");
	FILE *fp;
	
 	char *filename;
	filename = "../data/abc.txt";
 	fp=fopen(filename, "rb");
	fread(pofm, SIZE_PER_PAGE, 1, fp);//把文件内容读入到缓存
	fclose(fp);
	
	fp=fopen("../data/datafile2.dbf", "wb+");
	fwrite(pofm, SIZE_PER_PAGE, 1, fp ); //把数据写回文件 
 	fclose(fp);
 	*/
}