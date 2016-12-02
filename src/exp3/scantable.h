/*
 * used for table scan
 * have a iterator
 * print tuple
 * modified
 */
#ifndef _SCANTABLE_H
#define _SCANTABLE_H

#include "dict.h"
#include "page.h"
#include "pqp.h"

struct iterator {
	int is_tmp; /* whether is a tmp table */
	FILE *ttb;
	int cnt;
	int idx; /* what idx record */

	int extent; /* current which extent */
	int blk; /* in which block of the cur extent */
	char b[PAGE_SIZE]; /* cur page content point */
	int t; /* ?th tup of the cur page */
	int len; /* tup len */
};

struct iterator open_iter(struct logic_table *ltb);
/* fetch next tuple, store in rec */
int getnext_iter(struct iterator *iter, char *rec);
int close_iter(struct iterator *iter);

#endif

