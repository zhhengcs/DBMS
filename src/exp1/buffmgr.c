#include "stdio.h"
#include "stdlib.h"
#include "memblock.h"
#include "structfile.h"
//initial a queue
int initQueue (linkQueue *q)
{
    q->front=q->rear=(QueuePtr)malloc(sizeof(Qnode));
	//q->front = q->rear= (QueuePtr)malloc(sizeof(Qnode));
    if (!q->front) exit (0);
    return 0;
}

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
	struct MemBlock *pofm;
    pofm = malloc(buffSize+1);
    return pofm;
}   

/*int buffInsert(struct MemBlock *plfm,struct SysFile *sf,linkQueue *q,long fileid,long pageno)
{
    char *filename = sf->files[q->front->data.fileid].fileName;
    FILE *fp =fopen(filename,"r");
}*/

int buffSwith(struct MemBlock *pofm,struct SysFile *sf,linkQueue *q)
       // int buffSwith(struct MemBlock *pofm,struct SysFile *sf,linkQueue *q,long fileid,long pageno)
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
            /*char *filename = sf->files[q->front->data.fileid].fileName;
            FILE *fp=fopen(filename, "wb");
            fseek(fp, q->front->data.pageno*SIZE_PER_PAGE, SEEK_SET);
            fwrite(pofm->data, sizeof(pofm->data), 1, fp );
            //ch = '\0';
            fclose(fp);*/
            内存写回文件函数调用
        }
        else
            return q->front;
        //readPageFromFile(pageno,fileid,pofm,sf);
    }
    else
    {
        //(pageno,fileid,q.rear,sf);
        return q->rear;

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

struct Qnode *queryFreeBuff(struct MemBlock *pofm,struct SysFile *sf,linkQueue *q,long fileid,long pageno)
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
        q->rear->next->data.pageno=pageno;
        return q->rear->next;
        //buffInsert(pofm,sf,q,fileid,pageno);
    }
    else
    {
        buffSwith(pofm,sf,q);

    }

}


int initMemory()
{
	printf("Initial the buffer!\n");
	//allocateBuff(); 
	//struct MemBlock *pofm;
	//struct linkQnode *qofm;
	//pofm = allocateBuff(32*SIZE_PER_PAGE);
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