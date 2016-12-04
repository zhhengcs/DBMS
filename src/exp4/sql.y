%{
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "lqp.h"
#include "dict.h"
#define print_runtime(t1, t2) printf("This process used %.3lf seconds.\n", (double)((t2) - (t1)) / CLOCKS_PER_SEC)

extern int yylex();
extern int yyerror();

//定义的各种调用函数中变量
 
/* info used for create table */

static char *tbname;        
static int attrcnt;
static struct table_attr attr[MAXATTR_TB];

/* info used for create index and drop index */

static char *attrname;

/* info used for select statement */

static int selcnt;
static struct relattr selattrs[MAXATTR_SEL];
static int tbcnt;
static char *rel[MAXTB_FROM];
static int condcnt;
static int lorr;
static struct condition cond[MAXCOND_SEL];
static char *op;

/* used for order */

static int orderby;
static struct relattr orderattr;

/* info used for delete and insert */

static int valcnt;
static struct value val[MAXATTR_TB];

/* is first line */

int is_ares;
int is_uni;
int is_where;
int is_order;
int is_fin;
int stddbmsstart = 0;
int sys_exit = 0;

/* used for compute process time */

static clock_t before, after;
%}

//
%union {
	int intval;
	char *strval;
}

%start sql_list

//定义的数据类型 

%token <strval> NAME
%token <strval> STRING
%token <intval> NUMBER
%token <strval> COMPARISION

//操作符优先级 

%left AND
%left COMPARISION /* < <= > >= <> = */
%left '+' '-'
%left '*' '/'

//关键词
 
%token SELECT FROM WHERE ORDER BY ASC DESC
%token ALL UNIQUE DISTINCT
%token CREATE TABLE DROP INDEX
%token INSERT INTO VALUES DELETE
%token CHARACTER INTEGER DATE
%token SHOW TABLES
%token EXIT

%%

	/* start place */
sql_list:
		sql
		{
			stddbmsstart = is_fin = 1;
			return 0;
		}
	|	EXIT
		{
			sys_exit = 1;
			return 0;
		}
	;
sql:
		';'
	|	table_def      //创建table命令   
	|	table_drop     //删除table命令 
	|	index_create   //创建索引命令 
	|	index_drop     //删除索引命令 
	|	select_stat    //select语句 
	|	insert_stat    //insert语句 
	|	delete_stat    //delete语句 
	|	table_show     //显示所有创建的表 
	;

	/* show all tables */
table_show:
		SHOW TABLES ';'
		{
			before = clock();
			table_show();
			after = clock();
			print_runtime(before, after);
		}
	;
	/* create table */
table_def:    //创建表 
		CREATE TABLE table '(' table_attr_list ')' ';'
		{
			before = clock();
			create_table_lqp(tbname, attrcnt, attr);
			after = clock();
			print_runtime(before, after);
		}
	;

table:        //表的定义 
		NAME
		{
			tbname = $1;
			rel[tbcnt] = $1;
			is_where = 1;
		}
	;
table_attr_list:   //表的列定义
		column_def
	|	table_attr_list ',' column_def
	;
column_def:
		column data_type     //列名+数据类型 
		{
			attrcnt++;   //列的数量 
		}
	;
column:
		NAME            //列名的定义 
		{
			attrname = $1;
			attr[attrcnt].name = $1;
			selattrs[selcnt].tbname[0] = '\0';
			strcpy(selattrs[selcnt].attrname, $1);
			if (is_where == 1) {
				if (lorr == 0) {
					cond[condcnt].lhs_attr.tbname[0] = '\0';
					strcpy(cond[condcnt].lhs_attr.attrname, $1);
					lorr = 1;
				} else {
					cond[condcnt].rhs_attr.tbname[0] = '\0';
					strcpy(cond[condcnt].rhs_attr.attrname, $1);
				}
			}
			if (is_order == 1) {
				orderattr.tbname[0] = '\0';
				strcpy(orderattr.attrname, $1);
			}
		}
	|	NAME '.' NAME         //前一个NAME是表名，后一个NAME是一个列名 
		{
			strcpy(selattrs[selcnt].tbname, $1);   //1是表名，3是列名 
			strcpy(selattrs[selcnt].attrname, $3);
			if (is_where == 1) {
				if (lorr == 0) {
					strcpy(cond[condcnt].lhs_attr.tbname, $1);
					strcpy(cond[condcnt].lhs_attr.attrname, $3);
					lorr = 1;
				} else {
					strcpy(cond[condcnt].rhs_attr.tbname, $1);
					strcpy(cond[condcnt].rhs_attr.attrname, $3);
				}
			}
			if (is_order == 1) {
				strcpy(orderattr.tbname, $1);
				strcpy(orderattr.attrname, $3);
			}
		}
	;
data_type:  //数据类型的定义 
		CHARACTER '(' NUMBER ')'   //number类型 
		{
			attr[attrcnt].type = CHAR;
			attr[attrcnt].len = $3;
		}
	|	INTEGER                    //整型 
		{
			attr[attrcnt].type = INT;
			attr[attrcnt].len = sizeof(int);
		}
	|	DATE                     //时间类型 
		{
			attr[attrcnt].type = CHAR;
			attr[attrcnt].len = 10;
		}
	;
	/* drop table */
table_drop:         //删除表 
		DROP TABLE table ';'
		{
			before = clock();
			drop_table_lqp(tbname);
			after = clock();
			print_runtime(before, after);
		}
	;
	/* create index */
index_create:
		CREATE INDEX table '(' column ')' ';'
		{
			index_create_lqp(tbname, attrname);
		}
	;
	/* drop index */
index_drop:
		DROP INDEX table '(' column ')' ';'
		{
			index_drop_lqp(tbname, attrname);
		}
	;
	/* select statements */
select_stat:
		select_clause FROM fromlist where_clause order_clause ';'
		{
			before = clock();
			select_lqp(selcnt, selattrs, tbcnt, rel, condcnt, cond, orderby, orderattr, is_uni, is_ares);
			after = clock();
			print_runtime(before, after);
		}
	;
select_clause:   //选择列 
		SELECT unique sellist
	|	SELECT unique '*'       
		{
			is_ares = 1;
		}
	;
unique:
		/* empty */
		{
			is_uni = 0;
		}
	|	ALL
		{
			is_uni = 0;
		}
	|	DISTINCT     //去重操作 
		{
			is_uni = 1;
		}
	|	UNIQUE
		{
			is_uni = 1;
		}
	;
order_clause:  //排序 
		/* empty */
	|	ORDER BY column
		{
			orderby = 1;
		}
	|	ORDER BY column ASC
		{
			orderby = 1;
		}
	|	ORDER BY column DESC
		{
			orderby = 2;
		}
	;
fromlist:    //from之后tables 
		table   //单个表 
		{
			is_where = 1;
			tbcnt++;
		}
	|	fromlist ',' table   //多个表 
		{
			is_where = 1;
			tbcnt++;
		}
	;
sellist:  //select 属性列 
		column    //单个列显示 
		{
			selcnt++;
		}
	|	sellist ',' column  //多个列显示 
		{
			selcnt++;
		}
	;
where_clause:      //条件语句 
		/* empty */   //无条件查询 
		{
			is_order = 1;
		}
	|	WHERE condition    //有条件查询 
		{
			is_order = 1;
		}
	;
condition:  //条件定义 
		expr
		{
			if (strcmp(op, "=") == 0) cond[condcnt].op = EQ;
			else if (strcmp(op, ">=") == 0) cond[condcnt].op = GE;
			else if (strcmp(op, "<=") == 0) cond[condcnt].op = LE;
			else if (strcmp(op, ">") == 0) cond[condcnt].op = GT;
			else if (strcmp(op, "<") == 0) cond[condcnt].op = LT;
			else if (strcmp(op, "<>") == 0) cond[condcnt].op = NE;
			condcnt++;
		}
	|	condition AND expr   //逻辑与 
		{
			if (strcmp(op, "=") == 0) cond[condcnt].op = EQ;
			else if (strcmp(op, ">=") == 0) cond[condcnt].op = GE;
			else if (strcmp(op, "<=") == 0) cond[condcnt].op = LE;
			else if (strcmp(op, ">") == 0) cond[condcnt].op = GT;
			else if (strcmp(op, "<") == 0) cond[condcnt].op = LT;
			else if (strcmp(op, "<>") == 0) cond[condcnt].op = NE;
			condcnt++;
		}
	;
expr:   //条件表达式 
		column COMPARISION column  //列之间比较 
		{
			cond[condcnt].is_rhs_attr = 1;
			op = $2;      //操作符是2 
			lorr = 0;
		}
	|	column COMPARISION NUMBER  //列和数字比较 
		{
			cond[condcnt].is_rhs_attr = 0;
			op = $2;
			lorr = 0;
			cond[condcnt].rhs_value.type = INT;
			memcpy(cond[condcnt].rhs_value.data, &$3, 4);
		}
	|	column COMPARISION STRING  //列和字符串比较 
		{
			cond[condcnt].is_rhs_attr = 0;
			op = $2;
			lorr = 0;
			cond[condcnt].rhs_value.type = CHAR;
			strcpy(cond[condcnt].rhs_value.data, $3);
		}
	;
	/* insert statements */

insert_stat:     //insert 语句 
		
		INSERT INTO table VALUES '(' insert_list ')' ';'
		{
			before = clock();
			insert_lqp(tbname, valcnt, val);
			after = clock();
			print_runtime(before, after);
		}
	;
insert_list: //插入值列表 
		NUMBER  //插入数字 
		{
			val[valcnt].type = INT;
			memcpy(val[valcnt].data, &$1, 4);
			valcnt++;
		}
	|	STRING  //插入字符串 
		{
			val[valcnt].type = CHAR;
			strcpy(val[valcnt].data, $1);
			valcnt++;
		}
	|	insert_list ',' NUMBER  //插入多个数字 
		{
			val[valcnt].type = INT;
			memcpy(val[valcnt].data, &$3, 4);
			valcnt++;
		}
	|	insert_list ',' STRING  //插入多个string 
		{
			val[valcnt].type = CHAR;
			strcpy(val[valcnt].data, $3);
			valcnt++;
		}
	;
	/* delete statement */
	
delete_stat:  // delete 语句
 
		DELETE FROM table where_clause ';'
		{
			before = clock();
			delete_lqp(tbname, condcnt, cond);
			after = clock();
			print_runtime(before, after);
		}
	;
%%
int init_param() //初始化变量  
{
	attrcnt = selcnt = tbcnt = 0;
	condcnt = valcnt = 0;
	lorr = 0;
	is_ares = 0;
	is_fin = 0;
	is_where = 0;
	is_order = 0;
	is_uni = 0;
	orderby = 0;
	return 0;
}

