#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "memblock.h"
#include "structfile.h"
//initial a queue
int initQueue (linkQnode *q)
{
    q -> front = q -> rear = (QueuePtr) malloc (sizeof (Queue));
    if (!q -> front) exit (0);
    q -> front -> next = NULL;
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
    q -> rear -> next = p;
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


int allocateBuff(struct MemBlock *pofm,long buffSize,linkQnode *qofm)
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
    int buffPageNum; //分配的buff包含的块的数量
    pofm=(long *)malloc(buffSize+1);
    initQueue(qofm);
    return 0;
 //buffPageNum = buffSize/
}   

int queryFreeBuff(struct MenBlcok *pofm,struct SysFile *sf,linkQnode *q,long fileid,long pageno)
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
        q->rear->next=(QueuePtr) malloc (sizeof (Queue));
        return q->rear->next;
    }
    else
        buffSwith(pofm,sf,q,fileid,pageno);
} 

int buffSwith(struct MemBlock *pofm,struct SysFile *sf,linkQnode *q,long fileid,long pageno)
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
        readDataFile(pageno,fileid,pofm,sf);
    }
    else
    {
        readDataFile(pageno,fileid,q->rear,sf);

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
