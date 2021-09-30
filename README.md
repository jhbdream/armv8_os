# ARMv8 operating system study
> a simple operating system base armv8 arch by qemu enumlature

## 1. 简介
希望可以基于 qemu 进行操作系统学习，目前计划基于armv8处理器架构，使用qemu模拟器模拟硬件，做到不依赖开发板，随时随地都可以编写运行程序，测试验证，简化开发流程，重点关注操作系统逻辑开发

目前已经完成基础环境搭建和基本代码编写，支持aarch64架构汇编启动、中断和异常处理、串口输入输出，在此基础上进行操作系统相关学习完成以下内容

希望可以一步步完成一个操作系统的最小系统，窥探其实现原理，并逐渐去添砖加瓦，丰富完善，做到从功能到性能

- 基础组件
    - [x] 中断 GICV3
    - [x] 定时器
    - [ ] MMU
    - [x] qemu仿真
    - [x] 串口输入输出

- 操作系统功能
    - [ ] 任务切换
    - [ ] 内存管理
    - [ ] 任务同步机制
    - [ ] 任务通信机制

## 2. 使用教程
    本项目在ubuntu系统下运行，需要依赖qemu模拟硬件和aarch64交叉编译工具链编译代码

> A.搭建环境

安装qemu工具，建议使用ubuntu18及以上系统进行安装，否则qemu版本太低，可能无法模拟arm64架构处理器

```shell
sudo apt-get install system-qemu-arm
```
安装交叉编译工具链，用于编译程序,工具链下载地址:
 https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-a/downloads

 一般需要下载 x86_64 Linux hosted cross compilers版本，就是说主机是X86_64架构，目标是aarch64架构，下载之后配置好交叉编译工具链,输入`aarch64-none-elf-gcc -v`命令出现如下提示表示工具链配置成功

 ```
aarch64-none-elf-gcc -v
Using built-in specs.
COLLECT_GCC=aarch64-none-elf-gcc
COLLECT_LTO_WRAPPER=/opt/toolchain/gcc-arm-10.3-2021.07-x86_64-aarch64-none-elf/bin/../libexec/gcc/aarch64-none-elf/10.3.1/lto-wrapper
Target: aarch64-none-elf
Configured with: /data/jenkins/workspace/GNU-toolchain/arm-10/src/gcc/configure --target=aarch64-none-elf --prefix=/data/jenkins/workspace/GNU-toolchain/arm-10/build-aarch64-none-elf/install// --with-gmp=/data/jenkins/workspace/GNU-toolchain/arm-10/build-aarch64-none-elf/host-tools --with-mpfr=/data/jenkins/workspace/GNU-toolchain/arm-10/build-aarch64-none-elf/host-tools --with-mpc=/data/jenkins/workspace/GNU-toolchain/arm-10/build-aarch64-none-elf/host-tools --with-isl=/data/jenkins/workspace/GNU-toolchain/arm-10/build-aarch64-none-elf/host-tools --disable-shared --disable-nls --disable-threads --disable-tls --enable-checking=release --enable-languages=c,c++,fortran --with-newlib --with-pkgversion='GNU Toolchain for the A-profile Architecture 10.3-2021.07 (arm-10.29)' --with-bugurl=https://bugs.linaro.org/
Thread model: single
Supported LTO compression algorithms: zlib
gcc version 10.3.1 20210621 (GNU Toolchain for the A-profile Architecture 10.3-2021.07 (arm-10.29))
 ```


> B.下载代码

执行如下命令从GitHub下载代码,并拉取关联仓库代码

```
git clone https://github.com/jhbdream/armv8_os.git

git submodule init

git submodule update
```

> C.运行程序

使用下面的命令编译代码，启动仿真器执行代码

```
make

make qemu
```
运行正常会有如下打印信息

```
make qemu
qemu-system-aarch64 -machine virt,gic-version=3 -cpu cortex-a57 -smp 1 -m 1024 -nographic -serial mon:stdio -kernel app
I/elog     [    0.156507] EasyLogger V2.2.99 is initialize success.
I/"common/interrupt/interrupt.c" [    1.382065] irq_enter
I/"common/interrupt/interrupt.c" [    1.382931] handle_domain_irq: [30]
I/"user/board.c" [    1.383767] arch timer_handler!
I/"common/interrupt/interrupt.c" [    1.383945] irq_exit
```

## 3. 软件架构介绍
> common

与硬件无关的逻辑代码，但是也会有调用硬件的接口

> libcpu/aarch64

armv8 aarch64架构的cpu启动代码，完成处理器的一些寄存器配置，准备c运行环境，跳转到`main`函数入口

> driver

外设驱动程序，如串口、中断控制器等

> user

用户应用程序目录
