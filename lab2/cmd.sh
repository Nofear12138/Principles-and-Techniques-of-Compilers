flex yf.l
bison -d syntax.y
gcc syntax.tab.c -lfl -o parser tree.c
