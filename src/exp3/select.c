/* select */
#include <string.h>
#include <stdio.h>
#include "dict.h"
#include "buffer.h"
#include "scantable.h"
#include "select.h"

/* select operation, table a attr and a vlaue in select condition */
int sel_av(struct logic_table *ltb, struct attrdef *lhs, struct value *val, enum op op)
{
	int intval;
	char strval[1000];
	int j;
	FILE *res;
	char rec[1000];
	struct iterator iter;
	iter = open_iter(ltb);
	if (ltb->is_tmp == 1)
		res = iter.ttb;
	else
		res = fopen(ltb->tbname, "w+b"); 
	ltb->cnt = 0;
	switch (val->type) {
	case INT:
		intval = *(int *) val->data;
		break;
	case CHAR:
		strcpy(strval, val->data);
		break;
	}
	while (getnext_iter(&iter, rec) == 0) {
		/* condition */
		int comp;
		switch (lhs->type) {
		case INT:
			comp = *(int *) (rec + lhs->offset) - intval;
			break;
		case CHAR:
			comp = 0;
			if (lhs->len != strlen(strval))
				break;
			for (j = 0; j < lhs->len; j++)
				if (*(rec + lhs->offset + j) < strval[j]) {
					comp = -1;
					break;
				} else if (*(rec + lhs->offset + j) > strval[j]) {
					comp = 1;
					break;
				}
			if (j == lhs->len)
				comp = 0;
			break;
		}
		if ((op == EQ && comp == 0) ||
			(op == NE && comp != 0) ||
			(op == LE && comp <= 0) ||
			(op == GE && comp >= 0) ||
			(op == LT && comp < 0) ||
			(op == GT && comp > 0))
			; /* its ok */
		else
			continue;
		fseek(res, ltb->cnt * ltb->len, SEEK_SET);
		fwrite(rec, ltb->len, 1, res);
		ltb->cnt++;
	}
	close_iter(&iter);
	if (ltb->is_tmp == 0)
		fclose(res);
	ltb->is_tmp = 1;
	return 0;
}

