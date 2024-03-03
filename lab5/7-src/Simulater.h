#ifndef _SIMULATER_H_
#define _SIMULATER_H_

#include "tree.h"
#include "operand.h"
#include "SymbolTable.h"

struct Register
{
    int state;  // 0 is empty, 1 is used
    char *name; // 寄存器的名称
};

struct StackNode
{               // 栈指针
    int offset; // 偏移量
    int kind;   // 类型
    int varno;  // 编号
    struct StackNode *next;
};


void init_reg();
void init_data(FILE *fp);
int checkvar(Operand op);
int findoperand(Operand cur);
void push_operand(Operand op, int offset);
void pop_operand();
void init_code(FILE *fp);
int findoperand_offset(Operand cur);
void Load_Reg(Operand op, int reg_, FILE *fp);
void Save_Reg(Operand op, int reg_, FILE *fp);
void PrintRelop(char *relop, FILE *fp);
void TransLateCode(FILE *fp, struct InterCodes *cur);
void TransLateAllCode(FILE *fp);
void CodeGenerate(FILE *fp);

#endif