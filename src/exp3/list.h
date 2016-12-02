/**/
#ifndef LIST_H
#define LIST_H

#include "lqp.h"
#include "pqp.h"

/* where condition list node */
struct iNode{
    struct condition cond;
    struct iNode *next;
};

void head_insert(struct iNode **head, struct condition c);
void tail_insert(struct iNode **head, struct condition c);
struct condition get(struct iNode **head);
void printl(struct iNode **head);

/* table name list node */
struct node {
	struct logic_table ltb;
	char *tabname;
	struct node *next;
};

void insert_tablst(struct node **head, char *d, struct logic_table ltb);
void del_tablst(struct node **head, char *d);
void get_tablst(struct node **head, char *fn, struct logic_table *ltb);
struct node *search_tablst(struct node **head, char *tn);
void update_dul_list(char * tab_name, char * tab1, char * tab2);

#endif

