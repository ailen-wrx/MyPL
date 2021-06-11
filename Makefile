default: compiler

OBJS = parser.o \
       lexer.o \
	   util.o \
	   node.o \
       codeGen.o \
	   binop.o \
       main.o \

LLVMCONFIG = /opt/llvm/bin/llvm-config
CPPFLAGS = `$(LLVMCONFIG) --cxxflags`
LDFLAGS = `$(LLVMCONFIG) --ldflags` -lpthread -ltinfo
LIBS = `$(LLVMCONFIG) --libs`

lexer.l: parser.cpp

parser.cpp: parser.y node.h codeGen.h
	bison -d -o $@ $<

lexer.cpp: lexer.l
	flex -o $@ $<

%.o: %.cpp node.h codeGen.h
	g++ -g -c $(CPPFLAGS) -o $@ $<

compiler: $(OBJS)
	g++ -g $(CPPFLAGS) $(OBJS) $(LIBS) $(LDFLAGS) -o $@

run: compiler
	./compiler <test.input


.PHONY: clean run
clean:
	rm -f parser.cpp parser.hpp lexer.cpp *.o compiler
	
