#include "operand.h"

const int MAX_DEPTH = 2147483646;
extern Node *GetChild(Node *cur, int depth);

struct InterCodes *inter_head = NULL; //链表头
struct InterCodes *inter_tail = NULL; //链表尾
int labelcnt = 0;                     //标签个数
int varcnt = 0;                       //变量个数
int tmpcnt = 0;                       //临时变量个数

int deccnt = 0; //lab4, the last var dec
/*
    插入结点操作；
    head -> node1 -> node2 -> ... -> [node]
*/
void add_list(struct InterCodes *node)
{
    node->next = inter_head;
    node->prev = inter_tail;
    inter_tail->next = node;
    inter_head->prev = node;
    inter_tail = node;
}

/*
    构造 [运算对象] ；
    对变量类型：获取变量名并编号
    对函数类型：获取函数名
    对常量类型：获取数值
    对标签类型：编号
    对临时变量类型：编号
*/
Operand new_op(int kind, int ifaddress, ...)
{
    va_list args;
    va_start(args, ifaddress);
    Operand op = (Operand)malloc(sizeof(struct Operand_));
    op->kind = kind;
    op->ifaddress = ifaddress;
    switch (kind)
    {
    case OP_VARIABLE:
    {
        op->varname = va_arg(args, char *); //获取变量名
        varcnt++;                           //变量个数++
        op->varno = varcnt;                 //赋值
        break;
    }
    case OP_FUNCTION:
    {
        op->funcname = va_arg(args, char *); //获取函数名
        op->varname = NULL;
        break;
    }
    case OP_CONSTANT:
    {
        op->value = va_arg(args, int); //获取int值
        op->varname = NULL;
        break;
    }
    case OP_LABEL:
    {
        labelcnt++; //标签个数++
        op->varno = labelcnt;
        op->varname = NULL;
        break;
    }
    case OP_TEMPVAR:
    {
        tmpcnt++;
        op->varno = tmpcnt;
        op->varname = NULL;
        break;
    }
    default:
        break;
    }
    va_end(args);
    return op;
}

/*
    符号反转,条件反转
*/
char *Reverse_relop(char *relop)
{
    char *result = NULL;
    if (strcmp(relop, ">") == 0)
    {
        result = "<=";
    }
    else if (strcmp(relop, "<") == 0)
    {
        result = ">=";
    }
    else if (strcmp(relop, ">=") == 0)
    {
        result = "<";
    }
    else if (strcmp(relop, "<=") == 0)
    {
        result = ">";
    }
    else if (strcmp(relop, "==") == 0)
    {
        result = "!=";
    }
    else if (strcmp(relop, "!=") == 0)
    {
        result = "==";
    }
    return result;
}

Operand copyop(Operand op)
{
    Operand res = (Operand)malloc(sizeof(struct Operand_));
    res->kind = op->kind;
    res->funcname = op->funcname;
    res->depth = op->depth;
    res->ifaddress = op->ifaddress;
    res->value = op->value;
    res->varno = op->varno;
    res->varname = op->varname;
}

/*
    把四则运算转换成kind
*/
int transformtokind(char *cur)
{
    if (strcmp(cur, "PLUS") == 0)
    {
        return IN_ADD;
    }
    else if (strcmp(cur, "MINUS") == 0)
    {
        return IN_SUB;
    }
    else if (strcmp(cur, "STAR") == 0)
    {
        return IN_MUL;
    }
    else if (strcmp(cur, "DIV") == 0)
    {
        return IN_DIV;
    }
}

/*
    打印运算对象
*/
void printoperand(Operand op, FILE *f)
{
    switch (op->kind)
    {
    case OP_VARIABLE: //如果是变量类型
    {
        if (op->ifaddress == OP_ADDRESS) //地址类型
        {
            fprintf(f, "&"); //取地址号
        }
        fprintf(f, "v%d", op->varno); //输出编号
        break;
    }
    case OP_CONSTANT:
    {
        fprintf(f, "#%d", op->value); //立即数，#开头
        break;
    }
    case OP_FUNCTION:
    {
        fprintf(f, "%s", op->funcname); //是函数，输出函数名
        break;
    }
    case OP_TEMPVAR:
    {
        if (op->ifaddress == OP_ADDRESS) //如果是地址类型，使用*号当作取对应的值
        {
            fprintf(f, "*");
        }
        fprintf(f, "t%d", op->varno);
        break;
    }
    case OP_LABEL:
    {
        fprintf(f, "%d", op->varno); //输出标签编号
        break;
    }

    default:
        break;
    }
}

/*
    打印链表
*/
void printintercode(FILE *f)
{
    struct InterCodes *tmp = inter_head->next;
    while (tmp != inter_head)
    {
        switch (tmp->code.kind)
        {
        case IN_FUNCTION: //函数类型
        {
            fprintf(f, "FUNCTION ");
            printoperand(tmp->code.u.one.op0, f); //输出函数名称
            fprintf(f, " : \n");
            break;
        }
        case IN_PARAM: //参数
        {
            fprintf(f, "PARAM ");
            printoperand(tmp->code.u.one.op0, f);
            fprintf(f, "\n");
            break;
        }
        case IN_RETURN:
        {
            fprintf(f, "RETURN ");
            printoperand(tmp->code.u.one.op0, f);
            fprintf(f, "\n");
            break;
        }
        case IN_LABEL:
        {
            fprintf(f, "LABEL label");
            printoperand(tmp->code.u.one.op0, f); //输出编号
            fprintf(f, " : \n");
            break;
        }
        case IN_GOTO:
        {
            fprintf(f, "GOTO label");
            printoperand(tmp->code.u.one.op0, f);
            fprintf(f, "\n");
            break;
        }
        case IN_WRITE:
        {
            fprintf(f, "WRITE ");
            printoperand(tmp->code.u.one.op0, f);
            fprintf(f, "\n");
            break;
        }
        case IN_READ:
        {
            fprintf(f, "READ ");
            printoperand(tmp->code.u.one.op0, f);
            fprintf(f, "\n");
            break;
        }
        case IN_ARG:
        {
            fprintf(f, "ARG ");
            printoperand(tmp->code.u.one.op0, f);
            fprintf(f, " \n");
            break;
        }

        case IN_ASSIGN:
        {
            printoperand(tmp->code.u.two.left, f); //输出变量名
            fprintf(f, " := ");
            printoperand(tmp->code.u.two.right, f);
            fprintf(f, "\n");
            break;
        }
        case IN_DEC:
        {
            deccnt = tmp->code.u.two.left->varno;
            fprintf(f, "DEC ");
            printoperand(tmp->code.u.two.left, f);
            fprintf(f, " %d", tmp->code.u.two.right->value); //输出整数
            fprintf(f, "\n");
            break;
        }
        case IN_CALL:
        {
            printoperand(tmp->code.u.two.left, f);
            fprintf(f, " := CALL ");
            printoperand(tmp->code.u.two.right, f);
            fprintf(f, "\n");
            break;
        }

        case IN_ADD:
        {
            printoperand(tmp->code.u.three.result, f);
            fprintf(f, " := ");
            printoperand(tmp->code.u.three.op1, f);
            fprintf(f, " + ");
            printoperand(tmp->code.u.three.op2, f);
            fprintf(f, "\n");
            break;
        }
        case IN_SUB:
        {
            printoperand(tmp->code.u.three.result, f);
            fprintf(f, " := ");
            printoperand(tmp->code.u.three.op1, f);
            fprintf(f, " - ");
            printoperand(tmp->code.u.three.op2, f);
            fprintf(f, "\n");
            break;
        }
        case IN_MUL:
        {
            printoperand(tmp->code.u.three.result, f);
            fprintf(f, " := ");
            printoperand(tmp->code.u.three.op1, f);
            fprintf(f, " * ");
            printoperand(tmp->code.u.three.op2, f);
            fprintf(f, "\n");
            break;
        }
        case IN_DIV:
        {
            printoperand(tmp->code.u.three.result, f);
            fprintf(f, " := ");
            printoperand(tmp->code.u.three.op1, f);
            fprintf(f, " / ");
            printoperand(tmp->code.u.three.op2, f);
            fprintf(f, "\n");
            break;
        }
        case IN_IFGOTO:
        {
            fprintf(f, "IF ");
            printoperand(tmp->code.u.four.op1, f);
            fprintf(f, " %s ", tmp->code.u.four.relop);
            printoperand(tmp->code.u.four.op2, f);
            fprintf(f, " GOTO label");
            printoperand(tmp->code.u.four.op3, f);
            fprintf(f, "\n");
            break;
        }
        default:
        {
            break;
        }
        }
        tmp = tmp->next;
    }
    fclose(f);
}

/*
    构造链表结点类型
*/
void new_intercode(int kind, ...)
{
    va_list args;
    va_start(args, kind);
    struct InterCodes *tmp = (struct InterCodes *)malloc(sizeof(struct InterCodes));
    tmp->code.kind = kind;
    tmp->next = NULL;
    switch (kind)
    {
    case IN_FUNCTION:
    case IN_PARAM:
    case IN_RETURN:
    case IN_LABEL:
    case IN_GOTO:
    case IN_WRITE:
    case IN_READ:
    case IN_ARG:
        tmp->code.u.one.op0 = va_arg(args, Operand);
        break;
    case IN_ASSIGN:
    case IN_DEC:
    case IN_CALL:
    {
        tmp->code.u.two.left = va_arg(args, Operand);
        tmp->code.u.two.right = va_arg(args, Operand);
        break;
    }
    case IN_ADD:
    case IN_SUB:
    case IN_MUL:
    case IN_DIV:
    {
        tmp->code.u.three.result = va_arg(args, Operand);
        tmp->code.u.three.op1 = va_arg(args, Operand);
        tmp->code.u.three.op2 = va_arg(args, Operand);
        break;
    }
    case IN_IFGOTO:
    {
        tmp->code.u.four.op1 = va_arg(args, Operand);
        tmp->code.u.four.relop = va_arg(args, char *);
        tmp->code.u.four.op2 = va_arg(args, Operand);
        tmp->code.u.four.op3 = va_arg(args, Operand);
        break;
    }
    default:
        break;
    }
    add_list(tmp);
    va_end(args);
}

/*
    返回大小, DEC
*/
int GetSize(Type cur)
{
    if (cur->kind == BASIC) // int类型，返回4
    {
        return 4;
    }
    else if (cur->kind == STRUCTURE) //结构体，大小相加
    {
        int res = 0;
        FieldList field = cur->u.structure_.structure;
        while (field != NULL)
        {
            Type type = field->type;
            int size = GetSize(type);
            res += size;
            field = field->tail;
        }
        return res;
    }
    else if (cur->kind == ARRAY) //数组类型，大小相乘 a[5][4][3] -> 3 * 4 * 5
    {
        int res = 1;
        Type type = cur;
        while (type != NULL)
        {
            if (type->kind != ARRAY)
                break;
            res *= type->u.array_.size;
            type = type->u.array_.elem;
        }
        int OneSize = GetSize(type);
        res *= OneSize;
        return res;
    }
}

void Generate(Node *cur, FILE *f)
{
    varcnt = 0;
    tmpcnt = 0;
    labelcnt = 0;
    inter_head = (struct InterCodes *)malloc(sizeof(struct InterCodes));
    inter_head->next = inter_head;
    inter_head->prev = inter_head;
    inter_tail = inter_head;
    ProgramZJDM(cur);
    //printintercode(f);  //lab4
}

int ProgramZJDM(Node *cur)
{
    Node *ExtDefListNode = GetChild(cur, 0);
    int res = ExtDefListZJDM(ExtDefListNode);
    return res;
}

int ExtDefListZJDM(Node *cur)
{
    Node *ExtDefNode = GetChild(cur, 0);
    int success = 0;
    if (ExtDefNode != NULL)
    {
        int res1 = ExtDefZJDM(ExtDefNode);
        success = res1 ? 1 : 0;
        Node *ExtDefListNode = GetChild(cur, 1);
        if (ExtDefListNode != NULL)
        {
            int res2 = ExtDefListZJDM(ExtDefListNode);
            success &= res2 ? 1 : 0;
        }
    }
    return success;
}

int ExtDefZJDM(Node *cur)
{
    int success = 1;
    Node *tmp = GetChild(cur, 1);
    if (strcmp(tmp->name, "FunDec") == 0)
    {
        Node *tmp2 = GetChild(cur, 2);
        if (strcmp(tmp2->name, "CompSt") == 0)
        {
            int res1 = FunDecZJDM(tmp);
            int res2 = CompStZJDM(tmp2);
            success = res1 & res2;
        }
    }
    return success;
}

int FunDecZJDM(Node *cur)
{
    Node *IDNode = GetChild(cur, 0);
    Operand funcop = new_op(OP_FUNCTION, OP_VAR, IDNode->id); //创建函数对象

    new_intercode(IN_FUNCTION, funcop); //加入链表中

    int success = 0;
    struct SymbolNode *funcsymbol = querySymbolNodeZJDM(funcop->funcname, &success); //获取符号表结点
    int paramnum = funcsymbol->field.type->u.fuction.num;                            //得到参数数量

    if (paramnum != 0)
    {
        FieldList params = funcsymbol->field.type->u.fuction.parameters; //获得函数的参数域
        while (params != NULL)
        {
            Operand paramop = NULL;
            if (params->type->kind == ARRAY || params->type->kind == STRUCTURE) //参数为数组或者结构体时
            {
                paramop = new_op(OP_VARIABLE, OP_ADDRESS, (char *)(params->name)); //创建变量、地址类型的对象
            }
            else
            {
                paramop = new_op(OP_VARIABLE, OP_VAR, (char *)(params->name)); //创建简单变量对象
            }
            int success = 0;
            struct SymbolNode *QueryNode = querySymbolNodeZJDM(params->name, &success); //获取符号表

            QueryNode->var_no = paramop->varno; //把对象的编号传给符号表
            QueryNode->ifaddress = paramop->ifaddress;
            paramop->ifaddress = OP_VAR;     
            new_intercode(IN_PARAM, paramop); //以参数类型放入链表
            params = params->tail;
        }
    }
    return 1;
}

int CompStZJDM(Node *cur)
{
    Node *TmpNode = GetChild(cur, 1);
    if (strcmp(TmpNode->name, "DefList") == 0)
    {
        DefListZJDM(TmpNode);
        Node *StmtListNode = GetChild(cur, 2);
        if (strcmp(StmtListNode->name, "StmtList") == 0)
        {
            StmtListZJDM(StmtListNode);
        }
    }
    else if (strcmp(TmpNode->name, "StmtList") == 0)
    {
        StmtListZJDM(TmpNode);
    }
    return 1;
}
int DefListZJDM(Node *cur)
{
    if (cur == NULL)
        return 1;
    Node *TmpNode = GetChild(cur, 0);
    if (TmpNode != NULL)
    {
        Node *DefNode = TmpNode;
        Node *DefListNode = GetChild(cur, 1);
        DefZJDM(DefNode);
        if (DefListNode != NULL)
        {
            DefListZJDM(DefListNode);
        }
    }
    return 1;
}

int DefZJDM(Node *cur)
{
    Node *DecListNode = GetChild(cur, 1);
    DecListZJDM(DecListNode);
    return 1;
}

int StmtListZJDM(Node *cur)
{
    if (cur == NULL)
        return 1;
    Node *StmtNode = GetChild(cur, 0);
    if (StmtNode != NULL)
    {
        if (strcmp(StmtNode->name, "Stmt") == 0)
        {
            StmtZJDM(StmtNode);
            Node *StmtListNode = GetChild(cur, 1);
            if (StmtListNode != NULL)
            {
                StmtListZJDM(StmtListNode);
            }
        }
    }
    return 1;
}

int StmtZJDM(Node *cur)
{
    Node *tmp1 = GetChild(cur, 0);
    if (strcmp(tmp1->name, "CompSt") == 0)
    {
        CompStZJDM(tmp1);
    }
    else if (strcmp(tmp1->name, "Exp") == 0)
    {
        ExpZJDM(tmp1);
    }
    else if (strcmp(tmp1->name, "RETURN") == 0)
    {
        Node *ExpNode = GetChild(cur, 1);
        Operand ExpOp = ExpZJDM(ExpNode);
        new_intercode(IN_RETURN, ExpOp);
    }
    else if (strcmp(tmp1->name, "WHILE") == 0)
    {
        Operand label1 = new_op(OP_LABEL, OP_VAR);
        Operand label2 = new_op(OP_LABEL, OP_VAR);
        new_intercode(IN_LABEL, label1);
        Node *ExpNode = GetChild(cur, 2);
        ConditionZJDM(ExpNode, NULL, label2);
        Node *StmtNode = GetChild(cur, 4);
        StmtZJDM(StmtNode);
        new_intercode(IN_GOTO, label1);
        new_intercode(IN_LABEL, label2);
    }
    else if (strcmp(tmp1->name, "IF") == 0)
    {
        Operand label1 = new_op(OP_LABEL, OP_VAR);

        Node *ExpNode = GetChild(cur, 2);
        ConditionZJDM(ExpNode, NULL, label1);
        Node *StmtNode = GetChild(cur, 4);
        StmtZJDM(StmtNode);
        Node *tmpnode = GetChild(cur, 5);
        if (tmpnode == NULL)
        {
            new_intercode(IN_LABEL, label1);
        }
        else
        {
            Operand label2 = new_op(OP_LABEL, OP_VAR);

            new_intercode(IN_GOTO, label2);
            new_intercode(IN_LABEL, label1);
            Node *StmtNode2 = GetChild(cur, 6);
            StmtZJDM(StmtNode2);
            new_intercode(IN_LABEL, label2);
        }
    }
    return 1;
}

int DecListZJDM(Node *cur)
{
    Node *DecNode = GetChild(cur, 0);
    DecZJDM(DecNode);
    Node *COMMANODE = GetChild(cur, 1);
    if (COMMANODE != NULL)
    {
        Node *DecListNode = GetChild(cur, 2);
        if (DecListNode != NULL)
        {
            DecListZJDM(DecListNode);
        }
    }
    return 1;
}

int DecZJDM(Node *cur)
{
    Node *VarDecNode = GetChild(cur, 0);
    Node *ASSIGNOPNode = GetChild(cur, 1);
    if (ASSIGNOPNode == NULL)
    {
        VarDecZJDM(VarDecNode);
    }
    else
    {
        Operand VarDecOp = VarDecZJDM(VarDecNode);
        Node *ExpNode = GetChild(cur, 2);
        Operand ExpOp = ExpZJDM(ExpNode);
        new_intercode(IN_ASSIGN, VarDecOp, ExpOp);
    }
    return 1;
}

Operand VarDecZJDM(Node *cur)
{
    Operand res = NULL;
    if (cur == NULL)
        return NULL;
    Node *tmp = GetChild(cur, 0);
    if (strcmp(tmp->name, "ID") == 0)
    {
        int success = 0;
        struct SymbolNode *QueryID = querySymbolNodeZJDM(tmp->id, &success);
        int typesize = GetSize(QueryID->field.type);

        res = new_op(OP_VARIABLE, OP_VAR, tmp->id);
        QueryID->ifaddress = res->ifaddress;
        QueryID->var_no = res->varno;

        if (typesize == 4)
        {
            // int
        }
        else
        {
            printf("%d\n",typesize);
            Operand tmpOp = new_op(OP_CONSTANT, OP_VAR, typesize);
            new_intercode(IN_DEC, res, tmpOp);
        }
    }
    else
    {
        Node *FindNode = GetChild(tmp, 0);
        while (strcmp(FindNode->name, "ID"))
        {
            FindNode = FindNode->child;
        }
        int success = 0;
        struct SymbolNode *QueryID = querySymbolNodeZJDM(FindNode->id, &success);

        res = new_op(OP_VARIABLE, OP_VAR, FindNode->id);
        QueryID->ifaddress = res->ifaddress;
        QueryID->var_no = res->varno;
        int ArraySize = GetSize(QueryID->field.type);
        Operand tmpOp = new_op(OP_CONSTANT, OP_VAR, ArraySize);
        new_intercode(IN_DEC, res, tmpOp);
    }
    return res;
}

int GetArrayDepth(struct SymbolNode *ArrayNode)
{
    int res = 0;
    Type tmp = ArrayNode->field.type;
    while (tmp->kind == ARRAY)
    {
        res++;
        tmp = tmp->u.array_.elem;
    }
    return res;
}

void ArgZJDM(Node *cur, FieldList param)
{
    if (cur == NULL)
        return;
    if (param == NULL)
        return;

    Node *tmp = GetChild(cur, 0);
    Operand tmpOp = ExpZJDM(tmp);
    Operand op = copyop(tmpOp);

    if (param->type->kind == STRUCTURE || param->type->kind == ARRAY)
    {
        int flag = 0;
        if (param->type->kind == ARRAY)
        {
            int success = 0;
            char *queryname = op->varname;
            struct SymbolNode *queryNode = querySymbolNodeZJDM(queryname, &success);
            int ArrayDepth = GetArrayDepth(queryNode);
            if (op->depth < ArrayDepth)
            {
                flag = 1;
            }
            if (op->depth == 0)
            {
                flag = 0;
            }
        }
        if (flag == 1)
        {
            op->ifaddress = OP_VAR;
        }
        else if (op->ifaddress == OP_ADDRESS)
        {
            op->ifaddress = OP_VAR;
        }
        else
        {
            op->ifaddress = OP_ADDRESS;
        }
    }

    Node *tmpnode2 = GetChild(cur, 1);
    if (tmpnode2 == NULL)
    {
    }
    else
    {
        Node *tmpnode3 = GetChild(cur, 2);
        ArgZJDM(tmpnode3, param->tail);
    }
    new_intercode(IN_ARG, op);
}

Operand ExpZJDM(Node *cur)
{
    Operand res = NULL;

    Node *tmp1 = GetChild(cur, 0);
    if (tmp1 == NULL)
        return res;

    if (strcmp(tmp1->name, "ID") == 0)
    {
        Node *tmp2 = GetChild(cur, 1);
        if (tmp2 == NULL)
        {
            int success = 0;
            struct SymbolNode *QueryID = querySymbolNodeZJDM(tmp1->id, &success);

            if (QueryID->field.type->kind == ARRAY || QueryID->field.type->kind == STRUCTURE)
            {
                if (QueryID->ifaddress == OP_ADDRESS)
                {
                    res = new_op(OP_VARIABLE, OP_ADDRESS, tmp1->id);
                }
                else
                {
                    res = new_op(OP_VARIABLE, OP_VAR, tmp1->id);
                }
                varcnt--;
                res->varno = QueryID->var_no;
                res->depth = 0;
                return res;
            }
            else
            {
                res = new_op(OP_VARIABLE, OP_VAR, tmp1->id);
                varcnt--;
                res->varno = QueryID->var_no;
                res->depth = 0;
                return res;
            }
        }
        else
        {
            Node *tmp3 = GetChild(cur, 2);
            if (strcmp(tmp1->id, "write") == 0)
            {
                if (strcmp(tmp3->name, "Args") == 0)
                {
                    Node *tmp31 = GetChild(tmp3, 0);
                    Operand tmp = NULL;
                    if (strcmp(tmp31->name, "Exp") == 0)
                    {
                        tmp = ExpZJDM(tmp31);
                    }
                    if (tmp != NULL)
                        new_intercode(IN_WRITE, tmp);

                    Operand constantop = new_op(OP_CONSTANT, OP_VAR, 0);
                    res = new_op(OP_TEMPVAR, OP_VAR);
                    new_intercode(IN_ASSIGN, res, constantop);
                    return res;
                }
            }
            res = new_op(OP_TEMPVAR, OP_VAR);
            if (strcmp(tmp1->id, "read") == 0)
            {
                new_intercode(IN_READ, res);
                return res;
            }

            Operand functionop = new_op(OP_FUNCTION, OP_VAR, tmp1->id);
            if (strcmp(tmp3->name, "RP") == 0)
            {
                new_intercode(IN_CALL, res, functionop);
                return res;
            }
            else if (strcmp(tmp3->name, "Args") == 0)
            {
                int success = 0;
                struct SymbolNode *QueryID = querySymbolNodeZJDM(tmp1->id, &success);
                ArgZJDM(tmp3, QueryID->field.type->u.fuction.parameters);
                new_intercode(IN_CALL, res, functionop);
                return res;
            }
            return res;
        }
    }
    else if (strcmp(tmp1->name, "INT") == 0)
    {
        res = new_op(OP_CONSTANT, OP_VAR, tmp1->intvalue);
        return res;
    }
    else if (strcmp(tmp1->name, "FLOAT") == 0)
    {
        res = new_op(OP_CONSTANT, OP_VAR, 0);
        return res;
    }
    else if (strcmp(tmp1->name, "LP") == 0)
    {
        Node *ExpNode = GetChild(cur, 1);
        return ExpZJDM(ExpNode);
    }
    else if (strcmp(tmp1->name, "MINUS") == 0)
    {
        // Operand zero = new_op(OP_CONSTANT,OP_VAR,0);
        Node *ExpNode = GetChild(cur, 1);
        // Operand op1 = ExpZJDM(ExpNode);
        // Operand op2 = new_op(OP_TEMPVAR,OP_VAR);
        // new_intercode(IN_SUB,op2,zero,op1);
        int fushu = ExpNode->intvalue * -1;
        Operand op2 = new_op(OP_CONSTANT, OP_VAR, fushu);
        res = op2;
        return res;
    }
    else if (strcmp(tmp1->name, "NOT") == 0 ||
             (strcmp(tmp1->name, "Exp") == 0 && tmp1->sibling != NULL && strcmp(tmp1->sibling->name, "RELOP") == 0) ||
             (strcmp(tmp1->name, "Exp") == 0 && tmp1->sibling != NULL && strcmp(tmp1->sibling->name, "AND") == 0) ||
             (strcmp(tmp1->name, "Exp") == 0 && tmp1->sibling != NULL && strcmp(tmp1->sibling->name, "OR") == 0))
    {
        Operand label1 = new_op(OP_LABEL, OP_VAR);
        Operand label2 = new_op(OP_LABEL, OP_VAR);
        res = new_op(OP_TEMPVAR, OP_VAR);
        Operand zero = new_op(OP_CONSTANT, OP_VAR, 0);
        new_intercode(IN_ASSIGN, res, zero);
        ConditionZJDM(cur, label1, label2);
        new_intercode(IN_LABEL, label1);
        Operand one = new_op(OP_CONSTANT, OP_VAR, 1);
        new_intercode(IN_ASSIGN, res, one);
        new_intercode(IN_LABEL, label2);
        return res;
    }
    else if (strcmp(tmp1->name, "Exp") == 0)
    {
        Node *tmp2 = GetChild(cur, 1);
        if ((strcmp(tmp2->name, "PLUS") == 0) ||
            (strcmp(tmp2->name, "MINUS") == 0) ||
            (strcmp(tmp2->name, "STAR") == 0) ||
            (strcmp(tmp2->name, "DIV")) == 0)
        {
            int inkind = transformtokind(tmp2->name);
            res = new_op(OP_TEMPVAR, OP_VAR);
            Node *ExpNode1 = tmp1;
            Node *ExpNode2 = GetChild(cur, 2);
            Operand op1 = ExpZJDM(ExpNode1);
            Operand op2 = ExpZJDM(ExpNode2);
            if (op1 != NULL && op2 != NULL)
            {
                new_intercode(inkind, res, op1, op2);
            }
            return res;
        }
        else if (strcmp(tmp2->name, "ASSIGNOP") == 0)
        {
            Node *ExpNode1 = tmp1;
            Node *ExpNode2 = GetChild(cur, 2);
            Operand op1 = ExpZJDM(ExpNode1);
            Operand op2 = ExpZJDM(ExpNode2);
            int flag = 0;
            if (op1->varname != NULL && op2->varname != NULL)
            {
                int success = 0;
                struct SymbolNode *QueryID1 = querySymbolNodeZJDM(op1->varname, &success);
                struct SymbolNode *QueryID2 = querySymbolNodeZJDM(op2->varname, &success);
                if (QueryID1->field.type->kind == ARRAY && QueryID2->field.type->kind == ARRAY)
                {
                    if (op1->ifaddress == OP_VAR && op2->ifaddress == OP_VAR)
                    {
                        flag = 1;
                    }
                }
            }
            if (flag == 1)
            {
                int success = 0;
                struct SymbolNode *queryid1 = querySymbolNodeZJDM(op1->varname, &success);
                struct SymbolNode *queryid2 = querySymbolNodeZJDM(op2->varname, &success);
                int depth1 = op1->depth;
                int depth2 = op2->depth;
                int typesize = GetSize(queryid2->field.type);
                Operand constantop = new_op(OP_CONSTANT, OP_VAR, typesize);
                Operand four = new_op(OP_CONSTANT, OP_VAR, 4);
                Operand v1 = copyop(op1);

                if (v1->kind == OP_VARIABLE)
                {
                    v1->ifaddress = OP_ADDRESS;
                }
                Operand v2 = copyop(op2);
                if (v2->kind == OP_VARIABLE)
                {
                    v2->ifaddress = OP_ADDRESS;
                }

                Operand t1op = new_op(OP_TEMPVAR, OP_VAR);
                new_intercode(IN_ASSIGN, t1op, v1);

                Operand t2op = new_op(OP_TEMPVAR, OP_VAR);
                new_intercode(IN_ASSIGN, t2op, v2);

                Operand oriop = new_op(OP_VARIABLE, OP_ADDRESS, op2->varname);
                oriop->varno = queryid2->var_no;
                varcnt--;
                Operand endop = new_op(OP_TEMPVAR, OP_VAR);
                new_intercode(IN_ADD, endop, oriop, constantop);

                Operand labelop1 = new_op(OP_LABEL, OP_VAR);
                Operand labelop2 = new_op(OP_LABEL, OP_VAR);
                new_intercode(IN_LABEL, labelop1);
                new_intercode(IN_IFGOTO, t2op, ">=", endop, labelop2);
                Operand tmpt1op = copyop(t1op);
                Operand tmpt2op = copyop(t2op);
                tmpt1op->ifaddress = OP_ADDRESS;
                tmpt2op->ifaddress = OP_ADDRESS;
                new_intercode(IN_ASSIGN, tmpt1op, tmpt2op);
                new_intercode(IN_ADD, t1op, t1op, four);
                new_intercode(IN_ADD, t2op, t2op, four);
                new_intercode(IN_GOTO, labelop1);
                new_intercode(IN_LABEL, labelop2);
            }
            else if (op1 != NULL && op2 != NULL)
            {
                new_intercode(IN_ASSIGN, op1, op2);
            }
            res = op1;
            return res;
        }
        else if (strcmp(tmp2->name, "DOT") == 0)
        {
            Operand ExpOp = ExpZJDM(tmp1);
            Operand tmpExpOp = copyop(ExpOp);
            Node *tmp3 = GetChild(cur, 2);
            int success = 0;
            struct SymbolNode *QueryId = querySymbolNodeZJDM(tmp3->id, &success);
            int offset = QueryId->offset;
            //printf("%d\n",offset);
            if (offset == 0)
            {
                Operand tmpop = new_op(OP_TEMPVAR, OP_VAR);
                if (tmpExpOp->ifaddress == OP_ADDRESS)
                {
                    tmpExpOp->ifaddress = OP_VAR;
                }
                else
                {
                    tmpExpOp->ifaddress = OP_ADDRESS;
                }
                new_intercode(IN_ASSIGN, tmpop, tmpExpOp);
                res = copyop(tmpop);
                res->ifaddress = OP_ADDRESS;
                res->varname = tmp3->id;
                return res;
            }
            else
            {
                Operand constantop = new_op(OP_CONSTANT, OP_VAR, offset);
                Operand tmpop = new_op(OP_TEMPVAR, OP_VAR);

                if (tmpExpOp->ifaddress == OP_ADDRESS)
                {
                    tmpExpOp->ifaddress = OP_VAR;
                }
                else
                {
                    tmpExpOp->ifaddress = OP_ADDRESS;
                }
                new_intercode(IN_ADD, tmpop, tmpExpOp, constantop);
                //printf("in_add = %d\n",offset);
                res = copyop(tmpop);
                res->ifaddress = OP_ADDRESS;
                res->varname = tmp3->id;
                return res;
            }
        }
        else if (strcmp(tmp2->name, "LB") == 0)
        {
            Operand ExpOp1 = copyop(ExpZJDM(tmp1));
            int depth = ExpOp1->depth;
            int success = 0;
            struct SymbolNode *queryid = querySymbolNodeZJDM(ExpOp1->varname, &success);
            Type ttype = queryid->field.type;
            Type tmpttype = ttype;
            int cnt = 0;
            while (tmpttype->kind == ARRAY)
            {
                cnt++;
                tmpttype = tmpttype->u.array_.elem;
            }
            int typesize = GetSize(tmpttype);
            int *arraysize = (int *)malloc(sizeof(int) * (cnt + 1));
            cnt = 0;
            tmpttype = ttype;
            while (tmpttype->kind == ARRAY)
            {
                arraysize[cnt] = tmpttype->u.array_.size;
                cnt++;
                tmpttype = tmpttype->u.array_.elem;
            }
            int ptr = cnt - 1;
            int tmpdepth = cnt - depth - 1;
            int offset = 1;
            while (tmpdepth)
            {
                offset *= arraysize[ptr];
                tmpdepth--;
                ptr--;
            }
            free(arraysize);
            offset = offset * typesize;

            Node *tmp3 = GetChild(cur, 2);
            Operand ExpOp2 = ExpZJDM(tmp3);

            Operand tmpop1 = new_op(OP_TEMPVAR, OP_VAR);
            
            Operand constantop = new_op(OP_CONSTANT, OP_VAR, offset);  //every dimension's size;

            int value = 0;  //offset;
            if (ExpOp2->kind == OP_CONSTANT){
                value = ExpOp2->value * constantop->value;
                //Operand valueop = new_op(OP_CONSTANT,OP_VAR,value);
                //new_intercode(IN_ASSIGN,tmpop1,valueop);
            }
            else new_intercode(IN_MUL, tmpop1, ExpOp2, constantop);
            
            Operand tmpop2 = new_op(OP_TEMPVAR, OP_VAR);
            tmpop2->varname = ExpOp1->varname;
            tmpop2->depth = depth + 1;
            if (depth == 0 && ExpOp1->ifaddress == OP_VAR)
            {
                ExpOp1->ifaddress = OP_ADDRESS;
            }
            else
            {
                ExpOp1->ifaddress = OP_VAR;
            }
            if (ExpOp2->kind == OP_CONSTANT){
                Operand valueop = new_op(OP_CONSTANT,OP_VAR,value);
                if (value == 0) new_intercode(IN_ASSIGN,tmpop2,ExpOp1);
                else new_intercode(IN_ADD,tmpop2,ExpOp1,valueop);
            }
            else new_intercode(IN_ADD, tmpop2, ExpOp1, tmpop1);
            res = copyop(tmpop2);
            if (tmpop2->depth == cnt)
            {
                res->ifaddress = OP_ADDRESS;
            }
            return res;
        }
    }
    return res;
}

int ConditionZJDM(Node *cur, Operand label_True, Operand label_False)
{
    Operand zero = new_op(OP_CONSTANT, OP_VAR, 0);

    if (cur == NULL)
    {
    }
    else
    {
        Node *tmp1 = GetChild(cur, 0);
        if (strcmp(tmp1->name, "Exp") == 0)
        {
            Node *tmp2 = GetChild(cur, 1);
            if (strcmp(tmp2->name, "ASSIGNOP") == 0)
            {
                Operand op1 = ExpZJDM(tmp1);
                Node *tmp3 = GetChild(cur, 2);
                Operand op2 = ExpZJDM(tmp3);
                new_intercode(IN_ASSIGN, op1, op2);
                if (label_True != NULL && label_False != NULL)
                {
                    if (op1 != NULL)
                        new_intercode(IN_IFGOTO, op1, "!=", zero, label_True);
                    new_intercode(IN_GOTO, label_False);
                }
                else if (label_True != NULL)
                {
                    if (op1 != NULL)
                    {
                        new_intercode(IN_IFGOTO, op1, "!=", zero, label_True);
                    }
                }
                else if (label_False != NULL)
                {
                    if (op1 != NULL)
                    {
                        new_intercode(IN_IFGOTO, op1, "==", zero, label_False);
                    }
                }
            }
            else if (strcmp(tmp2->name, "AND") == 0)
            {
                if (label_False != NULL)
                {
                    ConditionZJDM(tmp1, NULL, label_False);
                    Node *tmp3 = GetChild(cur, 2);
                    ConditionZJDM(tmp3, label_True, label_False);
                }
                else
                {
                    Operand newlabel = new_op(OP_LABEL, OP_VAR);
                    ConditionZJDM(tmp1, NULL, newlabel);
                    Node *tmp3 = GetChild(cur, 2);
                    ConditionZJDM(tmp3, label_True, label_False);
                    new_intercode(IN_LABEL, newlabel);
                }
            }
            else if (strcmp(tmp2->name, "OR") == 0)
            {
                if (label_True != NULL)
                {
                    ConditionZJDM(tmp1, label_True, NULL);
                    Node *tmp3 = GetChild(cur, 2);
                    ConditionZJDM(tmp3, label_True, label_False);
                }
                else
                {
                    Operand newlabel = new_op(OP_LABEL, OP_VAR);
                    ConditionZJDM(tmp1, newlabel, NULL);
                    Node *tmp3 = GetChild(cur, 2);
                    ConditionZJDM(tmp3, label_True, label_False);
                    new_intercode(IN_LABEL, newlabel);
                }
            }
            else if (strcmp(tmp2->name, "RELOP") == 0)
            {
                Operand op1 = ExpZJDM(tmp1);
                Node *tmp3 = GetChild(cur, 2);
                Operand op2 = ExpZJDM(tmp3);
                if (label_True != NULL && label_False != NULL)
                {
                    if (op1 != NULL)
                        new_intercode(IN_IFGOTO, op1, tmp2->id, op2, label_True);
                    new_intercode(IN_GOTO, label_False);
                }
                else if (label_True != NULL)
                {
                    if (op1 != NULL)
                    {
                        new_intercode(IN_IFGOTO, op1, tmp2->id, op2, label_True);
                    }
                }
                else if (label_False != NULL)
                {
                    if (op1 != NULL)
                    {
                        new_intercode(IN_IFGOTO, op1, Reverse_relop(tmp2->id), op2, label_False);
                    }
                }
            }
            else if (strcmp(tmp2->name, "PLUS") == 0 || strcmp(tmp2->name, "MINUS") == 0 || strcmp(tmp2->name, "STAR") == 0 || strcmp(tmp2->name, "DIV") == 0)
            {
                Operand op1 = ExpZJDM(tmp1);
                Node *tmp3 = GetChild(cur, 2);
                Operand op2 = ExpZJDM(tmp3);
                int inkind = transformtokind(tmp2->name);
                Operand res = new_op(OP_TEMPVAR, OP_VAR);
                if (op1 != NULL && op2 != NULL)
                {
                    new_intercode(inkind, res, op1, op2);
                }

                if (label_True != NULL && label_False != NULL)
                {
                    new_intercode(IN_IFGOTO, res, "!=", zero, label_True);
                    new_intercode(IN_GOTO, label_False);
                }
                else if (label_True != NULL)
                {
                    new_intercode(IN_IFGOTO, res, "!=", zero, label_True);
                }
                else if (label_False != NULL)
                {
                    new_intercode(IN_IFGOTO, res, "==", zero, label_False);
                }
            }
            else if (strcmp(tmp2->name, "LB") == 0)
            {
                Operand op = ExpZJDM(cur);
                if (label_True != NULL && label_False != NULL)
                {
                    new_intercode(IN_IFGOTO, op, "!=", zero, label_True);
                    new_intercode(IN_GOTO, label_False);
                }
                else if (label_True != NULL)
                {
                    new_intercode(IN_IFGOTO, op, "!=", zero, label_True);
                }
                else if (label_False != NULL)
                {
                    new_intercode(IN_IFGOTO, op, "==", zero, label_False);
                }
            }
            else if (strcmp(tmp2->name, "DOT") == 0)
            {
                Operand op = ExpZJDM(cur);
                if (label_True != NULL && label_False != NULL)
                {
                    new_intercode(IN_IFGOTO, op, "!=", zero, label_True);
                    new_intercode(IN_GOTO, label_False);
                }
                else if (label_True != NULL)
                {
                    new_intercode(IN_IFGOTO, op, "!=", zero, label_True);
                }
                else if (label_False != NULL)
                {
                    new_intercode(IN_IFGOTO, op, "==", zero, label_False);
                }
            }
        }
        else if (strcmp(tmp1->name, "NOT") == 0)
        {
            Node *ExpNode = GetChild(cur, 1);
            ConditionZJDM(ExpNode, label_False, label_True);
        }
        else if (strcmp(tmp1->name, "MINUS") == 0)
        {
            Operand op = ExpZJDM(cur);
            if (label_True != NULL && label_False != NULL)
            {
                new_intercode(IN_IFGOTO, op, "!=", zero, label_True);
                new_intercode(IN_GOTO, label_False);
            }
            else if (label_True != NULL)
            {
                new_intercode(IN_IFGOTO, op, "!=", zero, label_True);
            }
            else if (label_False != NULL)
            {
                new_intercode(IN_IFGOTO, op, "==", zero, label_False);
            }
        }
        else if (strcmp(tmp1->name, "LP") == 0)
        {
            Node *ExpNode = GetChild(cur, 1);
            ConditionZJDM(ExpNode, label_True, label_False);
        }
        else if (strcmp(tmp1->name, "ID") == 0)
        {
            Operand op = ExpZJDM(cur);
            if (label_True != NULL && label_False != NULL)
            {
                new_intercode(IN_IFGOTO, op, "!=", zero, label_True);
                new_intercode(IN_GOTO, label_False);
            }
            else if (label_True != NULL)
            {
                new_intercode(IN_IFGOTO, op, "!=", zero, label_True);
            }
            else if (label_False != NULL)
            {
                new_intercode(IN_IFGOTO, op, "==", zero, label_False);
            }
        }
        else if (strcmp(tmp1->name, "INT") == 0)
        {
            if (label_True != NULL && tmp1->intvalue)
            {
                new_intercode(IN_GOTO, label_True);
            }
            if (label_False != NULL && !tmp1->intvalue)
            {
                new_intercode(IN_GOTO, label_False);
            }
        }
    }
    return 0;
}

void printop(Operand op)
{
    switch (op->kind)
    {
    case OP_VARIABLE: //如果是变量类型
    {
        if (op->ifaddress == OP_ADDRESS) //地址类型
        {
            printf("&"); //取地址号
        }
        printf("v%d", op->varno); //输出编号
        break;
    }
    case OP_CONSTANT:
    {
        printf("#%d", op->value); //立即数，#开头
        break;
    }
    case OP_FUNCTION:
    {
        printf("%s", op->funcname); //是函数，输出函数名
        break;
    }
    case OP_TEMPVAR:
    {
        if (op->ifaddress == OP_ADDRESS) //如果是地址类型，使用*号当作取对应的值
        {
            printf("*");
        }
        printf("t%d", op->varno);
        break;
    }
    case OP_LABEL:
    {
        printf("%d", op->varno); //输出标签编号
        break;
    }

    default:
        break;
    }
    printf("\n");
}