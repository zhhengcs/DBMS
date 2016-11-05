#include <stdio.h>
#include <malloc.h>
#include <memory.h>
#include <string.h>
#define T 3			//b+树的度数
#define KeyType int
#define Pointer int
//节点结构体
typedef struct BPNode
{
	unsigned int id;//记录这个节点在文件的中的编号
	unsigned int n;	//记录这个节点有多少个关键字
	int leaf;		//判断是否为页节点
	KeyType key[2*T];//关键字（及对应每个孩子节点的中关键字最小的关键字）
	Pointer child[2*T];//指针，记录每个孩子在文件的第几个位置
	Pointer next;//指针，，记录下一个兄弟
}BPNode,*P_BPNode;

//树的结构体
typedef struct BPTree
{
	P_BPNode root;
	unsigned int locate;//记录根节点的在文件中的标号，即id
	unsigned int num;	//记录共有多少个节点
	char name[100];		//用于存储B+树的节点文件的名字
	FILE *fp;			//打开写入name文件时，使用
	int start;		//最小的数据所在的叶节点
}BPTree,*P_BPTree;

BPTree indexBPTree;		//全局变量，b+树


int writeNode(P_BPNode w)
{
	fseek(indexBPTree.fp, sizeof(BPNode)*(w->id - 1) + 2*sizeof(int), SEEK_SET);
	fwrite(w, sizeof(BPNode),1,indexBPTree.fp);

	return 0;
}

int readNode(P_BPNode r, Pointer id)
{
	fseek(indexBPTree.fp, (sizeof(BPNode))*(id - 1) + 2*sizeof(int), SEEK_SET);
	fread(r, (sizeof(BPNode)),1,indexBPTree.fp);
	return 0;
}
int pNode(P_BPNode n);

int createIndexBPTree (char *tableName, char *attr)
{//创建B+树,并进行相应的初始化，B+树的结构体是一个全局变量。
	P_BPNode root;
	indexBPTree.root = (P_BPNode)malloc(sizeof(BPNode));
	indexBPTree.num = 1;
	indexBPTree.start = 1;
	memcpy(indexBPTree.name, "../data/", sizeof("../data/"));
	strcat(indexBPTree.name, tableName);
	strcat(indexBPTree.name, ".");
	strcat(indexBPTree.name, attr);
	puts(indexBPTree.name);
	root = indexBPTree.root; 
	root->n = 0;
	root->leaf = 1;
	root->next = -1;
	root->id = 1;
	indexBPTree.locate = 1;

	indexBPTree.fp = fopen(indexBPTree.name, "wb");
	fwrite(&indexBPTree.num, sizeof(int),1,indexBPTree.fp);
	fwrite(&indexBPTree.locate, sizeof(int),1,indexBPTree.fp);
	writeNode(root);
	
	
	fclose(indexBPTree.fp);
	free(root);
	indexBPTree.root = NULL;
	return 0;
}


int splitBPNode (P_BPNode p, P_BPNode c, int i)
{//节点的分裂，要求p节点至少还能插入一个节点，c节点是满的，即n为2*T;
	int j;
	P_BPNode b;
	b = (P_BPNode)malloc(sizeof(BPNode));
	b->leaf = c->leaf;
	b->n = T;
	b->id = indexBPTree.num+1;	//为b赋值id号，用于表示该节点，，同时id号就是这个节点在文件的位置
	b->next = c->next;			//为b的next赋值，即原来的c节点的next
	//将c节点的后半部分关键字复制给b
	for (j = 0; j < T; j++)
	{
		b->key[j] = c->key[j+T];
		b->child[j] = c->child[j+T];
	}

	//至此b节点的对应元素已经建立好了，但还需要写入文件

	indexBPTree.num++;
	c->n = T;	//c节点的关键字数目减半
	c->next = b->id;



	//将p节点的i之后的节点后移
	for (j = p->n - 1; j > i; j--)
	{
		p->key[j+1] = p->key[j];
		p->child[j+1] = p->child[j];
	}
	
	//将b节点插入p中
	p->key[i+1] = b->key[0];
	p->child[i+1] = b->id;

	p->n++;	//p关键字个数加一
	//写入p
	writeNode(p);
	writeNode(c);
	writeNode(b);
	free(b);
	return 0;
}//splitBPNode


int insertBPNodeNotFull(P_BPNode s, KeyType k, unsigned int id)
{//插入，要求s节点不是满的

	int i = s->n-1;

	if (s->leaf)
	{//叶节点，找的合适的位置
		while (i >= 0 && s->key[i] > k)
		{
			s->key[i+1] = s->key[i];
			s->child[i+1] = s->child[i];
			i--;
		}

		s->key[i+1] = k;
		s->child[i+1] = id;
		s->n++;
		writeNode(s);
	}
	else
	{
		P_BPNode tmp = (P_BPNode)malloc(sizeof(BPNode));
		while (i >= 0 && s->key[i] > k)
		{
			i--;
		}
		if (i < 0)
		{//插入的元素最小，则把这个元素插入第一个节点，并修改对应的key
			i++;
			s->key[i] = k;
		}
		writeNode(s);
		readNode(tmp, s->child[i]);		//读取对应的
		if (tmp->n == 2*T)
		{
			splitBPNode(s, tmp, i);
			if (k > s->key[i+1])
				i++;
			readNode(tmp, s->child[i]);		//重新读取，，有待优化	
		}
		insertBPNodeNotFull(tmp, k, id);
		free(tmp);
	}
	
	return 0;
}

Pointer equalSearch(P_BPTree tree, KeyType k)
{//等值查询，给出key值，查找对应的id，并返回。如果不存在该节点，返回一个负数
	int i;
	int result;
	P_BPNode r;
	r = tree->root;
	if (k < r->key[0])	//比最小的节点小
		return -1;
	P_BPNode tmp = (P_BPNode)malloc(sizeof(BPNode));
	while (1)
	{
		i = r->n - 1;
		while (i >= 0 && r->key[i] > k)
			i--;

		if (r->leaf)	//是叶子，结束
			break;
		readNode(tmp, r->child[i]);
		r = tmp;
	}//while

	if (r->key[i] < k)
		return -1;
	result = r->child[i];

	free(tmp);
	tmp = NULL;
	return result;
}//equalSearch

int rangeSearch (P_BPTree tree, KeyType low, KeyType high)
{//范围查找，key值大于等于low，小于等于high。返回范围内的个数，
	unsigned int i;
	P_BPNode r = tree->root;
	Pointer *result;
	P_BPNode tmp = (P_BPNode)malloc(sizeof(BPNode));

	if (high < low)	//low <= high才有能有结果
		return 0;
	if (high < r->key[0])
		return 0;

	if (low < r->key[0])
		low = r->key[0];
	
	while (1)
	{
		i = r->n - 1;
		while (i >= 0 && r->key[i] > low)
			i--;
		if (r->leaf)	//是叶子，结束
			break;
		readNode(tmp, r->child[i]);
		r = tmp;
	}//while
	
	if (r->key[i] < low)
		i++;
	unsigned int num=100;
	result = (Pointer *)malloc(sizeof(Pointer)*num);
	unsigned int j = 0;

	while (1)
	{
		for (; i < r->n && r->key[i] <= high; i++)
		{
			if (j >= num)
			{
				num += 100;
				realloc(result, sizeof(Pointer)*num);
			}
			result[j++] = r->child[i];
		//	printf("sid:%d  iid: %d     id:%d\n", r->key[i],r->id, r->child[i]);
		}
		if (i <r->n || r->next < 0)
			break;
		
		readNode(tmp, r->next);
		r = tmp;
		i = 0;
	}//while 
	free(tmp);
	tmp = NULL;
	return j;
}//rangeSearch

int insertKeyInBPTree (P_BPTree tree, KeyType k, Pointer id)
{//向树中插入节点
	P_BPNode r = tree->root;

	if (equalSearch(tree, k) > 0)
	{
		printf("元素已存在！");
		return -1;
	}

	if (tree->root->n == 2*T)
	{//根节点满了，重新分配根节点，并进行初始化
		P_BPNode s = (P_BPNode)malloc(sizeof(BPNode));
		s->leaf = 0;
		s->n = 1;
		s->key[0] = r->key[0];
		s->child[0] = r->id;
		s->id = tree->num + 1;
	
		s->next = -1;
		//将新的根写入磁盘
		writeNode(s);
	
		tree->num++;
	
		writeNode(r);

		splitBPNode (s, r, 0);

		//根变为s，所以将新根copy到tree->root指针所指向的内存。（tree->root将一直指向一片开辟了的内存，且时刻保存树根的整个节点）
		memcpy(tree->root, s, sizeof(BPNode));
		tree->locate = s->id;

		insertBPNodeNotFull(s, k, id);
		free(s);	//释放内存		
	}
	else
		insertBPNodeNotFull(r, k, id);
	return 0;
}//insertBPNode

int initIndexBPTree(char *tableName, char *attr)
{//初始化BPTree，打开相应文件，fp记录；为root分配内存可以存储一个节点的内存，并读入根节点
	indexBPTree.root = (P_BPNode)malloc(sizeof(BPNode));
	indexBPTree.start = 1;

	memcpy(indexBPTree.name, "../data/", sizeof("../data/"));
	strcat(indexBPTree.name, tableName);
	strcat(indexBPTree.name, ".");
	strcat(indexBPTree.name, attr);
	indexBPTree.fp = fopen(indexBPTree.name, "rb+");
	fread(&indexBPTree.num,sizeof(int),1, indexBPTree.fp);
	fread(&indexBPTree.locate,sizeof(int),1,indexBPTree.fp);

	readNode(indexBPTree.root, indexBPTree.locate);
	return 0;
}

int endBPTree()
{//将建立的树结束
	fseek(indexBPTree.fp, 0, SEEK_SET);
	fwrite(&indexBPTree.num, sizeof(int),1,indexBPTree.fp);
	fwrite(&indexBPTree.locate, sizeof(int),1,indexBPTree.fp);
	free(indexBPTree.root);
	fclose(indexBPTree.fp);
	return 0;
}

int pNode(P_BPNode n)
{//输出节点
	printf("%s  id:%d  next:%d  个数：%d\n" ,n->leaf?"是叶节点":"不是叶节点", n->id, n->next, n->n);
	unsigned int i;
	for(i = 0; i < n->n; i++)
		printf("  key[%d]:%d\t",i,n->key[i]);
	puts("");
	for(i = 0; i < n->n; i++)
		printf("child[%d]:%d\t",i,n->child[i]);
	puts("");
	return 0;
}//pNode

int replaceKeyInBPTree(P_BPTree tree, KeyType oldkey, KeyType newkey)
{//将oldkey替换为newkey
	P_BPNode r = tree->root;
	int i;
	P_BPNode tmp = (P_BPNode)malloc(sizeof(BPNode));

	while (1)
	{
		i = r->n - 1;
		while (i >= 0 && r->key[i] > oldkey)
			i--;
		if (r->key[i] == oldkey)
		{
			r->key[i] = newkey;
			writeNode(r);
		}

		if (r->leaf)
			break;
		readNode(tmp, r->child[i]);
		r = tmp;
	}
	free(tmp);
	return 0;
}

int adjustToDel(P_BPNode p, P_BPNode x, unsigned int i)
{//p指向x的父节点，i指的是，x是p的下标
	unsigned int j;
	P_BPNode left = NULL;
	P_BPNode right = NULL;
	P_BPNode tmp = (P_BPNode)malloc(sizeof(BPNode));

	if	(i > 0 )	//x有左兄弟
	{
		readNode(tmp, p->child[i-1]);
		left = tmp;
		if (left->n > T)
		{
			for (j = x->n; j > 0; j--)
			{
				x->key[j] = x->key[j-1];
				x->child[j] = x->child[j-1];
			}
			x->n++;
			x->key[0] = left->key[left->n-1];
			x->child[0] = left->child[left->n-1];
			writeNode(x);

			left->n--;
			writeNode(left);
			
			p->key[i] = x->key[0];
			writeNode(p);

			return 0;
		}
	}//if

	if (i < p->n - 1) //x有又兄弟
	{
		readNode(tmp, p->child[i+1]);
		right = tmp;
		left = NULL;
		if (right->n > T)
		{
			x->key[x->n] = right->key[0];
			x->child[x->n] = right->child[0];
			x->n++;
			writeNode(x);

			for (j = 0; j < right->n-1; j++)
			{
				right->key[j] = right->key[j+1];
				right->child[j] = right->child[j+1];
			}
			right->n--;
			writeNode(right);

			p->key[i+1] = right->key[0];
			writeNode(p);
			return 0;
		}
	}

	if (left == tmp)
	{
		for (j = 0; j < T; j++)
		{
			left->key[T+j] = x->key[j];
			left->child[T+j] = x->child[j];
		}
		left->n += T;
		left->next = x->next;
		writeNode(left);

		for (j = i; j < p->n - 1; j++)
		{
			p->key[j] = p->key[j+1];
			p->child[j] = p->child[j+1];
		}
		p->n--;
		writeNode(p);
		memcpy(x, left, sizeof(BPNode));
	}
	else
	{
		for (j = 0; j < T; j++)
		{
			x->key[T+j] = right->key[j];
			x->child[T+j] = right->child[j];
		}
		x->n += T;
		x->next = right->next;
		writeNode(x);

		for (j = i+1; j < p->n -1; j++)
		{
			p->key[j] = p->key[j+1];
			p->child[j] = p->child[j+1];
		}
		p->n--;
		writeNode(p);
	}
	free(tmp);
	left = right = tmp = NULL;
	return 0;
}
//调用这个函数是，参数节点p，必须满足相应的要求：
//①如果p是根节点且是叶子节点，则没有要求
//②如果p是根节点（非叶），则p节点的子节点个数不小于2（B+树本身就满足这个要求）。
//③如果p是非根节点，则节点p的子节点个数必须大于T
KeyType delKeyInBPNode(P_BPNode p, KeyType k)
{//以这个节点为起点，找到k并删除。要求确保k存在
	unsigned int i;
	unsigned int j;
	i = p->n - 1;

	while (p->key[i] > k)
		i--;

	//是叶节点(如果p本身又是根节点，则这个是情况①)
	if (p->leaf)
	{
		for (j = i; j < p->n-1; j++)
		{
			p->key[j] = p->key[j+1];
			p->child[j] = p->child[j+1];
		}//while
		p->n--;
		writeNode(p);
		if (i == 0)	//说明删除的关键字是该节点中最小的
			replaceKeyInBPTree(&indexBPTree, k, p->key[i]);
		return p->key[i];
	}//if
	

	//p是内节点
	P_BPNode x;

	x = (P_BPNode)malloc(sizeof(BPNode));
	
	readNode(x, p->child[i]);
	if (x->n > T)	//x的子节点的个数大于T，则直接调用
		return delKeyInBPNode(x, k);
	else			//x的子节点的个数等于T，需要调整
	{
		adjustToDel(p, x, i);
		return delKeyInBPNode(x, k);
	}//else

}//delKeyInNode

int delKeyInBPTree(P_BPTree tree, KeyType k)
{//1.如果一个根节点同时又是叶节点，则没有子节点限制（这个子节点指向的不再是树的节点）
 //2.非叶根节点至少保持有两个子节点，其他的节点至少有T个子节点。 
 
	if (equalSearch(tree, k) < 0)	//检查是否有k这个关键字
		return -1;

	P_BPNode r = tree->root;

	delKeyInBPNode(r, k);
	if (r->n == 1)
	{
		tree->locate = r->child[0];
		readNode(tree->root,r->child[0]);	//更换根节点
		tree->num--;
		//还应该将原始的根节点从磁盘上删除
	}
	r = NULL;
	return 0;
}//delKeyInBPTree


int main ()
{
	unsigned int i = 1;
	createIndexBPTree("student","idx");
	initIndexBPTree("student", "idx");

	insertKeyInBPTree(&indexBPTree, 50, i++);
	insertKeyInBPTree(&indexBPTree, 30, i++);
	insertKeyInBPTree(&indexBPTree, 60, i++);
	insertKeyInBPTree(&indexBPTree, 10, i++);
	insertKeyInBPTree(&indexBPTree, 90, i++);
	insertKeyInBPTree(&indexBPTree, 40, i++);
	insertKeyInBPTree(&indexBPTree, 100, i++);



	insertKeyInBPTree(&indexBPTree, 110, i++);
	insertKeyInBPTree(&indexBPTree, 150, i++);
	insertKeyInBPTree(&indexBPTree, 200, i++);
	insertKeyInBPTree(&indexBPTree, 0, i++);
	insertKeyInBPTree(&indexBPTree, 49, i++);
	insertKeyInBPTree(&indexBPTree, 45, i++);
	insertKeyInBPTree(&indexBPTree, -1, i++);
	insertKeyInBPTree(&indexBPTree, 210, i++);
	insertKeyInBPTree(&indexBPTree, 220, i++);
	insertKeyInBPTree(&indexBPTree, 230, i++);
	insertKeyInBPTree(&indexBPTree, 240, i++);
	insertKeyInBPTree(&indexBPTree, 250, i++);
	insertKeyInBPTree(&indexBPTree, 260, i++);
	insertKeyInBPTree(&indexBPTree, 270, i++);
	insertKeyInBPTree(&indexBPTree, 280, i++);
	insertKeyInBPTree(&indexBPTree, 290, i++);


	insertKeyInBPTree(&indexBPTree, 300, i++);
	insertKeyInBPTree(&indexBPTree, 310, i++);
	insertKeyInBPTree(&indexBPTree, 320, i++);
	insertKeyInBPTree(&indexBPTree, 330, i++);
	insertKeyInBPTree(&indexBPTree, 340, i++);
	insertKeyInBPTree(&indexBPTree, 350, i++);
	insertKeyInBPTree(&indexBPTree, 360, i++);
	insertKeyInBPTree(&indexBPTree, -360, i++);

	
	KeyType k;
	k = 340;

	int s = equalSearch(&indexBPTree, k);
	if (s > 0)
		printf("id:%d\n",s);
	else
		printf("不存在元素:%d\n",k); 
	

	rangeSearch(&indexBPTree, 500, 500);

	insertKeyInBPTree(&indexBPTree, 50, i++);
	insertKeyInBPTree(&indexBPTree, 55, i++);
	insertKeyInBPTree(&indexBPTree, 60, i++);
	insertKeyInBPTree(&indexBPTree, 100, i++);
	insertKeyInBPTree(&indexBPTree, 110, i++);
	insertKeyInBPTree(&indexBPTree, 115, i++);
	insertKeyInBPTree(&indexBPTree, 200, i++);

	insertKeyInBPTree(&indexBPTree, 210, i++);
	insertKeyInBPTree(&indexBPTree, 215, i++);
	insertKeyInBPTree(&indexBPTree, 220, i++);

	
	P_BPNode t = (P_BPNode)malloc(sizeof(BPNode));

	printf("个数：%d  根id:%d\n", indexBPTree.num, indexBPTree.locate);
	pNode(indexBPTree.root);
	
	
	delKeyInBPTree(&indexBPTree,100);
	printf("%d\n\n",indexBPTree.num);
	
	pNode(indexBPTree.root);
	
	for (i = 1; i < indexBPTree.num; i++)
	{
		readNode(t,i); 
		pNode(t);
	}
	endBPTree();
	return 0;
}

