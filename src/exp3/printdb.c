/* print db */
#include <stdio.h>
#include <string.h>
#include "printdb.h"

/*
 * print table header
 * attr[]: table's attr list
 */
 //打印表格的各个属性列的表头 
int print_header(int nattr, struct attrdef attr[])
{
	int i, j;
	int hsp[400];
	char ttcn[200];
	for (i = 0; i < nattr; i++) {
		hsp[i] = strlen(attr[i].relname) + strlen(attr[i].name) + 1;
		switch (attr[i].type) {
		case INT:
			if (hsp[i] < 11)
				hsp[i] = 11;
			break;
		case CHAR:
			if (hsp[i] < attr[i].len)
				hsp[i] = attr[i].len;
			break;
		default:
			break;
		}
	}
	printf("+");
	for (i = 0; i < nattr; i++) {
		for (j = 0; j < hsp[i] + 2; j++)
			printf("-");
		printf("+");
	}
	printf("\n");
	printf("|");
	for (i = 0; i < nattr; i++) {
		strcpy(ttcn, attr[i].relname);
		strcat(ttcn, ".");
		strcat(ttcn, attr[i].name);
		printf(" %*s |", hsp[i], ttcn);
	}
	printf("\n");
	printf("+");
	for (i = 0; i < nattr; i++) {
		for (j = 0; j < hsp[i] + 2; j++)
			printf("-");
		printf("+");
	}
	printf("\n");
	return 0;
}
/*
 * print a record in the result table
 * nattr: attr count
 * attr[]: table's attr list
 * rec: the record to print
 */
 //打印出选择表格 中的各个属性列中的值 
int printf_rec(char *rec, int nattr, struct attrdef attr[])
{
	int i, j;
	int hsp[400];
	for (i = 0; i < nattr; i++) {
		hsp[i] = strlen(attr[i].relname) + strlen(attr[i].name) + 1;
		switch (attr[i].type) {
		case INT:
			if (hsp[i] < 11)
				hsp[i] = 11;
			break;
		case CHAR:
			if (hsp[i] < attr[i].len)
				hsp[i] = attr[i].len;
			break;
		default:
			break;
		}
	}
	printf("|");
	for (i = 0; i < nattr; i++) {
		switch (attr[i].type) {
		case INT:
			printf(" %*d |", hsp[i], *(int *) (rec + attr[i].offset));
			break;
		case CHAR:
			printf(" ");
			for (j = attr[i].len; j < hsp[i]; j++)
				printf(" ");
			for (j = 0; j < attr[i].len; j++)
				printf("%c", *(rec + attr[i].offset + j));
			printf(" |");
			break;
		}
	}
	printf("\n");
	printf("+");
	for (i = 0; i < nattr; i++) {
		for (j = 0; j < hsp[i] + 2; j++)
			printf("-");
		printf("+");
	}
	printf("\n");
	return 0;
}

