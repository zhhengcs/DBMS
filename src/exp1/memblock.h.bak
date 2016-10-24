//
// Created by devy on 16-10-22.
//

#ifndef DBMS_MEMBLOCK_H
#define DBMS_MEMBLOCK_H
#endif //DBMS_MEMBLOCK_H

#define SIZE_PER_PAGE 8192
#define Max_Table_Size 100
#define MAX_BUFF_SIZE 32

//the tag of every memory block
struct Memtag
{
    long fileid;
    long pageno;
    long isedit;
    long timeofv;
};
//
struct MemBlock
{
    char data[Max_Table_Size][SIZE_PER_PAGE];      //the content of memory block
    struct Memtag blockno[Max_Table_Size];
};

//define a queue
typedef struct Qnode
{
    struct Memtag data;
    struct Qnode *next;
} Qnode,*QueuePtr;

typedef struct
{
    int buffsize;
    QueuePtr front;
    QueuePtr rear;
} linkQueue;



