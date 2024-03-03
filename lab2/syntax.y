%{
    #include <stdio.h>
    #include <stdlib.h>
    #include "tree.h"

    #define YYSTYPE struct node*

    void yyerror(char*);
    void printerror(char*);
    int yylex();
    extern int yylineno;
    extern int lineno;
    int errornum=0;    
%}

%token INT
%token FLOAT
%token TYPE
%token NEWLINE
%token ID
%token SEMI COMMA DOT
%token ASSIGNOP RELOP
%token PLUS MINUS STAR DIV
%token AND OR NOT
%token LP RP LB RB LC RC
%token STRUCT
%token RETURN
%token IF
%token ELSE
%token WHILE

%right ASSIGNOP
%left AND OR
%left RELOP
%left PLUS MINUS
%left STAR DIV
%right NAGATE NOT
%left LOWER_THAN_RB
%left DOT LP LB RP RB
%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

%%
Program : ExtDefList {
        $$ = InsertNode($1, "Program", $1->linenum, NON_TERMINATOR);
    }
    ;

ExtDefList : ExtDef ExtDefList {
        $$ = InsertNode($1, "ExtDefList", @1.first_line, NON_TERMINATOR);
        AddSibling($1,1,$2);
    }
    | {
        $$ = InsertNode(NULL, "ExtDefList", yylineno, NON_TERMINATOR);
    }
    ;

ExtDef : Specifier ExtDecList SEMI {
        $$ = InsertNode($1, "ExtDef", @1.first_line, NON_TERMINATOR);
        AddSibling($1,2,$2,$3);
    }
    | Specifier SEMI {
        $$ = InsertNode($1, "ExtDef", @1.first_line, NON_TERMINATOR);
        AddSibling($1,1,$2);
    }
    | Specifier FunDec CompSt {
        $$ = InsertNode($1, "ExtDef", @1.first_line, NON_TERMINATOR);
        AddSibling($1,2,$2,$3);
    }
    | Specifier error{
            char msg[100];
		sprintf(msg, "Missing ';' ");
		printerror(msg);
    }
    ;

ExtDecList : VarDec {
        $$ = InsertNode($1, "ExtDecList", @1.first_line, NON_TERMINATOR);
    }
    | VarDec COMMA ExtDecList {
        $$ = InsertNode($1, "ExtDecList", @1.first_line, NON_TERMINATOR);
        AddSibling($1,2,$2,$3);
    }
    ;

Specifier : TYPE {
        $$ = InsertNode($1, "Specifier", @1.first_line, NON_TERMINATOR);
    }
    | StructSpecifier {
        $$ = InsertNode($1, "Specifier", @1.first_line, NON_TERMINATOR);
    }    
    ;

StructSpecifier : STRUCT OptTag LC DefList RC {
        $$ = InsertNode($1, "StructSpecifier", @1.first_line, NON_TERMINATOR);
        AddSibling($1,4,$2,$3,$4,$5);
    }
    | STRUCT Tag {
        $$ = InsertNode($1, "StructSpecifier", @1.first_line, NON_TERMINATOR);
        AddSibling($1,1,$2);
    }
    | STRUCT OptTag LC error RC {
        char msg[110];
        sprintf(msg,"Syntax error");
        printerror(msg);
    }
    ;

OptTag : ID {
        $$ = InsertNode($1, "OptTag", @1.first_line, NON_TERMINATOR);
    }
    | {
        $$ = InsertNode(NULL, "OptTag", yylineno, NON_TERMINATOR);
    }
    ;

Tag : ID {
        $$ = InsertNode($1, "Tag", @1.first_line, NON_TERMINATOR);
    }
    ;

VarDec : ID {
        $$ = InsertNode($1, "VarDec", @1.first_line, NON_TERMINATOR);
    }
    | VarDec LB INT RB {
        $$ = InsertNode($1, "VarDec", @1.first_line, NON_TERMINATOR);
        AddSibling($1,3,$2,$3,$4);
    }
    | VarDec LB ID RB {
        $$ = InsertNode($1, "VarDec",@1.first_line, NON_TERMINATOR);
        AddSibling($1,3,$2,$3,$4);
    }
    | VarDec LB error RB {
            char msg[100];
		sprintf(msg, "Syntax error.");
		printerror(msg);
    }
    | VarDec LB ID error{
        char msg[100];
        sprintf(msg, "Missing \"]\".");
        printerror(msg);
    }
    ;

FunDec : ID LP VarList RP {
        $$ = InsertNode($1, "FunDec", @1.first_line, NON_TERMINATOR);
        AddSibling($1,3,$2,$3,$4);
    }
    | ID LP RP {
        $$ = InsertNode($1, "FunDec", @1.first_line, NON_TERMINATOR);
        AddSibling($1,2,$2,$3);
    }
    ;

VarList : ParamDec COMMA VarList {
        $$ = InsertNode($1, "VarList", @1.first_line, NON_TERMINATOR);
        AddSibling($1,2,$2,$3);
    }
    | ParamDec {
        $$ = InsertNode($1, "VarList", @1.first_line, NON_TERMINATOR);
    }
    ;

ParamDec : Specifier VarDec {
        $$ = InsertNode($1, "ParamDec", @1.first_line, NON_TERMINATOR);
        AddSibling($1,1,$2);
    }
    ;

CompSt : LC DefList StmtList RC {
       $$ = InsertNode($1, "CompSt", @1.first_line, NON_TERMINATOR);
       AddSibling($1,3,$2,$3,$4);
   }
   ;

StmtList : Stmt StmtList {
       $$ = InsertNode($1, "StmtList", @1.first_line, NON_TERMINATOR);
       AddSibling($1,1,$2);
   }
   | {
       $$ = InsertNode(NULL, "FunDec", yylineno, NON_TERMINATOR);
   }
   ;

Stmt : 
    Exp SEMI {
        $$ = InsertNode($1, "Stmt", @1.first_line, NON_TERMINATOR);
        AddSibling($1,1,$2);
    }
    | Exp error{
    	      char msg[100];
		sprintf(msg, "error: Missing \";\"");
		printerror(msg);
    }
    
    | CompSt {
        $$ = InsertNode($1, "Stmt", @1.first_line, NON_TERMINATOR);
    }
    | RETURN Exp SEMI {
        $$ = InsertNode($1, "Stmt", @1.first_line, NON_TERMINATOR);
        AddSibling($1,2,$2,$3);
    }
    | IF LP Exp RP Stmt %prec LOWER_THAN_ELSE {
        $$ = InsertNode($1, "Stmt", @1.first_line, NON_TERMINATOR);
        AddSibling($1,4,$2,$3,$4,$5);
    }
    | IF LP Exp RP Stmt ELSE Stmt {
        $$ = InsertNode($1, "Stmt", @1.first_line, NON_TERMINATOR);
        AddSibling($1,6,$2,$3,$4,$5,$6,$7);
    }
    | WHILE LP Exp RP Stmt {
        $$ = InsertNode($1, "Stmt", @1.first_line, NON_TERMINATOR);
        AddSibling($1,4,$2,$3,$4,$5);
    }
    | IF LP error RP Stmt %prec LOWER_THAN_ELSE{
            char msg[100];
		sprintf(msg, "Syntax error.");
		printerror(msg);
    }
    | IF LP error RP Stmt ELSE Stmt{
            char msg[100];
		sprintf(msg, "Syntax error.");
		printerror(msg);
    }
    | WHILE LP error RP Stmt{
            char msg[100];
		sprintf(msg, "Syntax error.");
		printerror(msg);
    }
    | RETURN Exp error{
            char msg[100];
		sprintf(msg, "Missing ';' .");
		printerror(msg);
    }
    ;

DefList : Def DefList {
       $$ = InsertNode($1, "DefList", @1.first_line, NON_TERMINATOR);
       AddSibling($1,1,$2);
   }
   | {
       $$ = InsertNode(NULL, "DefList", yylineno, NON_TERMINATOR);
   }
   ;

Def : Specifier DecList SEMI {
        $$ = InsertNode($1, "Def", @1.first_line, NON_TERMINATOR);
        AddSibling($1,2,$2,$3);
    }
    | Specifier error SEMI {
            char msg[100];
		sprintf(msg, "Syntax error.");
		printerror(msg);
    }
    | error DecList SEMI {
    	      char msg[100];
		sprintf(msg, "Syntax error.");
		printerror(msg);
	}
    ;

DecList : Dec {
        $$ = InsertNode($1, "DecList", @1.first_line, NON_TERMINATOR);
    }
    | Dec COMMA DecList {
        $$ = InsertNode($1, "DecList", @1.first_line, NON_TERMINATOR);
        AddSibling($1,2,$2,$3);
    }
	;

Dec : VarDec {
        $$ = InsertNode($1, "Dec", @1.first_line, NON_TERMINATOR);
    }
    | VarDec ASSIGNOP Exp {
        $$ = InsertNode($1, "Dec", @1.first_line, NON_TERMINATOR);
        AddSibling($1,2,$2,$3);
    }
    | VarDec ASSIGNOP error{
            char msg[100];
		sprintf(msg, "Syntax error.");
		printerror(msg);
    }
    ;

Exp : Exp ASSIGNOP Exp {
        $$ = InsertNode($1, "Exp", @1.first_line, NON_TERMINATOR);
        AddSibling($1,2,$2,$3);
    }
	| Exp ASSIGNOP error{ 
		char msg[100];
            sprintf(msg, "Syntax error."); 
		printerror(msg);
	}
    | Exp AND Exp {
        $$ = InsertNode($1, "Exp", @1.first_line, NON_TERMINATOR);
        AddSibling($1,2,$2,$3);
    }
    | Exp OR Exp {
        $$ = InsertNode($1, "Exp", @1.first_line, NON_TERMINATOR);
        AddSibling($1,2,$2,$3);
    }
    | Exp RELOP Exp {
        $$ = InsertNode($1, "Exp", @1.first_line, NON_TERMINATOR);
        AddSibling($1,2,$2,$3);
    }
    | Exp PLUS Exp {
        $$ = InsertNode($1, "Exp", @1.first_line, NON_TERMINATOR);
        AddSibling($1,2,$2,$3);
    }
    | Exp MINUS Exp {
        $$ = InsertNode($1, "Exp", @1.first_line, NON_TERMINATOR);
        AddSibling($1,2,$2,$3);
    }
    | Exp STAR Exp {
        $$ = InsertNode($1, "Exp", @1.first_line, NON_TERMINATOR);
        AddSibling($1,2,$2,$3);
    }
    | Exp DIV Exp {
        $$ = InsertNode($1, "Exp", @1.first_line, NON_TERMINATOR);
        AddSibling($1,2,$2,$3);
    }
    | LP Exp RP {
        $$ = InsertNode($1, "Exp", @1.first_line, NON_TERMINATOR);
        AddSibling($1,2,$2,$3);
    }
    | MINUS Exp {
        $$ = InsertNode($1, "Exp", @1.first_line, NON_TERMINATOR);
        AddSibling($1,1,$2);
    }
    | NOT Exp {
        $$ = InsertNode($1, "Exp", @1.first_line, NON_TERMINATOR);
        AddSibling($1,1,$2);
    }
    | ID LP Args RP {
        $$ = InsertNode($1, "Exp", @1.first_line, NON_TERMINATOR);
        AddSibling($1,3,$2,$3,$4);
    }
    | ID LP RP {
        $$ = InsertNode($1, "Exp", @1.first_line, NON_TERMINATOR);
        AddSibling($1,2,$2,$3);
    }
    | Exp LB Exp RB {
        $$ = InsertNode($1, "Exp", @1.first_line, NON_TERMINATOR);
        AddSibling($1,3,$2,$3,$4);
    }
	| Exp DOT ID {
		$$ = InsertNode($1, "Exp", @1.first_line, NON_TERMINATOR);
		AddSibling($1,2,$2,$3);
	}
	| ID {
		$$ = InsertNode($1, "Exp", @1.first_line, NON_TERMINATOR);
	}
	| INT {
		$$ = InsertNode($1, "Exp", @1.first_line, NON_TERMINATOR);
	}
	| FLOAT {
		$$ = InsertNode($1, "Exp", @1.first_line, NON_TERMINATOR);
	}
	| LP error RP {
		char msg[100];
        	sprintf(msg, "Syntax error.");
		printerror(msg);
	}
	| ID LP error RP {
		char msg[100];
        	sprintf(msg, "Syntax error.");
		printerror(msg);
	}
	| Exp LB error RB{
		char msg[100];
        	sprintf(msg, "Syntax error.");
		printerror(msg);
	}
    ;

Args : Exp COMMA Args {
        $$ = InsertNode($1, "CompSt", @1.first_line, NON_TERMINATOR);
        AddSibling($1,2,$2,$3);
    }
    | Exp {
        $$ = InsertNode($1, "CompSt", @1.first_line, NON_TERMINATOR);
    }
    ;
	
%%
#include "lex.yy.c"

int main(int argc, char** argv) {
	if(argc <= 1) return 1;
	FILE* f = fopen(argv[1], "r");
	if(!f) {
		perror(argv[1]);
		return 1;
	}
	yylineno = 1;
	yyrestart(f);
	yyparse();

	FILE *f1 = fopen("output.txt", "w");
	if(!f1) {
		perror(argv[1]);
		return 1;
	}
	if(!errornum) {
		f1 = fopen("output.txt", "w");
            TravelTree(root,0,f1);
	}
	return 0;
}


void yyerror(char *msg) {}

void printerror(char *msg) {
    fprintf(stderr, "Error type B at Line %d: %s\n", lineno, msg);
    errornum++;
}

