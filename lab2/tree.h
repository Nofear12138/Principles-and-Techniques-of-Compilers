#ifndef TREE_H
#define TREE_H

#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

typedef int NODE_TYPE;
#define NON_TERMINATOR 0
#define INT_TYPE 1
#define FLOAT_TYPE 2
#define STRING_TYPE 3
extern char *yytext;
extern int yylineno;

typedef struct node{
    struct node *child;
    struct node *sibling;
    int linenum;
    char *name;
    NODE_TYPE type;
    union{
        char *id;
        int intvalue;
        float floatvalue;
    };
}Node;

Node *root;
Node *CreateNode(char *name,int linenum,NODE_TYPE type);
Node *InsertNode(Node *child,char *name,int linenum,NODE_TYPE type);
void AddSibling(Node *first,int num,...);
void PrintNode(Node* node,FILE *f);
void TravelTree(Node *root,int dep,FILE *f);

#endif
