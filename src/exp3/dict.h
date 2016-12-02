/*
 * data dictionay
 */

#ifndef DICT_H
#define DICT_H

#include "lqp.h"

/* a attribution's define */
struct attrdef {
	char relname[MAXLEN_NAME];
	char name[MAXLEN_NAME];
	enum attr_type type;
	int len;
	int offset;
	int has_index;
};

/* a table's relation defination */
struct table_def {
	char relname[MAXLEN_NAME];
	int attrcnt;
	int reclen;
	int reccnt;
	struct attrdef attr[MAXATTR_TB];
	int first_ext;
};

/* a index table defination */
struct idx_def {
	char relname[MAXLEN_NAME];
	char name[MAXLEN_NAME];
	enum attr_type type;
	int btree_root;
	int btree_level;
};

int ntab; /* how many tables db has */
#define MAX_TABLE_NUM 100
struct table_def sys_tables[MAX_TABLE_NUM];
int nindex; /* how many index items */
#define MAX_IDX_ITEMS 1000
struct idx_def sys_indexes[MAX_IDX_ITEMS];

FILE *dictfd; /* data dict file description */
int load_dict();
int sav_dict();
int close_dict();

int create_table(char *tbname, int attrcnt, struct table_attr attr[]);
int drop_table(char *tbname);

int get_tbdef(const char *tbname); /**/
int table_show();

#endif

