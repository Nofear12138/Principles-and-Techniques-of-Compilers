%{
    #include <stdio.h>
    #include <stdlib.h>
    #include "tree.h"
    #include "semantic.h"
    #include "operand.h"
    #include "Simulater.h"

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
        $$ = InsertNode("Program", $1->linenum, NON_TERMINATOR);
        AddSibling($$,1,$1);
        root = $$;
    }
    ;

ExtDefList : ExtDef ExtDefList {
        $$ = InsertNode("ExtDefList", @$.first_line, NON_TERMINATOR);
        AddSibling($$,2,$1,$2);
    }
    | {
        $$ = NULL;
    }
    ;

ExtDef : Specifier ExtDecList SEMI {
        $$ = InsertNode("ExtDef", @$.first_line, NON_TERMINATOR);
        AddSibling($$,3,$1,$2,$3);
    }
    | Specifier SEMI {
        $$ = InsertNode("ExtDef", @$.first_line, NON_TERMINATOR);
        AddSibling($$,2,$1,$2);
    }
    | Specifier FunDec CompSt {
        $$ = InsertNode("ExtDef", @$.first_line, NON_TERMINATOR);
        AddSibling($$,3,$1,$2,$3);
    }
    | Specifier FunDec SEMI{
        $$ = InsertNode("ExtDef",@$.first_line, NON_TERMINATOR);
        AddSibling($$,3,$1,$2,$3);
    }
    | Specifier error{
            char msg[100];
		sprintf(msg, "Missing ';' ");
		printerror(msg);
    }
    ;

ExtDecList : VarDec {
        $$ = InsertNode("ExtDecList", @$.first_line, NON_TERMINATOR);
        AddSibling($$,1,$1);
    }
    | VarDec COMMA ExtDecList {
        $$ = InsertNode("ExtDecList", @$.first_line, NON_TERMINATOR);
        AddSibling($$,3,$1,$2,$3);
    }
    ;

Specifier : TYPE {
        $$ = InsertNode("Specifier", @$.first_line, NON_TERMINATOR);
        AddSibling($$,1,$1);
    }
    | StructSpecifier {
        $$ = InsertNode("Specifier", @$.first_line, NON_TERMINATOR);
        AddSibling($$,1,$1);
    }    
    ;

StructSpecifier : STRUCT OptTag LC DefList RC {
        $$ = InsertNode( "StructSpecifier", @$.first_line, NON_TERMINATOR);
        AddSibling($$,5,$1,$2,$3,$4,$5);
    }
    | STRUCT Tag {
        $$ = InsertNode("StructSpecifier", @$.first_line, NON_TERMINATOR);
        AddSibling($$,2,$1,$2);
    }
    | STRUCT OptTag LC error RC {
        char msg[110];
        sprintf(msg,"Syntax error");
        printerror(msg);
    }
    ;

OptTag : ID {
        $$ = InsertNode("OptTag", @$.first_line, NON_TERMINATOR);
        AddSibling($$,1,$1);
    }
    | {
        $$ = NULL;
    }
    ;

Tag : ID {
        $$ = InsertNode("Tag", @$.first_line, NON_TERMINATOR);
        AddSibling($$,1,$1);
    }
    ;

VarDec : ID {
        $$ = InsertNode("VarDec", @$.first_line, NON_TERMINATOR);
        AddSibling($$,1,$1);
    }
    | VarDec LB INT RB {
        $$ = InsertNode("VarDec", @$.first_line, NON_TERMINATOR);
        AddSibling($$,4,$1,$2,$3,$4);
    }
    | VarDec LB ID RB {
        $$ = InsertNode("VarDec",@$.first_line, NON_TERMINATOR);
        AddSibling($$,4,$1,$2,$3,$4);
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
        $$ = InsertNode("FunDec", @$.first_line, NON_TERMINATOR);
        AddSibling($$,4,$1,$2,$3,$4);
    }
    | ID LP RP {
        $$ = InsertNode("FunDec", @$.first_line, NON_TERMINATOR);
        AddSibling($$,3,$1,$2,$3);
    }
    ;

VarList : ParamDec COMMA VarList {
        $$ = InsertNode("VarList", @$.first_line, NON_TERMINATOR);
        AddSibling($$,3,$1,$2,$3);
    }
    | ParamDec {
        $$ = InsertNode("VarList", @$.first_line, NON_TERMINATOR);
        AddSibling($$,1,$1);
    }
    ;

ParamDec : Specifier VarDec {
        $$ = InsertNode("ParamDec", @$.first_line, NON_TERMINATOR);
        AddSibling($$,2,$1,$2);
    }
    ;

CompSt : LC DefList StmtList RC {
       $$ = InsertNode("CompSt", @$.first_line, NON_TERMINATOR);
       AddSibling($$,4,$1,$2,$3,$4);
   }
   ;

StmtList : Stmt StmtList {
       $$ = InsertNode("StmtList", @$.first_line, NON_TERMINATOR);
       AddSibling($$,2,$1,$2);
   }
   | {
       $$ = NULL;
   }
   ;

Stmt : 
    Exp SEMI {
        $$ = InsertNode("Stmt", @$.first_line, NON_TERMINATOR);
        AddSibling($$,2,$1,$2);
    }
    | Exp error{
    	      char msg[100];
		sprintf(msg, "error: Missing \";\"");
		printerror(msg);
    }
    
    | CompSt {
        $$ = InsertNode("Stmt", @$.first_line, NON_TERMINATOR);
        AddSibling($$,1,$1);
    }
    | RETURN Exp SEMI {
        $$ = InsertNode("Stmt", @$.first_line, NON_TERMINATOR);
        AddSibling($$,3,$1,$2,$3);
    }
    | IF LP Exp RP Stmt %prec LOWER_THAN_ELSE {
        $$ = InsertNode("Stmt", @$.first_line, NON_TERMINATOR);
        AddSibling($$,5,$1,$2,$3,$4,$5);
    }
    | IF LP Exp RP Stmt ELSE Stmt {
        $$ = InsertNode("Stmt", @$.first_line, NON_TERMINATOR);
        AddSibling($$,7,$1,$2,$3,$4,$5,$6,$7);
    }
    | WHILE LP Exp RP Stmt {
        $$ = InsertNode("Stmt", @$.first_line, NON_TERMINATOR);
        AddSibling($$,5,$1,$2,$3,$4,$5);
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
       $$ = InsertNode("DefList", @$.first_line, NON_TERMINATOR);
       AddSibling($$,2,$1,$2);
   }
   | {
       $$ = NULL;
   }
   ;

Def : Specifier DecList SEMI {
        $$ = InsertNode("Def", @$.first_line, NON_TERMINATOR);
        AddSibling($$,3,$1,$2,$3);
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
        $$ = InsertNode("DecList", @$.first_line, NON_TERMINATOR);
        AddSibling($$,1,$1);
    }
    | Dec COMMA DecList {
        $$ = InsertNode("DecList", @$.first_line, NON_TERMINATOR);
        AddSibling($$,3,$1,$2,$3);
    }
	;

Dec : VarDec {
        $$ = InsertNode("Dec", @$.first_line, NON_TERMINATOR);
        AddSibling($$,1,$1);
    }
    | VarDec ASSIGNOP Exp {
        $$ = InsertNode("Dec", @$.first_line, NON_TERMINATOR);
        AddSibling($$,3,$1,$2,$3);
    }
    | VarDec ASSIGNOP error{
            char msg[100];
		sprintf(msg, "Syntax error.");
		printerror(msg);
    }
    ;

Exp : Exp ASSIGNOP Exp {
        $$ = InsertNode("Exp", @$.first_line, NON_TERMINATOR);
        AddSibling($$,3,$1,$2,$3);
    }
	| Exp ASSIGNOP error{ 
		char msg[100];
            sprintf(msg, "Syntax error."); 
		printerror(msg);
	}
    | Exp AND Exp {
        $$ = InsertNode("Exp", @$.first_line, NON_TERMINATOR);
        AddSibling($$,3,$1,$2,$3);
    }
    | Exp OR Exp {
        $$ = InsertNode("Exp", @$.first_line, NON_TERMINATOR);
        AddSibling($$,3,$1,$2,$3);
    }
    | Exp RELOP Exp {
        $$ = InsertNode("Exp", @$.first_line, NON_TERMINATOR);
        AddSibling($$,3,$1,$2,$3);
    }
    | Exp PLUS Exp {
        $$ = InsertNode("Exp", @$.first_line, NON_TERMINATOR);
        AddSibling($$,3,$1,$2,$3);
    }
    | Exp MINUS Exp {
        $$ = InsertNode("Exp", @$.first_line, NON_TERMINATOR);
        AddSibling($$,3,$1,$2,$3);
    }
    | Exp STAR Exp {
        $$ = InsertNode("Exp", @$.first_line, NON_TERMINATOR);
        AddSibling($$,3,$1,$2,$3);
    }
    | Exp DIV Exp {
        $$ = InsertNode("Exp", @$.first_line, NON_TERMINATOR);
        AddSibling($$,3,$1,$2,$3);
    }
    | LP Exp RP {
        $$ = InsertNode("Exp", @$.first_line, NON_TERMINATOR);
        AddSibling($$,3,$1,$2,$3);
    }
    | MINUS Exp {
        $$ = InsertNode("Exp", @$.first_line, NON_TERMINATOR);
        AddSibling($$,2,$1,$2);
    }
    | NOT Exp {
        $$ = InsertNode("Exp", @$.first_line, NON_TERMINATOR);
        AddSibling($$,2,$1,$2);
    }
    | ID LP Args RP {
        $$ = InsertNode("Exp", @$.first_line, NON_TERMINATOR);
        AddSibling($$,4,$1,$2,$3,$4);
    }
    | ID LP RP {
        $$ = InsertNode("Exp", @$.first_line, NON_TERMINATOR);
        AddSibling($$,3,$1,$2,$3);
    }
    | Exp LB Exp RB {
        $$ = InsertNode("Exp", @$.first_line, NON_TERMINATOR);
        AddSibling($$,4,$1,$2,$3,$4);
    }
	| Exp DOT ID {
		$$ = InsertNode("Exp", @$.first_line, NON_TERMINATOR);
		AddSibling($$,3,$1,$2,$3);
	}
	| ID {
		$$ = InsertNode("Exp", @$.first_line, NON_TERMINATOR);
        AddSibling($$,1,$1);
	}
	| INT {
		$$ = InsertNode("Exp", @$.first_line, NON_TERMINATOR);
        AddSibling($$,1,$1);
	}
	| FLOAT {
		$$ = InsertNode("Exp", @$.first_line, NON_TERMINATOR);
        AddSibling($$,1,$1);
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
        $$ = InsertNode("Args", @$.first_line, NON_TERMINATOR);
        AddSibling($$,3,$1,$2,$3);
    }
    | Exp {
        $$ = InsertNode("Args", @$.first_line, NON_TERMINATOR);
        AddSibling($$,1,$1);
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
    
    FILE* fp=fopen(argv[2],"wt+");
    if(!fp){
        perror(argv[2]);
        return 1;
    }

	yylineno = 1;
	yyrestart(f);
	yyparse();

    if (!errornum){
       //f = fopen("output.txt", "w");
            //TravelTree(root,0,f);
            CheckSemantic(root);
            Generate(root,fp);
            CodeGenerate(fp);
    }
	return 0;
}


void yyerror(char *msg) {}

void printerror(char *msg) {
    fprintf(stderr, "Error type B at Line %d: %s\n", lineno, msg);
    errornum++;
}

