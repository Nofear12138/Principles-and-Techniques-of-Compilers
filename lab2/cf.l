%{
	#include "syntax.tab.h"
	extern int errornum;
	//YYLTYPE yylloc;
	int column=1;
	int lineno = 1;
	int templineno = 1;
	int yycolumn=1;
	#define YY_USER_ACTION  \
	  yylloc.first_line = yylloc.last_line = yylineno; \
        yylloc.first_column = yycolumn; \
        yylloc.last_column = yycolumn + yyleng - 1; \
        yycolumn += yyleng;
%}
%option yylineno
DIGIT [0-9]
INT 0|[1-9]{DIGIT}*
LETTER [a-zA-Z]
ILLGALDIGITSTR {DIGIT}*{LETTER}[0-9a-zA-Z]*
INT8 0[0-7]+
INT16 0(x|X)[0-9a-fA-F]+
ILLGALINT8 0([89a-wyzA-WYZ][0-9a-zA-Z]*|[0-7]+[89a-zA-Z][0-9a-zA-Z]*)
ILLGALINT16 0(x|X)[0-9a-fA-F]*[g-zG-Z][0-9a-zA-Z]*
FLOAT ({DIGIT}+\.{DIGIT}*|{DIGIT}*\.{DIGIT}+)
ILLGALFLOAT ({DIGIT}+|{FLOAT})(e|E)({FLOAT}|{ILLGALDIGITSTR})?
ID ({LETTER}|_)({LETTER}|_|{DIGIT})*
TYPE int|float
IF if
ELSE else
WHILE while
RETURN return
STRUCT struct
SEMI ;
COMMA ,
ASSIGNOP =
RELOP \>|\<|>=|<=|==|!=
PLUS \+
MINUS \-
STAR \*
DIV \/
AND &&
OR \|\|
DOT \.
NOT !
LP \(
RP \)
LB \[
RB \]
LC \{
RC \}
SPACE (" "+)
TAB \t
ENTER \n
NOTE "//".*
NOTEE "/*"([^"*/"]*)"*/"

%%
{ENTER} {yycolumn = 1;}
{COMMA} {
	lineno=templineno;
	templineno=yylineno;
	struct node *pnode = CreateNode("COMMA", 0, STRING_TYPE);
	yylval = pnode;
	pnode->id = strdup(yytext);
	return COMMA;
}
{DOT} {
	lineno=templineno;
	templineno=yylineno;
	struct node *pnode = CreateNode("DOT", 0, STRING_TYPE);
	yylval = pnode;
	pnode->id = strdup(yytext);
	return DOT;
}
{NOT} {
	lineno=templineno;
	templineno=yylineno;
	struct node *pnode = CreateNode("NOT", 0, STRING_TYPE);
	yylval = pnode;
	pnode->id = strdup(yytext);
	return NOT;
}
{FLOAT} {
	lineno=templineno;
	templineno=yylineno;
	struct node *pnode = CreateNode("FLOAT", 0, STRING_TYPE);
	yylval = pnode;
	pnode->floatvalue = atof(yytext);
	return FLOAT;
}
{ILLGALFLOAT} {
	errornum ++ ;
	column = yycolumn-yyleng;
	lineno=templineno;
	templineno=yylineno;
	printf("Error Type A at line %d, char %d: Illegal float number: '%s'\n", yylineno, column, yytext);
	struct node *pnode = CreateNode("ILLGALFLOAT", 0, STRING_TYPE);
	yylval = pnode;
	pnode->id = strdup(yytext);
	return FLOAT;
}
{ILLGALINT8} {
	errornum ++ ;
	column = yycolumn-yyleng;
	lineno=templineno;
	templineno=yylineno;
	printf("Error Type A at line %d, char %d: Illegal float number: '%s'\n", yylineno, column, yytext);
	struct node *pnode = CreateNode("ILLGALINT8", 0, STRING_TYPE);
	yylval = pnode;
	pnode->id = strdup(yytext);
	return INT;
}
{ILLGALINT16} {
	errornum ++ ;
	column = yycolumn-yyleng;
	lineno=templineno;
	templineno=yylineno;
	printf("Error Type A at line %d, char %d: Illegal float number: '%s'\n", yylineno, column, yytext);
	struct node *pnode = CreateNode("ILLGALINT16A", 0, STRING_TYPE);
	yylval = pnode;
	pnode->id = strdup(yytext);
	return INT;
}
{INT} {
	lineno=templineno;
	templineno=yylineno;
	struct node *pnode = CreateNode("INT", 0, STRING_TYPE);
	yylval = pnode;
	pnode->intvalue = atoi(yytext);
	return INT;
}
{INT8} {
	lineno=templineno;
	templineno=yylineno;
	struct node *pnode = CreateNode("INT8", 0, STRING_TYPE);
	yylval = pnode;
	pnode->id = strdup(yytext);
	return INT;
}
{INT16} {
	lineno=templineno;
	templineno=yylineno;
	struct node *pnode = CreateNode("INT16", 0, STRING_TYPE);
	yylval = pnode;
	pnode->id = strdup(yytext);
	return INT;
}
{PLUS} {
	lineno=templineno;
	templineno=yylineno;
	struct node *pnode = CreateNode("PLUS", 0, STRING_TYPE);
	yylval = pnode;
	pnode->id = strdup(yytext);
	return PLUS;
}
{ASSIGNOP} {
	lineno=templineno;
	templineno=yylineno;
	struct node *pnode = CreateNode("ASSIGNOP", 0, STRING_TYPE);
	yylval = pnode;
	pnode->id = strdup(yytext);
	return ASSIGNOP;
}
{RELOP} {
	lineno=templineno;
	templineno=yylineno;
	struct node *pnode = CreateNode("RELOP", 0, STRING_TYPE);
	yylval = pnode;
	pnode->id = strdup(yytext);
	return RELOP;
}
{TYPE} {
	lineno=templineno;
	templineno=yylineno;
	struct node *pnode = CreateNode("TYPE", 0, STRING_TYPE);
	yylval = pnode;
	pnode->id = strdup(yytext);
	return TYPE;
}
{RETURN} {
	lineno=templineno;
	templineno=yylineno;
	struct node *pnode = CreateNode("RETURN", 0, STRING_TYPE);
	yylval = pnode;
	pnode->id = strdup(yytext);
	return RETURN;
}
{IF} {
	lineno=templineno;
	templineno=yylineno;
	struct node *pnode = CreateNode("IF", 0, STRING_TYPE);
	yylval = pnode;
	pnode->id = strdup(yytext);
	return IF;
}
{ELSE} {
	lineno=templineno;
	templineno=yylineno;
	struct node *pnode = CreateNode("ELSE", 0, STRING_TYPE);
	yylval = pnode;
	pnode->id = strdup(yytext);
	return ELSE;
}
{WHILE} {
	lineno=templineno;
	templineno=yylineno;
	struct node *pnode = CreateNode("WHILE", 0, STRING_TYPE);
	yylval = pnode;
	pnode->id = strdup(yytext);
	return WHILE;
}
{STRUCT} {
	lineno=templineno;
	templineno=yylineno;
	struct node *pnode = CreateNode("STRUCT", 0, STRING_TYPE);
	yylval = pnode;
	pnode->id = strdup(yytext);
	return STRUCT;
}
{AND} {
	lineno=templineno;
	templineno=yylineno;
	struct node *pnode = CreateNode("AND", 0, STRING_TYPE);
	yylval = pnode;
	pnode->id = strdup(yytext);
	return AND;
}
{OR} {
	lineno=templineno;
	templineno=yylineno;
	struct node *pnode = CreateNode("COMMA", 0, STRING_TYPE);
	yylval = pnode;
	pnode->id = strdup(yytext);
	return OR;
}
{STAR} {
	lineno=templineno;
	templineno=yylineno;
	struct node *pnode = CreateNode("STAR", 0, STRING_TYPE);
	yylval = pnode;
	pnode->id = strdup(yytext);
	return STAR;
}
{MINUS} {
	lineno=templineno;
	templineno=yylineno;
	struct node *pnode = CreateNode("MINUS", 0, STRING_TYPE);
	yylval = pnode;
	pnode->id = strdup(yytext);
	return MINUS;
}
{DIV} {
	lineno=templineno;
	templineno=yylineno;
	struct node *pnode = CreateNode("DIV", 0, STRING_TYPE);
	yylval = pnode;
	pnode->id = strdup(yytext);
	return DIV;
}
{NOTE} { } 
{NOTEE} {} 
{LP} {
	lineno=templineno;
	templineno=yylineno;
	struct node *pnode = CreateNode("LP", 0, STRING_TYPE);
	yylval = pnode;
	pnode->id = strdup(yytext);
	return LP;
}
{RP} {
	lineno=templineno;
	templineno=yylineno;
	struct node *pnode = CreateNode("RP", 0, STRING_TYPE);
	yylval = pnode;
	pnode->id = strdup(yytext);
	return RP;
}
{LB} {
	lineno=templineno;
	templineno=yylineno;
	struct node *pnode = CreateNode("LB", 0, STRING_TYPE);
	yylval = pnode;
	pnode->id = strdup(yytext);
	return LB;
}
{RB} {
	lineno=templineno;
	templineno=yylineno;
	struct node *pnode = CreateNode("RB", 0, STRING_TYPE);
	yylval = pnode;
	pnode->id = strdup(yytext);
	return RB;
}
{LC} {
	lineno=templineno;
	templineno=yylineno;
	struct node *pnode = CreateNode("LC", 0, STRING_TYPE);
	yylval = pnode;
	pnode->id = strdup(yytext);
	return LC;
}
{RC} {
	lineno=templineno;
	templineno=yylineno;
	struct node *pnode = CreateNode("RC", 0, STRING_TYPE);
	yylval = pnode;
	pnode->id = strdup(yytext);
	return RC;
}
{SEMI} {
	lineno=templineno;
	templineno=yylineno;
	struct node *pnode = CreateNode("SEMI", 0, STRING_TYPE);
	yylval = pnode;
	pnode->id = strdup(yytext);
	return SEMI;
}
{ID} {
	lineno=templineno;
	templineno=yylineno;
	struct node *pnode = CreateNode("ID", 0, STRING_TYPE);
	yylval = pnode;
	pnode->id = strdup(yytext);
	return ID;
} 
{SPACE} {}
{TAB} {yycolumn += 3;}
. {
	errornum++;
	column = yycolumn-yyleng;
	printf("Error Type A at line %d, char %d: Mysterious character: '%s'\n", yylineno, column, yytext);
}

