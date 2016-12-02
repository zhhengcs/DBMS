/* project */
#include <stdio.h>
#include <string.h>
#include "project.h"
#include "scantable.h"

/* take project operation with the selattr list in selattr[] */
struct logic_table project(struct logic_table *ltb, int nsel, struct relattr selattr[])
{
	int i, j;
	struct iterator iter;
	struct logic_table lr;
	int oflen;
	int ofs[200];
	char rec[1000];
	char rec2[1000];
	char *ptr;
	FILE *res;
	/* generate new ltb */
	lr.is_tmp = 1;
	strcpy(lr.tbname, ltb->tbname);
	lr.nattr = nsel;
	lr.cnt = 0;
	for (i = 0; i < nsel; i++)
		for (j = 0; j < ltb->nattr; j++)
			if ((strcmp(selattr[i].tbname, ltb->attr[j].relname) == 0) &&
				(strcmp(selattr[i].attrname, ltb->attr[j].name) == 0)) {
				lr.attr[i] = ltb->attr[j];
				break;
			}
	oflen = 0;
	for (i = 0; i < nsel; i++) {
		ofs[i] = lr.attr[i].offset;
		lr.attr[i].offset = oflen;
		oflen += lr.attr[i].len;
		oflen = (oflen + 3) / 4 * 4;
	}
	lr.len = oflen;
	/* project */
	iter = open_iter(ltb);
	if (ltb->is_tmp == 1)
		res = iter.ttb;
	else
		res = fopen(ltb->tbname, "w+b"); 
	while (getnext_iter(&iter, rec) == 0) {
		for (i = 0; i < nsel; i++) {
			ptr = rec2 + lr.attr[i].offset;
			memcpy(ptr, rec + ofs[i], lr.attr[i].len);
		}
		fseek(res, lr.cnt * lr.len, SEEK_SET);
		fwrite(rec2, lr.len, 1, res);
		lr.cnt++;
	}
	close_iter(&iter);
	if (ltb->is_tmp == 0)
		fclose(res);
	return lr;
}

