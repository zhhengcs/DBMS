/*
 * For every join method

 * sort join in sortjoin.c
 * hash join in hashjoin.c
 * other in join.c
 */
#ifndef _JOIN_H
#define _JOIN_H

int product(const char *tb1, const char *tb2);
int nest_tuple_join(const char *tb1, const char *tb2);
int nest_blk_join(const char *tb1, const char *tb2);
int index_join(const char *tb1, const char *tb2);
int hash_join(const char *tb1, const char *tb2);
int equaljoin(struct logic_table *ltb, struct logic_table *tbn1, struct logic_table *tbn2, struct attrdef *lhs, struct attrdef *rhs);
int sort_join(struct logic_table *ltb, struct logic_table *ltb1, struct logic_table *ltb2, struct attrdef *lhs, struct attrdef *rhs);
int cross(struct logic_table *ltb, struct logic_table *ltb1, struct logic_table *ltb2);
int order_tb(struct logic_table *ltb, int orderby, struct relattr orderattr);
int unique_tb(struct logic_table *ltb);

#endif

