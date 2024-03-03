#ifndef _OPERAND_H_
#define _OPERAND_H_

#include <stdio.h>
#include <stdarg.h>
#include "tree.h"
#include "SymbolTable.h"

typedef struct Operand_ *Operand;

struct Operand_ //运算对象
{
    enum
    {
        OP_VARIABLE, //变量 v_i
        OP_CONSTANT, //常量 # int
        OP_FUNCTION, //函数
        OP_TEMPVAR,  //临时变量 t_i
        OP_LABEL     //标签 label_i
    } kind;

    enum
    {
        OP_ADDRESS,
        OP_VAR
    } ifaddress; //地址类型还是变量类型

    char *varname;  //变量名
    int varno;      //变量编号
    int value;      // int值
    char *funcname; //函数名称
    int depth;      
};

struct InterCode
{
    enum
    {
        // one
        IN_FUNCTION,
        IN_PARAM,
        IN_RETURN,
        IN_LABEL,
        IN_GOTO,
        IN_WRITE,
        IN_READ,
        IN_ARG,
        // two
        IN_ASSIGN,
        IN_DEC,
        IN_CALL,
        // three
        IN_ADD,
        IN_SUB,
        IN_MUL,
        IN_DIV,
        // four
        IN_IFGOTO
    } kind;

    union
    {
        struct
        {
            Operand op0;
        } one;

        struct
        {
            Operand left;
            Operand right;
        } two;

        struct
        {
            Operand result;
            Operand op1;
            Operand op2;
        } three;

        struct
        { //实际上是op1,relop1,op2,op3
            Operand op1;
            Operand op2;
            Operand op3;
            char *relop;
        } four;

    } u;
};

//双向链表
struct InterCodes
{
    struct InterCode code;
    struct InterCodes *prev;
    struct InterCodes *next;
};

void Generate(Node *cur, FILE *f);
void add_list(struct InterCodes *node);
Operand new_op(int kind, int ifaddress, ...);
char *Reverse_relop(char *relop);
Operand copyop(Operand op);
int transformtokind(char *cur);
void printoperand(Operand op, FILE *f);
void printintercode(FILE *f);
void new_intercode(int kind, ...);
int GetSize(Type cur);
int GetArrayDepth(struct SymbolNode *ArrayNode);
int ProgramZJDM(Node *cur);
int ExtDefListZJDM(Node *cur);
int ExtDefZJDM(Node *cur);
int FunDecZJDM(Node *cur);
int CompStZJDM(Node *cur);
int DefListZJDM(Node *cur);
int DefZJDM(Node *cur);
int StmtListZJDM(Node *cur);
int StmtZJDM(Node *cur);
int DecListZJDM(Node *cur);
int DecZJDM(Node *cur);
void ArgZJDM(Node *cur, FieldList param);
Operand VarDecZJDM(Node *cur);
Operand ExpZJDM(Node *cur);

void printop(Operand op);

int ConditionZJDM(Node *cur,Operand label_True,Operand label_False);
#endif