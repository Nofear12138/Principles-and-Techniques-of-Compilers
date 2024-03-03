#include "SymbolTable.h"

#define Length 0x3fff                   //符号表长度
struct SymbolBucket GlobalHead[Length]; //全局符号表
struct SymbolBucket StructHead[Length]; //结构体符号表

struct SymbolBucket *scopehead = NULL; //作用域控制链表
struct Dec_Fuc *fuchead = NULL;        //函数定义的链表
extern struct SymbolBucket* GlobalScope;

unsigned hashcode(char *name) // hash
{
    unsigned val = 0, i;
    for (; *name; ++name)
    {
        val = (val << 2) + *name;
        if (i = val & ~0x3fff)
        {
            val = (val ^ (i >> 12)) & 0x3fff;
        }
    }
    return val;
}

void Insert(Type type, char *name, int isdef, int depth)
{
    int val = hashcode(name);
    if (GlobalHead[val].head == NULL)
    {
        struct SymbolNode *Node = (struct SymbolNode *)malloc(sizeof(struct SymbolNode));
        Node->field.type = type;
        Node->next = NULL;
        // Node->field.name = strdup(name);
        strcpy(Node->field.name, name);
        Node->depth = depth;
        Node->isdef = isdef;
        GlobalHead[val].head = Node;
    }
    else
    {
        struct SymbolNode *tmphead = GlobalHead[val].head;
        struct SymbolNode *Node = (struct SymbolNode *)malloc(sizeof(struct SymbolNode));
        Node->field.type = type;
        Node->next = tmphead;
        // Node->field.name = strdup(name);
        strcpy(Node->field.name, name);
        Node->depth = depth;
        Node->isdef = isdef;
        GlobalHead[val].head = Node;
    }
}

void InsertSymbolNode(struct SymbolNode *Node, struct SymbolBucket *bucket)
{
    Node->var_no = -1;
    char *Name = Node->field.name;
    if (Node->areanext != NULL || Node->next != NULL)
    {
        printf("Node->areanext/next should be NULL!\n");
        assert(0);
    }
    int val = hashcode(Name);
    if (bucket == NULL)
    {
        printf("InsertSymbolNode:bucket is NULL!\n");
        assert(0);
    }
    else
    {
        struct SymbolNode *Tail = bucket->head;
        if (Tail == NULL)
        {
            bucket->head = Node;
        }
        else
        {
            while (Tail->areanext != NULL)
            {
                Tail = Tail->areanext;
            }
            Tail->areanext = Node;
        }
        if (GlobalHead[val].head == NULL)
        {
            GlobalHead[val].head = Node;
        }
        else
        {
            struct SymbolNode *tmphead = GlobalHead[val].head;
            Node->next = tmphead;
            GlobalHead[val].head = Node;
        }
    }
}

struct SymbolNode *CreateSymbolNodeZJDM(int kind,Type type,char *name, int isdef, int depth){
    struct SymbolNode *Insert = (struct SymbolNode*)malloc(sizeof(struct SymbolNode));
    Insert->next = NULL;
    Insert->areanext = NULL;
    Insert->kind = kind;
    Insert->field.type = type;
    strcpy(Insert->field.name,name);
    Insert->depth = depth;
    Insert->isdef = isdef;
    Insert->var_no = -1;
    return Insert;
}

int InsertStruct(Type type, char *name,int offset,char* belongtostructname)
{
    //zjdm
    //printf("insertstruct offset = %d\n",offset);
    struct SymbolNode* Insert = CreateSymbolNodeZJDM(VARIABLE_NAME,type,name,1,0);
    Insert->offset = offset;
    Insert->belongtostructname = belongtostructname;
    InsertSymbolNode(Insert,GlobalScope);

    int val = hashcode(name);
    if (StructHead[val].head == NULL)
    {
        struct SymbolNode *Node = (struct SymbolNode *)malloc(sizeof(struct SymbolNode));
        Node->field.type = type;
        Node->next = NULL;
        Node->offset = offset;
        Node->belongtostructname = belongtostructname;

        char *structsymbolname = (char *)malloc(strlen(name) + 1);
        strcpy(structsymbolname, name);
        Node->structsymbolname = structsymbolname;
        StructHead[val].head = Node;
    }
    else
    {
        struct SymbolNode *tmphead = StructHead[val].head;
        if (strcmp(tmphead->field.name, name) == 0)
        {
            return -1; //此结构体已经存在
        }
        else
        {
            struct SymbolNode *Node = (struct SymbolNode *)malloc(sizeof(struct SymbolNode));
            Node->field.type = type;
            Node->next = tmphead;
            Node->offset = offset;
            Node->belongtostructname = belongtostructname;
            strcpy(Node->field.name, name);
            char* structsymbolname = (char*)malloc(strlen(name)+1);
            strcpy(structsymbolname,name);
            Node->structsymbolname = structsymbolname;
            StructHead[val].head = Node;
        }
    }
    return 0;
}

int querySymbol(Type *type, char *name, int *isdef, int depth, int area)
{
    int val = hashcode(name);
    if (GlobalHead[val].head == NULL)
    {
        return -1; // none
    }
    else
    {
        struct SymbolNode *tmp = GlobalHead[val].head;
        int flag = 0;
        if (area == 0)
        {
            while (tmp != NULL)
            {
                if (strcmp(tmp->field.name, name) == 0 && tmp->depth == depth)
                {
                    *type = tmp->field.type;
                    *isdef = tmp->isdef;
                    flag = 1;
                    return 0; // success
                }
                tmp = tmp->next;
                if (tmp == NULL)
                {
                    break;
                }
            }
            if (flag == 0)
            {
                return -1; // failure
            }
        }
        else
        {
            while (tmp != NULL)
            {
                if (strcmp(tmp->field.name, name) == 0 && tmp->depth <= depth)
                {
                    *type = tmp->field.type;
                    *isdef = tmp->isdef;
                    flag = 1;
                    return 0; // success
                }
                tmp = tmp->next;
                if (tmp == NULL)
                {
                    break;
                }
            }
            if (flag == 0)
            {
                return -1; // failure
            }
        }
    }
}

int querySymbol2(Type *type, char *name, int *isdef, int depth, int *kind)
{
    int val = hashcode(name);
    if (GlobalHead[val].head == NULL)
    {
        return -1; // none
    }
    else
    {
        struct SymbolNode *tmp = GlobalHead[val].head;
        int flag = 0;
        while (tmp != NULL)
        {
            if (strcmp(tmp->field.name, name) == 0 && tmp->depth <= depth)
            {
                *type = tmp->field.type;
                *isdef = tmp->isdef;
                *kind = tmp->kind;
                flag = 1;
                return 0;
            }
            tmp = tmp->next;
            if (tmp == NULL)
                break;
        }
        if (flag == 0)
        {
            return -1; // failure
        }
    }
}

int queryStruct(Type *type, char *name)
{
    int val = hashcode(name);
    if (StructHead[val].head == NULL)
    {
        return -1;
    }
    else
    {
        struct SymbolNode *tmp = StructHead[val].head;
        int flag = 0;
        while (tmp != NULL)
        {
            if (strcmp(tmp->structsymbolname, name) == 0)
            {
                *type = tmp->field.type;
                flag = 1;
                return 0; // success
            }
            tmp = tmp->next;
            if (tmp == NULL)
                break;
        }
        if (flag == 0)
        {
            return -1;
        }
    }
}

int querySymbolname(char *name, int depth)
{
    Type tmptype = (Type)malloc(sizeof(struct Type_));
    int tmpdef;
    return querySymbol(&tmptype, name, &tmpdef, depth, 0);
}

int queryStructname(char *name)
{
    Type tmptype = (Type)malloc(sizeof(struct Type_));
    return queryStruct(&tmptype, name);
}

struct SymbolBucket *initSymbolBucket()
{
    for (int i = 0; i < Length; i++)
    {
        GlobalHead[i].head == NULL;
        StructHead[i].head == NULL;
    }
    scopehead = (struct SymbolBucket *)malloc(sizeof(struct SymbolBucket));
    scopehead->next = NULL;
    scopehead->head = NULL;
    return scopehead;
}

struct SymbolBucket *enterscope()
{
    struct SymbolBucket *enter = (struct SymbolBucket *)malloc(sizeof(struct SymbolBucket));
    enter->next = NULL;
    enter->head = NULL;
    struct SymbolBucket *tail = scopehead;
    while (tail->next != NULL)
    {
        tail = tail->next;
    }
    tail->next = enter;
    return enter;
}

struct SymbolBucket *exitscope()
{
    return scopehead;
    struct SymbolBucket *exit = scopehead;
    struct SymbolBucket *exitbefore = scopehead;
    while (exit->next != NULL)
    {
        exitbefore = exit;
        exit = exit->next;
    }
    // tmp,notice that
    if (exit == scopehead)
    {
        printf("Cant exit scopehead\n");
        assert(0);
    }
    else if (exit->head != NULL)
    {
        struct SymbolNode *scopetail = exit->head;
        struct SymbolNode *scopetailbefore = scopetail;
        int cnt = 0;
        while (scopetail->areanext != NULL)
        {
            scopetailbefore = scopetail;
            scopetail = scopetail->areanext;
            cnt++;
        }

        struct SymbolNode **scopelist = (struct SymbolNode **)malloc(sizeof(struct SymbolNode **) * (cnt + 2));
        scopetail = exit->head;
        scopetailbefore = scopetail;
        cnt = 0;
        scopelist[cnt] = scopetail;
        // given value
        while (scopetail->areanext != NULL)
        {
            scopelist[cnt] = scopetail;
            scopetailbefore = scopetail;
            scopetail = scopetail->areanext;
            cnt++;
        }
        scopelist[cnt] = scopetail; //退出域里的所有符号节点
        // delete operation 删除符号表内的结点
        for (; cnt >= 0; cnt--)
        {
            int val = hashcode(scopelist[cnt]->field.name);
            if (GlobalHead[val].head == NULL)
            {
                printf("Not find!\n");
                assert(0);
            }
            struct SymbolNode *tmphead = GlobalHead[val].head;
            int flag = 0;
            if (tmphead == scopelist[cnt])
            {
                struct SymbolNode *tmpheadnext = scopelist[cnt]->next;
                GlobalHead[val].head = tmpheadnext;
                free(scopelist[cnt]);
            }
            else
            {
                while (tmphead->next != NULL)
                {
                    if (tmphead->next == scopelist[cnt])
                    {
                        flag = 1;
                        break;
                    }
                    tmphead = tmphead->next;
                }
                if (flag == 0)
                {
                    printf("Not found\n");
                    assert(0);
                }
                struct SymbolNode *tmpheadnext = scopelist[cnt]->next;
                tmphead->next = tmpheadnext;
                free(scopelist[cnt]);
            }
        }
        free(scopelist);
        scopelist = NULL;
    }
    exitbefore->next = NULL; //删除退出域
    free(exit);
    exit = NULL;
    return exitbefore;
}

void ShowGlobalTable()
{
    printf("----------Global Table below-------------\n");
    for (int i = 0; i < Length; i++)
    {
        if (GlobalHead[i].head != NULL)
        {
            printf("i:%d ", i);
            struct SymbolNode *tmphead = GlobalHead[i].head;
            while (tmphead != NULL)
            {
                printf("name: %s, type:%d ; ", tmphead->field.name, tmphead->field.type->kind);
                tmphead = tmphead->next;
            }
            printf("\n");
        }
    }
    printf("----------Global Table above-------------\n");
}

void ShowStructTable()
{
    printf("----------Struct Table below-------------\n");
    for (int i = 0; i < Length; i++)
    {
        if (StructHead[i].head != NULL)
        {
            printf("i:%d ", i);
            struct SymbolNode *tmphead = StructHead[i].head;
            while (tmphead != NULL)
            {
                printf("name: %s, type:%d ; ", tmphead->structsymbolname, tmphead->field.type->kind);
                tmphead = tmphead->next;
            }
            printf("\n");
        }
    }
    printf("----------Struct Table above-------------\n");
}

void ShowScopeTable()
{
    printf("----------Scope Table below-------------\n");
    struct SymbolBucket *tmp = scopehead;
    int cnt = 0;
    while (tmp != NULL)
    {
        printf("cnt: %d ", cnt);
        struct SymbolNode *cont = tmp->head;
        while (cont != NULL)
        {
            printf("%s ", cont->field.name);
            cont = cont->areanext;
        }
        printf("\n");
        tmp = tmp->next;
        cnt++;
    }
    printf("----------Scope Table above-------------\n");
}

int CheckType(Type A, Type B)
{
    if (A == B)
    {
        return 1;
    }
    else
    {
        if (A == NULL)
        {
            printf("A is NULL!\n");
            assert(0);
            return 0;
        }
        else if (B == NULL)
        {
            printf("B is NULL!\n");
            assert(0);
            return 0;
        }
        if (A->kind != B->kind)
        {
            return 0;
        }
        else
        {
            switch (A->kind)
            {
            case BASIC:
            {
                return A->u.basic == B->u.basic;
                break;
            }
            case ARRAY:
            {
                int cnta = 0, cntb = 0; //数组的维度
                Type aa = A, bb = B;
                while (aa != NULL)
                {
                    aa = aa->u.array_.elem;
                    cnta++;
                    if (aa->kind != ARRAY)
                        break;
                }
                while (bb != NULL)
                {
                    bb = bb->u.array_.elem;
                    cntb++;
                    if (bb->kind != ARRAY)
                        break;
                }
                return cnta == cntb;
                break;
            }

            case STRUCTURE:
            {
                FieldList A_f = A->u.structure_.structure;
                FieldList B_f = B->u.structure_.structure;
                while (A_f != NULL && B_f != NULL) //长度
                {
                    A_f = A_f->tail;
                    B_f = B_f->tail;
                }
                if (A_f != NULL || B_f != NULL)
                {
                    return 0;
                }
                A_f = A->u.structure_.structure;
                B_f = B->u.structure_.structure;

                while (A_f != NULL && B_f != NULL)
                {
                    if (A_f->type->kind != B_f->type->kind)
                    {
                        return 0;
                    }
                    if (A_f->type->kind == ARRAY)
                    {
                        int checkres = CheckArrayType(A_f->type, B_f->type);
                        if (checkres == 0)
                            return 0;
                        else
                            return 1;
                    }
                    else
                    {
                        if (CheckType(A_f->type, B_f->type) == 0)
                        {
                            return 0;
                        }
                    }
                    A_f = A_f->tail;
                    B_f = B_f->tail;
                }
                return 1;
                break;
            }

            case FUNCTION:
            {
                if (A->u.fuction.num != B->u.fuction.num)
                {
                    return 0;
                }
                if (CheckType(A->u.fuction.ReturnParameter, B->u.fuction.ReturnParameter) == 0)
                {
                    return 0;
                }
                FieldList A_f = A->u.fuction.parameters;
                FieldList B_f = B->u.fuction.parameters;

                while (A_f != NULL && B_f != NULL)
                {
                    if (CheckType(A_f->type, B_f->type) == 0)
                    {
                        return 0;
                    }
                    A_f = A_f->tail;
                    B_f = B_f->tail;
                }
                return 1;
                break;
            }

            default:
                return 0;
            }
        }
    }
    return 0;
}
int CheckArrayType(Type A, Type B)
{
    if (A->u.array_.size != B->u.array_.size)
    {
        return 0;
    }
    int res;
    if (A->u.array_.elem->kind != B->u.array_.elem->kind)
    {
        return 0;
    }
    if (A->u.array_.elem->kind == ARRAY)
    {
        res = CheckArrayType(A->u.array_.elem, B->u.array_.elem);
    }
    else
    {
        res = CheckType(A->u.array_.elem, B->u.array_.elem);
    }
    return res;
}

void PushDecFuc(char *name, int linenum)
{
    if (fuchead == NULL)
    {
        fuchead = (struct Dec_Fuc *)malloc(sizeof(struct Dec_Fuc));
        strcpy(fuchead->name, name);
        fuchead->linenum = linenum;
        fuchead->next = NULL;
    }
    else
    {
        struct Dec_Fuc *tmp = fuchead;
        while (tmp->next != NULL)
        {
            tmp = tmp->next;
        }
        tmp->next = (struct Dec_Fuc *)malloc(sizeof(struct Dec_Fuc));
        strcpy(tmp->next->name, name);
        tmp->next->linenum = linenum;
        tmp->next->next = NULL;
    }
}

void CheckFucDef()
{
    struct Dec_Fuc *tmp = fuchead;
    while (tmp != NULL)
    {
        char *name = tmp->name;
        int isdef = -1;
        Type tmptype = (Type)(malloc(sizeof(struct Type_)));
        int res = querySymbol(&tmptype, name, &isdef, 0, 0); // current area
        if (isdef != 1)
        {
            printf("Error type %d at Line %d: ", 18, tmp->linenum);
            printf("Undefined function \"%s\".\n", tmp->name);
        }
        tmp = tmp->next;
    }
}

struct SymbolNode *querySymbolNodeZJDM(char *name,int *success){
    int val = hashcode(name);

    if (GlobalHead[val].head == NULL){
        *success = 0;
        return NULL;
    }
    else{
        int flag = 0;
        struct SymbolNode *tmp = GlobalHead[val].head;
        while (tmp!=NULL){
            if (strcmp(tmp->field.name,name)==0){
                *success = 1;
                flag = 1;
                return tmp;
            }
            tmp = tmp->next;
            if (tmp == NULL) break;
        }
        if (!flag){
            *success = 0;
            return NULL;
        }
    }
}