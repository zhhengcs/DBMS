/**/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "list.h"

void head_insert(struct iNode **head, struct condition c)
{
    struct iNode *node = (struct iNode *)malloc(sizeof(struct iNode));
    node->cond = c;
    node->next = *head;
    *head = node;
}

void tail_insert(struct iNode **head, struct condition c)
{
    struct iNode *p = *head;
    struct iNode *node = (struct iNode *)malloc(sizeof(struct iNode));

	if(!p) {//list ???????
		node->cond = c;
		node->next = NULL;
		*head = node;
		return;
	}

    while(p) {
        if(p->next ==NULL) break;
		p = p->next;
    }
    
    node->cond = c;
    node->next = p->next;
    p->next = node;
}

struct condition get(struct iNode **head)
{
    struct iNode *p = *head;
    struct condition d;
	if(!*head) {
		printf("list null error");
		return d;
	}
	d = (*head)->cond;

	if((*head)->next == NULL) 
		*head = NULL;
	else
		(*head) = (*head)->next;
    free(p);
	return d;
}

void printl(struct iNode **head)
{
    struct iNode *p = *head;
    if(p !=NULL)
    {
        do{
            printf("%s ",p->cond.lhs_attr.tbname);
            p = p->next;
        }while(p !=NULL);
    }else{
        printf("List ERROR\n");
    }
    printf("\n");
}

void insert_tablst(struct node **head, char *d, struct logic_table lt)
{
	struct node *p = *head;
    struct node *n = (struct node *)malloc(sizeof(struct node));

	if(!p) {
		n->tabname = (char*) malloc(50);
		strcpy(n->tabname, d);
		n->ltb = lt;
		n->next = NULL;
		*head = n;
		return;
	}

    while(p) {
        if(p->next == NULL) break;
		p = p->next;
    }
    
	n->tabname = (char*) malloc(50);
	strcpy(n->tabname, d);
	n->ltb = lt;
    n->next = NULL;
    p->next = n;
}

void del_tablst(struct node **head, char *d)
{
	struct node *p = *head, *temp;
	if(!p) {
		return;
	}

	while(p) {
		if(strcmp(p->tabname, d) == 0) {
			*head = p->next;
			free(p);
			p = *head;
		} else {
			break;
		}
	}
	
	while(p) {
		temp = p->next;
		if(temp == NULL) return;
		if(strcmp(temp->tabname, d) == 0) {
			p->next = temp->next;
			free(temp);
			return;
		}
		p = p->next;
	}
}

void get_tablst(struct node **head, char *tn, struct logic_table *ltb)
{
    struct node *p = *head;
	if(!*head) {
		printf("list null error");
		return;
	}
	strcpy(tn, (*head)->tabname);
	*ltb = (*head)->ltb;
	//我加入的打印内容 
	//printf("%s",tn); 

	if((*head)->next == NULL) 
		*head = NULL;
	else
		(*head) = (*head)->next;
    free(p);   //释放掉这个内容的目的是什么？ 
}

struct node *search_tablst(struct node **head, char *tn)
{
	struct node *p = *head;
	if(!p) {
		return NULL;
	}

	while(p) {
		if(strcmp(p->tabname, tn) == 0) {
			return p;
		}
		p = p->next;
	}
	
	return NULL;
}

