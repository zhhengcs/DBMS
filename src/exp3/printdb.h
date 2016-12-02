/*
 * print db
 */
#ifndef PRINTDB_H
#define PRINTDB_H

#include "dict.h"

int print_header(int nattr, struct attrdef attr[]);
int printf_rec(char *rec, int nattr, struct attrdef attr[]);

#endif

