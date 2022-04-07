# ARMv8 operating system study

> a simple operating system base armv8 arch by qemu enumlature
>
> 基于armv8处理器架构，基于qemu模拟器仿真、学习操作系统



## 1. 简介

```
1、希望可以通过自己动手编写一个极简的操作系统，来进行操作系统学习

2、基于armv8处理器架构，使用qemu模拟器模拟硬件，做到不依赖开发板，随时随地都可以编写运行程序，测试验证，简化开发流程，重点关注操作系统逻辑开发

3、目前支持在ubuntu系统环境下进行软件代码的开发和编译以及调试
```

​		目前已经完成基础环境搭建和基本代码编写，支持aarch64架构汇编启动、中断和异常处理、串口输入输出，在此基础上进行操作系统相关学习并完成以下内容

希望可以一步步完成一个操作系统的最小系统，窥探其实现原理，并逐渐去添砖加瓦，丰富完善，做到从功能到性能

当前进度以及目标计划：

- 基础组件
  - [x] 中断 GICV3
  - [x] 定时器
  - [ ] MMU
  - [x] qemu仿真
  - [x] 串口输入输出

- 操作系统功能
  - [x] 任务切换
  - [x] 内存管理
  - [ ] 任务同步机制
  - [ ] 任务通信机制



## 2. 使用教程

    本项目在ubuntu系统下运行，需要依赖qemu模拟硬件和aarch64交叉编译工具链编译代码

### 2.1 开发环境搭建

1. 安装qemu工具，建议使用 **ubuntu18** 及以上系统进行安装，否则qemu版本太低，可能无法模拟arm64架构处理器

```shell
sudo apt-get install qemu-system-arm
```

2. 安装交叉编译工具链，用于编译程序。工具链下载地址:

   https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-a/downloads

   

   一般需要下载 x86_64 Linux hosted cross compilers版本，就是说主机是X86_64架构，目标是aarch64架构，交叉编译工具链配置过程如下：

- 下载交叉编译工具文件： **gcc-arm-10.3-2021.07-x86_64-aarch64-none-elf.tar.xz** 

- 配置工具链:

  - 创建一个文件夹用于放置工具链 `sudo mkdir /opt/toolchain` 

  - 解压压缩包到上面的目录

  - `sudo tar -xvf path/gcc-arm-10.3-2021.07-x86_64-aarch64-none-elf.tar.xz /opt/toolchain `

    > 注意：path/gcc-arm-10.3-2021.07-x86_64-aarch64-none-elf.tar.xz 是指实际文件实际下载的目录

- 解压后可删除tar压缩包

  `sudo rm gcc-arm-10.3-2021.07-x86_64-aarch64-none-elf.tar.xz`

3. 配置环境变量:

- 执行下面的命令添加环境变量
  `sudo gedit /etc/profile`

- 在文件的最后一行添加：
  `export PATH=$PATH:/opt/toolchain/gcc-arm-10.3-2021.07-x86_64-aarch64-none-elf/bin`

- 重启虚拟机或者执行命令`source /etc/profile` ,使环境变量配置生效


4. 验证配置结果

   置好交叉编译工具链,输入`aarch64-none-elf-gcc -v`命令出现如下提示表示工具链配置成功（若提示找不到aarch64-none-elf-gcc，可在命令行执行`export PATH=$PATH:/usr/local/arm/gcc-arm-10.3-2021.07-x86_64-aarch64-none-elf/bin`后重试）

   ```shell
   aarch64-none-elf-gcc -v
   
   gcc version 10.3.1 20210621 (GNU Toolchain for the A-profile Architecture 10.3-2021.07 (arm-10.29))
   ```



### 2.2 下载源代码

执行下面的命令从github克隆代码

```shell
git clone https://github.com/jhbdream/armv8_os.git
```

### 2.3 运行程序

使用下面的命令编译代码，启动qemu仿真器执行代码

```
make

make qemu
```

如果环境正常的话，运行正常会有如下打印信息:

```c
qemu-system-aarch64 -machine virt,gic-version=3 -cpu cortex-a57 -smp 1 -m 1024 -nographic -serial mon:stdio -kernel app
run in main!
[    0.002090] EasyLogger V2.2.99 is initialize success.
[    0.003600] i am taska_fun run! line:[21]!
[    0.503791] i am taska_fun run! line:[21]!
[    1.003931] i am taska_fun run! line:[21]!
[    1.004807] irq_enter
[    1.004984] handle_domain_irq: [30]
[    1.005165] switch from pid:[0]!
[    1.005272] switch to pid:[1]!
[    1.005392] irq_exit
[    1.005618] i am taskb_fun run! line:[37]!
[    1.505746] i am taskb_fun run! line:[37]!
[    2.005875] i am taskb_fun run! line:[37]!
[    2.006238] irq_enter
[    2.006288] handle_domain_irq: [30]
[    2.006405] switch from pid:[1]!
[    2.006537] switch to pid:[2]!
[    2.006625] irq_exit
[    2.006721] i am taskc_fun run! line:[47]!
[    2.506848] i am taskc_fun run! line:[47]!
[    3.006940] i am taskc_fun run! line:[47]!
```



## 3. 软件架构介绍

### 3.1 主要目录

> common

​	与硬件无关的逻辑代码，但是也会有调用硬件的接口

> arch/aarch64

​	armv8 aarch64架构的cpu启动代码，完成处理器的一些寄存器配置，准备c运行环境，跳转到`main`函数入口

> driver

​	外设驱动程序，如串口、中断控制器等

> user

​	用户应用程序目录



### 3.2 代码初始化流程

1. 在 gcc.ld 文件中指定了代码的入口函数为`StartUp`,该函数位于libcpu/aarch64/startup.S 文件中

2. _startup 主要由汇编代码组成，完成一些处理器相关寄存器的初始化配置，跳转到c环境中

3. 关于处理器相关配置，该部分与处理器架构相关较大，如果感兴趣可以去了解一下armv8处理器架构相关资料

4. 也可以掠过该部分，重点关注后面的操作系统逻辑

5. 进入c环境之前，比较重要的工作有下面几个部分

   - 对 bss 段进行清零

   - 设置堆栈寄存器


6. 完成上面的设置，可以尽快进入到C语言环境中，方便开发，避免过多设计汇编等语法

   

### 3.3 中断使用

本项目提供了操作系统需要的几个最小组成部分

- 串口输入输出
- 中断、异常
- 定时器中断

通过定时器中断`timer_handler`驱动任务切换调度，基本可以满足开箱即用



## 4. 编译结构介绍

本项目使用了makefile作为编译工具，编译文件的组织方式如下

在项目的根目录存在一个主makefile文件：makefile

为支持多个处理器架构，设计了arch目录，该目录下每个架构会创建一个文件目录存放架构相关程序，并在架构下添加架构的makefile文件： arch_config.mk

因为不同的架构的编译参数如clfag等不相同，故需要使用 arch_config.mk 添加架构私有的编译参数

编译时候需要早主makefile中指定架构和编译器：

```
ARCH =aarch64
CROSS_COMPILE =aarch64-linux-gnu-
```

ARCH 参数从arch目录下的目录名称中选择

编译生成文件介绍：

obj: 该目录是编译生成中间文件存放目录，在编译时候自动创建

os: 该文件时elf格式镜像文件

os.bin: 该文件是binary格式的镜像文件

os.dis:代码的反汇编文件

os.map:代码的map文件



## 5. 其他

本项目使用了 [EasyLogger](https://www.github.com/armink/EasyLogger) 组件用作调试日志输出 ，可以提供非常美观便捷的串口输出功能,可以到项目主页了解详情

本项目在微信公众号平台同步更新全部的开发教程，欢迎扫码关注：

![a](document/image/a.png)
