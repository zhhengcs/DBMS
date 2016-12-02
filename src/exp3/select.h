/*
 * used for the various select operation
 */

#ifndef _SELECT_H
#define _SELECT_H

#include "lqp.h"
#include "pqp.h"

int sel_av(struct logic_table *ltb, struct attrdef *lhs, struct value *val, enum op op);
int sel_aa(struct logic_table *ltb, struct attrdef *lhs, struct attrdef *rhs, enum op op);
int sel(struct logic_table *ltb, struct condition *cond);

#endif
