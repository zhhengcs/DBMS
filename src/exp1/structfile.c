#define SIZE_PER_PAGE 8192
#define Max_Table_Size 100
#define Max_File_Number 1000 
//#define SIZE_BUFF 32
//#define Page_Per_Segment 128 

struct File
{
	int file_deleted;
	char fileName[64];
};

struct SysFile
{
/*
定义系统文件
系统文件中存放了一些系统信息，还存放了各个数据文件的地址。 
*/  
	long sizePerPage;           //每一个页的大小
	long fileNumber;			//整个数据库包含的文件个数 
    struct File files[Max_File_Number]; 	//整个数据库所包含的所有数据文件 
};

struct DataFile  
{
/*
定义数据文件 
*/ 
	long fileId;				//数据文件ID 
	long pageOfFile;			//文件的大小，包含多少页 
  
  //空闲空间管理 
	long freeCount;             //当前可用的页数 
	long bitMapAddr;			//bitMap在存储文件中的地址
	long sizeBitMap;			//bitMap的大小,以字节为单位
	long *data;                 //指向当前的地址
    char *pageDataMemoryHead;   //数据区域头指针
};
/*
struct mapTable                 //逻辑地址到物理地址的映射表 
{
	long *logic;                 //逻辑地址 
	long *physical;              //物理地址 
}maTable[Max_Table_Size];       
*/
/*struct segment                //段的定义 
{
	long segmentNo;           //段号 
	long pageAvaiOfSegment;		//当前段有多少可用的页
}; */
struct Page
{
   long sizeOfhead;           // 头 head 的大小  
   long free_size;
   long recordNumber;         //里面有多少记录
   long *addressOfLastRecord; //最后 一个记录的地址
};
