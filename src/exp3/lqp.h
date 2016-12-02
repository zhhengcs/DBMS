/*
 * For some definition for convert
 * parser tree to query plan

 */

#ifndef _QP_H
#define _QP_H

/* relation name and attrname's max len */
#define MAXLEN_NAME 100
/* how many attr can a table have */
#define MAXATTR_TB 100
/* how many attr can a select have */
#define MAXATTR_SEL 200
/* how table can a from have */
#define MAXTB_FROM 100
/* how many condition can a where have */
#define MAXCOND_SEL 200
//枚举类型，只支持int和char类型 
enum attr_type { INT, CHAR };

const char* getAttrType(enum attr_type attrType);

/* table attrbute */
struct table_attr {
	char *name;
	enum attr_type type;
	int len;
};

int create_table_lqp(char *tbname, int attrcnt, struct table_attr attr[]);
int drop_table_lqp(char *tbname);
int index_create_lqp(char *tbnmae, char *attrname);
int index_drop_lqp(char *tbname, char *attrname);

struct relattr {
	char condnm[MAXLEN_NAME];
	char tbname[MAXLEN_NAME];
	char attrname[MAXLEN_NAME];
};
/* store value and type in where */
struct value {
	enum attr_type type;
	char data[2000];
};
enum op { LT, GT, LE, GE, EQ, NE };
const char* getOP(enum op op);

/* store a condition */
struct condition {
	struct relattr lhs_attr;
	enum op op;
	int is_rhs_attr; 
	//判定右侧是列名还是值 
	struct relattr rhs_attr;
	struct value rhs_value;
};

/*
 * select statement's logic qp
 * selcnt: select list attr num; selattrs[]: the all attrs in sellist
 * tbcnt: rel number in from list; rel[]: the from list table list
 * condcnt: the condition count; cond[]: all condition exists in where clause
 * orderby: whether order by?
 * orderattr: the attr name used to order the result table
 * return 0 if succed
 */
int select_lqp(int selcnt, struct relattr selattrs[], int tbcnt, char *rel[], int condcnt, struct condition cond[], int orderby, struct relattr orderattr, int is_uni, int is_ares);

int delete_lqp(char *tbname, int condcnt, struct condition cond[]);
int insert_lqp(char *tbname, int valcnt, struct value val[]);

#endif

