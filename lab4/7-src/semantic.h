#ifndef _SEMANTIC_H_
#define _SEMANTIC_H_

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "tree.h"
#include "SymbolTable.h"

void Write();
void Read();

int CheckSemantic(Node *cur);

int ProgramAnalyse(Node *cur);
int ExtDefListAnalyse(Node *cur);
int ExtDefAnalyse(Node *cur);
int ExtDecListAnalyse(Node *cur, Type type);
int CompStAnalyse(Node *cur, struct SymbolBucket *scope, Type type);
int StmtListAnalyse(Node *cur, struct SymbolBucket *scope, Type type);
int StmtAnalyse(Node *cur, struct SymbolBucket *scope, Type type);
int DefListAnalyse(Node *cur, struct SymbolBucket *scope);
int DefAnalyse(Node *cur, struct SymbolBucket *scope);
int DecListAnalyse(Node *cur, struct SymbolBucket *scope, Type type);
int DecAnalyse(Node *cur, struct SymbolBucket *scope, Type type);
Type ExpAnalyse(Node *cur);
int ArgAnalyse(Node *cur, FieldList params, char *functionname);
void FunDecAnalyse(Node *cur, const int isdef, const Type type, struct SymbolBucket *scope);
void errorprint(int type, int linenum, char *content);
FieldList VarListAnalyse(Node *cur, struct SymbolBucket *scope);
FieldList ParamDecAnalyse(Node *cur);
Type SpecifierAnalyse(Node *cur);
FieldList DefStruct(Node *cur, char *structname,int curoffset,int *tmpoffset);
FieldList DecStruct(Node *cur, Type type);
FieldList VarDecAnalyse(Node *cur, Type type);
void errorprint12(int linenum, float b);
void errorprint9(int linenum, char *s1, char *s2);

#endif