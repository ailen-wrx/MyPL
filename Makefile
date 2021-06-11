default: compiler

OBJS = parser.o \
       lexer.o \
	   util.o \
	   node.o \
       codeGen.o \
	   binop.o \
	   builtin.o \
       main.o \

LLVMCONFIG = /opt/llvm/bin/llvm-config
CPPFLAGS = `$(LLVMCONFIG) --cxxflags`
LDFLAGS = `$(LLVMCONFIG) --ldflags` -lpthread -ltinfo
LIBS = `$(LLVMCONFIG) --libs`

codeGen.h: node.h

parser.cpp: parser.y codeGen.h
	bison -d -o $@ $<

lexer.l: parser.cpp

lexer.cpp: lexer.l
	flex -o $@ $<

%.o: %.cpp codeGen.h
	g++ -g -c $(CPPFLAGS) -o $@ $<

compiler: $(OBJS)
	g++ -g $(CPPFLAGS) $(OBJS) $(LIBS) $(LDFLAGS) -o $@

run: compiler
	./compiler <test.input


.PHONY: clean run
clean:
	rm -f parser.cpp parser.hpp lexer.cpp *.o compiler
	
