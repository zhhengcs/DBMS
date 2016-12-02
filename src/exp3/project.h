/*
 * used for project
 */
#ifndef PROJECT_H
#define PROJECT_H

#include "pqp.h"
#include "lqp.h"

struct logic_table project(struct logic_table *ltb, int nsel, struct relattr selattr[]);

#endif

