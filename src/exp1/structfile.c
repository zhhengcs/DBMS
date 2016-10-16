#define SIZE_PER_PAGE (8*1024)
#define Max_Table_Size 100
//#define SIZE_BUFF 32
//#define Page_Per_Segment 128 

struct DataFile  
{
	long pageOfFile;			//	文件的大小，包含多少页 
//	long pageOfSegment;         //   每个段的大小，包含多少页 
    long sizePerPage;           //	每一个页的大小

	long totalPage;             //总的页数
	long pageAvail;          //当前可用的页
	long bitMapAddr;			//	bitMap在存储文件中的地址
	long sizeBitMap;			//	bitMap的大小,以字节为单位
	long *data;                 //指向当前的地址	
};

struct mapTable                 //逻辑地址到物理地址的映射表 
{
	long *logic;                 //逻辑地址 
	long *physical;              //物理地址 
}maTable[Max_Table_Size];         

/*struct segment                //段的定义 
{
	long segmentNo;           //段号 
	long pageAvaiOfSegment;		//当前段有多少可用的页
}; */
struct Page 
{  
   long *head;                // 指向head的指针 
   long sizeOfhead;           // 头 head 的大小  
   long recordNumber;         //里面有多少记录
   long *addressOfLastRecord; //最后 一个记录的地址 
};