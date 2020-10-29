obj = ./src/main.o ./src/lexer.o ./src/parser.o ./src/ast.o ./src/table.o
cc = clang
cflags = -Werror -g -o

simplec : $(obj)
	${cc} ${cflags} simplec $(obj)

$(obj) : ./src/simplec.h

.PHONY : clean
clean : 
	rm simplec $(obj)