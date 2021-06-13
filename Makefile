default: compiler

OBJS = parser.o \
       lexer.o \
	   util.o \
	   node.o \
       codeGen.o \
       objGen.o \
	   binop.o \
	   builtin.o \
       main.o 


LLVMCONFIG = /opt/llvm/bin/llvm-config
# LLVMCONFIG = llvm-config
CPPFLAGS = `$(LLVMCONFIG) --cxxflags`
LDFLAGS = `$(LLVMCONFIG) --ldflags --libs` -lpthread -ltinfo -ldl -lz -lncurses -rdynamic
LIBS = `$(LLVMCONFIG) --libs`

objGen.cpp: objGen.h

parser.cpp: parser.y codeGen.h node.h
	bison -d -o $@ $<

lexer.l: parser.cpp

lexer.cpp: lexer.l
	flex -o $@ $<

%.o: %.cpp codeGen.h node.h
	g++ -g -c $(CPPFLAGS) -o $@ $<

compiler: $(OBJS)
	g++ -g $(CPPFLAGS) $(OBJS) $(LIBS) $(LDFLAGS) -o $@



.PHONY: clean run test selftest qstest mmtest

run: compiler
	make run -C testcase

test:
	make test -C testcase

selftest:
	make selftest -C testcase

qstest:
	make qstest -C testcase

mmtest:
	make mmtest -C testcase

clean:
	rm -f parser.cpp parser.hpp lexer.cpp *.o compiler

