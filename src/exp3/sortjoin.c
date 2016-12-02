/*
 * sort join
 */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "scantable.h"
#include "dict.h"
#include "join.h"
#include "buffer.h"

#define BUFSIZE 2000
#define TOTALREC 100000
/* assume max dup records for convinient, not right */
#define DUPMAX 40
#define MAXNUM 0x7fffffff

/* two tmp file */
static FILE *ftb1, *ftb2;
static char buf[BUFSIZE][PAGE_SIZE];
static char buf1[BUFSIZE][PAGE_SIZE]; /* i am cheat, i actually use two mem size */
/* sort struct */
static struct qnode {
	char key[120];
	int pos;
} qsts[TOTALREC];
static int total;
static int loop1, loop2;
static int subidx[BUFSIZE];
static int blkidx[BUFSIZE];
static int blktot[BUFSIZE];
static int len1, len2;
static struct attrdef *hs;

/* compare func used for sort */
int cmp(const void *a, const void *b)
{
	int i;
	struct qnode *c = (struct qnode *)a;
	struct qnode *d = (struct qnode *)b;
	switch (hs->type) {
	case INT:
		return *(int *) (c->key) - *(int *) (d->key);
		break;
	case CHAR:
		for (i = 0; i < hs->len; i++)
			if(*(c->key + i) < *(d->key + i))
				return -1;
			else if(*(c->key + i) > *(d->key + i))
				return 1;
		return 0;
		break;
	default:
		break;
	}
	return 0;
}

int cmpd(const void *a, const void *b)
{
	int i;
	struct qnode *d = (struct qnode *)a;
	struct qnode *c = (struct qnode *)b;
	switch (hs->type) {
	case INT:
		return *(int *) (c->key) - *(int *) (d->key);
		break;
	case CHAR:
		for (i = 0; i < hs->len; i++)
			if(*(c->key + i) < *(d->key + i))
				return -1;
			else if(*(c->key + i) > *(d->key + i))
				return 1;
		return 0;
		break;
	default:
		break;
	}
	return 0;
}

/* sort a table and output to a tmp file */
int sort_output(FILE *ftb, struct iterator *itertb, int *loop)
{
	int i;
	int blk, idx;
	int cur, cnt;
	int tot = (PAGE_SIZE - 4) / itertb->len;
	for (i = 0; i < total; i++) {
		blk = i / tot;
		idx = i % tot;
		qsts[i].pos = i;
		memcpy(qsts[i].key, buf[blk] + PGHDR_SIZE + idx * itertb->len + hs->offset, hs->len);
	}
	qsort(qsts, total, sizeof(qsts[0]), cmp);
	memcpy(buf1, buf, sizeof(buf));
	for (i = 0; i < total; i++) {
		blk = qsts[i].pos / tot;
		idx = qsts[i].pos % tot;
		cur = i / tot;
		cnt = i % tot;
		memcpy(buf[cur] + PGHDR_SIZE + cnt * itertb->len, buf1[blk] + PGHDR_SIZE + idx * itertb->len, itertb->len);
	}
	fseek(ftb, sizeof(buf) * *loop, SEEK_SET);
	fwrite(buf, sizeof(char), sizeof(buf), ftb);
	return 0;
}

/* init the table want to sort */
int initsort(struct logic_table *ltb, FILE *ftb, int *loop)
{
	int cur;
	int cnt, tot;
	char rec[400];
	struct iterator itertb;
	itertb = open_iter(ltb);
	tot = (PAGE_SIZE - 4) / itertb.len;
	/* hahahahahhahahahhahahhaha */
	/* hahahahhahahahhahahhahaha */
	memset(buf, 0, sizeof(buf));
	cnt = cur = total = 0;
	*loop = 0;
	while (getnext_iter(&itertb, rec) == 0) {
		if (cnt == tot) {
			memcpy(buf[cur], (char *)&cnt, 4);
			cur++;
			cnt = 0;
		}
		if (cur == BUFSIZE) {
			/* sort and output all buf */
			sort_output(ftb, &itertb, loop);
			*loop = *loop + 1;
			cur = cnt = total = 0;
			memset(buf, 0, sizeof(buf));
		}
		memcpy(buf[cur] + PGHDR_SIZE + cnt * itertb.len, rec, itertb.len);
		cnt++;
		total++;
	}
	memcpy(buf[cur], (char *)&cnt, 4);
	if (total != 0) {
		sort_output(ftb, &itertb, loop);
		*loop = *loop + 1;
	}
	close_iter(&itertb);
	return 0;
}

static struct qnode qst[1100000];
static int flag[1100000];

/* elimite dul tuples */
int unique_tb(struct logic_table *ltb)
{
	int i;
	char rec[1000];
	struct attrdef ad;
	struct iterator itertb;
	FILE *res;
	int cnt;
	ad.len = ltb->len;
	ad.type = CHAR;
	hs = &ad;
	cnt = 0;
	itertb = open_iter(ltb);
	while (getnext_iter(&itertb, rec) == 0) {
		qst[cnt].pos = cnt;
		memcpy(qst[cnt].key, rec, hs->len);
		cnt++;
	}
	close_iter(&itertb);
	qsort(qst, cnt, sizeof(qst[0]), cmp);
	memset(flag, 0, sizeof(flag));
	for (i = 0; i < cnt; i++)
		if (i != 0 && cmp(&qst[i - 1], &qst[i]) == 0)
			flag[qst[i].pos] = 1;
	itertb = open_iter(ltb);
	if (ltb->is_tmp == 1)
		res = itertb.ttb;
	else
		res = fopen(ltb->tbname, "w+b"); 
	cnt = 0;
	ltb->cnt = 0;
	while (getnext_iter(&itertb, rec) == 0) {
		if (flag[cnt] == 0) {
			fseek(res, ltb->cnt * ltb->len, SEEK_SET);
			fwrite(rec, ltb->len, 1, res);
			ltb->cnt++;
		}
		cnt++;
	}
	close_iter(&itertb);
	if (ltb->is_tmp == 0)
		fclose(res);
	ltb->is_tmp = 1;
	return 0;
}

/*
 * used the order attr orderattr to sort the result relation
 * return 0 if no error occur
 */
int order_tb(struct logic_table *ltb, int orderby, struct relattr orderattr)
{
	int i;
	int cnt;
	char rec[1000];
	struct iterator itertb;
	FILE *res;
	FILE *tmp;
	for (i = 0; i < ltb->nattr; i++)
		if (strcmp(orderattr.tbname, ltb->attr[i].relname) == 0 &&
			strcmp(orderattr.attrname, ltb->attr[i].name) == 0) {
			hs = &(ltb->attr[i]);
			break;
		}
	itertb = open_iter(ltb);
	cnt = 0;
	tmp = fopen("stmp", "w+b");
	fseek(tmp, 0, SEEK_SET);
	while (getnext_iter(&itertb, rec) == 0) {
		fwrite(rec, ltb->len, 1, tmp);
		qst[cnt].pos = cnt;
		memcpy(qst[cnt].key, rec + hs->offset, hs->len);
		cnt++;
	}
	close_iter(&itertb);
	ltb->cnt = cnt;
	ltb->is_tmp = 1;
	if (orderby == 1)
		qsort(qst, cnt, sizeof(qst[0]), cmp);
	else if (orderby == 2)
		qsort(qst, cnt, sizeof(qst[0]), cmpd);
	/* write back */
	res = fopen(ltb->tbname, "w+b");
	fseek(res, 0, SEEK_SET);
	for (i = 0; i < cnt; i++) {
		fseek(tmp, qst[i].pos * ltb->len, SEEK_SET);
		fread(rec, ltb->len, 1, tmp);
		fwrite(rec, ltb->len, 1, res);
	}
	fclose(tmp);
	remove("stmp");
	fclose(res);
	return 0;
}

/*

 * can use array pt[1-2], loop[1-2], ...... 
 * i do not want to modify, just remain this
 */
int sjoin(struct logic_table *ltb, struct attrdef *lhs, struct attrdef *rhs)
{
	/* shit!!!!!!!!!!!!!!!!!!!!!!!!!!!! */
	char rec[1000];
	char rec1[DUPMAX][500], rec2[DUPMAX][500];
	int cnt1, cnt2;
	int i, j;
	int tot = loop1 + loop2; /* total sub table */
	int pt1, pt2;
	FILE *res = fopen(ltb->tbname, "w+b"); 
	/*************************/
	pt1 = 0;
	pt2 = pt1 + loop1;
	memset(subidx, 0, sizeof(subidx));
	/* read all sub table into mem */
	for (i = 0; i < loop1; i++) {
		fseek(ftb1, sizeof(buf) * i + subidx[pt1 + i] * PAGE_SIZE, SEEK_SET);
		fread(buf[pt1 + i], sizeof(char), PAGE_SIZE, ftb1);
	}
	for (i = 0; i < loop2; i++) {
		fseek(ftb2, sizeof(buf) * i + subidx[pt2 + i] * PAGE_SIZE, SEEK_SET);
		fread(buf[pt2 + i], sizeof(char), PAGE_SIZE, ftb2);
	}
	/* start join */
	memset(blkidx, 0, sizeof(blkidx));
	for (i = 0; i < tot; i++)
		blktot[i] = *(int *)buf[i];
	for ( ; ; ) {
		int min, min1, min2;
		char smin[500], smin1[500], smin2[500], smax[500];
		/* find the min key in tb1 */
		min = min1 = min2 = MAXNUM;
		memset(smin, 0x7f, sizeof(smin));
		memset(smin1, 0x7f, sizeof(smin1));
		memset(smin2, 0x7f, sizeof(smin2));
		smin[499] = smin1[499] = smin2[499] = '\0';
		strcpy(smax, smin);
		for (i = 0; i < loop1; i++) {
			char key[500];
			if (subidx[pt1 + i] >= BUFSIZE)
				continue;
			memcpy(key, buf[pt1 + i] + PGHDR_SIZE + blkidx[pt1 + i] * len1 + lhs->offset, lhs->len);
			key[lhs->len] = '\0';
			if (lhs->type == INT) {
				if (*(int *)key < min1)
					min1 = *(int *)key;
			} else if (lhs->type == CHAR) {
				if (strcmp(key, smin1) < 0)
					strcpy(smin1, key);
			}
		}
		if (lhs->type == INT) {
			if (min1 == MAXNUM)
				break;
		} else if (lhs->type == CHAR) {
			if (strcmp(smax, smin1) == 0)
				break;
		}
		/* find the min key in tb2 */
		for (i = 0; i < loop2; i++) {
			char key[500];
			if (subidx[pt2 + i] >= BUFSIZE)
				continue;
			memcpy(key, buf[pt2 + i] + PGHDR_SIZE + blkidx[pt2 + i] * len2 + rhs->offset, rhs->len);
			key[rhs->len] = '\0';
			if (rhs->type == INT) {
				if (*(int *)key < min2)
					min2 = *(int *)key;
			} else if (lhs->type == CHAR) {
				if (strcmp(key, smin2) < 0)
					strcpy(smin2, key);
			}
		}
		if (rhs->type == INT) {
			if (min2 == MAXNUM)
				break;
		} else if (rhs->type == CHAR) {
			if (strcmp(smax, smin2) == 0)
				break;
		}

		/* the min value, take for join */
		if (rhs->type == INT) {
			min = min1 < min2 ? min1 : min2;
		} else if (rhs->type == CHAR) {
			if (strcmp(smin1, smin2) < 0)
				strcpy(smin, smin1);
			else
				strcpy(smin, smin2);
		}
		cnt1 = cnt2 = 0;
		/* get all record that key value is min to rec1 */
		for (i = 0; i < loop1; i++) {
			char key[500];
			for ( ; ; ) {
				if (subidx[pt1 + i] >= BUFSIZE)
					break;
				memcpy(key, buf[pt1 + i] + PGHDR_SIZE + blkidx[pt1 + i] * len1 + lhs->offset, lhs->len);
				key[lhs->len] = '\0';
				if (lhs->type == INT) {
					if (*(int *)key != min)
						break;
				} else if (lhs->type == CHAR) {
					if (strcmp(key, smin) != 0)
						break;
				}
				memcpy(rec1[cnt1++], buf[pt1 + i] + PGHDR_SIZE + blkidx[pt1 + i] * len1, len1);
				blkidx[pt1 + i]++;
				/* need another block */
				while (blkidx[pt1 + i] == blktot[pt1 + i]) {
					subidx[pt1 + i]++;
					blkidx[pt1 + i] = 0;
					if (subidx[pt1 + i] < BUFSIZE) {
						fseek(ftb1, sizeof(buf) * i + subidx[pt1 + i] * PAGE_SIZE, SEEK_SET);
						fread(buf[pt1 + i], sizeof(char), PAGE_SIZE, ftb1);
						blktot[pt1 + i] = *(int *)buf[pt1 + i];
					} else {
						break;
					}
				}
			}
		} /* end for */
		/* get all record that key value is min to rec2 */
		for (i = 0; i < loop2; i++) {
			char key[500];
			for ( ; ; ) {
				if (subidx[pt2 + i] >= BUFSIZE)
					break;
				memcpy(key, buf[pt2 + i] + PGHDR_SIZE + blkidx[pt2 + i] * len2 + rhs->offset, rhs->len);
				key[rhs->len] = '\0';
				if (rhs->type == INT) {
					if (*(int *)key != min)
						break;
				} else if (lhs->type == CHAR) {
					if (strcmp(key, smin) != 0)
						break;
				}
				memcpy(rec2[cnt2++], buf[pt2 + i] + PGHDR_SIZE + blkidx[pt2 + i] * len2, len2);
				blkidx[pt2 + i]++;
				/* need another block */
				while (blkidx[pt2 + i] == blktot[pt2 + i]) {
					subidx[pt2 + i]++;
					blkidx[pt2 + i] = 0;
					if (subidx[pt2 + i] < BUFSIZE) {
						fseek(ftb2, sizeof(buf) * i + subidx[pt2 + i] * PAGE_SIZE, SEEK_SET);
						fread(buf[pt2 + i], sizeof(char), PAGE_SIZE, ftb2);
						blktot[pt2 + i] = *(int *)buf[pt2 + i];
					} else {
						break;
					}
				}
			}
		} /* end for */
		/* just join */
		if (cnt1 == 0 || cnt2 == 0)
			continue;
		for (i = 0; i < cnt1; i++) {
			for (j = 0; j < cnt2; j++) {
				memcpy(rec, rec1[i], len1);
				memcpy(rec + len1, rec2[j], len2);
				fseek(res, ltb->cnt * ltb->len, SEEK_SET);
				fwrite(rec, ltb->len, 1, res);
				ltb->cnt++;
			}
		}
	}
	fclose(res);
	return 0;
}

/* sort join two table ltb1 and ltb2 */
int sort_join(struct logic_table *ltb, struct logic_table *ltb1, struct logic_table *ltb2, struct attrdef *lhs, struct attrdef *rhs)
{
	/* sort two table */
	ftb1 = fopen("tmp1", "w+b");  //生成两个临时表 
	ftb2 = fopen("tmp2", "w+b");
	len1 = ltb1->len;
	len2 = ltb2->len;
	hs = lhs;
	initsort(ltb1, ftb1, &loop1);
	hs = rhs;
	initsort(ltb2, ftb2, &loop2);
	/* actual join */
	sjoin(ltb, lhs, rhs);
	/* close tmp file */
	fclose(ftb1);
	fclose(ftb2);
	remove("tmp1");
	remove("tmp2");
	return 0;
}

