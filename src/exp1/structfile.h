#define SIZE_PER_PAGE 8192
#define Max_Table_Size 100
#define Max_File_Number 100
#define Max_Page_Per_File 1024
#include "io.h"
//#define Page_Per_Segment 128 

#define TRUE 1
#define FALSE 0

#define OK 1
#define ERROR 0 

struct FileMeta
{
	int file_deleted;
	long file_id;
	char fileName[64];  //每个文件名字最长为64位 
};

struct SysFile
{
/*
定义系统文件
系统文件中存放了一些系统信息，还存放了各个数据文件的地址。 
*/  
	long sizePerPage;           //每一个页的大小
	long buffNum;				//缓冲区块数 
	long fileNumber;			//整个数据库包含的文件个数 
    struct FileMeta files[Max_File_Number]; 	//整个数据库所包含的所有数据文件 
};

struct DataFileHead
{
/*
定义数据文件 
*/ 
	long fileId;				//数据文件ID 
	long pageOfFile;			//文件总共包含多少页 
  
  //空闲空间管理 
	long freeCount;             //当前可用的页数 
	char isPageFree[Max_Page_Per_File];	//存放每一个page是否为空 
};


struct EMP
{
	int eno;
	char ename[20];
	float weight;
};
 
struct Page
{
   int pageID;	//page ID 
   long free_size;
   long recordNumber;         //Page里面有多少记录
   struct EMP emp[247];
};

