#include <string.h>
#include <stdio.h>
#include "scantable.h"
#include "dict.h"
#include "join.h"
#include "buffer.h"

/*
 * Table ltb1 and ltb2 to product
 * the result schema store in ltb
 * return 0 if no error occur
 */
int cross(struct logic_table *ltb, struct logic_table *ltb1, struct logic_table *ltb2)
{
	FILE *res;
	char rec[1000], rec1[500], rec2[500];
	struct iterator itertb1, itertb2;
	int len1 = ltb1->len;
	int len2 = ltb2->len;
	res = fopen(ltb->tbname, "w+b");
	itertb1 = open_iter(ltb1);
	while (getnext_iter(&itertb1, rec1) == 0) {
		memcpy(rec, rec1, len1);
		itertb2 = open_iter(ltb2);
		while (getnext_iter(&itertb2, rec2) == 0) {
			memcpy(rec + len1, rec2, len2);
			fseek(res, ltb->cnt * ltb->len, SEEK_SET);
			fwrite(rec, ltb->len, 1, res);
			ltb->cnt++;
		}
		close_iter(&itertb2);
	}
	close_iter(&itertb1);
	fclose(res);
	return 0;
}

#if 0
int product(const char *tb1, const char *tb2)
{
	int num = 0;
	int table, buffid;
	char rec[400], rec1[200], rec2[200];
	struct iterator itertb1, itertb2;
	struct table_def *td;
	table = get_tbdef("product");
	buffid = getpg_LRU(table);
	td = (struct table_def *)buff[buffid].page;
	print_tbhdr(td);
	itertb1 = open_iter(tb1);
	while (getnext_iter(&itertb1, rec1) == 0) {
		itertb2 = open_iter(tb2);
		while (getnext_iter(&itertb2, rec2) == 0) {
			/* output rec1 x rec2 */
			memcpy(rec, rec1, itertb1.len);
			memcpy(rec + itertb1.len, rec2 + TUPHDR_SIZE, itertb2.len - TUPHDR_SIZE);
			buffid = getpg_LRU(table);
			td = (struct table_def *)buff[buffid].page;
			print_tuple(rec, td);
			num++;
		}
		close_iter(&itertb2);
	}
	close_iter(&itertb1);
	printf("Total %d tuples...\n", num);
	return 0;
}

int nest_tuple_join(const char *tb1, const char *tb2)
{
	int num = 0;
	int table, buffid;
	char rec[400], rec1[200], rec2[200];
	struct iterator itertb1, itertb2;
	struct table_def *td;
	table = get_tbdef("join");
	buffid = getpg_LRU(table);
	td = (struct table_def *)buff[buffid].page;
	print_tbhdr(td);
	itertb1 = open_iter(tb1);
	while (getnext_iter(&itertb1, rec1) == 0) {
		itertb2 = open_iter(tb2);
		while (getnext_iter(&itertb2, rec2) == 0) {
			/* output rec1 |x| rec2 */
			int key1 = *(int *)(rec1 + TUPHDR_SIZE);
			int key2 = *(int *)(rec2 + TUPHDR_SIZE);
			if (key1 == key2) { /* can join */
				memcpy(rec, rec1, itertb1.len);
				memcpy(rec + itertb1.len, rec2 + TUPHDR_SIZE + 4, itertb2.len - TUPHDR_SIZE - 4);
				buffid = getpg_LRU(table);
				td = (struct table_def *)buff[buffid].page;
				print_tuple(rec, td);
				num++;
			}
		}
		close_iter(&itertb2);
	}
	close_iter(&itertb1);
	printf("Total %d tuples...\n", num);
	return 0;
}

int nest_blk_join(const char *tb1, const char *tb2)
{
	int num = 0;
	int key;
	int taller;
	struct BNode *avl = NULL;
	struct Record e;
	int table, buffid;
	char rec[400], rec1[200], rec2[200];
	struct iterator itertb1, itertb2;
	struct table_def *td;
	table = get_tbdef("join");
	buffid = getpg_LRU(table);
	td = (struct table_def *)buff[buffid].page;
	print_tbhdr(td);

	itertb1 = open_iter(tb1);
	while (getnext_iter(&itertb1, rec1) == 0) {
		key = *(int *)(rec1 + TUPHDR_SIZE);
		e.key = key;
		memcpy(e.data, rec1, itertb1.len);
		avl = InsertBT(avl, e, &taller);
	}

	itertb2 = open_iter(tb2);
	while (getnext_iter(&itertb2, rec2) == 0) {
		struct Result *res, *tmp;
		key = *(int *)(rec2 + TUPHDR_SIZE);
		res = SearchBT(avl, key);
		tmp =res;
		while (res != NULL) {
			memcpy(rec, res->rec.data, itertb1.len);
			memcpy(rec + itertb1.len, rec2 + TUPHDR_SIZE + 4, itertb2.len - TUPHDR_SIZE - 4);
			buffid = getpg_LRU(table);
			td = (struct table_def *)buff[buffid].page;
			print_tuple(rec, td);
			num++;
			res = res->next;
		}
		ClearResult(tmp);
	}
	ClearBT(avl);
	close_iter(&itertb1);
	close_iter(&itertb2);
	printf("Total %d tuples...\n", num);
	return 0;
}

int index_join(const char *tb1, const char *tb2)
{
	int num = 0;
	int table, buffid;
	char rec[400], rec1[200], rec2[200];
	struct iterator itertb1;
	struct table_def *td;
	table = get_tbdef("join");
	buffid = getpg_LRU(table);
	td = (struct table_def *)buff[buffid].page;
	print_tbhdr(td);
	itertb1 = open_iter(tb1);
	while (getnext_iter(&itertb1, rec1) == 0) {
		int addr, offset;
		int recid, len, tdefid, res;
		char *ptr;
		int nid, idx;
		int key = *(int *)(rec1 + TUPHDR_SIZE);
		int table2 = get_tbdef(tb2);
		struct table_def *tdef;
		struct btree_node *bnode;
		tdefid = getpg_LRU(table2);
		tdef = (struct table_def *)buff[tdefid].page;
		if ((res = btree_search(key, table2, &nid, &idx)) != -1) {
			while (nid != -1) {
				buffid = getpg_LRU(nid);
				bnode = (struct btree_node *)buff[buffid].page;
				for ( ; idx < bnode->d; idx++) {
					if (bnode->key[idx] != key)
						break;
					addr = bnode->ptr[idx] / ADDRBASE;
					offset = bnode->ptr[idx] % ADDRBASE;
					recid = getpg_LRU(addr);
					ptr = buff[recid].page;
					ptr += PGHDR_SIZE;
					len = tdef->rel.reclen;
					memcpy(rec2, ptr + offset * len, len);
					memcpy(rec, rec1, itertb1.len);
					memcpy(rec + itertb1.len, rec2 + TUPHDR_SIZE + 4, len - TUPHDR_SIZE - 4);
					buffid = getpg_LRU(table);
					td = (struct table_def *)buff[buffid].page;
					print_tuple(rec, td);
					num++;
				}
				if (idx == bnode->d) {
					nid = bnode->ptr[KEY_NUM];
					idx = 0;
				} else {
					break;
				}
			}
		} else { // nothing to do
		}
	}
	close_iter(&itertb1);
	printf("Total %d tuples...\n", num);
	return 0;
}
#endif
