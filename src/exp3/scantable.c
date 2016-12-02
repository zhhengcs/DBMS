/* scan table */
#include <string.h>
#include <stdio.h>
#include "dict.h"
#include "tablespace.h"
#include "scantable.h"
#include "buffer.h"

/*
 * open a iterator about table tbname
 */
 //逻辑表还是真实存在的表？ 
struct iterator open_iter(struct logic_table *ltb)
{
	int pgid, recid;
	struct iterator res;
	int table;
	/* is a tmp table */
	if (ltb->is_tmp == 1) {
		res.is_tmp = 1;
		res.ttb = fopen(ltb->tbname, "r+b");
		res.cnt = ltb->cnt;
		res.idx = 0;
		res.len = ltb->len;
		return res;
	}
	/* is a fact table */
	res.is_tmp = 0;
	table = get_tbdef(ltb->tbname);

	res.extent = sys_tables[table].first_ext;
	res.len = sys_tables[table].reclen;
	res.blk = 0;

	if (res.extent != -1) {
		pgid = FILE_HDRSIZE + res.extent * PAGE_PER_EXTENT + res.blk + EXTHDR_SIZE;
		recid = getpg_LRU(pgid);
		memcpy(res.b, buff[recid].page, PAGE_SIZE);
		res.t = *(int *) res.b - 1;
	}
	return res;
}

/*
 * get next tuple of iter
 * put that record into rec
 * return 1 if no next, return 0 if ok
 */
int getnext_iter(struct iterator *iter, char *rec)
{
	int pgid, recid, buffid;
	char *ptr;
	struct extent_header *eh;

	/* is a tmp table */
	if (iter->is_tmp == 1) {
		if (iter->idx >= iter->cnt)
			return 1;
		fseek(iter->ttb, iter->len * iter->idx, SEEK_SET);
		fread(rec, iter->len, 1, iter->ttb);
		iter->idx++;
		return 0;
	}
	if (iter->extent == -1)
		return 1;
	ptr = iter->b;
	ptr += 4;
	if (iter->t >= 0) { /* in cur block */
		ptr += iter->t * iter->len;
		memcpy(rec, ptr, iter->len);
		iter->t--;
		return 0;
	}//????????????????
	/* have to read another block */
	if (iter->blk < MAX_DAT_PAGE - 1) { /* in cur extent */
		iter->blk++;
		pgid = FILE_HDRSIZE + iter->extent * PAGE_PER_EXTENT + iter->blk + EXTHDR_SIZE;
		recid = getpg_LRU(pgid);
		memcpy(iter->b, buff[recid].page, PAGE_SIZE);
		iter->t = *(int *) iter->b - 1;
		return getnext_iter(iter, rec);
	}
	/* have to read another extent */
	pgid = FILE_HDRSIZE + iter->extent * PAGE_PER_EXTENT;
	buffid = getpg_LRU(pgid);
	eh = (struct extent_header *)buff[buffid].page;
	if (eh->next == -1) {
		return 1;
	} else {
		iter->extent = eh->next;
		iter->blk = 0;
		pgid = FILE_HDRSIZE + iter->extent * PAGE_PER_EXTENT + iter->blk + EXTHDR_SIZE;
		recid = getpg_LRU(pgid);
		memcpy(iter->b, buff[recid].page, PAGE_SIZE);
		iter->t = *(int *) iter->b - 1;
		return getnext_iter(iter, rec);
	}
}

/*
 * close everything
 */
int close_iter(struct iterator *iter)
{
	/* is a tmp table */
	if (iter->is_tmp == 1) {
		fclose(iter->ttb);
	}
	return 0;
}

/* for test
int printtb()
{
	int buffid, table;
	char rec[200];
	struct iterator iter;
	struct table_def *td;
	table = get_tbdef("join1");
	buffid = getpg_LRU(table);
	td = (struct table_def *)buff[buffid].page;
	print_tbhdr(td);
	iter = open_iter("join1");
	while (getnext_iter(&iter, rec) == 0) {
		print_tuple(rec, td);
	}
	return 0;
}
*/
#if 0
int print_tuple(char *rec, struct table_def *tdef)
{
	FILE *fp = stdout;
	int i, j, k;
	char hdrln[400];
	char *ptr = rec;
	fprintf(fp, "|");
	for (j = 0; j < tdef->rel.attrcnt; j++) {
		ptr = rec + TUPHDR_SIZE + tdef->attr[j].offset;
		switch (tdef->attr[j].type) {
		case INT:
			fprintf(fp, "%9d ", *(int *)ptr);
			break;
		case CHAR:
			fprintf(fp, " ");
			for (k = 0; k < tdef->attr[j].len; k++)
				fprintf(fp, "%c", *(ptr+k));
			fprintf(fp, " ");
			break;
		default:
			break;
		}
		fprintf(fp, "|");
	}
	fprintf(fp, "\n");
	ptr = hdrln;
	*ptr++ = '+';
	for (i = 0; i < tdef->rel.attrcnt; i++) {
		switch (tdef->attr[i].type) {
			case INT:
				for (j = 0; j < 10; j++)
					*ptr++ = '-';
				break;
			case CHAR:
				for (j = 0; j < tdef->attr[i].len + 2; j++)
					*ptr++ = '-';
				break;
			default:
				break;
		}
		*ptr++ = '+';
	}
	*ptr++ = '\0';
	fprintf(fp, "%s\n", hdrln);
	return 0;
}

int print_tbhdr(struct table_def *tdef)
{
	FILE *fp = stdout;
	int i, j;
	char hdrln[400];
	char *ptr = hdrln;
	*ptr++ = '+';
	for (i = 0; i < tdef->rel.attrcnt; i++) {
		switch (tdef->attr[i].type) {
			case INT:
				for (j = 0; j < 10; j++)
					*ptr++ = '-';
				break;
			case CHAR:
				for (j = 0; j < tdef->attr[i].len + 2; j++)
					*ptr++ = '-';
				break;
			default:
				break;
		}
		*ptr++ = '+';
	}
	*ptr++ = '\0';
	fprintf(fp, "%s\n", hdrln);
	fprintf(fp, "|");
	for (i = 0; i < tdef->rel.attrcnt; i++) {
		switch (tdef->attr[i].type) {
			case INT:
				fprintf(fp, "%9s |", tdef->attr[i].name);
				break;
			case CHAR:
				fprintf(fp, "%*s  |", tdef->attr[i].len, tdef->attr[i].name);
				break;
			default:
				break;
		}
	}
	fprintf(fp, "\n");
	fprintf(fp, "%s\n", hdrln);
	return 0;
}
#endif
