default: compiler

OBJS = main.o

LLVMCONFIG = /opt/llvm/bin/llvm-config
CPPFLAGS = `$(LLVMCONFIG) --cxxflags`
LDFLAGS = `$(LLVMCONFIG) --ldflags`
LIBS = `$(LLVMCONFIG) --libs`

%.o: %.cpp
	g++ -c $(CPPFLAGS) -o $@ $<

compiler: $(OBJS)
	g++ $(CPPFLAGS) -o $@ $(OBJS) $(LIBS) $(LDFLAGS)

.PHONY: clean
clean:
	rm $(OBJS)