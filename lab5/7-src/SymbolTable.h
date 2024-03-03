#ifndef _SYMBOLTABLE_H
#define _SYMBOLTABLE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "tree.h"

typedef struct Type_ *Type;
typedef struct FieldList_ *FieldList;

struct Type_
{
    enum
    {
        BASIC,     //基本类型
        ARRAY,     //数组类型
        STRUCTURE, //结构体
        FUNCTION   //函数
    } kind;
    union
    {
        int basic; // 0->int , 1->float
        struct
        {
            Type elem;
            int size;
        } array_; //数组:包括元素的类型和数组的大小；

        struct
        {
            FieldList structure;
            char *name;
        } structure_; //结构体:包括结构体名和结构体的域

        struct
        {
            Type ReturnParameter;
            int num;
            FieldList parameters;
        } fuction; //返回参数的类型、参数个数、参数列表
    } u;
};

struct FieldList_
{
    char name[32];
    Type type;
    FieldList tail; //域包括域名，域类型，整体是作为一个链表；
};

struct error9
{
    char *name;
    char *nameandtype;
    struct error9 *next; //对错误9的数据结构，包括函数名称，函数和参数类型构成的字符串，整体作为一个链表
};

struct SymbolNode
{
    enum
    {
        VARIABLE_NAME, //变量
        STRUCT_NAME,   //结构体
        FUNCTION_NAME  //函数
    } kind;
    struct SymbolNode *next;     //同一hash值的下一个结点
    struct SymbolNode *areanext; //
    struct FieldList_ field;     //存符号结点的类型和名称
    char *structsymbolname;      //结构体的名称
    int isdef;                   // 0->声明；1->定义
    int depth;                   //所在的深度
    //lab4
    int var_no;
    int ifaddress;
    int offset;
    char *belongtostructname;
};

struct SymbolBucket
{
    struct SymbolNode *head;
    struct SymbolBucket *next; //符号表，整体是一个链表
};

struct Dec_Fuc //存函数的名称，用来检验是否被定义
{
    char name[32];        //函数名称
    int linenum;          //行号
    struct Dec_Fuc *next; //链表
};

unsigned hashcode(char *name);                                               //求hash
void Insert(Type type, char *name, int isdef, int depth);                    //给定信息，创建符号结点插入符号表
void InsertSymbolNode(struct SymbolNode *Node, struct SymbolBucket *bucket); //给定符号结点和作用域插入结点
int InsertStruct(Type type, char *name,int offset,char* belongtostructname);                                     //插入struct的类型和名称
int querySymbol(Type *type, char *name, int *isdef, int depth, int area);    // 给定名字，查询type,isdef,不存在返回-1，存在返回0；area是0说明找当前层,1是找所有层
int querySymbol2(Type *type, char *name, int *isdef, int depth, int *kind);  //给定名字，查询type,isdef,kind,不存在返回-1，存在返回0,查所有层；
int queryStruct(Type *type, char *name);                                     //
int querySymbolname(char *name, int depth);                                  //查当前层是否存在这个名字
int queryStructname(char *name);                                            //查询是否存在某结构体名
struct SymbolBucket *initSymbolBucket();                                     //符号表初始化
struct SymbolBucket *enterscope();                                           //进入作用域
struct SymbolBucket *exitscope();                                            //出作用域

void ShowGlobalTable();
void ShowStructTable();
void ShowScopeTable();

int CheckType(Type A, Type B);      //查A和B类型是否一致
int CheckArrayType(Type A, Type B); //进一步查A数组和B数组类型是否一致
void CheckFucDef();                 //检查函数是否被定义

void PushDecFuc(char *name, int column); //把未定义的函数放到里面

struct SymbolNode *CreateSymbolNodeZJDM(int kind,Type type,char *name, int isdef, int depth);
struct SymbolNode *querySymbolNodeZJDM(char *name,int *success);

#endif