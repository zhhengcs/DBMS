/*
 * Dict
 */
#include <stdio.h>
#include <string.h>
#include "tablespace.h"
#include "dict.h"
#include "printdb.h"

/* load dict into memery */
int load_dict()
{
	dictfd = fopen("dd", "r+b");
	if (dictfd == NULL) {
		/* create a dict file */
		printf("do not hava a dd, create a dd file..\n");
		dictfd = fopen("dd", "w+b");
	}
	fseek(dictfd, 0, SEEK_SET);
	fread(&ntab, sizeof(int), 1, dictfd);
	/* read ntab table define */
	fseek(dictfd, sizeof(int), SEEK_SET);
	fread(sys_tables, sizeof(struct table_def), ntab, dictfd);
	fseek(dictfd, sizeof(int) + ntab * sizeof(struct table_def), SEEK_SET);
	fread(&nindex, sizeof(int), 1, dictfd);
	/* read nindex index info */
	fseek(dictfd, 2 * sizeof(int) + ntab * sizeof(struct table_def), SEEK_SET);
	fread(sys_indexes, sizeof(struct idx_def), nindex, dictfd);
	return 0;
}

/* save dict data to dd file */
int sav_dict()
{
	fseek(dictfd, 0, SEEK_SET);
	fwrite(&ntab, sizeof(int), 1, dictfd);
	/* read ntab table define */
	fseek(dictfd, sizeof(int), SEEK_SET);
	fwrite(sys_tables, sizeof(struct table_def), ntab, dictfd);
	fseek(dictfd, sizeof(int) + ntab * sizeof(struct table_def), SEEK_SET);
	fwrite(&nindex, sizeof(int), 1, dictfd);
	/* read nindex index info */
	fseek(dictfd, 2 * sizeof(int) + ntab * sizeof(struct table_def), SEEK_SET);
	fwrite(sys_indexes, sizeof(struct idx_def), nindex, dictfd);
	/* printf("ntab == %d\tnindexes == %d\n", ntab, nindex); */
	return 0;
}

/* clode dd file, write all change to file */
int close_dict()
{
	sav_dict();
	fclose(dictfd);
	return 0;
}

/*
 * insert meta data into sys_tables
 * if ok return 0
 * if fail return -1;
 */
int create_table(char *tbname, int attrcnt, struct table_attr attr[])
{
	int j;
	int oflen;
	struct attrdef tattr;
	/* first examine is exists? */
	if (get_tbdef(tbname) != -1) {
		printf("SORRY..This table already have.\n");
		printf("Please change to another table name..\n");
		return -1;
	}
	/* ok insert it */
	strcpy(sys_tables[ntab].relname, tbname);
	sys_tables[ntab].attrcnt = attrcnt;
	oflen = 0;
	for (j = 0; j < attrcnt; j++) {
		strcpy(tattr.relname, tbname);
		strcpy(tattr.name, attr[j].name);
		tattr.type = attr[j].type;
		tattr.len = attr[j].len;
		tattr.has_index = 0;
		tattr.offset = oflen;
		oflen += tattr.len;
		oflen = (oflen + 3) / 4 * 4;
		sys_tables[ntab].attr[j] = tattr;
	}
	sys_tables[ntab].reclen = oflen;
	sys_tables[ntab].first_ext = next_data_extent(oflen);
	/* sys_tables[ntab].first_ext = -1; */
	ntab++;
	sav_dict();
	printf("TABLE %s created.\n", tbname);
	return 0;
}

/*
 * drop table tbname
 * return 0 if succeed
 */
int drop_table(char *tbname)
{
	int table;
	/* first examine is exists? */
	if ((table = get_tbdef(tbname)) == -1) {
		printf("SORRY..Cannot find that table...\n");
		return -1;
	}
	/* delete all tuple */
	del_tb_ext(sys_tables[table].first_ext);

	sys_tables[table] = sys_tables[ntab];//??????????????λ??
	ntab--;
	printf("Table %s dropped.\n", tbname);
	return 0;
}

/*
 * search if tbname is exist
 * if ok, return idx
 * if not return -1
 */
 //这里判定表是否存在 
int get_tbdef(const char *tbname)
{
	int i;
	for (i = 0; i < ntab; i++)
		if (strcmp(tbname, sys_tables[i].relname) == 0)
			return i;
	return -1;
}

/* show all tables cur in database */
int table_show()
{
	int i, j;
	int max;
	int cnt;
	char split[2000];
	max = 0;
	for (i = 0; i < ntab; i++)
		if (sys_tables[i].attrcnt > max)
			max = sys_tables[i].attrcnt;
	cnt = 0;
	split[cnt++] = '+';
	for (i = 0; i < max + 1; i++) {
		for (j = 0; j < 12; j++)
			split[cnt++] = '-';
		split[cnt++] = '+';
	}
	split[cnt] = '\0';
	printf("%s\n", split);
	printf("| %10s |", "table name");
	for (i = 0; i < max; i++)
		printf(" %9s%d |", "col name ", i);
	printf("\n");
	printf("%s\n", split);
	for (j = 0; j < ntab; j++) {
		printf("| %10s |", sys_tables[j].relname);
		for (i = 0; i < max; i++)
			printf(" %10s |", sys_tables[j].attr[i].name);
		printf("\n");
		printf("%s\n", split);
	}
	return 0;
}

