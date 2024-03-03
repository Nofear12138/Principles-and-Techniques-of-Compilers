%{
	#include <stdio.h>
	#include <stdlib.h>
	int column=1;
%}
%option yylineno
DIGIT[0-9]
INT 0|[+-]?[1-9]{DIGIT}*
INT8 0(0|[1-7][0-7]*)
ILLGALINT8 0([0-7]*)?[8-9]+[0-9]*
INT16 0[x|X](0|[1-9A-fa-f][0-9A-fa-f]*)
ILLGALINT16 0[x|X]([0-9A-Fa-f]*)?([g-zG-Z])+([0-9]|[a-zA-Z])*
FLOAT [+-]?([0-9]*\.[0-9]+|[0-9]+\.)([eE][+-]?[0-9]+)?
ILLGALFLOAT [+-]?[0-9]*\.?[0-9]*[eE]([A-Za-z0-9]*)\.?([A-Za-z0-9]*)
LETTER[a-zA-Z]
ID (_|{LETTER})({LETTER}|_|{DIGIT})*
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
BRACKET \(|\)|\[|\]|\{|\}
TYPE int|float
KEYWORD struct|if|else|while|return
NOTE (\/\/.*)
NOTEE (\/\*(.|\n)*\*\/)
%%
\n {column=1;}
{NOTEE} {printf("NOTEE at line %d,char %d: %s\n",yylineno,column,yytext),column+=yyleng;}
{NOTE} {printf("NOTE at line %d,char %d: %s\n",yylineno,column,yytext),column+=yyleng;}
{TYPE} {printf("TYPE at line %d,char %d: %s\n",yylineno,column,yytext),column+=yyleng;}
{SEMI} {printf("SEMI at line %d,char %d: %s\n",yylineno,column,yytext),column+=yyleng;}
{COMMA} {printf("COMMA at line %d,char %d: %s\n",yylineno,column,yytext),column+=yyleng;}
{KEYWORD} {printf("KEYWORD at line %d,char %d: %s\n",yylineno,column,yytext),column+=yyleng;}
{PLUS} {printf("PLUS at line %d,char %d: %s\n",yylineno,column,yytext),column+=yyleng;}
{MINUS} {printf("MINUS at line %d,char %d: %s\n",yylineno,column,yytext),column+=yyleng;}
{STAR} {printf("STAR at line %d,char %d: %s\n",yylineno,column,yytext),column+=yyleng;}
{DIV} {printf("DIV at line %d,char %d: %s\n",yylineno,column,yytext),column+=yyleng;}
{AND} {printf("AND at line %d,char %d: %s\n",yylineno,column,yytext),column+=yyleng;}
{OR} {printf("OR at line %d,char %d: %s\n",yylineno,column,yytext),column+=yyleng;}
{BRACKET} {printf("BRACKET at line %d,char %d: %s\n",yylineno,column,yytext),column+=yyleng;}
{INT8} {printf("INT8 at line %d,char %d: %s\n",yylineno,column,yytext),column+=yyleng;}
{ILLGALINT8} {printf("Error Type A at line %d,char %d: Illgal octal number: %s\n",yylineno,column,yytext),column+=yyleng;}
{INT16} {printf("INT16 at line %d,char %d: %s\n",yylineno,column,yytext),column+=yyleng;}
{ILLGALINT16} {printf("Error Type A at line %d,char %d: Illgal hexadecimal number: %s\n",yylineno,column,yytext),column+=yyleng;}
{INT} {printf("INT data at line %d,char %d: %s\n",yylineno,column,yytext),column+=yyleng;}
{FLOAT} {printf("FLOAT at line %d,char %d: %s\n",yylineno,column,yytext),column+=yyleng;}
{ID} {printf("ID at line %d,char %d: %s\n",yylineno,column,yytext),column+=yyleng;}
{ILLGALFLOAT} {printf("Error Type A at line %d,char %d: Illgal float number :%s\n",yylineno,column,yytext),column+=yyleng;}
{ASSIGNOP} {printf("ASSIGNOP at line %d,char %d: %s\n",yylineno,column,yytext),column+=yyleng;}
{RELOP} {printf("RELOP at line %d,char %d: %s\n",yylineno,column,yytext),column+=yyleng;}

{DOT} {printf("DOT at line %d,char %d: %s\n",yylineno,column,yytext),column+=yyleng;}
{NOT} {printf("NOT at line %d,char %d: %s\n",yylineno,column,yytext),column+=yyleng;}
" " {column+=1;}
\t {column+=4;}
. {printf("ERROR Type A at line %d,char %d: Mysterious charcter:'%s'\n",yylineno,column,yytext),column++;}
%%
int main(int argc,char* argv[])
{
	if (argc > 1){
		if (!(yyin = fopen(argv[1],"r"))){
			perror(argv[1]);
			return 1;
		}
		while (yylex()!=0);
	}
	return 0;
}
