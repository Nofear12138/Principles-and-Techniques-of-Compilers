flex cf.l
bison -d syntax.y
gcc syntax.tab.c tree.c SymbolTable.c semantic.c  -lfl -ly -o parser
