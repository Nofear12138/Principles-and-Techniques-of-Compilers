#include "Simulater.h"

extern struct InterCodes *inter_head; // 双向链表

int fuctionstate = 0;       // 区分main函数和其他函数
char *function_name = NULL; // 当前函数名称
struct StackNode *stack_sp; // 栈指针
struct StackNode *stack_fp; // 帧指针
struct StackNode *stack_head;
struct Register reg[32];   // 寄存器数组，保存名称、状态

/*
    初始化寄存器；
    使用朴素寄存器方法，主要使用t0,t1,t2；
*/
void init_reg()
{
    for (int i = 0; i < 32; i++)
    {
        reg[i].state = 0;
    }
    reg[0].name = "$zero";
    reg[1].name = "$at";
    reg[2].name = "$v0";
    reg[3].name = "$v1";
    reg[4].name = "$a0";
    reg[5].name = "$a1";
    reg[6].name = "$a2";
    reg[7].name = "$a3";
    reg[8].name = "$t0";
    reg[9].name = "$t1";
    reg[10].name = "$t2";
    reg[11].name = "$t3";
    reg[12].name = "$t4";
    reg[13].name = "$t5";
    reg[14].name = "$t6";
    reg[15].name = "$t7";
    reg[16].name = "$s0";
    reg[17].name = "$s1";
    reg[18].name = "$s2";
    reg[19].name = "$s3";
    reg[20].name = "$s4";
    reg[21].name = "$s5";
    reg[22].name = "$s6";
    reg[23].name = "$s7";
    reg[24].name = "$t8";
    reg[25].name = "$t9";
    reg[26].name = "$k0";
    reg[27].name = "$k1";
    reg[28].name = "$gp";
    reg[29].name = "$sp";
    reg[30].name = "$fp";
    reg[31].name = "$ra";
}

/*
    常规输出
*/
void init_data(FILE *fp)
{
    fprintf(fp, ".data\n");
    fprintf(fp, "_prompt: .asciiz \"Enter an integer:\"\n_ret: .asciiz \"\\n\"\n.globl main\n");
}

/*
    查看运算对象是否为变量类型
*/
int checkvar(Operand op)
{
    return (op->kind == OP_VARIABLE || op->kind == OP_TEMPVAR);
}

/*
    对于变量类型，在fp中查找是否存在当前的运算对象
*/
int findoperand(Operand cur)
{
    struct StackNode *tmp = stack_fp;
    int kind = cur->kind;
    int varno = cur->varno;
    int success = 0;

    while (tmp != NULL) // 遍历帧
    {
        if (tmp->varno == varno && tmp->kind == kind)
        {
            success = 1;
            break;
        }
        tmp = tmp->next;
    }
    return success;
}

/*
    将运算对象放入栈指针中
*/
void push_operand(Operand op, int offset)
{
    struct StackNode *tmp = (struct StackNode *)malloc(sizeof(struct StackNode));
    int kind = op->kind;
    int varno = op->varno;
    tmp->kind = kind;
    tmp->varno = varno;
    tmp->offset = offset;
    tmp->next = NULL;
    stack_sp->next = tmp;
    stack_sp = tmp;
}

void pop_operand()
{
    struct StackNode *tmp = stack_head;
    int success = 0;
    while (1)
    {
        if (tmp == NULL)
        {
            break;
        }
        if (tmp == stack_fp)
        {
            tmp->next = NULL;
            success = 1;
            break;
        }
        if (tmp->next == stack_fp)
        {
            tmp->next = NULL;
            success = 1;
            break;
        }
        tmp = tmp->next;
    }
    stack_fp = stack_head;
    stack_sp = tmp;
}

/*
    输出read和write的代码 + 数据结构的初始化
*/
void init_code(FILE *fp)
{
    // read
    fprintf(fp, ".text\n");
    fprintf(fp, "read:\n");
    fprintf(fp, "  li $v0, 4\n");
    fprintf(fp, "  la $a0, _prompt\n");
    fprintf(fp, "  syscall\n");
    fprintf(fp, "  li $v0, 5\n");
    fprintf(fp, "  syscall\n");
    fprintf(fp, "  jr $ra\n\n");
    // write
    fprintf(fp, "write:\n");
    fprintf(fp, "  li $v0, 1\n");
    fprintf(fp, "  syscall\n");
    fprintf(fp, "  li $v0, 4\n");
    fprintf(fp, "  la $a0, _ret\n");
    fprintf(fp, "  syscall\n");
    fprintf(fp, "  move $v0, $0\n");
    fprintf(fp, "  jr $ra\n");

    // 初始化
    stack_head = (struct StackNode *)malloc(sizeof(struct StackNode));
    stack_head->next = NULL;
    stack_fp = stack_head;
    stack_sp = stack_head;
    stack_head->varno = -1;
}

/*
    计算运算对象的偏移量
*/
int findoperand_offset(Operand cur)
{
    struct StackNode *tmp = stack_fp;
    int kind = cur->kind;
    int varno = cur->varno;
    int offset = -1;
    while (tmp != NULL)
    {
        if (tmp->kind == kind && tmp->varno == varno)
        {
            offset = tmp->offset;
            break;
        }
        tmp = tmp->next;
    }
    return offset;
}

/*
    给定运算对象，根据类型和其计算得到的偏移量，将值加载到寄存器中
*/
void Load_Reg(Operand op, int reg_, FILE *fp)
{
    switch (op->kind)
    {
    case OP_VARIABLE: // 变量类型
    {
        int offset = findoperand_offset(op);
        if (op->ifaddress == OP_ADDRESS) // 将地址加载到目标寄存器
        {
            fprintf(fp, "  la %s, %d($fp)\n", reg[reg_].name, -offset);
        }
        else // 将值加载到目标寄存器
        {
            fprintf(fp, "  lw %s, %d($fp)\n", reg[reg_].name, -offset);
        }
        break;
    }
    case OP_CONSTANT: // 如果是常数，用li赋值给寄存器
    {
        fprintf(fp, "  li %s, %d\n", reg[reg_].name, op->value);
        break;
    }
    case OP_TEMPVAR: // 临时变量类型
    {
        int offset = findoperand_offset(op);
        if (op->ifaddress == OP_ADDRESS) // 如果是地址类型
        {
            // 赋给寄存器目标地址，然后再得到值
            fprintf(fp, "  lw %s, %d($fp)\n", reg[14].name, -offset);
            // t6
            fprintf(fp, "  lw %s, 0(%s)\n", reg[reg_].name, reg[14].name);
        }
        else // 将值加载到目标寄存器
        {
            fprintf(fp, "  lw %s, %d($fp)\n", reg[reg_].name, -offset);
        }
        break;
    }
    case OP_FUNCTION:
    {
        // 赋给寄存器函数的地址
        fprintf(fp, "  la %s, %s\n", reg[reg_].name, op->funcname);
        break;
    }
    case OP_LABEL:
    {
        fprintf(fp, "  la %s, label%d\n", reg[reg_].name, op->varno);
        break;
    }
    }
}

/*
    把寄存器的值写回
*/
void Save_Reg(Operand op, int reg_, FILE *fp)
{
    switch (op->kind)
    {
    case OP_VARIABLE:
    {
        int offset = findoperand_offset(op);
        fprintf(fp, "  sw %s, %d($fp)\n", reg[reg_].name, -offset);
        break;
    }
    case OP_TEMPVAR:
    {
        int offset = findoperand_offset(op);
        if (op->ifaddress == OP_ADDRESS)
        {
            // t6存取目标地址
            fprintf(fp, "  lw %s, %d($fp)\n", reg[14].name, -offset);
            // 存到目标地址中
            fprintf(fp, "  sw %s, 0(%s)\n", reg[reg_].name, reg[14].name);
        }
        else
        {
            fprintf(fp, "  sw %s, %d($fp)\n", reg[reg_].name, -offset);
        }
        break;
    }
    }
}

/*
    符号转换
*/
void PrintRelop(char *relop, FILE *fp)
{
    if (strcmp(relop, "==") == 0)
    {
        fprintf(fp, "beq ");
    }
    else if (strcmp(relop, "!=") == 0)
    {
        fprintf(fp, "bne ");
    }
    else if (strcmp(relop, ">") == 0)
    {
        fprintf(fp, "bgt ");
    }
    else if (strcmp(relop, "<") == 0)
    {
        fprintf(fp, "blt ");
    }
    else if (strcmp(relop, ">=") == 0)
    {
        fprintf(fp, "bge ");
    }
    else if (strcmp(relop, "<=") == 0)
    {
        fprintf(fp, "ble ");
    }
}

/*
    逐句翻译
*/
void TransLateCode(FILE *fp, struct InterCodes *cur)
{
    int kind = cur->code.kind;
    switch (kind)
    {
    case IN_FUNCTION: // 对于函数类型
    {
        pop_operand(); // 更新stack
        function_name = cur->code.u.one.op0->funcname;

        int offset = 0;
        if (strcmp(function_name, "main") == 0) // main函数
        {
            fprintf(fp, "\n%s:\n", function_name);
            fuctionstate = 0;
            function_name = NULL;
            struct InterCodes *tmp = cur;
            int cnt = 0;
            // 返回地址
            offset += 8;

            while (tmp != inter_head)
            {
                switch (tmp->code.kind)
                {
                case IN_DEC:
                {
                    Operand tmpop1 = tmp->code.u.two.left;
                    int res = findoperand(tmpop1); // 寻找是否已经存在
                    if (res == 0)                  // 不存在则开辟空间，并放入运算对象
                    {
                        offset += tmp->code.u.two.right->value;
                        push_operand(tmpop1, offset);
                    }
                    break;
                }
                case IN_ARG: // 参数类型
                {
                    Operand tmpop = tmp->code.u.one.op0;
                    if (checkvar(tmpop)) // 如果是变量
                    {
                        int res = findoperand(tmpop); // 看是否存在
                        if (res == 0)                 // 不存在的话
                        {
                            offset += 4;                 // 增加偏移量
                            push_operand(tmpop, offset); // 放入运算对象
                        }
                    }
                    break;
                }
                case IN_ADD:
                case IN_MUL:
                case IN_SUB:
                case IN_DIV:
                {
                    Operand tmpop1 = tmp->code.u.three.op1;
                    if (checkvar(tmpop1))
                    {
                        int res = findoperand(tmpop1);
                        if (res == 0)
                        {
                            offset += 4;
                            push_operand(tmpop1, offset);
                        }
                    }

                    Operand tmpop2 = tmp->code.u.three.op2;
                    if (checkvar(tmpop2))
                    {
                        int res = findoperand(tmpop2);
                        if (res == 0)
                        {
                            offset += 4;
                            push_operand(tmpop2, offset);
                        }
                    }

                    Operand tmpop3 = tmp->code.u.three.result;
                    int res = findoperand(tmpop3);
                    if (res == 0)
                    {
                        offset += 4;
                        push_operand(tmpop3, offset);
                    }
                    break;
                }
                case IN_READ:
                {
                    Operand tmpop = tmp->code.u.one.op0;
                    int res = findoperand(tmpop);
                    if (res == 0)
                    {
                        offset += 4;
                        push_operand(tmpop, offset);
                    }
                    break;
                }
                case IN_ASSIGN:
                {
                    Operand tmpop1 = tmp->code.u.two.left;
                    if (checkvar(tmpop1))
                    {
                        int res = findoperand(tmpop1);
                        if (res == 0)
                        {
                            offset += 4;
                            push_operand(tmpop1, offset);
                        }
                    }
                    Operand tmpop2 = tmp->code.u.two.right;
                    if (checkvar(tmpop2))
                    {
                        int res = findoperand(tmpop2);
                        if (res == 0)
                        {
                            offset += 4;
                            push_operand(tmpop2, offset);
                        }
                    }
                    break;
                }
                case IN_CALL:
                {
                    Operand tmpop = tmp->code.u.two.left;
                    int res = findoperand(tmpop);
                    if (res == 0)
                    {
                        offset += 4;
                        push_operand(tmpop, offset);
                    }
                    break;
                }
                }
                tmp = tmp->next;
            }
            // 开辟sp大小
            fprintf(fp, "  addi $sp, $sp, -%d\n", offset);
            // 更新地址位置:
            fprintf(fp, "  sw $ra, %d($sp)\n", offset - 4);
            fprintf(fp, "  sw $fp, %d($sp)\n", offset - 8);
            fprintf(fp, "  addi $fp, $sp, %d\n", offset);
        }
        else // 其他函数时
        {
            fprintf(fp, "\n%s:\n", function_name);
            fuctionstate = 1;

            // 处理参数
            int paramcnt = 0;
            struct InterCodes *tmp = cur->next;
            while (1)
            {
                if (tmp->code.kind == IN_PARAM) // 计算参数个数，用于分配空间
                {
                    paramcnt++;
                }
                else
                    break;
                tmp = tmp->next;
            }
            tmp = cur->next;
            offset = 4;
            while (1)
            {
                if (tmp->code.kind == IN_PARAM)
                {
                    offset -= 4;
                    Operand tmpop = tmp->code.u.one.op0;
                    push_operand(tmpop, offset);
                }
                else
                    break;
                tmp = tmp->next;
            }
            offset += (paramcnt * 4) - 4;

            // 和main类似
            offset += 8;
            while (1)
            {
                if (tmp == inter_head || tmp->code.kind == IN_FUNCTION)
                {
                    break;
                }
                switch (tmp->code.kind)
                {
                case IN_DEC:
                {
                    Operand tmpop = tmp->code.u.two.left;
                    int res = findoperand(tmpop);
                    if (res == 0)
                    {
                        offset += tmp->code.u.two.right->value;
                        push_operand(tmpop, offset);
                    }
                    break;
                }
                case IN_ARG:
                {
                    Operand tmpop = tmp->code.u.one.op0;
                    if (checkvar(tmpop))
                    {
                        int res = findoperand(tmpop);
                        if (res == 0)
                        {
                            offset += 4;
                            push_operand(tmpop, offset);
                        }
                    }
                    break;
                }
                case IN_ADD:
                case IN_SUB:
                case IN_MUL:
                case IN_DIV:
                {
                    Operand tmpop1 = tmp->code.u.three.op1;
                    if (checkvar(tmpop1))
                    {
                        int res = findoperand(tmpop1);
                        if (res == 0)
                        {
                            offset += 4;
                            push_operand(tmpop1, offset);
                        }
                    }
                    Operand tmpop2 = tmp->code.u.three.op2;
                    if (checkvar(tmpop2))
                    {
                        int res = findoperand(tmpop2);
                        if (res == 0)
                        {
                            offset += 4;
                            push_operand(tmpop2, offset);
                        }
                    }
                    Operand resop = tmp->code.u.three.result;
                    int res = findoperand(resop);
                    if (res == 0)
                    {
                        offset += 4;
                        push_operand(resop, offset);
                    }
                    break;
                }
                case IN_READ:
                {
                    Operand tmpop = tmp->code.u.one.op0;
                    int res = findoperand(tmpop);
                    if (res == 0)
                    {
                        offset += 4;
                        push_operand(tmpop, offset);
                    }
                    break;
                }
                case IN_ASSIGN:
                {
                    Operand tmpop1 = tmp->code.u.two.left;
                    if (checkvar(tmpop1))
                    {
                        int res = findoperand(tmpop1);
                        if (res == 0)
                        {
                            offset += 4;
                            push_operand(tmpop1, offset);
                        }
                    }
                    Operand tmpop2 = tmp->code.u.two.right;
                    if (checkvar(tmpop2))
                    {
                        int res = findoperand(tmpop2);
                        if (res == 0)
                        {
                            offset += 4;
                            push_operand(tmpop2, offset);
                        }
                    }
                    break;
                }
                case IN_CALL:
                {
                    Operand tmpop = tmp->code.u.two.left;
                    int res = findoperand(tmpop);
                    if (res == 0)
                    {
                        offset += 4;
                        push_operand(tmpop, offset);
                    }
                    break;
                }
                }
                tmp = tmp->next;
            }
            // 开辟sp:
            fprintf(fp, "  addi $sp, $sp, -%d\n", offset);
            // 更新地址位置:
            fprintf(fp, "  sw $ra, %d($sp)\n", offset - 4);
            fprintf(fp, "  sw $fp, %d($sp)\n", offset - 8);
            fprintf(fp, "  addi $fp, $sp, %d\n", offset);
        }
        break;
    }
    case IN_PARAM:
    {
        // 在IN_FUNCTION中处理
        break;
    }
    case IN_RETURN:
    {
        if (cur->code.u.one.op0->kind == OP_CONSTANT) // return的结果是常数
        {
            int value = cur->code.u.one.op0->value;
            int tmpreg = 8;
            if (cur->code.u.one.op0->value == 0) // 如果return的0，直接使用$zero返回即可
            {
                fprintf(fp, "  move $v0, %s\n", reg[0].name);
            }
            else // 需要用一个寄存器存取立即数
            {
                Load_Reg(cur->code.u.one.op0, tmpreg, fp);
                fprintf(fp, "  move $v0, %s\n", reg[tmpreg].name);
            }
        }
        else
        {
            int returnreg = 8;
            Load_Reg(cur->code.u.one.op0, returnreg, fp);
            fprintf(fp, "  move $v0, %s\n", reg[returnreg].name);
        }
        // 结果保存,修改返回地址和fp;
        fprintf(fp, "  move $sp, $fp\n");
        fprintf(fp, "  lw $ra, -4($fp)\n");
        fprintf(fp, "  lw $fp, -8($fp)\n");
        fprintf(fp, "  jr $ra\n");
        break;
    }
    case IN_LABEL: // 如果是label类型，输出label标签号
    {
        fprintf(fp, "label%d:\n", cur->code.u.one.op0->varno);
        break;
    }
    case IN_GOTO: // 如果是GOTO类型，输出跳转到哪个标签
    {
        fprintf(fp, "  j label%d\n", cur->code.u.one.op0->varno);
        break;
    }
    case IN_WRITE:
    {
        int tmpreg = 8;
        int writereg = 4;  //$a0，参数寄存器
        Load_Reg(cur->code.u.one.op0, tmpreg, fp);
        fprintf(fp, "  move %s, %s\n", reg[writereg].name, reg[tmpreg].name);
        fprintf(fp, "  jal write\n");
        break;
    }
    case IN_READ:
    {
        int tmpreg = 8;
        int readreg = 2;  //$v0,返回值寄存器
        fprintf(fp, "  jal read\n");
        fprintf(fp, "  move %s, %s\n", reg[tmpreg].name, reg[readreg].name);
        Save_Reg(cur->code.u.one.op0, tmpreg, fp);
        break;
    }
    case IN_ARG: // 碰到参数类型
    {
        fprintf(fp, "  addi $sp,$sp,-4\n"); // 开辟栈空间
        int argreg = 16;
        Load_Reg(cur->code.u.one.op0, argreg, fp);
        fprintf(fp, "  sw %s, 0($sp)\n", reg[argreg].name);
        break;
    }
    case IN_ASSIGN: // 赋值语句
    {
        int leftreg = 8;
        int rightreg = 9;
        Load_Reg(cur->code.u.two.left, leftreg, fp);
        Load_Reg(cur->code.u.two.right, rightreg, fp);
        fprintf(fp, "  move %s, %s\n", reg[leftreg].name, reg[rightreg].name);
        Save_Reg(cur->code.u.two.left, leftreg, fp);
        break;
    }
    case IN_DEC:
    {
        break;
    }
    case IN_CALL: // 函数调用语句
    {
        fprintf(fp, "  jal %s\n", cur->code.u.two.right->funcname); // 跳转到函数
        int returnreg = 2;                                          // 使用$v0
        Save_Reg(cur->code.u.two.left, returnreg, fp);  //写回
        break;
    }
    case IN_ADD: // 加法,区分为有立即数和无立即数
    {
        if (cur->code.u.three.op2->kind == OP_CONSTANT || cur->code.u.three.op1->kind == OP_CONSTANT)
        {
            int resreg = 8;
            int varreg = 9;
            Load_Reg(cur->code.u.three.op1, varreg, fp);
            if (cur->code.u.three.op2->kind == OP_CONSTANT)
                fprintf(fp, "  addi %s, %s, %d\n", reg[resreg].name, reg[varreg].name, cur->code.u.three.op2->value);
            else{
                fprintf(fp, "  addi %s, %s, %d\n", reg[resreg].name, reg[varreg].name, cur->code.u.three.op1->value);
            }
            Save_Reg(cur->code.u.three.result, resreg, fp);
        }
        else
        {
            int resreg = 8;
            int leftreg = 9;
            int rightreg = 10;
            Load_Reg(cur->code.u.three.op1, leftreg, fp);
            Load_Reg(cur->code.u.three.op2, rightreg, fp);
            fprintf(fp, "  add %s, %s, %s\n", reg[resreg].name, reg[leftreg].name, reg[rightreg].name);
            Save_Reg(cur->code.u.three.result, resreg, fp);
        }
        break;
    }
    case IN_SUB: // 减法
    {
        int resreg = 8;
        int leftreg = 9;
        int rightreg = 10;
        Load_Reg(cur->code.u.three.op1, leftreg, fp);
        Load_Reg(cur->code.u.three.op2, rightreg, fp);
        fprintf(fp, "  sub %s, %s, %s\n", reg[resreg].name, reg[leftreg].name, reg[rightreg].name);
        Save_Reg(cur->code.u.three.result, resreg, fp);
        break;
    }
    case IN_MUL: // 乘法
    {
        int resreg = 8;
        int leftreg = 9;
        int rightreg = 10;
        Load_Reg(cur->code.u.three.op1, leftreg, fp);
        Load_Reg(cur->code.u.three.op2, rightreg, fp);
        fprintf(fp, "  mul %s, %s, %s\n", reg[resreg].name, reg[leftreg].name, reg[rightreg].name);
        Save_Reg(cur->code.u.three.result, resreg, fp);
        break;
    }
    case IN_DIV: // 除法
    {
        int resreg = 8;
        int leftreg = 9;
        int rightreg = 10;
        Load_Reg(cur->code.u.three.op1, leftreg, fp);
        Load_Reg(cur->code.u.three.op2, rightreg, fp);
        fprintf(fp, "  div %s, %s, %s\n", reg[resreg].name, reg[leftreg].name, reg[rightreg].name);
        fprintf(fp, "  mflo %s\n", reg[resreg].name);
        Save_Reg(cur->code.u.three.result, resreg, fp);
        break;
    }
    case IN_IFGOTO: // 条件语句
    {
        int op1reg = 8;
        int op2reg = 9;
        Load_Reg(cur->code.u.four.op1, op1reg, fp);
        Load_Reg(cur->code.u.four.op2, op2reg, fp);
        fprintf(fp, "  ");
        PrintRelop(cur->code.u.four.relop, fp);
        fprintf(fp, "%s, %s, label%d\n", reg[op1reg].name, reg[op2reg].name, cur->code.u.four.op3->varno);
        break;
    }
    }
}

/*
    生成目标代码：
    如果使用上面注释的语句，则按照中间代码的顺序生成；
    下面没有被注释的语句是先分析main函数，再分析其他函数；
*/
void TransLateAllCode(FILE *fp)
{
    // struct InterCodes *tmp = inter_head->next;
    // while (tmp != inter_head){
    //     TransLateCode(fp,tmp);
    //     tmp = tmp->next;
    // }
    struct InterCodes *begintmp = inter_head->next;
    while (begintmp != inter_head)
    {
        if (begintmp->code.kind == IN_FUNCTION)
        {
            if (strcmp(begintmp->code.u.one.op0->funcname, "main") == 0) // 找到main函数为止
                break;
        }
        begintmp = begintmp->next;
    }
    struct InterCodes *tmp = begintmp;
    while (tmp != inter_head)
    {
        TransLateCode(fp, tmp);
        tmp = tmp->next;
    }
    tmp = inter_head->next;
    while (tmp != begintmp)
    {
        TransLateCode(fp, tmp);
        tmp = tmp->next;
    }
}

/*
    初始化 + 开始翻译
*/
void CodeGenerate(FILE *fp)
{
    init_reg();
    init_data(fp);
    init_code(fp);
    TransLateAllCode(fp);
}