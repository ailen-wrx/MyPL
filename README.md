This is the repo of our course design for *Compiler Principles*, spring-summer semester 2021, ZJU.



##  Environment

- Ubuntu 20.04 LTS x86_64
- flex 2.6.4
- bison 3.5.1
- llvm 12.0



### Configuration

1. `sudo apt install flex bison`
2. Download llvm pre-build from: 
 https://github.com/llvm/llvm-project/releases/download/llvmorg-12.0.0/clang+llvm-12.0.0-x86_64-linux-gnu-ubuntu-20.04.tar.xz
3. Extract folder: `tar xf clang+llvm-12.0.0-x86_64-linux-gnu-ubuntu-20.04.tar.xz `
4. Change diretory: `sudo mv clang+llvm-12.0.0-x86_64-linux-gnu-ubuntu-20.04 /opt/llvm`



## Grammar and Documentation

[click to browse](Documentation.md)



## Compilation and Test

Simply run `make` to compile and get our compiler (with name 'compiler').
Our compiler accepts code from standard input. To get code from a file (e.g. `test.input`), run `./compiler <test.input`.

Directory `testcase` contains 2 test files(mm for matrix mulpication and qs for quicksort). `mmtest` and `qstest` are testpoint executables, `mm.in` and `qs.in` are source code file of our grammar, `ourmm` and `ourqs` are executables generated by our compiler and to be tested. To test them, run `make mmtest` and `make qstest` (which will run `./mmtest ./ourmm` and `./qstest ./ourqs`).

Other commands are listed as follows:
- `make run` will run `./compiler <testcase/XXX.in` (`XXX` is defined as `TESTTARGET` in `testcase/Makefile`) and generate object code to `output.o` in directory `testcase`.
- `make test` will run `g++ output.o -o XXX && ./XXXtest ./XXX` in directory `testcase`.
- `make selftest` will run `g++ output.o -o XXX && ./XXX` in directory `testcase`.



## Group members

Chengyuan Pan, Ruixin Wang, Ziyi Luo