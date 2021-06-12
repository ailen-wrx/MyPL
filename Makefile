default: compiler

OBJS = parser.o \
       lexer.o \
	   util.o \
	   node.o \
       codeGen.o \
       objGen.o \
	   binop.o \
	   builtin.o \
       main.o \

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


.PHONY: clean run output

output: output.o
	g++ output.o -o output -no-pie && ./output

run: compiler
	./compiler <test.input

clean:
	rm -f parser.cpp parser.hpp lexer.cpp *.o compiler
	
