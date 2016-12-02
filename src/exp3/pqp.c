/* pqp */
#include <stdio.h>
#include <string.h>
#include "pqp.h"
#include "select.h"
#include "join.h"
#include "tablespace.h"
#include "scantable.h"
#include "lqp.h"

/* select physical plan */
int select_pqp(struct logic_table *ltb, struct condition *cond)
{
	int i;
	struct attrdef lhs, rhs;
	struct value val;
	int is_rhs_attr = cond->is_rhs_attr;
	for (i = 0; i < ltb->nattr; i++)
		if (strcmp(cond->lhs_attr.tbname, ltb->attr[i].relname) == 0 &&
			strcmp(cond->lhs_attr.attrname, ltb->attr[i].name) == 0) {
			lhs = ltb->attr[i];
			break;
		}
	if (is_rhs_attr == 1) {
		for (i = 0; i < ltb->nattr; i++)
			if (strcmp(cond->rhs_attr.tbname, ltb->attr[i].relname) == 0 &&
				strcmp(cond->rhs_attr.attrname, ltb->attr[i].name) == 0) {
				rhs = ltb->attr[i];
				break;
			}
	} else {
		val = cond->rhs_value;
	}
	/* examine */
	if (is_rhs_attr == 1) {
		if (lhs.type != rhs.type) {
			printf("WHERE condition doesnot fit.\n");
			return -1;
		}
	} else if (lhs.type != cond->rhs_value.type) {
		printf("WHERE condition doesnot fit.\n");
		return -1;
	}
/*	if (lhs.type == CHAR)
		if (cond->op == LT || cond->op == GT || cond->op == LE || cond->op == GE) {
			printf("string may only use == or <>\n");
			return -1;
		}
*/	//if (is_rhs_attr == 1)
	//	sel_aa(ltb, &lhs, &rhs, cond->op);   // select a value: µÈÖµ²éÕÒ 
	//else
		sel_av(ltb, &lhs, &val, cond->op);  // select a value: µÈÖµ²éÕÒ 
	return 0;
}
/* delete pqp */
int delete_pqp(struct logic_table *ltb, int condcnt, struct condition cond[])
{
	int intval;
	char strval[1000];
	int i, j, k;
	char rec[1000];
	int cnt;
	struct iterator iter;
	struct attrdef *lhs[100];
	struct value *val[100];
	for (k = 0; k < condcnt; k++) {
		for (i = 0; i < ltb->nattr; i++)
			if (strcmp(cond[k].lhs_attr.attrname, ltb->attr[i].name) == 0) {
				lhs[k] = &(ltb->attr[i]);
				break;
			}
		val[k] = &(cond[k].rhs_value);
		if (lhs[k]->type != cond[k].rhs_value.type) {
			printf("WHERE condition doesnot fit.\n");
			return -1;
		}
/*		if (lhs[k]->type == CHAR)
			if (cond[k].op == LT || cond[k].op == GT || cond[k].op == LE || cond[k].op == GE) {
				printf("string may only use == or <>\n");
				return -1;
			}
*/	}
	iter = open_iter(ltb);
	cnt = 0;
	while (getnext_iter(&iter, rec) == 0) {
		int comp;
		for (k = 0; k < condcnt; k++) {
			switch (val[k]->type) {
			case INT:
				intval = *(int *) val[k]->data;
				break;
			case CHAR:
				strcpy(strval, val[k]->data);
				break;
			}
			/* condition */
			switch (lhs[k]->type) {
			case INT:
				comp = *(int *) (rec + lhs[k]->offset) - intval;
				break;
			case CHAR:
				comp = 0;
				if (lhs[k]->len != strlen(strval))
					break;
				for (j = 0; j < lhs[k]->len; j++)
					if (*(rec + lhs[k]->offset + j) < strval[j]) {
						comp = -1;
						break;
					} else if (*(rec + lhs[k]->offset + j) > strval[j]) {
						comp = 1;
						break;
					}
				if (j == lhs[k]->len)
					comp = 0;
				break;
			}
			if ((cond[k].op == EQ && comp == 0) ||
				(cond[k].op == NE && comp != 0) ||
				(cond[k].op == LE && comp <= 0) ||
				(cond[k].op == GE && comp >= 0) ||
				(cond[k].op == LT && comp < 0) ||
				(cond[k].op == GT && comp > 0))
				; /* its ok */
			else
				break;
		}
		if (k == condcnt) {
			int addr;
			int pgid;
			/* this rec is ok delete */
			cnt++;
			pgid = FILE_HDRSIZE + iter.extent * PAGE_PER_EXTENT + EXTHDR_SIZE + iter.blk;
			addr = pgid * ADDRBASE + iter.t + 1;
			del_tuple(addr, ltb->tbname);
		}
	}
	printf("Total %d tuples deleted.\n", cnt);
	close_iter(&iter);
	return 0;
}

/*
 * equal join pqp
 * table tbn1 and tbn2 for join with conditon cond
 * return the schema after join
 */
struct logic_table join_pqp(char *tbname, struct logic_table *tbn1, struct logic_table *tbn2, struct condition *cond)
{
	int i;
	struct logic_table ltb;
	struct attrdef lhs, rhs;
	/* generate new table schema */
	for (i = 0; i < tbn1->nattr; i++)
		if (strcmp(cond->lhs_attr.tbname, tbn1->attr[i].relname) == 0 &&
			strcmp(cond->lhs_attr.attrname, tbn1->attr[i].name) == 0) {
			lhs = tbn1->attr[i];
			break;
		}
	for (i = 0; i < tbn2->nattr; i++)
		if (strcmp(cond->rhs_attr.tbname, tbn2->attr[i].relname) == 0 &&
			strcmp(cond->rhs_attr.attrname, tbn2->attr[i].name) == 0) {
			rhs = tbn2->attr[i];
			break;
		}
	if (lhs.type != rhs.type) {
		printf("WHERE condition doesnot fit.\n");
		return ltb;
	}
	if (cond->op != EQ) {
		printf("Only support natrual join and equal join\n");//Ö»Ö§ï¿½ï¿½ï¿½ï¿½È»ï¿½ï¿½ï¿½ÓºÍµï¿½Öµï¿½ï¿½ï¿½ï¿½
		return ltb;
	}
	ltb.is_tmp = 1;
	ltb.cnt = 0;
	strcpy(ltb.tbname, tbname);
	ltb.len = tbn1->len + tbn2->len;
	ltb.nattr = tbn1->nattr + tbn2->nattr;//joinï¿½ï¿½ï¿½Ôªï¿½é³¤ï¿½Èµï¿½ï¿½ï¿½Ô­ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ôªï¿½ï¿½Ä³ï¿½ï¿½ï¿½Ö®ï¿½Í£ï¿½ï¿½ï¿½ï¿½ï¿½Ò²ï¿½ï¿½ï¿½ï¿½ï¿½ÇµÄºï¿½
	for (i = 0; i < tbn1->nattr; i++)
		ltb.attr[i] = tbn1->attr[i];
	for (i = 0; i < tbn2->nattr; i++) {
		ltb.attr[i + tbn1->nattr] = tbn2->attr[i];
		ltb.attr[i + tbn1->nattr].offset += tbn1->len;
	}
	sort_join(&ltb, tbn1, tbn2, &lhs, &rhs);
	return ltb;
}
/*
 * product pqp µÏ¿¨¶û»ý 
 * table tbn1 and tbn2 for product
 * return the schema after product
 */
struct logic_table cross_pqp(char *tbname, struct logic_table *tbn1, struct logic_table *tbn2)
{
	int i;
	struct logic_table ltb;
	ltb.is_tmp = 1;
	ltb.cnt = 0;
	strcpy(ltb.tbname, tbname);
	ltb.len = tbn1->len + tbn2->len;
	ltb.nattr = tbn1->nattr + tbn2->nattr;
	for (i = 0; i < tbn1->nattr; i++)
		ltb.attr[i] = tbn1->attr[i];
	for (i = 0; i < tbn2->nattr; i++) {
		ltb.attr[i + tbn1->nattr] = tbn2->attr[i];
		ltb.attr[i + tbn1->nattr].offset += tbn1->len;
	}
	cross(&ltb, tbn1, tbn2);
	return ltb;
}

