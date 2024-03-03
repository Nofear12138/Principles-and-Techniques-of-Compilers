#include <malloc.h>
#include "tree.h"

Node *CreateNode(char *name,int linenum,NODE_TYPE type){
    Node *node = (Node *)malloc(sizeof(Node));
    node->child = NULL;
    node->sibling=NULL;
    node->name = strdup(name);
    node->linenum = linenum;
    node->type = type;
    node->intvalue = 1;
    return node;
}
Node *InsertNode(Node *child,char *name,int linenum,NODE_TYPE type){
    Node *father = (Node *)malloc(sizeof(Node));
    father->child = child;
    father->sibling = NULL;
    father->name = strdup(name);
    father->linenum = linenum;
    father->type = type;
    father->intvalue = 1;
    root = father;
    return father;
}
void AddSibling(Node *first,int num,...){
    va_list valist;
    va_start(valist,num);
    int i;
    Node *cur = first;
    for (i=0;i<num;i++){
        Node *curNode = va_arg(valist,Node *);
        cur->sibling = curNode;
        cur = cur->sibling;
    }
}
void PrintNode(Node* node,FILE *f){
    if (node->type == STRING_TYPE){
        fprintf(f,"%s : %s\n",node->name,node->id);
    }
    else if (node->type == FLOAT_TYPE){
        fprintf(f,"FLOAT: %f\n",node->floatvalue);
    }
    else if (node->type == INT_TYPE){
        fprintf(f,"INT: %d\n",node->intvalue);
    }
    else{
        fprintf(f,"%s (%d)\n",node->name,node->linenum);
    }
}
void TravelTree(Node *root,int dep,FILE *f){
    if (root == NULL) return;
    int i;
    for (i=0;i<dep;i++){
        fprintf(f,"\t");
    }
    PrintNode(root,f);
    TravelTree(root->child,dep+1,f);
    TravelTree(root->sibling,dep,f);
}
