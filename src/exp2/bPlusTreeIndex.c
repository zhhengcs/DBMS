#include <stdio.h>
#include <bPlusTreeIndex.h>

BPKeyTree indexBPKeyTree;		//全局变量，b+树

int saveNodeToFile(P_BPKeyNode kNode)
{
	fseek(indexBPKeyTree.pof, sizeof(BPKeyNode)*(kNode->nodeId - 1) + 2*sizeof(int), SEEK_SET);
	fwrite(kNode, sizeof(BPKeyNode),1,indexBPKeyTree.pof);

	return 0;
}

int getNodeFromFile(P_BPKeyNode kNode, int nodeId)
{
	fseek(indexBPKeyTree.pof, (sizeof(BPKeyNode))*(nodeId - 1) + 2*sizeof(int), SEEK_SET);
	fread(kNode, (sizeof(BPKeyNode)),1,indexBPKeyTree.pof);
	return 0;
}

int displayNode(P_BPKeyNode kNode)
{//显示节点
	printf("%s  id:%d  next:%d  个数：%d\n" ,kNode->isLeaf?"是叶节点":"不是叶节点", kNode->nodeId, kNode->nextBrother, kNode->keyNumInNode);
	int i;
	for(i = 0; i < kNode->keyNumInNode; i++)
		printf("  key[%d]:%d\t",i,kNode->keyInNode[i]);
	puts("");
	for(i = 0; i < kNode->keyNumInNode; i++)
		printf("child[%d]:%d\t",i,kNode->children[i]);
	puts("");
	return 0;
}

int createIndexBPKeyTree (char *tableName, char *attr)
{//创建B+树,并进行相应的初始化，B+树的结构体是一个全局变量。
	//Initial the root node
	P_BPKeyNode rootNode;


	indexBPKeyTree.treeRoot = (P_BPKeyNode)malloc(sizeof(BPKeyNode));
	indexBPKeyTree.nodeNum = 1;
	indexBPKeyTree.keyNum = 0;
	indexBPKeyTree.fistLeaf = 1;
	//Start: the file path of the index tree
	//memcpy(indexBPKeyTree.treeFileName, "../data/", sizeof("../data/"));
	strcpy(indexBPKeyTree.treeFileName, "../data/");
	strcat(indexBPKeyTree.treeFileName, tableName);
	strcat(indexBPKeyTree.treeFileName, ".");
	strcat(indexBPKeyTree.treeFileName, attr);
	puts(indexBPKeyTree.treeFileName);
	//End: the file path of the index tree
	rootNode = indexBPKeyTree.treeRoot;
	rootNode->keyNumInNode = 0;
	rootNode->isLeaf = 1;
	rootNode->nextBrother = -1;
	rootNode->nodeId = 1;
	indexBPKeyTree.rootFileId = 1;

	indexBPKeyTree.pof = fopen(indexBPKeyTree.treeFileName, "wb");
	fwrite(&indexBPKeyTree.nodeNum, sizeof(int),1,indexBPKeyTree.pof);     //文件头先写入两个int,这是第一个
	fwrite(&indexBPKeyTree.rootFileId, sizeof(int),1,indexBPKeyTree.pof); //文件头先写入两个int,这是第二个
	saveNodeToFile(rootNode);

	fclose(indexBPKeyTree.pof);
	free(rootNode);
	indexBPKeyTree.treeRoot = NULL;
	return 0;
}

int initIndexBPKeyTree(char *tableName, char *attr)
{//初始化BPTree，打开相应文件，fp记录；为root分配内存可以存储一个节点的内存，并读入根节点
	indexBPKeyTree.treeRoot = (P_BPKeyNode)malloc(sizeof(BPKeyNode));
	indexBPKeyTree.fistLeaf = 1;

	//memcpy(indexBPKeyTree.treeFileName, "../data/", sizeof("../data/"));
	strcpy(indexBPKeyTree.treeFileName, "../data/");
	strcat(indexBPKeyTree.treeFileName, tableName);
	strcat(indexBPKeyTree.treeFileName, ".");
	strcat(indexBPKeyTree.treeFileName, attr);
	indexBPKeyTree.pof = fopen(indexBPKeyTree.treeFileName, "rb+");
	fread(&indexBPKeyTree.nodeNum,sizeof(int),1, indexBPKeyTree.pof);
	fread(&indexBPKeyTree.rootFileId,sizeof(int),1,indexBPKeyTree.pof);

	getNodeFromFile(indexBPKeyTree.treeRoot, indexBPKeyTree.rootFileId);
	return 0;
}


int divideBPKeyNode (P_BPKeyNode newkNode, P_BPKeyNode fullkNode, int i)
{//节点的分裂，要求p节点至少还能插入一个节点，c节点是满的，即n为2*T;
	int j;
	P_BPKeyNode b;
	b = (P_BPKeyNode)malloc(sizeof(BPKeyNode));
	b->isLeaf = fullkNode->isLeaf;
	b->keyNumInNode = Degree;
	b->nodeId = indexBPKeyTree.nodeNum+1;	//为b赋值id号，用于表示该节点，，同时id号就是这个节点在文件的位置
	b->nextBrother = fullkNode->nextBrother;			//为b的next赋值，即原来的c节点的next
	//将c节点的后半部分关键字复制给b
	for (j = 0; j < Degree; j++)
	{
		b->keyInNode[j] = fullkNode->keyInNode[j+Degree];
		b->children[j] = fullkNode->children[j+Degree];
	}

	//至此b节点的对应元素已经建立好了，但还需要写入文件

	indexBPKeyTree.nodeNum++;
	fullkNode->keyNumInNode = Degree;	//c节点的关键字数目减半
	fullkNode->nextBrother = b->nodeId;

	//将p节点的i之后的节点后移
	for (j = newkNode->keyNumInNode - 1; j > i; j--)
	{
		newkNode->keyInNode[j+1] = newkNode->keyInNode[j];
		newkNode->children[j+1] = newkNode->children[j];
	}
	
	//将b节点插入p中
	newkNode->keyInNode[i+1] = b->keyInNode[0];
	newkNode->children[i+1] = b->nodeId;

	newkNode->keyNumInNode++;	//p关键字个数加一
	//写入p
	saveNodeToFile(newkNode);
	saveNodeToFile(fullkNode);
	saveNodeToFile(b);
	free(b);
	return 0;
}


int insertBPNodeNotFull(P_BPKeyNode kNode, KeyDataType key, int id)
{//插入，要求s节点不是满的

	int i = kNode->keyNumInNode-1;

	if (kNode->isLeaf)
	{//叶节点，找的合适的位置
		while (i >= 0 && kNode->keyInNode[i] > key)
		{
			kNode->keyInNode[i+1] = kNode->keyInNode[i];
			kNode->children[i+1] = kNode->children[i];
			i--;
		}

		//插入key值
		kNode->keyInNode[i+1] = key;
		kNode->children[i+1] = id;
		kNode->keyNumInNode++;
		saveNodeToFile(kNode);
	}
	else
	{
		P_BPKeyNode tmp = (P_BPKeyNode)malloc(sizeof(BPKeyNode));
		while (i >= 0 && kNode->keyInNode[i] > key)
		{
			i--;
		}
		if (i < 0)
		{//插入的元素最小，则把这个元素插入第一个节点，并修改对应的key
			i++;
			kNode->keyInNode[i] = key;
		}
		saveNodeToFile(kNode);
		getNodeFromFile(tmp, kNode->children[i]);		//读取对应的子结点
		if (tmp->keyNumInNode == 2*Degree)
		{
			divideBPKeyNode(kNode, tmp, i);
			if (key > kNode->keyInNode[i+1])
				i++;
			getNodeFromFile(tmp, kNode->children[i]);		//重新读取，有待优化
		}
		insertBPNodeNotFull(tmp, key, id);
		free(tmp);
	}

	return 0;
}

Pointer equalSearch(P_BPKeyTree tree, KeyDataType key)
{//等值查询，给出key值，查找对应的id，并返回。如果不存在该节点，返回一个负数
	int i;
	int result;
	P_BPKeyNode r;
	r = tree->treeRoot;
	if (key < r->keyInNode[0])	//比最小的节点小
		return -1;
	P_BPKeyNode tmp = (P_BPKeyNode)malloc(sizeof(BPKeyNode));
	while (1)
	{
		i = r->keyNumInNode - 1;
		while (i >= 0 && r->keyInNode[i] > key)
			i--;

		if (r->isLeaf)	//是叶子，结束
			break;
		getNodeFromFile(tmp, r->children[i]);
		r = tmp;
	}//while

	if (r->keyInNode[i] < key)
		return -1;
	result = r->children[i];

	free(tmp);
	tmp = NULL;
	return result;
}

int rangeSearch (P_BPKeyTree tree, KeyDataType lowValue, KeyDataType highValue)
{//范围查找，key值大于等于lowValue，小于等于highValue。返回范围内的个数，
	int i;
	P_BPKeyNode r = tree->treeRoot;
	Pointer *result;
	P_BPKeyNode tmp = (P_BPKeyNode)malloc(sizeof(BPKeyNode));

	if (highValue < lowValue)	//low <= high才有能有结果
		return 0;
	if (highValue < r->keyInNode[0])
		return 0;

	if (lowValue < r->keyInNode[0])
		lowValue = r->keyInNode[0];
	
	while (1)
	{
		i = r->keyNumInNode - 1;
		while (i >= 0 && r->keyInNode[i] > lowValue)
			i--;
		if (r->isLeaf)	//是叶子，结束
			break;
		getNodeFromFile(tmp, r->children[i]);
		r = tmp;
	}//while
	
	if (r->keyInNode[i] < lowValue)
		i++;
	int num=100;
	result = (Pointer *)malloc(sizeof(Pointer)*num);
	int j = 0;

	while (1)
	{
		for (; i < r->keyNumInNode && r->keyInNode[i] <= highValue; i++)
		{
			if (j >= num)
			{
				num += 100;
				realloc(result, sizeof(Pointer)*num);
			}
			result[j++] = r->children[i];
		//	printf("sid:%d  iid: %d     id:%d\n", r->key[i],r->id, r->child[i]);
		}
		if (i <r->keyNumInNode || r->nextBrother < 0)
			break;
		
		getNodeFromFile(tmp, r->nextBrother);
		r = tmp;
		i = 0;
	}//while 
	free(tmp);
	tmp = NULL;
	return j;
}//rangeSearch

int insertKeyInBPKeyTree (P_BPKeyTree tree, KeyDataType key, int id)
{//向树中插入节点
	P_BPKeyNode currentRoot = tree->treeRoot;

	if (equalSearch(tree, key) > 0)
	{
		printf("元素已存在！");
		return -1;
	}

	if (tree->treeRoot->keyNumInNode == 2*Degree)
	{//根节点满了，重新分配根节点，并进行初始化
		P_BPKeyNode newRoot = (P_BPKeyNode)malloc(sizeof(BPKeyNode));
		newRoot->isLeaf = 0;
		newRoot->keyNumInNode = 1;
		newRoot->keyInNode[0] = currentRoot->keyInNode[0];
		newRoot->children[0] = currentRoot->nodeId;
		newRoot->nodeId = tree->nodeNum + 1;
	
		newRoot->nextBrother = -1;
		//将新的根写入磁盘
		saveNodeToFile(newRoot);
	
		tree->nodeNum++;
	
		saveNodeToFile(currentRoot);

		divideBPKeyNode (newRoot, currentRoot, 0);

		//根变为s，所以将新根copy到tree->root指针所指向的内存。（tree->root将一直指向一片开辟了的内存，且时刻保存树根的整个节点）
		memcpy(tree->treeRoot, newRoot, sizeof(BPKeyNode));
		tree->rootFileId = newRoot->nodeId;

		insertBPNodeNotFull(newRoot, key, id);
		free(newRoot);	//释放内存
	}
	else
		insertBPNodeNotFull(currentRoot, key, id);
	indexBPKeyTree.keyNum++;
	return 0;
}//insertBPNode


int endBPKeyTree()
{//将建立的树结束
	fseek(indexBPKeyTree.pof, 0, SEEK_SET);
	fwrite(&indexBPKeyTree.nodeNum, sizeof(int),1,indexBPKeyTree.pof);
	fwrite(&indexBPKeyTree.rootFileId, sizeof(int),1,indexBPKeyTree.pof);
	free(indexBPKeyTree.treeRoot);
	fclose(indexBPKeyTree.pof);
	return 0;
}


int replaceKeyInBPKeyTree(P_BPKeyTree tree, KeyDataType oldkey, KeyDataType newkey)
{//将oldkey替换为newkey
	P_BPKeyNode currentRoot = tree->treeRoot;
	int i;
	P_BPKeyNode tmp = (P_BPKeyNode)malloc(sizeof(BPKeyNode));

	while (1)
	{
		i = currentRoot->keyNumInNode - 1;
		while (i >= 0 && currentRoot->keyInNode[i] > oldkey)
			i--;
		if (currentRoot->keyInNode[i] == oldkey)
		{
			currentRoot->keyInNode[i] = newkey;
			saveNodeToFile(currentRoot);
		}

		if (currentRoot->isLeaf)
			break;
		getNodeFromFile(tmp, currentRoot->children[i]);
		currentRoot = tmp;
	}
	free(tmp);
	return 0;
}

int adjustToDel(P_BPKeyNode p, P_BPKeyNode x, int i)
{//p指向x的父节点，i指的是，x是p的下标
	int j;
	P_BPKeyNode left = NULL;
	P_BPKeyNode right = NULL;
	P_BPKeyNode tmp = (P_BPKeyNode)malloc(sizeof(BPKeyNode));

	if	(i > 0 )	//x有左兄弟
	{
		getNodeFromFile(tmp, p->children[i-1]);
		left = tmp;
		if (left->keyNumInNode > Degree)
		{
			for (j = x->keyNumInNode; j > 0; j--)
			{
				x->keyInNode[j] = x->keyInNode[j-1];
				x->children[j] = x->children[j-1];
			}
			x->keyNumInNode++;
			x->keyInNode[0] = left->keyInNode[left->keyNumInNode-1];
			x->children[0] = left->children[left->keyNumInNode-1];
			saveNodeToFile(x);

			left->keyNumInNode--;
			saveNodeToFile(left);
			
			p->keyInNode[i] = x->keyInNode[0];
			saveNodeToFile(p);

			return 0;
		}
	}//if

	if (i < p->keyNumInNode - 1) //x有右兄弟
	{
		getNodeFromFile(tmp, p->children[i+1]);
		right = tmp;
		left = NULL;
		if (right->keyNumInNode > Degree)
		{
			x->keyInNode[x->keyNumInNode] = right->keyInNode[0];
			x->children[x->keyNumInNode] = right->children[0];
			x->keyNumInNode++;
			saveNodeToFile(x);

			for (j = 0; j < right->keyNumInNode-1; j++)
			{
				right->keyInNode[j] = right->keyInNode[j+1];
				right->children[j] = right->children[j+1];
			}
			right->keyNumInNode--;
			saveNodeToFile(right);

			p->keyInNode[i+1] = right->keyInNode[0];
			saveNodeToFile(p);
			return 0;
		}
	}

	if (left == tmp)
	{
		for (j = 0; j < Degree; j++)
		{
			left->keyInNode[Degree+j] = x->keyInNode[j];
			left->children[Degree+j] = x->children[j];
		}
		left->keyNumInNode += Degree;
		left->nextBrother = x->nextBrother;
		saveNodeToFile(left);

		for (j = i; j < p->keyNumInNode - 1; j++)
		{
			p->keyInNode[j] = p->keyInNode[j+1];
			p->children[j] = p->children[j+1];
		}
		p->keyNumInNode--;
		saveNodeToFile(p);
		memcpy(x, left, sizeof(BPKeyNode));
	}
	else
	{
		for (j = 0; j < Degree; j++)
		{
			x->keyInNode[Degree+j] = right->keyInNode[j];
			x->children[Degree+j] = right->children[j];
		}
		x->keyNumInNode += Degree;
		x->nextBrother = right->nextBrother;
		saveNodeToFile(x);

		for (j = i+1; j < p->keyNumInNode -1; j++)
		{
			p->keyInNode[j] = p->keyInNode[j+1];
			p->children[j] = p->children[j+1];
		}
		p->keyNumInNode--;
		saveNodeToFile(p);
	}
	free(tmp);
	left = right = tmp = NULL;
	return 0;
}
//调用这个函数是，参数节点p，必须满足相应的要求：
//①如果p是根节点且是叶子节点，则没有要求
//②如果p是根节点（非叶），则p节点的子节点个数不小于2（B+树本身就满足这个要求）。
//③如果p是非根节点，则节点p的子节点个数必须大于T

KeyDataType delKeyInBPNode(P_BPKeyNode kNode, KeyDataType key)
{//以这个节点为起点，找到k并删除。要求确保k存在
	int i;
	int j;
	i = kNode->keyNumInNode - 1;

	while (kNode->keyInNode[i] > key)
		i--;

	//是叶节点(如果p本身又是根节点，则这个是情况①)
	if (kNode->isLeaf)
	{
		for (j = i; j < kNode->keyNumInNode-1; j++)
		{
			kNode->keyInNode[j] = kNode->keyInNode[j+1];
			kNode->children[j] = kNode->children[j+1];
		}//while
		kNode->keyNumInNode--;
		indexBPKeyTree.keyNum--;
		saveNodeToFile(kNode);
		if (i == 0)	//说明删除的关键字是该节点中最小的
			replaceKeyInBPKeyTree(&indexBPKeyTree, key, kNode->keyInNode[i]);
		return kNode->keyInNode[i];
	}//if
	

	//p是内节点
	P_BPKeyNode x;

	x = (P_BPKeyNode)malloc(sizeof(BPKeyNode));
	
	getNodeFromFile(x, kNode->children[i]);
	if (x->keyNumInNode > Degree)	//x的子节点的个数大于T，则直接调用
		return delKeyInBPNode(x, key);
	else			//x的子节点的个数等于T，需要调整
	{
		adjustToDel(kNode, x, i);
		return delKeyInBPNode(x, key);
	}//else
	

}

int delKeyInBPTree(P_BPKeyTree tree, KeyDataType key)
{//1.如果一个根节点同时又是叶节点，则没有子节点限制（这个子节点指向的不再是树的节点）
 //2.非叶根节点至少保持有两个子节点，其他的节点至少有T个子节点。 
 
	if (equalSearch(tree, key) < 0)	//检查是否有k这个关键字
		return -1;

	P_BPKeyNode currentRoot = tree->treeRoot;

	delKeyInBPNode(currentRoot, key);
	if (currentRoot->keyNumInNode == 1)
	{
		tree->rootFileId = currentRoot->children[0];
		getNodeFromFile(tree->treeRoot,currentRoot->children[0]);	//更换根节点
		tree->nodeNum--;
		//还应该将原始的根节点从磁盘上删除
	}
	currentRoot = NULL;
	
	return 0;
}


int main ()
{
	int i = 1;
	puts("建立索引树");
	createIndexBPKeyTree("student","idx");
	puts("初使化索引树");
	initIndexBPKeyTree("student", "idx");

	puts("插入key到索引");
	insertKeyInBPKeyTree(&indexBPKeyTree, 50, i++);
	insertKeyInBPKeyTree(&indexBPKeyTree, 30, i++);
	insertKeyInBPKeyTree(&indexBPKeyTree, 60, i++);
	insertKeyInBPKeyTree(&indexBPKeyTree, 10, i++);
	insertKeyInBPKeyTree(&indexBPKeyTree, 90, i++);
	insertKeyInBPKeyTree(&indexBPKeyTree, 40, i++);
	insertKeyInBPKeyTree(&indexBPKeyTree, 100, i++);
	insertKeyInBPKeyTree(&indexBPKeyTree, 110, i++);
	insertKeyInBPKeyTree(&indexBPKeyTree, 150, i++);
	insertKeyInBPKeyTree(&indexBPKeyTree, 200, i++);
	insertKeyInBPKeyTree(&indexBPKeyTree, 0, i++);
	insertKeyInBPKeyTree(&indexBPKeyTree, 49, i++);
	insertKeyInBPKeyTree(&indexBPKeyTree, 45, i++);
	insertKeyInBPKeyTree(&indexBPKeyTree, -1, i++);
	insertKeyInBPKeyTree(&indexBPKeyTree, 210, i++);
	insertKeyInBPKeyTree(&indexBPKeyTree, 220, i++);
	insertKeyInBPKeyTree(&indexBPKeyTree, 230, i++);
	insertKeyInBPKeyTree(&indexBPKeyTree, 240, i++);
	insertKeyInBPKeyTree(&indexBPKeyTree, 250, i++);
	insertKeyInBPKeyTree(&indexBPKeyTree, 260, i++);
	insertKeyInBPKeyTree(&indexBPKeyTree, 270, i++);
	insertKeyInBPKeyTree(&indexBPKeyTree, 280, i++);
	insertKeyInBPKeyTree(&indexBPKeyTree, 290, i++);
	insertKeyInBPKeyTree(&indexBPKeyTree, 300, i++);
	insertKeyInBPKeyTree(&indexBPKeyTree, 310, i++);
	insertKeyInBPKeyTree(&indexBPKeyTree, 320, i++);
	insertKeyInBPKeyTree(&indexBPKeyTree, 330, i++);
	insertKeyInBPKeyTree(&indexBPKeyTree, 340, i++);
	insertKeyInBPKeyTree(&indexBPKeyTree, 350, i++);
	insertKeyInBPKeyTree(&indexBPKeyTree, 360, i++);
	insertKeyInBPKeyTree(&indexBPKeyTree, -360, i++);

	KeyDataType key;
	key = 340;
	printf("\n###在索引中搜索关键字：%d ####\n", key);
	int s = equalSearch(&indexBPKeyTree, key);
	if (s > 0)
		printf("id:%d\n",s);
	else
		printf("不存在元素:%d\n",key);
	printf("###在索引中搜索关键字结束：%d ####\n", key);

//	puts("\n############Range search########\n");
//	rangeSearch(&indexBPKeyTree, 300, 500);
//	puts("\n############Range search########\n");



	puts("\n############ 打印索引树信息 ########\n");
	P_BPKeyNode temp = (P_BPKeyNode)malloc(sizeof(P_BPKeyNode));

	printf("节点个数：%d\t根id:%d\tkey个数:%d\n", indexBPKeyTree.nodeNum, indexBPKeyTree.rootFileId, indexBPKeyTree.keyNum);
	displayNode(indexBPKeyTree.treeRoot);
	for (i = 1; i < indexBPKeyTree.nodeNum; i++)
	{
		getNodeFromFile(temp,i);
		displayNode(temp);
	}

	puts("\n############ 打印删除结点100后的树 ########\n");
	delKeyInBPTree(&indexBPKeyTree,100);
	printf("节点个数：%d\t根id:%d\tkey个数:%d\n", indexBPKeyTree.nodeNum, indexBPKeyTree.rootFileId, indexBPKeyTree.keyNum);
	displayNode(indexBPKeyTree.treeRoot);

	for (i = 1; i < indexBPKeyTree.nodeNum; i++)
	{
		getNodeFromFile(temp,i);
		displayNode(temp);
	}

	puts("\n############ 索引结束 ########\n");
	endBPKeyTree();
	return 0;
}

