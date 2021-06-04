default: compiler

OBJS = main.o

LLVMCONFIG = /opt/llvm/bin/llvm-config
CPPFLAGS = `$(LLVMCONFIG) --cxxflags`
LDFLAGS = `$(LLVMCONFIG) --ldflags`
LIBS = `$(LLVMCONFIG) --libs`

parser.cpp: parser.y
	bison -d -o $@ $<

lexer.cpp: lexer.l
	flex -o $@ $<

%.o: %.cpp
	g++ -c $(CPPFLAGS) -o $@ $<

compiler: $(OBJS)
	g++ $(CPPFLAGS) $(OBJS) $(LIBS) $(LDFLAGS) -o $@

.PHONY: clean
clean:
	rm -f parser.cpp lexer.cpp *.o compiler