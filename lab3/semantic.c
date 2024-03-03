#include "semantic.h"

extern struct Dec_Fuc *fuchead;
int Depth = 0;

int StructwithoutOptTagNum = 0;

struct error9 *error9head = NULL;
struct SymbolBucket *GlobalScope = NULL;

struct SymbolNode *CreateSymbolNode(Type type, char *name, int isdef, int depth, int kind)
{
    struct SymbolNode *newnode = (struct SymbolNode *)malloc(sizeof(struct SymbolNode));
    newnode->next = NULL;
    newnode->areanext = NULL;
    newnode->kind = kind;
    newnode->field.type = type;
    strcpy(newnode->field.name, name);
    newnode->depth = depth;
    newnode->isdef = isdef;
    return newnode;
}

Node *GetChild(Node *cur, int num)
{
    Node *tmp = cur;
    tmp = tmp->child;
    if (num == 0)
    {
        return tmp;
    }

    for (int i = 1; i <= num; i++)
    {
        tmp = tmp->sibling;
    }
    return tmp;
}

int CheckSemantic(Node *cur)
{
    Depth = 0;
    // printf("CheckSemantic run!\n");
    ProgramAnalyse(cur);
}

int ProgramAnalyse(Node *cur)
{
    // Program -> ExtDefList
    // printf("Program analyse run!\n");
    GlobalScope = initSymbolBucket();
    Node *ExtDefListNode = GetChild(cur, 0);
    ExtDefListAnalyse(ExtDefListNode);
    CheckFucDef();
    return 0;
}

int ExtDefListAnalyse(Node *cur)
{
    // ExtDefList -> ExtDef ExtDefList | empty
    // printf("ExtDefList analyse run!\n");
    Node *ExtDefNode = GetChild(cur, 0);
    Node *ExtDefListNode = GetChild(cur, 1);
    ExtDefAnalyse(ExtDefNode);
    if (ExtDefListNode != NULL)
    {
        ExtDefListAnalyse(ExtDefListNode);
    }
    return 0;
}

int ExtDefAnalyse(Node *cur)
{
    // printf("ExtDef analyse run!\n");
    Type Nodetype = NULL;
    Node *SpecifierNode = GetChild(cur, 0);
    Node *tmp1 = GetChild(cur, 1);
    Node *tmp2 = GetChild(cur, 2);
    if (SpecifierNode != NULL)
    {
        Nodetype = SpecifierAnalyse(SpecifierNode);
    }
    if (tmp2 == NULL)
    {
        // ExtDef -> Specifier SEMI
    }
    else
    {
        // ExtDef -> Specifier ExtDecList SEMI
        if (tmp1 != NULL && strcmp(tmp1->name, "ExtDecList") == 0)
        {
            int res = ExtDecListAnalyse(tmp1, Nodetype);
        }
        else
        {
            if (strcmp(tmp1->name, "FunDec") != 0)
            {
                printf("ExtDefAnalyse bug!\n");
                assert(0);
            }
            Node *FunDecNode = tmp1;
            // ExtDef -> Specifier FunDec SEMI
            if (strcmp(tmp2->name, "SEMI") == 0)
            {
                struct SymbolBucket *tmpscope = enterscope();
                FunDecAnalyse(FunDecNode, 0, Nodetype, tmpscope);
                exitscope();
            }
            else
            {
                // Sepcifier FunDec CompSt
                struct SymbolBucket *tmpscope = enterscope();
                FunDecAnalyse(FunDecNode, 1, Nodetype, tmpscope);
                Node *CompStNode = tmp2;
                Depth++;
                CompStAnalyse(CompStNode, tmpscope, Nodetype);
                Depth--;
                exitscope();
            }
        }
    }
    return 0;
}

int CompStAnalyse(Node *cur, struct SymbolBucket *scope, Type type)
{
    // printf("CompSt analyse run!\n");
    //  CompSt -> LC DefList StmtList RC
    Node *tmp = GetChild(cur, 1);
    if (strcmp(tmp->name, "DefList") == 0)
    {
        DefListAnalyse(tmp, scope);
        Node *StmtListNode = GetChild(cur, 2);
        if (strcmp(StmtListNode->name, "StmtList") == 0)
        {
            StmtListAnalyse(StmtListNode, scope, type);
        }
    }
    else if (strcmp(tmp->name, "StmtList") == 0)
    {
        Node *StmtListNode = tmp;
        StmtListAnalyse(StmtListNode, scope, type);
    }
    return 0;
}

int StmtListAnalyse(Node *cur, struct SymbolBucket *scope, Type type)
{
    //printf("StmtList analyse run!\n");
    Node *StmtNode = GetChild(cur, 0);
    Node *tmp = GetChild(cur, 1);
    StmtAnalyse(StmtNode, scope, type);
    if (tmp != NULL)
    {
        StmtListAnalyse(tmp, scope, type);
    }
}

int StmtAnalyse(Node *cur, struct SymbolBucket *scope, Type type)
{
    //printf("StmtAnalyse analyse run!\n");
    Node *tmp = GetChild(cur, 0);
    // Stmt -> Exp SEMI
    if (strcmp(tmp->name, "Exp") == 0)
    {
        Type tmptype = ExpAnalyse(tmp);
    }
    // Stmt -> CompSt
    else if (strcmp(tmp->name, "CompSt") == 0)
    {
        Depth++;
        struct SymbolBucket *tmpscope = enterscope();
        CompStAnalyse(tmp, tmpscope, type);
        exitscope();
        Depth--;
    }
    // Stmt -> RETURN Exp SEMI
    else if (strcmp(tmp->name, "RETURN") == 0)
    {
        Node *ExpNode = GetChild(cur, 1);
        Type Returntype = ExpAnalyse(ExpNode);
        if (Returntype != NULL)
        {
            int res = CheckType(type, Returntype);
            if (res == 0)
            {
                errorprint(8, cur->linenum, NULL);
                return -1;
            }
        }
    }
    // Stmt -> IF LP Exp RP Stmt
    //       | IF LP Exp RP Stmt ELSE Stmt
    else if (strcmp(tmp->name, "IF") == 0)
    {
        Node *ExpNode = GetChild(cur, 2);
        Node *elseNode = GetChild(cur, 5);
        Type istype = ExpAnalyse(ExpNode);
        if (istype != NULL)
        {
            if (istype->kind = BASIC && istype->u.basic == 0)
            {
                // right
            }
            else
            {
                errorprint(7, cur->linenum, NULL);
            }
        }
        // IF LP Exp RP Stmt
        if (elseNode == NULL)
        {
            Node *StmtNode = GetChild(cur, 4);
            StmtAnalyse(StmtNode, scope, type);
        }
        else
        {
            Node *StmtNode1 = GetChild(cur, 4);
            Node *StmtNode2 = GetChild(cur, 6);
            StmtAnalyse(StmtNode1, scope, type);
            StmtAnalyse(StmtNode2, scope, type);
        }
    }
    // Stmt -> WHILE LP Exp RP Stmt
    else if (strcmp(tmp->name, "WHILE") == 0)
    {
        Node *ExpNode = GetChild(cur, 2);
        Node *StmtNode = GetChild(cur, 4);
        Type exptype = ExpAnalyse(ExpNode);
        if (exptype != NULL)
        {
            if (exptype->kind == BASIC && exptype->u.basic == 0)
            {
                // right
            }
            else
            {
                errorprint(7, cur->linenum, NULL);
            }
        }
        StmtAnalyse(StmtNode, scope, type);
    }
    return 0;
}

int DefListAnalyse(Node *cur, struct SymbolBucket *scope)
{
    //printf("DefList analyse run!\n");
    // DefList -> Def Deflist  |  empty
    Node *tmp = GetChild(cur, 0);
    if (tmp != NULL)
    {
        Node *DefNode = tmp;
        Node *DefListNode = GetChild(cur, 1);
        DefAnalyse(DefNode, scope);
        if (DefListNode != NULL)
        {
            DefListAnalyse(DefListNode, scope);
        }
    }
    return 0;
}

int DefAnalyse(Node *cur, struct SymbolBucket *scope)
{
    //printf("Def analyse run!\n");
    // Def -> Specifier DecList SEMI
    Node *SpecifierNode = GetChild(cur, 0);
    Node *DecListNode = GetChild(cur, 1);
    Type stype = SpecifierAnalyse(SpecifierNode);
    DecListAnalyse(DecListNode, scope, stype);
    return 0;
}

int DecListAnalyse(Node *cur, struct SymbolBucket *scope, Type type)
{
    //printf("DecList analyse run!\n");
    // DecList -> Dec
    //          | Dec COMMA DecList
    Node *DecNode = GetChild(cur, 0);
    DecAnalyse(DecNode, scope, type);
    Node *tmp = GetChild(cur, 1);
    if (tmp != NULL)
    {
        Node *DecListNode = GetChild(cur, 2);
        if (DecListNode != NULL)
        {
            DecListAnalyse(DecListNode, scope, type);
        }
    }
    return 0;
}

int DecAnalyse(Node *cur, struct SymbolBucket *scope, Type type)
{
    // printf("Dec analyse run!\n");
    //  printf("DecAnalyse run! turn = %d\n",debug++);
    /* Dec -> VarDec
            | VarDec ASSIGNOP Exp
    */
    Node *VarDecNode = GetChild(cur, 0);
    FieldList tmp = VarDecAnalyse(VarDecNode, type);
    Node *tmpnode = GetChild(cur, 1);
    if (tmpnode == NULL)
    {
        // Dec -> VarDec
        int res = querySymbolname(tmp->name, Depth);
        Type queryType = (Type)malloc(sizeof(struct Type_));
        int queryisdef;
        int querykind;
        int res2 = querySymbol2(&queryType, tmp->name, &queryisdef, Depth, &querykind);
        if (res == 0)
        {
            errorprint(3, cur->linenum, tmp->name);
        }
        else if (res2 == 0 && queryType->kind == STRUCTURE && querykind == STRUCT_NAME)
        {
            errorprint(3, cur->linenum, tmp->name);
        }
        else
        {
            struct SymbolNode *insertNode = CreateSymbolNode(tmp->type, tmp->name, 1, Depth, VARIABLE_NAME);
            InsertSymbolNode(insertNode, scope);
        }
    }
    else
    {
        // Dec -> VarDec ASSIGNOP Exp
        int res = querySymbolname(tmp->name, Depth);
        Type queryType = (Type)malloc(sizeof(struct Type_));
        int queryisdef;
        int querykind;
        int res2 = querySymbol2(&queryType, tmp->name, &queryisdef, Depth, &querykind);
        if (res == 0)
        {
            errorprint(3, cur->linenum, tmp->name);
        }
        else
        {
            struct SymbolNode *insertNode = CreateSymbolNode(tmp->type, tmp->name, 1, Depth, VARIABLE_NAME);
            InsertSymbolNode(insertNode, scope);

            Node *ExpNode = GetChild(cur, 2);
            Type exptype = ExpAnalyse(ExpNode);

            if (exptype != NULL)
            {
                int res3 = CheckType(tmp->type, exptype);
                if (res3 == 0)
                {
                    errorprint(5, cur->linenum, NULL);
                }
                else if (res2 == 0 && queryType->kind == STRUCTURE && querykind == STRUCT_NAME)
                {
                    errorprint(3, cur->linenum, NULL);
                }
            }
        }
    }
    return 0;
}

Type ExpAnalyse(Node *cur)
{
    if (cur == NULL)
    {
        return NULL;
    }
    Type ans = NULL;
    Node *tmp1 = GetChild(cur, 0);
    Node *tmp2 = GetChild(cur, 1);
    // error 6 judgement
    if (strcmp(tmp1->name, "Exp") == 0)
    {
        // Exp -> Exp ASSIGNOP Exp
        if (tmp2 != NULL && strcmp(tmp2->name, "ASSIGNOP") == 0)
        {
            Node *tmp11 = GetChild(tmp1, 0);
            Node *tmp12 = GetChild(tmp1, 1);
            if (tmp12 == NULL)
            {
                if (strcmp(tmp11->name, "ID") != 0)
                {
                    errorprint(6, cur->linenum, NULL);
                    return NULL;
                }
            }
            else
            {
                Node *tmp13 = GetChild(tmp1, 2);
                if (tmp13 != NULL)
                {
                    Node *tmp14 = GetChild(tmp1, 3);
                    // 3
                    if (tmp14 == NULL)
                    {
                        if (strcmp(tmp11->name, "Exp") == 0 && strcmp(tmp12->name, "DOT") == 0 && strcmp(tmp13->name, "ID") == 0)
                        {
                        }
                        else
                        {
                            errorprint(6, cur->linenum, NULL);
                            return NULL;
                        }
                    }
                    // 4
                    else
                    {
                        if (strcmp(tmp11->name, "Exp") == 0 && strcmp(tmp12->name, "LB") == 0 && strcmp(tmp13->name, "Exp") == 0 && strcmp(tmp14->name, "RB") == 0)
                        {
                        }
                        else
                        {
                            errorprint(6, cur->linenum, NULL);
                            return NULL;
                        }
                    }
                }
                // 2
                else
                {
                    errorprint(6, cur->linenum, NULL);
                    return NULL;
                }
            }
        }
    }

    if (tmp2 == NULL)
    {
        // only one element : tmp1
        if (strcmp(tmp1->name, "ID") == 0)
        {
            Type type0 = (Type)malloc(sizeof(struct Type_));
            int isdef0;
            // current area
            int res0 = querySymbol(&type0, tmp1->id, &isdef0, Depth, 0);

            Type type1 = (Type)malloc(sizeof(struct Type_));
            int isdef1;
            int kind1;
            // global area
            int res1 = querySymbol2(&type1, tmp1->id, &isdef1, Depth, &kind1);

            if (res0 == 0)
            {
                ans = type0;
                return ans;
            }
            else
            {
                if (res1 != 0 || (res1 == 0 && kind1 != VARIABLE_NAME))
                {
                    errorprint(1, cur->linenum, tmp1->id);
                    return NULL;
                }
                else
                {
                    ans = type1;
                    return ans;
                }
            }
        }
        else if (strcmp(tmp1->name, "INT") == 0)
        {
            ans = (Type)malloc(sizeof(struct Type_));
            ans->kind = BASIC;
            ans->u.basic = 0;
            return ans;
        }
        else if (strcmp(tmp1->name, "FLOAT") == 0)
        {
            ans = (Type)malloc(sizeof(struct Type_));
            ans->kind = BASIC;
            ans->u.basic = 1;
            return ans;
        }
    }
    else
    {
        Node *tmp3 = GetChild(cur, 2);
        if (tmp3 != NULL)
        {
            Node *tmp4 = GetChild(cur, 3);
            if (tmp4 == NULL && strcmp(tmp3->name, "Exp") == 0 && strcmp(tmp2->name, "LB") != 0)
            {
                Node *ExpNode1 = tmp1;
                Node *ExpNode2 = tmp3;
                Type exp1type = ExpAnalyse(ExpNode1);
                Type exp2type = ExpAnalyse(ExpNode2);

                if (exp1type != NULL && exp2type != NULL)
                {
                    int check = CheckType(exp1type, exp2type);
                    if (check == 0 && strcmp(tmp2->name, "ASSIGNOP") == 0)
                    {
                        errorprint(5, cur->linenum, NULL);
                        return NULL;
                    }

                    if (check == 0)
                    {
                        errorprint(7, cur->linenum, NULL);
                        return NULL;
                    }
                    else
                    {
                        ans = exp1type;
                        return ans;
                    }
                }
                else
                {
                    return NULL;
                }
            }
        }

        if (strcmp(tmp1->name, "LP") == 0 || strcmp(tmp1->name, "MINUS") == 0 || strcmp(tmp1->name, "NOT") == 0)
        {
            Node *expNode = tmp2;
            Type exptype = ExpAnalyse(expNode);
            ans = exptype;
            return ans;
        }
        /*
        | ID LP Args RP
        | ID LP RP
        | Exp LB exp RB
        | Exp DOT ID
        */
        if (strcmp(tmp1->name, "ID") == 0)
        {
            char *fuctionname = tmp1->id;
            // printf("fuctionname:%s\n", fuctionname);
            Type fuctype = (Type)malloc(sizeof(struct Type_));
            int fucisdef = -1;
            int fucres = querySymbol(&fuctype, fuctionname, &fucisdef, Depth, 1);
            ans = fuctype->u.fuction.ReturnParameter;
            if (fucres == 0)
            {
                if (fuctype->kind != FUNCTION)
                {
                    errorprint(11, cur->linenum, fuctionname);
                    return NULL;
                }
            }
            if (fucres == -1)
            {
                errorprint(2, cur->linenum, fuctionname);
                return NULL;
            }

            if (strcmp(tmp3->name, "Args") == 0)
            {
                if (fuctype->u.fuction.parameters == NULL)
                {
                    char *defss = (char *)malloc(1024);
                    struct error9 *tmp9head = error9head;
                    while (tmp9head != NULL)
                    {
                        //printf("tmp9head:%s\n", tmp9head->nameandtype);
                        if (strcmp(tmp9head->name, fuctionname) == 0)
                        {
                            strcat(defss, tmp9head->nameandtype);
                            break;
                        }
                        tmp9head = tmp9head->next;
                    }
                    //printf("%s\n\n", defss);
                    char *usess = (char *)malloc(1024);
                    strcat(usess, "(");
                    Node *tmptmp3 = tmp3;
                    Node *ttmp2 = GetChild(tmp3, 1);
                    while (GetChild(tmptmp3, 1) != NULL)
                    {
                        Node *ttmp1 = GetChild(tmptmp3, 0);

                        Node *tttmp = GetChild(ttmp1, 0);
                        //printf("name = %d\n", tttmp->intvalue);
                        if (strcmp(tttmp->name, "INT") == 0)
                        {
                            strcat(usess, "int");
                        }
                        else if (strcmp(tttmp->name, "FLOAT") == 0)
                        {
                            strcat(usess, "float");
                        }

                        tmptmp3 = GetChild(tmptmp3, 2);

                        strcat(usess, ",");
                    }
                    // last one
                    Node *ttmp1 = GetChild(tmptmp3, 0);
                    Node *tttmp = GetChild(ttmp1, 0);
                    //printf("name = %d\n", tttmp->intvalue);
                    if (strcmp(tttmp->name, "INT") == 0)
                    {
                        strcat(usess, "int");
                    }
                    else if (strcmp(tttmp->name, "FLOAT") == 0)
                    {
                        strcat(usess, "float");
                    }

                    strcat(usess, ")");
                    //printf("\n\n%s\n\n", usess);
                    errorprint9(cur->linenum, defss, usess);
                    //printf("error 9 No.1\n\n");
                    // errorprint(9, cur->linenum, NULL);
                    return NULL;
                }
                else
                { // none empty
                    int cnt = 0;
                    Node *cntNode = tmp3;
                    while (1)
                    {
                        cnt++;
                        Node *tmpcntNode = GetChild(cntNode, 1);
                        if (tmpcntNode == NULL)
                            break;
                        cntNode = GetChild(cntNode, 2);
                    }

                    if (cnt != fuctype->u.fuction.num)
                    {
                        char *defss = (char *)malloc(1024);
                        struct error9 *tmp9head = error9head;
                        while (tmp9head != NULL)
                        {
                            // printf("tmp9head:%s\n",tmp9head->nameandtype);
                            if (strcmp(tmp9head->name, fuctionname) == 0)
                            {
                                strcat(defss, tmp9head->nameandtype);
                                break;
                            }
                            tmp9head = tmp9head->next;
                        }
                        // printf("%s\n\n",defss);
                        char *usess = (char *)malloc(1024);
                        strcat(usess, "(");
                        Node *tmptmp3 = tmp3;
                        Node *ttmp2 = GetChild(tmp3, 1);
                        while (GetChild(tmptmp3, 1) != NULL)
                        {
                            Node *ttmp1 = GetChild(tmptmp3, 0);

                            Node *tttmp = GetChild(ttmp1, 0);
                            //printf("name = %s\n", tttmp->name);
                            if (strcmp(tttmp->name, "INT") == 0)
                            {
                                strcat(usess, "int");
                            }
                            else if (strcmp(tttmp->name, "FLOAT") == 0)
                            {
                                strcat(usess, "float");
                            }

                            tmptmp3 = GetChild(tmptmp3, 2);

                            strcat(usess, ",");
                        }
                        // last one
                        Node *ttmp1 = GetChild(tmptmp3, 0);
                        Node *tttmp = GetChild(ttmp1, 0);
                        //printf("name = %d\n", tttmp->intvalue);
                        if (strcmp(tttmp->name, "INT") == 0)
                        {
                            strcat(usess, "int");
                        }
                        else if (strcmp(tttmp->name, "FLOAT") == 0)
                        {
                            strcat(usess, "float");
                        }
                        strcat(usess, ")");
                        // printf("\n\n%s\n\n",usess);
                        errorprint9(cur->linenum, defss, usess);
                        printf("error 9 No.2\n\n");
                        // errorprint(9,cur->linenum,NULL);
                        return NULL;
                    }
                    int argres = ArgAnalyse(tmp3, fuctype->u.fuction.parameters, fuctionname);
                    if (argres == 0)
                    {
                        return ans;
                    }
                    else
                    {
                        return NULL;
                    }
                }
            }
            // wu can shu
            else
            {
                if (fuctype->u.fuction.parameters != NULL)
                {
                    char *defss = (char *)malloc(1024);
                    struct error9 *tmp9head = error9head;
                    while (tmp9head != NULL)
                    {
                        //printf("tmp9head:%s\n", tmp9head->nameandtype);
                        if (strcmp(tmp9head->name, fuctionname) == 0)
                        {
                            strcat(defss, tmp9head->nameandtype);
                            break;
                        }
                        tmp9head = tmp9head->next;
                    }
                    //printf("%s\n\n", defss);
                    char *usess = (char *)malloc(1024);
                    strcat(usess, "(");

                    strcat(usess, ")");
                    //printf("\n\n%s\n\n", usess);
                    errorprint9(cur->linenum, defss, usess);
                    //printf("error 9 No.3\n\n");
                    // errorprint(9,cur->linenum,NULL);
                    return NULL;
                }
                else
                {
                    return ans;
                }
            }
        }

        else
        {
            Node *tmp4 = GetChild(cur, 3);
            if (tmp4 == NULL)
            {
                // struct
                if (strcmp(tmp1->name, "Exp") == 0 && strcmp(tmp2->name, "DOT") == 0 && strcmp(tmp3->name, "ID") == 0)
                {
                    Type exptype = ExpAnalyse(tmp1);
                    if (exptype != NULL)
                    {
                        if (exptype->kind != STRUCTURE)
                        {
                            errorprint(13, cur->linenum, NULL);
                            return NULL;
                        }
                        else
                        {
                            // yu!
                            char *tmpid = tmp3->id;
                            char *filedname = (char *)malloc(sizeof(char) * (strlen(tmpid) + strlen(exptype->u.structure_.name) + 1));
                            strcpy(filedname, tmpid);
                            strcat(filedname, exptype->u.structure_.name);
                            Type structtype = (Type)malloc(sizeof(struct Type_));
                            int res = queryStruct(&structtype, filedname);
                            // success
                            if (res == 0)
                            {
                                ans = structtype;
                                return ans;
                            }
                            else
                            {
                                errorprint(14, cur->linenum, tmpid);
                                return NULL;
                            }
                        }
                    }
                    else
                    {
                        return NULL;
                    }
                }
            }
            else
            {
                // Exp ->  Exp LB Exp RB
                if (strcmp(tmp1->name, "Exp") == 0 && strcmp(tmp2->name, "LB") == 0 && strcmp(tmp3->name, "Exp") == 0)
                {
                    Type type1 = ExpAnalyse(tmp1);
                    Type type3 = ExpAnalyse(tmp3);
                    if (type1 == NULL || type3 == NULL)
                    {
                        return NULL;
                    }
                    if (type1->kind != ARRAY)
                    {
                        Node *tt = GetChild(tmp1, 0);
                        if (strcmp(tt->name, "ID") == 0)
                            errorprint(10, cur->linenum, tt->id);
                        return NULL;
                    }
                    else
                    {
                        if (type3->kind == BASIC && type3->u.basic == 0)
                        {
                        }
                        else
                        {
                            Node *tmppp = GetChild(tmp3, 0);
                            errorprint12(cur->linenum, tmppp->floatvalue);
                            return NULL;
                        }
                    }
                    ans = type1->u.array_.elem;
                    return ans;
                }
            }
        }
    }
    return NULL;
}

int ArgAnalyse(Node *cur, FieldList params, char *functionname)
{
    //printf("Args analyse run!\n");
    Node *ExpNode = GetChild(cur, 0);
    Node *tmpNode = GetChild(cur, 1);
    // if (params == NULL){
    //     errorprint(9,cur->linenum,NULL);

    //     return -1;
    // }
    Type tmptype = ExpAnalyse(ExpNode);
    if (tmptype != NULL)
    {
        if (params->type == NULL)
        {
            errorprint(9, cur->linenum, NULL);
            printf("error 9 No.4\n\n");
            // return -1;
        }
        else
        {
            int check = CheckType(tmptype, params->type);
            if (check == 0)
            {
                Node *tmp3 = cur;
                char *defss = (char *)malloc(1024);
                struct error9 *tmp9head = error9head;
                while (tmp9head != NULL)
                {
                    //printf("tmp9head:%s\n", tmp9head->nameandtype);
                    if (strcmp(tmp9head->name, functionname) == 0)
                    {
                        strcat(defss, tmp9head->nameandtype);
                        break;
                    }
                    tmp9head = tmp9head->next;
                }
                //printf("%s\n\n", defss);
                char *usess = (char *)malloc(1024);
                strcat(usess, "(");
                Node *tmptmp3 = tmp3;

                while (GetChild(tmptmp3, 1) != NULL)
                {
                    Node *ttmp1 = GetChild(tmptmp3, 0);
                    //printf("name = %s\n", ttmp1->name);
                    Node *tttmp = GetChild(ttmp1, 0);
                    //printf("name = %s\n", tttmp->name);
                    if (strcmp(tttmp->name, "INT") == 0)
                    {
                        strcat(usess, "int");
                    }
                    else if (strcmp(tttmp->name, "FLOAT") == 0)
                    {
                        strcat(usess, "float");
                    }

                    tmptmp3 = GetChild(tmptmp3, 2);

                    strcat(usess, ",");
                }
                // last one
                Node *ttmp1 = GetChild(tmptmp3, 0);
                Node *tttmp = GetChild(ttmp1, 0);
                //printf("name = %d\n", tttmp->intvalue);
                if (strcmp(tttmp->name, "INT") == 0)
                {
                    strcat(usess, "int");
                }
                else if (strcmp(tttmp->name, "FLOAT") == 0)
                {
                    strcat(usess, "float");
                }
                strcat(usess, ")");
                //printf("\n\n%s\n\n", usess);
                errorprint9(cur->linenum, defss, usess);
                printf("error 9 No.5\n\n");
                // errorprint(9, cur->linenum, NULL);
                return -1;
            }
        }
    }
    if (tmpNode != NULL)
    {
        if (params->tail != NULL)
        {
            Node *tmp3 = cur;
            char *defss = (char *)malloc(1024);
            struct error9 *tmp9head = error9head;
            while (tmp9head != NULL)
            {
                //printf("tmp9head:%s\n", tmp9head->nameandtype);
                if (strcmp(tmp9head->name, functionname) == 0)
                {
                    strcat(defss, tmp9head->nameandtype);
                    break;
                }
                tmp9head = tmp9head->next;
            }
            //printf("%s\n\n", defss);
            char *usess = (char *)malloc(1024);
            strcat(usess, "(");
            Node *tmptmp3 = tmp3;

            while (GetChild(tmptmp3, 1) != NULL)
            {
                Node *ttmp1 = GetChild(tmptmp3, 0);
                //printf("name = %s\n", ttmp1->name);
                Node *tttmp = GetChild(ttmp1, 0);
                //printf("name = %s\n", tttmp->name);
                if (strcmp(tttmp->name, "INT") == 0)
                {
                    strcat(usess, "int");
                }
                else if (strcmp(tttmp->name, "FLOAT") == 0)
                {
                    strcat(usess, "float");
                }

                tmptmp3 = GetChild(tmptmp3, 2);

                strcat(usess, ",");
            }
            // last one
            Node *ttmp1 = GetChild(tmptmp3, 0);
            Node *tttmp = GetChild(ttmp1, 0);
            //printf("name = %d\n", tttmp->intvalue);
            if (strcmp(tttmp->name, "INT") == 0)
            {
                strcat(usess, "int");
            }
            else if (strcmp(tttmp->name, "FLOAT") == 0)
            {
                strcat(usess, "float");
            }
            strcat(usess, ")");
            //printf("\n\n%s\n\n", usess);
            errorprint9(cur->linenum, defss, usess);
            // errorprint(9,cur->linenum,NULL);
            printf("error 9 No.6\n\n");
            return -1;
        }
        else
        {
            Node *ArgsNode = GetChild(cur, 2);
            return ArgAnalyse(ArgsNode, params->tail, functionname);
        }
    }
    return 0;
}

void FunDecAnalyse(Node *cur, const int isdef, const Type type, struct SymbolBucket *scope)
{
    //printf("FunDec analyse run!\n");
    // if not def, insert the def;
    Type tmptype = NULL;
    int tmpisdef;
    Node *IDNODE = GetChild(cur, 0);

    char *funcname = IDNODE->id;

    int res = querySymbol(&tmptype, funcname, &tmpisdef, Depth, 0);
    int flag = 0;

    Node *tmpNode = GetChild(cur, 2);
    Type functiontype = (Type)malloc(sizeof(struct Type_));
    FieldList params = NULL;
    if (strcmp(tmpNode->name, "VarList") != 0)
    {
        functiontype->u.fuction.parameters = NULL;
        functiontype->u.fuction.num = 0;
        char *ss = (char *)malloc(1024);
        strcat(ss, funcname);
        strcat(ss, "(");
        strcat(ss, ")");
        //printf("%s\n\n\n", ss);
        if (error9head == NULL)
        {
            struct error9 *tmp = (struct error9 *)malloc(sizeof(struct error9));
            tmp->name = funcname;
            tmp->nameandtype = ss;
            tmp->next = NULL;
            error9head = tmp;
        }
        else
        {
            struct error9 *tmp = (struct error9 *)malloc(sizeof(struct error9));
            tmp->name = funcname;
            tmp->nameandtype = ss;
            tmp->next = error9head;
            error9head = tmp;
        }
    }
    else
    {
        Node *VarListNode = tmpNode;
        Depth++;
        params = VarListAnalyse(VarListNode, scope);
        // try
        char *ss = (char *)malloc(1024);
        strcat(ss, funcname);
        strcat(ss, "(");
        FieldList tmpparams = params;
        while (tmpparams != NULL)
        {
            if (tmpparams->type->u.basic == 0)
            {
                strcat(ss, "int");
            }
            else if (tmpparams->type->u.basic == 1)
            {
                strcat(ss, "float");
            }
            if (tmpparams->tail != NULL)
            {
                strcat(ss, ",");
            }
            tmpparams = tmpparams->tail;
        }
        strcat(ss, ")");
        //printf("%s\n\n\n", ss);
        if (error9head == NULL)
        {
            struct error9 *tmp = (struct error9 *)malloc(sizeof(struct error9));
            tmp->name = funcname;
            tmp->nameandtype = ss;
            tmp->next = NULL;
            error9head = tmp;
        }
        else
        {
            struct error9 *tmp = (struct error9 *)malloc(sizeof(struct error9));
            tmp->name = funcname;
            tmp->nameandtype = ss;
            tmp->next = error9head;
            error9head = tmp;
        }
        // while (error9head != NULL){
        //     printf("%s\n",error9head->nameandtype);
        //     error9head = error9head->next;
        // }
        // end
        Depth--;
        int cnt = 0;
        FieldList tmpfield = params;
        while (tmpfield != NULL)
        {
            cnt++;
            tmpfield = tmpfield->tail;
        }
        functiontype->u.fuction.num = cnt;
        functiontype->u.fuction.parameters = params;
    }
    functiontype->kind = FUNCTION;
    functiontype->u.fuction.ReturnParameter = type;
    // check
    if (res == 0)
    {
        flag = 1;
        if (isdef == 1)
        {
            if (tmpisdef == 1)
            {
                errorprint(4, cur->linenum, IDNODE->id);
                flag = 2;
            }
            else if (CheckType(tmptype, functiontype) == 0)
            {
                errorprint(19, cur->linenum, IDNODE->id);
                flag = 3;
            }
            else
            {
                struct SymbolNode *insertNode = CreateSymbolNode(functiontype, funcname, isdef, Depth, FUNCTION_NAME);
                InsertSymbolNode(insertNode, GlobalScope);
            }
        }
        else
        {
            if (CheckType(functiontype, tmptype) == 0)
            {
                errorprint(19, cur->linenum, IDNODE->id);
                flag = 4;
            }
        }
    }
    // not found
    else
    {
        struct SymbolNode *insertNode = CreateSymbolNode(functiontype, funcname, isdef, Depth, FUNCTION_NAME);
        InsertSymbolNode(insertNode, GlobalScope);
        // char *ss = (char*)malloc(sizeof(char)*1024);
        // ss = queryfucdef("func",params);
        // printf("%s\n",ss);
        // no def, push it;
        if (isdef == 0)
        {

            PushDecFuc(funcname, cur->linenum);
        }
    }
}

FieldList VarListAnalyse(Node *cur, struct SymbolBucket *scope)
{
    //printf("VarList analyse run!\n");
    Node *ParamDecNode = GetChild(cur, 0);
    FieldList res = ParamDecAnalyse(ParamDecNode);
    Type tmptype1 = (Type)malloc(sizeof(struct Type_));
    int tmpisdef1;
    int res2 = querySymbol(&tmptype1, res->name, &tmpisdef1, 0, 0);

    if (res2 == 0 && tmptype1 != NULL && tmptype1->kind == STRUCTURE)
    {
        errorprint(3, cur->linenum, res->name);
    }

    struct SymbolNode *insertNode = CreateSymbolNode(res->type, res->name, 1, Depth, VARIABLE_NAME);
    InsertSymbolNode(insertNode, scope);

    FieldList tmpfield = res;
    Node *tmpNode = cur;
    while (1)
    {
        if (GetChild(tmpNode, 1) == NULL)
        {
            break;
        }
        tmpNode = GetChild(tmpNode, 2);
        Node *tmpParamNode = GetChild(tmpNode, 0);
        FieldList tmpf = ParamDecAnalyse(tmpParamNode);

        Type qtype = (Type)malloc(sizeof(struct Type_));
        int qisdef;
        int ress = querySymbol(&qtype, tmpf->name, &qisdef, 0, 0);
        if (ress == 0 && qtype != NULL && qtype->kind == STRUCTURE)
        {
            errorprint(3, cur->linenum, tmpf->name);
        }

        insertNode = CreateSymbolNode(tmpf->type, tmpf->name, 1, Depth, VARIABLE_NAME);
        InsertSymbolNode(insertNode, scope);
        tmpfield->tail = tmpf;
        tmpfield = tmpfield->tail;
    }
    tmpfield->tail = NULL;
    return res;
}

FieldList ParamDecAnalyse(Node *cur)
{
    //printf("ParamDec analyse run!\n");
    Node *SpecifierNode = GetChild(cur, 0);
    Node *VarDecNode = GetChild(cur, 1);
    Type nodetype = SpecifierAnalyse(SpecifierNode);
    FieldList res = VarDecAnalyse(VarDecNode, nodetype);
    return res;
}

Type SpecifierAnalyse(Node *cur)
{
    //printf("Specifier analyse run!\n");
    Type type = (Type)malloc(sizeof(struct Type_));
    Node *tmp0 = GetChild(cur, 0);
    if (strcmp(tmp0->name, "TYPE") == 0)
    {
        type->kind = BASIC;
        if (strcmp(tmp0->id, "int") == 0)
        {
            type->u.basic = 0;
        }
        else if (strcmp(tmp0->id, "float") == 0)
        {
            type->u.basic = 1;
        }
    }
    else if (strcmp(tmp0->name, "StructSpecifier") == 0)
    {
        type->kind = STRUCTURE;
        Node *StructNode = GetChild(tmp0, 0);
        Node *tmp1 = GetChild(tmp0, 1);

        if (strcmp(tmp1->name, "OptTag") == 0)
        {
            /*
            OptTag -> ID | empty
            */
            Node *tmp2 = GetChild(tmp1, 0);
            if (tmp2 == NULL)
            {
            }
            else if (strcmp(tmp2->name, "ID") == 0)
            {
                char *structName = tmp2->id;
                Type tmptype = (Type)malloc(sizeof(struct Type_));
                int tmpisdef;
                int res = querySymbol(&tmptype, structName, &tmpisdef, Depth, 1);
                if (res == 0)
                {
                    errorprint(16, tmp1->linenum, structName);
                    return NULL;
                }
                else
                {
                    // put it in struct table
                    type->u.structure_.name = (char *)malloc(sizeof(char) * 32);
                    strcpy(type->u.structure_.name, structName);
                    Node *DefListNode = GetChild(tmp0, 3);
                    if (strcmp(DefListNode->name, "DefList") != 0)
                    {
                        // empty
                        type->u.structure_.structure = NULL;
                    }
                    else
                    {
                        Node *tmpDefListNode = DefListNode;
                        FieldList res = NULL;
                        FieldList tmpf = NULL;
                        while (1)
                        {
                            Node *tmpDefNode = GetChild(tmpDefListNode, 0);
                            if (tmpDefNode == NULL)
                            {
                                break;
                            }
                            FieldList tmpDefField = DefStruct(tmpDefNode, structName);
                            if (res == NULL)
                            {
                                res = tmpDefField;
                                tmpf = res;
                            }
                            else
                            {
                                tmpf->tail = tmpDefField;
                                tmpf = tmpf->tail;
                            }
                            tmpDefListNode = GetChild(tmpDefListNode, 1);
                            if (tmpDefListNode == NULL)
                                break;
                        }
                        type->u.structure_.structure = res;
                    }
                }
                struct SymbolNode *insertNode = CreateSymbolNode(type, structName, 1, Depth, STRUCT_NAME);
                InsertSymbolNode(insertNode, GlobalScope);
            }
        }
        else if (strcmp(tmp1->name, "Tag") == 0)
        {
            // check
            Node *IDNode = GetChild(tmp1, 0);
            char *tmpidname = IDNode->id;
            Type tmptype = NULL;
            int tmpisdef;
            int tmpres = querySymbol(&tmptype, tmpidname, &tmpisdef, Depth, 1);

            if (tmpres != 0)
            {
                errorprint(17, tmp1->linenum, tmpidname);
                return NULL;
            }
            else if (tmptype == NULL || tmptype->kind != STRUCTURE)
            {
                errorprint(17, tmp1->linenum, tmpidname);
                return NULL;
            }
            else
            {
                return tmptype;
            }
        }
        else if (strcmp(tmp1->name, "LC") == 0)
        {
            StructwithoutOptTagNum++;
            char *structName = (char *)malloc(33);
            sprintf(structName, "No name %d", StructwithoutOptTagNum);
            type->u.structure_.name = (char *)malloc(sizeof(char) * 32);
            strcpy(type->u.structure_.name, structName);
            Node *DefListNode = GetChild(tmp0, 2);
            if (strcmp(DefListNode->name, "DefList") != 0)
            {
                // empty yu!
                type->u.structure_.structure = NULL;
            }
            else
            {
                /* DefList analyse
                   DefList -> Def DefList | empty

                */
                Node *tmpdeflistNode = DefListNode;
                FieldList field = NULL;
                FieldList tmpfield = NULL;
                while (1)
                {
                    Node *tmpdefNode = GetChild(tmpdeflistNode, 0);
                    if (tmpdefNode == NULL)
                    {
                        break;
                    }
                    FieldList tmpdeffield = DefStruct(tmpdefNode, structName);
                    if (field == NULL)
                    {
                        field = tmpdeffield;
                        tmpfield = field;
                    }
                    else
                    {
                        tmpfield->tail = tmpdeffield;
                        tmpfield = tmpfield->tail;
                    }
                    tmpdeflistNode = GetChild(tmpdeflistNode, 1);
                    if (tmpdeflistNode == NULL)
                    {
                        break;
                    }
                }
                type->u.structure_.structure = field;
            }
        }
        else
        {
            
        }
    }
    else
    {
       
    }

    return type;
}

FieldList DefStruct(Node *cur, char *structname)
{
    /*
    Def -> Specifier DecList SEMI
    */
    //printf("DefStruct run!\n");
    Node *SpecifierNode = GetChild(cur, 0);
    Type sptype = SpecifierAnalyse(SpecifierNode);

    Node *DecListNode = GetChild(cur, 1);
    Node *tmpdeclist = DecListNode;
    FieldList field = NULL;
    FieldList tmpfield = NULL;
    /* DecList -> Dec
                | Dec COMMA DecList
    */

    while (GetChild(tmpdeclist, 1) != NULL)
    {
        Node *DecNode = GetChild(tmpdeclist, 0);
        FieldList tmpdecf = DecStruct(DecNode, sptype);
        char *DecName = (char *)malloc(strlen(structname) + strlen(tmpdecf->name) + 1);
        strcpy(DecName, tmpdecf->name);
        strcat(DecName, structname);
        if (queryStructname(DecName) == 0)
        {
            errorprint(15, DecNode->linenum, tmpdecf->name);
        }
        else
        {
            InsertStruct(tmpdecf->type, DecName);
        }
        if (field == NULL)
        {
            field = tmpdecf;
            tmpfield = field;
        }
        else
        {
            tmpfield->tail = tmpdecf;
            tmpfield = tmpfield->tail;
        }
        tmpdeclist = GetChild(tmpdeclist, 2);
    }
    // the last Dec
    Node *DecNode = GetChild(tmpdeclist, 0);
    FieldList tmpdecf = DecStruct(DecNode, sptype);
    char *DecName = (char *)malloc(strlen(structname) + strlen(tmpdecf->name) + 1);
    strcpy(DecName, tmpdecf->name);
    strcat(DecName, structname);
    if (queryStructname(DecName) == 0)
    {
        errorprint(15, DecNode->linenum, tmpdecf->name);
    }
    else
    {
        InsertStruct(tmpdecf->type, DecName);
    }
    if (field == NULL)
    {
        field = tmpdecf;
        tmpfield = field;
    }
    else
    {
        tmpfield->tail = tmpdecf;
        tmpfield = tmpfield->tail;
    }
    return field;
}

FieldList DecStruct(Node *cur, Type type)
{
    //printf("DecStruct run!\n");
    FieldList field = NULL;
    Node *VarDecNode = GetChild(cur, 0);
    Node *tmpNode = GetChild(cur, 1);
    field = VarDecAnalyse(VarDecNode, type);
    if (tmpNode != NULL)
    {
        errorprint(15, cur->linenum, field->name);
    }
    return field;
}

FieldList VarDecAnalyse(Node *cur, Type type)
{
    //printf("VarDec analyse run!\n");
    FieldList field = (FieldList)malloc(sizeof(struct FieldList_));
    field->tail = NULL;
    Node *tmpNode = GetChild(cur, 0);
    /*  VarDec -> ID    */
    if (strcmp(tmpNode->name, "ID") == 0)
    {
        field->type = type;
        strcpy(field->name, tmpNode->id);
        return field;
    }
    else
    {
        /*
            VarDec -> VarDec LB INT RB
        */
        int cnt = 0;
        while (tmpNode->child != NULL)
        {
            tmpNode = tmpNode->child;
            cnt += 1;
        }
        struct Type_ **typelist = (struct Type_ **)malloc(sizeof(struct Type_ **) * (cnt + 2));
        
        strcpy(field->name, tmpNode->id);

        tmpNode = GetChild(cur, 0);
        // Array
        Node *intNode = NULL;
        Type tmptype = NULL;
        cnt -= 1;
        int Maxcnt = cnt;
        while (tmpNode->child != NULL)
        {
            Type curtype = (Type)malloc(sizeof(struct Type_));
            intNode = tmpNode->sibling->sibling;
            curtype->kind = ARRAY;
            curtype->u.array_.size = intNode->intvalue;
            typelist[cnt] = curtype;
            cnt -= 1;
            tmpNode = tmpNode->child;
        }
        tmptype = typelist[0];
        Type tmptype2 = tmptype;
        typelist[Maxcnt]->u.array_.elem = type;
        for (int i = 1; i <= Maxcnt; i++)
        {
            tmptype2->u.array_.elem = typelist[i];
            tmptype2 = tmptype2->u.array_.elem;
        }
        field->type = tmptype;

        return field;
    }
}

int ExtDecListAnalyse(Node *cur, Type type)
{
    /*
        ExtDecList -> VarDec
                    | VarDec COMMA ExtDecList
    */
    //printf("ExtDecList analyse run!\n");
    Node *VarDecNode = GetChild(cur, 0);
    FieldList vardecf = VarDecAnalyse(VarDecNode, type);
    if (querySymbolname(vardecf->name, Depth) == 0)
    {
        errorprint(3, cur->linenum, vardecf->name);
    }

    struct SymbolNode *insertNode = CreateSymbolNode(vardecf->type, vardecf->name, 1, Depth, VARIABLE_NAME);
    InsertSymbolNode(insertNode, GlobalScope);

    Node *tmpNode = GetChild(cur, 1);
    if (tmpNode == NULL)
    {
        return 0;
    }
    else
    {
        Node *ExtDecListNode = GetChild(cur, 2);
        if (ExtDecListNode != NULL)
        {
            ExtDecListAnalyse(ExtDecListNode, type);
        }
    }
    return 0;
}

void errorprint(int type, int linenum, char *content)
{
    printf("Error type %d at Line %d: ", type, linenum);
    switch (type)
    {
    case 1:
        printf("Undefined variable \"%s\".\n", content);
        break;
    case 2:
        printf("Undefined function \"%s\".\n", content);
        break;
    case 3:
        printf("Redefined variable \"%s\".\n", content);
        break;
    case 4:
        printf("Redefined function \"%s\".\n", content);
        break;
    case 5:
        printf("Type mismatched for assigment.\n");
        break;
    case 6:
        printf("The left-hand side of an assignment must be a variable.\n");
        break;
    case 7:
        printf("Type mismatched for operands.\n");
        break;
    case 8:
        printf("Type mismatched for return.\n");
        break;
    case 9:
        printf("Function is not applicable for arguments.\n");
        break;
    case 10:
        printf("\"%s\" is not an array.\n", content);
        break;
    case 11:
        printf("\"%s\" is not a function.\n", content);
        break;
    case 13:
        printf("Illegal use of \".\".\n");
        break;
    case 14:
        printf("Non-existent field \"%s\".\n", content);
        break;
    case 15:
        printf("Redefined field \"%s\".\n", content);
        break;
    case 16:
        printf("Duplicated name \"%s\".\n", content);
        break;
    case 17:
        printf("Undefined structure \"%s\".\n", content);
        break;
    case 18:
        printf("Undefined function \"%s\".\n", content);
        break;
    case 19:
        printf("Inconsistent declaration of function \"%s\".\n", content);
        break;
    default:
        printf("Other semantic type \"%s\" that we didn't consider.\n", content);
        break;
    }
}

void errorprint12(int linenum, float b)
{
    printf("Error type 12 at Line %d: ", linenum);
    printf("%lf is not an integer.\n", b);
}

void errorprint9(int linenum, char *s1, char *s2)
{
    printf("Error type 9 at Line %d: ", linenum);
    printf("Function \"%s\" is not applicable for arguments \"%s\"\n", s1, s2);
}
