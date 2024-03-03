flex cf.l
bison -d syntax.y
gcc syntax.tab.c operand.c tree.c SymbolTable.c semantic.c Simulater.c -lfl -ly -o parser
