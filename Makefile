CC      = gcc
CFLAGS  = -Wall -g -I.
FLEX    = flex
BISON   = bison
TARGET  = parser
OBJS    = parser.tab.o lexer.o ast.o sym_table.o types.o typecheck.o

.PHONY: all run clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

parser.tab.c parser.tab.h: parser.y
	$(BISON) -d -o parser.tab.c parser.y

lexer.c: lexer.l parser.tab.h
	$(FLEX) -o lexer.c lexer.l

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@


parser.o: ast.h sym_table.h typecheck.h

typecheck.o: ast.h

ast.o: sym_table.h

sym_table.o: types.h


run: $(TARGET)
	./$(TARGET) test.c

clean:
	rm -f *.o $(TARGET) parser.tab.c parser.tab.h lexer.c