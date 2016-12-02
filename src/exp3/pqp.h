/*
 * Used for physic query plan
 */

#ifndef PQP_H
#define PQP_H

#include <stdio.h>
#include "lqp.h"
#include "dict.h"

/* logic table store this table's schema */
struct logic_table {
	int is_tmp;
	int cnt; /* how many tuple */
	char tbname[200];
	int len;
	int nattr;
	struct attrdef attr[MAXATTR_TB];
};

int select_pqp(struct logic_table *ltb, struct condition *cond);
struct logic_table join_pqp(char *tbname, struct logic_table *tbn1, struct logic_table *tbn2, struct condition *cond);
struct logic_table cross_pqp(char *tbname, struct logic_table *tbn1, struct logic_table *tbn2);
int delete_pqp(struct logic_table *ltb, int condcnt, struct condition cond[]);

#endif

