/**/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lqp.h"
#include "printdb.h"
#include "pqp.h"
#include "project.h"
#include "page.h"
#include "scantable.h"
#include "list.h"
#include "join.h"

/*
 * return op as string
 * */
 
const char* getOP(enum op op) /*条件操作*/ 
{
	switch (op)
	{
		case LT: return "LT";
		case GT: return "GT";
		case LE: return "LE";
		case GE: return "GE";
		case EQ: return "EQ";
		case NE: return "NE";
	}
}

/*
 * return attr type as string
 * */
const char* getAttrType(enum attr_type attrType)
{
	switch (attrType)
	{
		case INT: return "INTEGER";
		case CHAR: return "CHARS";
	}
}

/*
 * create table logic query plan
 * attrcnt is attr num
 * attr[] store num attrname
 */
 //创建表 
int create_table_lqp(char *tbname, int attrcnt, struct table_attr attr[])
{
	/* semantic check */
	// check if table already exists.
	if (get_tbdef(tbname) != -1) {
		printf("ERROR: TABLE %s already exists.", tbname);
		return -1;
	}

	printf("#### CREATE TABLE #########\n");  //打印出创建的表名 
	printf("TABLE %s\n", tbname);           
	int i;
	for (i = 0; i < attrcnt; i++) {
		printf("\tATTR %d: %s %s(%d)\n", i, attr[i].name, getAttrType(attr[i].type), attr[i].len);
	}

	create_table(tbname, attrcnt, attr);
	return 0;
}
//删除表 

int drop_table_lqp(char *tbname)
{
	// semantic check
	if (get_tbdef(tbname) == -1) {
		printf("ERROR: TABLE %s does not exist.", tbname);
		return -1;
	}

	printf("#### DROP TABLE #########\n");
	printf("TABLE %s\n", tbname);

	/* delete all index */
	drop_table(tbname);
	return 0;
}
//创建索引 
int index_create_lqp(char *tbnmae, char *attrname)
{
	//TODO: Implement index_create_lqp
	printf("#### CREATE INDEX #########\n");
	printf("INDEX %s.%s", tbnmae, attrname);
	return 0;
}
//删除索引 
int index_drop_lqp(char *tbname, char *attrname)
{
	//TODO: Implement index_drop_lqp
	printf("#### DROP INDEX #########\n");
	printf("INDEX %s.%s", tbname, attrname);
	return 0;
}
//调用表 
int fill_relname(struct relattr *ra, int tbcnt, char *rel[])
{
	int j, k;
	int table;
	int attr_valid = -1;
	for (j = 0; j < tbcnt; j++) {
		table = get_tbdef(rel[j]);
		for (k = 0; k < sys_tables[table].attrcnt; k++) {
			if (strcmp(ra->attrname, sys_tables[table].attr[k].name) == 0) {
				attr_valid = 0;
				break;
			}
		}
		if (k != sys_tables[table].attrcnt) {
			strcpy(ra->tbname, rel[j]);
			break;
		}
	}
	if (attr_valid == 0) {
		return 0;
	} else {
		printf("ATTR %s is invalid.", ra->attrname);
		return -1;
	}
}

/*
 * for all attribute name in select list and condlist
 * if its relname is unkown, we take a scan of the from list
 * to complete it to contain its relname
 */
 //根据条件调用表 
int fill_relnames(int selcnt, struct relattr selattrs[], int tbcnt, char *rel[], int condcnt, struct condition cond[], int orderby, struct relattr *orderattr)
{
	int i;
	if (orderby != 0 && orderattr->tbname[0] == '\0')
		if (fill_relname(orderattr, tbcnt, rel) != 0) return -1;
	for (i = 0; i < selcnt; i++)
		if (selattrs[i].tbname[0] == '\0')
			if (fill_relname(&selattrs[i], tbcnt, rel) != 0) return -1;
	for (i = 0; i < condcnt; i++) {
		if (cond[i].lhs_attr.tbname[0] == '\0')
			if (fill_relname(&cond[i].lhs_attr, tbcnt, rel) != 0) return -1;
		if (cond[i].is_rhs_attr == 1 && cond[i].rhs_attr.tbname == NULL)
			if (fill_relname(&cond[i].rhs_attr, tbcnt, rel) != 0) return -1;
		strcpy(cond[i].lhs_attr.condnm, cond[i].lhs_attr.tbname);
		if (cond[i].is_rhs_attr == 1)
			strcpy(cond[i].rhs_attr.condnm, cond[i].rhs_attr.tbname);
	}
	return 0;
}

#define MAX_SEL_TAB 100

struct iNode *sglst, *sghlst, *dulst, *duhlst;//iNode
struct node *tablst;
//打印条件操作 
void dis_cond(struct condition cond)
{
	printf("%s.%s  ", cond.lhs_attr.tbname, cond.lhs_attr.attrname);
		printf("%s  ", getOP(cond.op));
		if (cond.is_rhs_attr == 1) {
			printf("%s.%s\t", cond.rhs_attr.tbname, cond.rhs_attr.attrname);
		} else {
			if (cond.rhs_value.type == CHAR)
				printf("%s\t", cond.rhs_value.data);
			else
				printf("%d\t", *(int *) cond.rhs_value.data);
		}
}
//打印出操作的表名 
void dis_tablst()
{
	struct node *tl = tablst;
	printf("tablst:");
	while(tl) {
		printf("%s, ", tl->tabname);
		if(tl->next == NULL) printf("NULL, ");
		tl = tl->next;
	}
	printf("\n");

}

int printres(struct logic_table *ltb);

/*
 * select statement's logic qp
 * selcnt: select list attr num; selattrs[]: the all attrs in sellist
 * tbcnt: rel number in from list; rel[]: the from list table list
 * condcnt: the condition count; cond[]: all condition exists in where clause
 * orderby: whether order by?
 * orderattr: the attr name used to order the result table
 * return 0 if succeed
 */
 //选择操作函数 
int select_lqp(int selcnt, struct relattr selattrs[], int tbcnt, char *rel[], int condcnt, struct condition cond[], int orderby, struct relattr orderattr, int is_uni, int is_ares)
{
	// select test1.id,test2.id from test1,test2 where test1.id = test2.id and test1.id = 1;
	printf("\nselcnt is %d\n",selcnt);
	int i=0;
	for (i = 0; i < selcnt; i++)
		printf("\t%s.%s \n", selattrs[i].tbname, selattrs[i].attrname);
	printf("tbcnt is %d\n",tbcnt);
	puts(*rel);
	printf("condcnt is %d\n",condcnt);
	for (i = 0; i < condcnt; i++)
		{
			printf("%s.%s\t\n", cond[i].lhs_attr.tbname, cond[i].lhs_attr.attrname);
			printf("is_rhs_attr is %d\nrhs_value.data is %s\n  ", cond[i].is_rhs_attr,cond[i].rhs_value.data);
			printf("%s.%s\t\n", cond[i].rhs_attr.tbname, cond[i].rhs_attr.attrname);
			printf("%d", cond[i].op);
		}
	printf("orderby is %d\n",orderby);
	if (orderby)
		printf("ORDER BY %s.%s\t%s\n", orderattr.tbname, orderattr.attrname, orderby == 1 ? "ASC" : "DESC");
	printf("is_uni is %d\n",is_uni);
	printf("is_ares is %d\n",is_ares);
	//i=0;
	int j;
	int table;
	int tab_cnt = 1;
	char *t;
	/* name of temp table */
	char *tab_name = (char *) malloc(100);   //给table分配100空间 
	struct logic_table ltb, ltb1, ltb2;
	/* fill up attr's rel name and do semantic check if attr exists */
	if (fill_relnames(selcnt, selattrs, tbcnt, rel, condcnt, cond, orderby, &orderattr) == -1) return -1;
	/* semantic check if all tables exist */
    //判定所有的表是否 
	for (i = 0; i < tbcnt; i++) {
		if (get_tbdef(rel[i]) == -1) {
			printf("ERROR: TABLE %s does not exist.", rel[i]);
			return -1;
		}
	}
/*
//	printf("#### SELECT AST #########\n");
	if (is_uni==1) {
		printf("SELECT DISTINCT  \n");
	} else {
		printf("SELECT  \n");
	}
//这里，我加了个if else 语句，为了将*输出 
	if(selcnt>0) 
	{
	for (i = 0; i < selcnt; i++)
		printf("\t%s.%s \n", selattrs[i].tbname, selattrs[i].attrname);
	printf("\n");
	}
	else
	{ 
	 printf("*\n");
	printf("FROM \n");
	} 
	for (i = 0; i < tbcnt; i++)
		printf("\t%s  \n", rel[i]);
	printf("\n");
	printf("WHERE \n");
	//判定表操作的条件，这个例子没有条件 
	*/
	for (i = 0; i < condcnt; i++) {
		printf("\t%s.%s  ", cond[i].lhs_attr.tbname, cond[i].lhs_attr.attrname);
		printf("%s  ", getOP(cond[i].op));
		if (cond[i].is_rhs_attr == 1) {
			printf("%s.%s  ", cond[i].rhs_attr.tbname, cond[i].rhs_attr.attrname);
		} else {
			if (cond[i].rhs_value.type == CHAR)
				printf("%s", cond[i].rhs_value.data);
			else
				printf("%d", *(int *) cond[i].rhs_value.data);
		}
		printf("\n");
	}
	//判定排序否，如果排序，就输出怎么排序的 
	if (orderby)
		printf("ORDER BY %s.%s\t%s\n", orderattr.tbname, orderattr.attrname, orderby == 1 ? "ASC" : "DESC");

	/* set up logic table */
	//表的模式 
	for(i = 0; i < tbcnt; i++) {
		ltb.is_tmp = 0;
		strcpy(ltb.tbname, rel[i]);
		table = get_tbdef(ltb.tbname);
		ltb.len = sys_tables[table].reclen;
		ltb.nattr = sys_tables[table].attrcnt;
		for (j = 0; j < ltb.len; j++)
			ltb.attr[j] = sys_tables[table].attr[j];
			//这个地方调不过去 我加的信息 
		insert_tablst(&tablst, rel[i], ltb);
	}

	/* scan the condition array, build the condition list: sglst and dulst */
	for (i = 0; i < condcnt; i++) {
		if(cond[i].is_rhs_attr == 0) //如果条件查找是个值时 
		{
			(cond[i].op == EQ) ? head_insert(&sglst, cond[i]) : tail_insert(&sglst, cond[i]);
			continue;
		}
		if(strcmp(cond[i].lhs_attr.condnm, cond[i].rhs_attr.condnm) == 0) //如果是个表的话就走这个 
		{	/* join with itself? */
			(cond[i].op == EQ) ? head_insert(&sglst, cond[i]) : tail_insert(&sglst, cond[i]);
			continue;
		}
		(cond[i].op == EQ) ? head_insert(&dulst, cond[i]) : tail_insert(&dulst, cond[i]);
	}
	
	sghlst = sglst;
	duhlst = dulst;
/*	printf("logical plan running\n");
	dis_tablst();
*/
	while(1) {
		if(!sglst && !dulst)
			break;

		while (sglst != NULL) {
			struct condition c;
			struct node *tbn;
			c = get(&sglst);
		  	
			printf("helloword! "); //这是我加的打印,不经过这里 
			/** select from the single table **/
			//我加的注释 begin 
	   	    printf("select * from %s where ", c.lhs_attr.condnm);
			dis_cond(c);
			printf("\tinto temp table name:%s\n", c.lhs_attr.condnm);
            //end
			tbn = search_tablst(&tablst, c.lhs_attr.condnm); /* get the table node */
/*			if(tbn != NULL) {
				printf("Fetch table node OK!\n");
			}
			printf("select,,,,,table:\n");
			printres(&(tbn->ltb));
*/
			select_pqp(&(tbn->ltb), &c);
			sghlst = sglst;
		}
		
		if (dulst != NULL) //  join分枝;  dulst = duplicate list
		{
			struct node *tbn1, *tbn2;
			char *num = (char*) malloc(25);
			struct condition c = get(&dulst);
			memset(tab_name,0, 50);
			strcpy(tab_name,"table_temp_");
			sprintf(num, "%d", tab_cnt);
			/* gen temp table name: "table_temp_"+tab_cnt */
			strcat(tab_name, num);
			/** rename attribute name that are the same in tab1 and tab2 **/
			/** make join of two tables in the conditions **/
/*			printf("join %s, %s on ", c.lhs_attr.condnm, c.rhs_attr.condnm);
			dis_cond(c);
			printf("\tinto temp table name: %s\n", tab_name);
*/
			tbn1 = search_tablst(&tablst, c.lhs_attr.condnm); /* get the left table node */
			tbn2 = search_tablst(&tablst, c.rhs_attr.condnm); /* get the right table node */
			ltb = join_pqp(tab_name, &(tbn1->ltb), &(tbn2->ltb), &c); //join the 2 tables
			if (tbn1->ltb.is_tmp == 1)
				remove(tbn1->ltb.tbname);
			if (tbn2->ltb.is_tmp == 1)
				remove(tbn2->ltb.tbname);
			/* is natral join or equal join */
			/* do not need */
			duhlst = dulst;
			update_dul_list(tab_name, c.lhs_attr.condnm, c.rhs_attr.condnm);
			del_tablst(&tablst, c.lhs_attr.condnm);
			del_tablst(&tablst, c.rhs_attr.condnm);
			insert_tablst(&tablst, tab_name, ltb);

			tab_cnt++;
		}
	}

	/** deal with tables not prsented in conditons **/
/*	dis_tablst(); */
	while (tablst != NULL && tablst->next != NULL) {
		char * t1 = (char *) malloc(50);
		char * t2 = (char *) malloc(50);

		char *num = (char*) malloc(25);
		memset(tab_name,0, 50);
		strcpy(tab_name,"table_temp_");
		sprintf(num, "%d", tab_cnt);
		strcat(tab_name, num);

		/** join without conditions 没有join条件的时候用笛卡尔积**/
		get_tablst(&tablst, t1, &ltb1);
		get_tablst(&tablst, t2, &ltb2);
/*		printf("JOIN %s, %s", t1, t2);
		printf("\tinto temp table name:%s\n", tab_name);
*/
		ltb = cross_pqp(tab_name, &ltb1, &ltb2);  //笛卡尔积 
		if (ltb1.is_tmp == 1)
			remove(ltb1.tbname);
		if (ltb2.is_tmp == 1)
			remove(ltb2.tbname);
		insert_tablst(&tablst, tab_name, ltb);

		free(t1);
		free(t2);
	}
	t = (char *) malloc(50);
	get_tablst(&tablst, t, &ltb); //为什么调用，没弄懂 
	/* order by */
	if (orderby != 0) {
		order_tb(&ltb, orderby, orderattr);
	}
	/** now make the project **/
/*	printf("project:");
	for(i = 0; i < selcnt; i++) {
		printf("%s.%s ", selattrs[i].tbname, selattrs[i].attrname);
	}
	printf("from %s\n", t);*/
	//如果不是*，就执行投影操作 ，is_ares判定表的个数 
	if (is_ares != 1)   //select * from : 1,  select id, name from: 0
	{
		ltb = project(&ltb, selcnt, selattrs);
	}
	/* unique */
	if (is_uni == 1) {
		unique_tb(&ltb);
	}
	printres(&ltb);  //打印构造好的逻辑表 
	free(t);
	remove(ltb.tbname);
	return 0;
}

/* print the result table into stdout */
int printres(struct logic_table *ltb)
{
	char rec[1000];
	struct iterator iter;
	iter = open_iter(ltb);
	print_header(ltb->nattr, ltb->attr);
	while (getnext_iter(&iter, rec) == 0) {
		printf_rec(rec, ltb->nattr, ltb->attr);
	}
	printf("Total %d tuples.\n", ltb->cnt);
	close_iter(&iter);
	return 0;
}

void update_dul_list(char * tab_name, char * tab1, char * tab2)
{
	struct iNode *lst;
	struct iNode *prev = NULL;
	for (lst = dulst; lst != NULL; lst = lst->next) {
		if(strcmp(lst->cond.lhs_attr.condnm, tab1) == 0 
			|| strcmp(lst->cond.lhs_attr.condnm, tab2) == 0) {
			strcpy(lst->cond.lhs_attr.condnm, tab_name);
		}
		if(strcmp(lst->cond.rhs_attr.condnm, tab1) == 0 
			|| strcmp(lst->cond.rhs_attr.condnm, tab2) == 0) {
			strcpy(lst->cond.rhs_attr.condnm, tab_name);
		}
		
		if(strcmp(lst->cond.lhs_attr.condnm, lst->cond.rhs_attr.condnm) == 0) {
			/* change to be a select */
			tail_insert(&sglst, lst->cond);
			if (prev == NULL) {
				dulst = lst->next;
			} else {
				prev->next = lst->next;
			}
		} else {
			prev = lst;
		}
	}
}

/* delete statement qp */
int delete_lqp(char *tbname, int condcnt, struct condition cond[])
{
	int i;
	int table;
	struct logic_table ltb;

	table = get_tbdef(tbname);
	if (table == -1) {
		printf("ERROR: TABLE %s does not exist.", tbname);
		return -1;
	}

	printf("########## DELETE TABLE ########\n");
	printf("TABLE %s\n", tbname);
	printf("WHERE \n");
	for (i = 0; i < condcnt; i++) {
		printf("\t%s.%s  ", cond[i].lhs_attr.tbname, cond[i].lhs_attr.attrname);
		printf("%s  ", getOP(cond[i].op));
		if (cond[i].is_rhs_attr == 1) {
			printf("%s.%s  ", cond[i].rhs_attr.tbname, cond[i].rhs_attr.attrname);
		} else {
			if (cond[i].rhs_value.type == CHAR)
				printf("%s", cond[i].rhs_value.data);
			else
				printf("%d", *(int *) cond[i].rhs_value.data);
		}
		printf("\n");
	}

	ltb.is_tmp = 0;
	strcpy(ltb.tbname, tbname);
	ltb.len = sys_tables[table].reclen;
	ltb.nattr = sys_tables[table].attrcnt;
	for (i = 0; i < ltb.nattr; i++)
		ltb.attr[i] = sys_tables[table].attr[i];
	delete_pqp(&ltb, condcnt, cond);
	return 0;
}

/* insert statement query plan */
int insert_lqp(char *tbname, int valcnt, struct value val[])
{
	int i;
	int table;
	int addr;
	char rec[1000];
	char *ptr;

	table = get_tbdef(tbname);

	/* semantic check if table exists */
	if (table == -1) {
		printf("ERROR: TABLE %s does not exist.", tbname);
		return -1;
	}

	/* semantic check if value type matches */
	for (i = 0; i< valcnt; i++) {
		if (val[i].type != sys_tables[table].attr[i].type) {
			printf("ERROR: INSERTION TYPE DOES NOT FIT.\n");
			return -1;
		}
	}

	printf("############ INSERT INTO TABLE ###########\n");
	printf("TABLE %s\n", tbname);
	printf("VALUES\n");
	printf("\t( \n");
	for (i = 0; i < valcnt; i++) {
		if (val[i].type == CHAR)
			printf("\t\t%s,  ", val[i].data);
		else if (val[i].type == INT)
			printf("\t\t%d,  ", *(int *) val[i].data);
	}
	printf("\n\t)\n");

	/* pick all value into a rec */
	for (i = 0; i < valcnt; i++) {
		ptr = rec + sys_tables[table].attr[i].offset;
		memcpy(ptr, val[i].data, sys_tables[table].attr[i].len);
	}
	insert_tuple(rec, &addr, tbname);
/*	printf("addr = %d\n", addr);
	//update index
*/	printf("INSERTION SUCCEED.\n");
	return 0;
}
