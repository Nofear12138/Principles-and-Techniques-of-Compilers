%{
	#include "syntax.tab.h"
      extern int errornum;
      YYLTYPE yylloc;
	int column = 1;
	int lineno=1;
	int templineno=1;
	int yycolumn = 1;
      #define YY_USER_ACTION  \
		yylloc.first_line = yylloc.last_line = yylineno; \
             yylloc.first_column = yycolumn; \
             yylloc.last_column = yycolumn + yyleng - 1; \
             yycolumn += yyleng;

%}
%option yylineno
DIGIT[0-9]
INT 0|[1-9]{DIGIT}*
INT8 0(0|[1-7][0-7]*)
ILLGALINT8 0([0-7]*)?[8-9]+[0-9]*
INT16 0(x|X)[0-9a-fA-F]+
ILLGALINT16 0[x|X][0-9a-fA-F]*[g-zG-Z][0-9a-zA-Z]*
FLOAT ([0-9]*\.[0-9]+|[0-9]+\.)([eE][+-]?[0-9]+)?
ILLGALFLOAT [0-9]*\.?[0-9]*[eE]([A-Za-z0-9]*)\.?([A-Za-z0-9]*)
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
LP \(
RP \)
LB \[
RB \]
LC \{
RC \}
TYPE int|float
IF if
ELSE else
WHILE while
RETURN return
STRUCT struct
NOTE (\/\/.*)
NOTEE (\/\*[^"*/"]*\*\/)
SPACE " "
ENTER \n
TAB \t
%%
{ENTER} {yycolumn=1;}
{NOTEE} {}
{NOTE} {}
{TYPE} {
	lineno=templineno;
	templineno=yylineno;
    Node *nodeTYPE = CreateNode("TYPE", 0, STRING_TYPE);
	yylval = nodeTYPE;
	nodeTYPE->id = strdup(yytext);
	return TYPE;
}
{SEMI} {
	lineno=templineno;
	templineno=yylineno;
	Node *nodeSEMI = CreateNode("SEMI", 0, STRING_TYPE);
	yylval = nodeSEMI;
	nodeSEMI->id = strdup(yytext);
	return SEMI;
}
{COMMA} {
	lineno=templineno;
	templineno=yylineno;
	Node *nodeCOMMA = CreateNode("COMMA", 0, STRING_TYPE);
	yylval = nodeCOMMA;
	nodeCOMMA->id = strdup(yytext);
	return COMMA;
}
{IF} {
	lineno=templineno;
	templineno=yylineno;
	Node *nodeIF = CreateNode("IF", 0, STRING_TYPE);
	yylval = nodeIF;
	nodeIF->id = strdup(yytext);
	return IF;
}
{ELSE} {
	lineno=templineno;
	templineno=yylineno;
	Node *nodeELSE = CreateNode("ELSE", 0, STRING_TYPE);
	yylval = nodeELSE;
	nodeELSE->id = strdup(yytext);
	return ELSE;
}
{WHILE} {
	lineno=templineno;
	templineno=yylineno;
	Node *nodeWHILE = CreateNode("WHILE", 0, STRING_TYPE);
	yylval = nodeWHILE;
	nodeWHILE->id = strdup(yytext);
	return WHILE;
}
{RETURN} {
	lineno=templineno;
	templineno=yylineno;
	Node *nodeRETURN = CreateNode("RETURN", 0, STRING_TYPE);
	yylval = nodeRETURN;
	nodeRETURN->id = strdup(yytext);
	return RETURN;
}
{STRUCT} {
	lineno=templineno;
	templineno=yylineno;
	Node *nodeSTRUCT = CreateNode("STRUCT", 0, STRING_TYPE);
	yylval = nodeSTRUCT;
	nodeSTRUCT->id = strdup(yytext);
	return STRUCT;
}
{PLUS} {
	lineno=templineno;
	templineno=yylineno;
	Node *nodePLUS = CreateNode("PLUS", 0, STRING_TYPE);
	yylval = nodePLUS;
	nodePLUS->id = strdup(yytext);
	return PLUS;
}
{MINUS} {
	lineno=templineno;
	templineno=yylineno;
	Node *nodeMINUS = CreateNode("MINUS", 0, STRING_TYPE);
	yylval = nodeMINUS;
	nodeMINUS->id = strdup(yytext);
	return MINUS;
}
{STAR} {
	lineno=templineno;
	templineno=yylineno;
	Node *nodeSTAR = CreateNode("STAR", 0, STRING_TYPE);
	yylval = nodeSTAR;
	nodeSTAR->id = strdup(yytext);
	return STAR;
}
{DIV} {
	lineno=templineno;
	templineno=yylineno;
	Node *nodeDIV = CreateNode("DIV", 0, STRING_TYPE);
	yylval = nodeDIV;
	nodeDIV->id = strdup(yytext);
	return DIV;
}
{AND} {
	lineno=templineno;
	templineno=yylineno;
	Node *nodeAND = CreateNode("AND", 0, STRING_TYPE);
	yylval = nodeAND;
	nodeAND->id = strdup(yytext);
	return AND;
}
{OR} {
	lineno=templineno;
	templineno=yylineno;
	Node *nodeOR = CreateNode("OR", 0, STRING_TYPE);
	yylval = nodeOR;
	nodeOR->id = strdup(yytext);
	return OR;
}
{LP} {
	lineno=templineno;
	templineno=yylineno;
	Node *nodeLP = CreateNode("LP", 0, STRING_TYPE);
	yylval = nodeLP;
	nodeLP->id = strdup(yytext);
	return LP;
}
{RP} {
	lineno=templineno;
	templineno=yylineno;
	Node *nodeRP = CreateNode("RP", 0, STRING_TYPE);
	yylval = nodeRP;
	nodeRP->id = strdup(yytext);
	return RP;
}
{LB} {
	Node *nodeLB = CreateNode("LB", 0, STRING_TYPE);
	yylval = nodeLB;
	nodeLB->id = strdup(yytext);
	return LB;
}
{RB} {
	lineno=templineno;
	templineno=yylineno;
	Node *nodeRB = CreateNode("RB", 0, STRING_TYPE);
	yylval = nodeRB;
	nodeRB->id = strdup(yytext);
	return RB;
}
{LC} {
	lineno=templineno;
	templineno=yylineno;
	Node *nodeLC = CreateNode("LC", 0, STRING_TYPE);
	yylval = nodeLC;
	nodeLC->id = strdup(yytext);
	return LC;
}
{RC} {
	lineno=templineno;
	templineno=yylineno;
	Node *nodeRC = CreateNode("RC", 0, STRING_TYPE);
	yylval = nodeRC;
	nodeRC->id = strdup(yytext);
	return RC;
}
{INT8} {
	lineno=templineno;
	templineno=yylineno;
	Node *nodeINT8 = CreateNode("INT8", 0, STRING_TYPE);
	yylval = nodeINT8;
	nodeINT8->id = strdup(yytext);
	return INT;
}
{ILLGALINT8} {
	lineno=templineno;
	templineno=yylineno;
	errornum++;
	column = yycolumn-yyleng;
	printf("Error Type A at line %d,char %d: Illgal octal number: %s\n",yylineno,column,yytext);
	Node *nodeILLGALINT8 = CreateNode("ILLGALINT8", 0, STRING_TYPE);
	yylval = nodeILLGALINT8;
	nodeILLGALINT8->id = strdup(yytext);
	return INT;
}
{INT16} {
	lineno=templineno;
	templineno=yylineno;
	printf("INT16 at line %d,char %d:number: %s\n",yylineno,column,yytext);
	Node *nodeINT16 = CreateNode("INT16", 0, STRING_TYPE);
	yylval = nodeINT16;
	nodeINT16->id = strdup(yytext);
	return INT;
}
{ILLGALINT16} {
	lineno=templineno;
	templineno=yylineno;
	errornum++;
	column = yycolumn-yyleng;
	printf("Error Type A at line %d,char %d: Illgal hexadecimal number: %s\n",yylineno,column,yytext);
	Node *nodeILLGALINT16 = CreateNode("ILLGALINT16", 0, STRING_TYPE);
	yylval = nodeILLGALINT16;
	nodeILLGALINT16->id = strdup(yytext);
	return INT;
}
{INT} {
	lineno=templineno;
	templineno=yylineno;
	Node *nodeINT = CreateNode("INT", 0, INT_TYPE);
	yylval = nodeINT;
	nodeINT->intvalue = atoi(yytext);
	return INT;
}
{FLOAT} {
	lineno=templineno;
	templineno=yylineno;
	Node *nodeFLOAT = CreateNode("FLOAT", 0, FLOAT_TYPE);
	yylval = nodeFLOAT;
	nodeFLOAT->floatvalue = atof(yytext);
	return FLOAT;
}
{ID} {
	lineno=templineno;
	templineno=yylineno;
	Node *nodeID = CreateNode("ID", 0, STRING_TYPE);
	yylval = nodeID;
	nodeID->id = strdup(yytext);
	//printf("cf.l: id = %s\n",nodeID->id);
	return ID;
}
{ILLGALFLOAT} {
	lineno=templineno;
	templineno=yylineno;
	errornum++;
	column = yycolumn-yyleng;
	printf("Error Type A at line %d,char %d: Illgal float number :%s\n",yylineno,column,yytext);
	Node *nodeILLGALFLOAT = CreateNode("ILLGALFLOAT", 0, STRING_TYPE);
	yylval = nodeILLGALFLOAT;
	nodeILLGALFLOAT->id = strdup(yytext);
	return FLOAT;
}
{ASSIGNOP} {
	lineno=templineno;
	templineno=yylineno;
	Node *nodeASSIGNOP = CreateNode("ASSIGNOP", 0, STRING_TYPE);
	yylval = nodeASSIGNOP;
	nodeASSIGNOP->id = strdup(yytext);
	return ASSIGNOP;
}
{RELOP} {
	lineno=templineno;
	templineno=yylineno;
	Node *nodeRELOP = CreateNode("RELOP", 0, STRING_TYPE);
	yylval = nodeRELOP;
	nodeRELOP->id = strdup(yytext);
	return RELOP;
}
{DOT} {
	lineno=templineno;
	templineno=yylineno;
	Node *nodeDOT = CreateNode("DOT", 0, STRING_TYPE);
	yylval = nodeDOT;
	nodeDOT->id = strdup(yytext);
	return DOT;
}
{NOT} {
	lineno=templineno;
	templineno=yylineno;
	Node *nodeNOT = CreateNode("NOT", 0, STRING_TYPE);
	yylval = nodeNOT;
	nodeNOT->id = strdup(yytext);
	return NOT;
}
{SPACE} {}
{TAB} {yycolumn+=3;}
. {
	errornum++;
	column = yycolumn-yyleng;
	printf("Error Type A at line %d,char %d: Mysterious charcter:'%s'\n",yylineno,column,yytext);
	
}
