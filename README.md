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



## Documentation

[click to browse](Documentation)



## Compilation and Test

Simply run `make` to compile the project.



## Group members

Chengyuan Pan, Ruixin Wang, Ziyi Luo