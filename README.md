This is the repo of our course design for *Compiler Principles*, spring-summer semester 2021, ZJU.



##  Environment

 - Ubuntu 20.04 LTS (or lower version)
 - cmake 3.16.3
 - flex 2.6.4
 - bison 3.5.1
 - llvm 6.0.0 (or higher version)



### Configuration

 - sudo apt-get install flex bison

 - Download llvm src: https://releases.llvm.org/6.0.0/llvm-6.0.0.src.tar.xz

 - Extract folder as `llvm-6.0.0`

 - ```bash
   cd .../llvm-6.0.0
   mkdir build
   cd build
   cmake .. -DLLVM_TARGETS_TO_BUILD=X86 -DCMAKE_BUILD_TYPE=Debug
   make -jN
   make install
   ```

 - ```bash
   # Expand swap space (if needed)
   sudo su
   mkdir /swap
   cd /swap
   sudo dd if=/dev/zero of=swapfile bs=1024 count=20000000
   sudo mkswap -f swapfile
   sudo swapon swapfile
   ```

   

 



## Group members

Chengyuan Pan, Ruixin Wang, Ziyi Luo