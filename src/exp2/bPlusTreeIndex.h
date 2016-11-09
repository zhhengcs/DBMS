#include <stdio.h>
#include <malloc.h>
#include <memory.h>
#include <string.h>
#define Degree 3			//b+树的度数  The half degree of the b plus tree
#define KeyDataType int
#define Pointer int

//节点结构体 The structure of the tree node
typedef struct BPKeyNode	//b plus key node
{
	int nodeId;//记录这个节点在文件的中的编号
	int keyNumInNode;	//记录这个节点有多少个关键字
	int isLeaf;		//判断是否为页节点
	KeyDataType keyInNode[2*Degree];//关键字（及对应每个孩子节点的中关键字最小的关键字）
	Pointer children[2*Degree];//指针，记录每个孩子在文件的第几个位置
	Pointer nextBrother;//指针，记录下一个兄弟
}BPKeyNode,*P_BPKeyNode;

//树的结构体
typedef struct BPKeyTree
{
	P_BPKeyNode treeRoot;
	int rootFileId;//记录根节点的在文件中的标号，即id
	int nodeNum;	//记录共有多少个节点
	long keyNum;	    //记录共有多少个key 
	char treeFileName[100];		//用于存储B+树的节点文件的名字
	FILE *pof;			//打开写入name文件时，使用  pointer of file
	int firstLeaf;		//最小的数据所在的叶节点
}BPKeyTree,*P_BPKeyTree;
