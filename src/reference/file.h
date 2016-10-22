
#define SIZE_DATA_SPACE (192*1024*1024)
#define SIZE_PER_PAGE (4*1024)
#define SIZE_BIT_MAP  (SIZE_DATA_SPACE/(SIZE_PER_PAGE*8))

#define TABLE_DESC_ADDR 0
#define BITMAP_ADDR 1024 
#define DATA_ADDR	(BITMAP_ADDR+SIZE_BIT_MAP)
#define INODE_ADDR (BITMAP_ADDR+SIZE_BIT_MAP+SIZE_DATA_SPACE)

#define PAGE_PER_INODE 28
#define MAX_FILE_NUM 64
#define FIRST_FID	1

#define SIZE_BUFF 32
#define BUFF_NOT_HIT -1
#define P_AVAI		1
#define P_UNAVAI	0
#define P_EDIT		1
#define P_UNEDIT	0
#define ALLO_FAIL	-1

struct iNode 
{
	long fid;					//	该Node属于哪个文件
	long count;					//	该Node中记录了多少个页
	long nextAddr;				//	下一个Node的指针
	long preAddr;				//	前一个Node的指针
	long pageNo[PAGE_PER_INODE];	//	记录每一个页的页号
};
struct FileDesc
{
	long fileID;				//	文件的标识
	long fileAddr;				//	文件的Node地址值
	long filePageNum;			//	文件占用多少页
};
struct SysDesc
{
	long sizeOfFile;			//	存储文件中，数据区的大小，默认设为196M
	long sizePerPage;           //	每一个页的大小
	long totalPage;				//	总共的页数
	long pageAvai;				//	当前有多少可用的页

	long bitMapAddr;			//	bitMap在存储文件中的地址
	long sizeBitMap;			//	bitMap的大小,以字节为单位

	long dataAddr;				//	存储文件中数据区的大小
	long iNodeNum;				//	Node的数量
	long iNodeAddr;				//	Node在存储文件的地址
	long iNodeCur;				//	目前使用过的Node数量

	long curfid;				//	目前可以分配的fid号
	long curFileNum;			//	目前有多少个文件，最多为 MAX_FILE_NUM
	struct FileDesc fileDesc[MAX_FILE_NUM];			//	对每一个文件进行描述
};
struct buffMap
{
    long pageNo;					//	该缓冲区块中存储的数据文件的页号
	long vstTime;					//	访问该缓冲区块的时间
	int edit;						//	该缓冲区块中的数据是否被修改
};
struct buffSpace
{
	 /*
		应该动态分配缓冲区数据的空间
		因为这样静态分配能分配到的空间太小
	*/
	char data[SIZE_BUFF][SIZE_PER_PAGE];	//	缓冲区数据块，目前设置SIZE_BUFF块，每一块的大小为页的大小	
	struct buffMap map[SIZE_BUFF];		//	记录每一个缓冲区块的信息
	long curTimeStamp;					//	目前的相对时间戳
};