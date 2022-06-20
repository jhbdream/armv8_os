# EEOS

[![GitHub forks](https://img.shields.io/github/forks/jhbdream/armv8_os)](https://github.com/jhbdream/armv8_os/network)
[![GitHub stars](https://img.shields.io/github/stars/jhbdream/armv8_os)](https://github.com/jhbdream/armv8_os/stargazers)
[![GitHub license](https://img.shields.io/github/license/jhbdream/armv8_os)](https://github.com/jhbdream/armv8_os/blob/main/LICENSE)
[![GitHub issues](https://img.shields.io/github/issues/jhbdream/armv8_os)](https://github.com/jhbdream/armv8_os/issues)


> a simple operating system support multi-architecture by qemu enumlature
>
> 支持多架构的操作系统学习，基于qemu模拟器仿真运行
>
> 目前加入 AARCH64\RISCV64 架构基础框架



## 1. 简介

```
1、希望可以通过自己动手编写一个极简的操作系统，来进行操作系统学习

2、支持多处理器架构、包括arm64、riscv64，使用qemu模拟器模拟硬件，做到不依赖开发板，随时随地都可以编写运行程序，测试验证，简化开发流程，重点关注操作系统逻辑开发

3、目前支持在ubuntu系统环境下进行软件代码的开发和编译以及调试
```



## 2. 使用说明

关于 armv8/aarch64 架构的一些说明可以查看前期的一些说明文档 [aarch64](./document/aarch64.md)

为了支持多个架构的软件实现，引入了 kbuild 编译机制，通过menuconfig配置界面选择架构以及驱动参与编译构建

- 工程配置 `make menuconfig`
- 工程编译 `make`
- 运行qemu仿真 `make qemu`



## 3. 工程说明

> arch 架构相关

在 arch 目录下存放各个架构的相关实现，其他目录计划上都是与架构无关的通用逻辑。

但是存在部分驱动程序不可避免是与架构相关的，如中断控制器 arm 使用gic，riscv使用plic。无法实现不同架构的兼容和通用。

为了实现编译以及实现的统一，目前在启动中定义了几个通用的抽象层次

- console 控制台输出
- 中断控制器

其相关驱动在实现完成之后，需要按照框架要求注册相关接口。

在 arch 目录下的架构实现主要包含了 ：

- cpu 启动汇编代码
- 异常向量入口定义与处理
- mmu 页表管理
- 架构相关定时器
- 提供一些全局的辅助功能 全局中断使能、全局中断关闭等



> driver 驱动程序

### 中断子系统

在架构的中断异常入口中，统一调用 `handle_domain_irq`接口，该接口进一步调用中断控制器注册的接口

`int request_irq(unsigned int irq, irq_handler_t handler, unsigned long flags, const char *name, void *data)`

将硬件中断号与中断处理函数绑定



`int set_irq_chip(struct irq_chip *irq_chip)`

注册中断控制器的一些功能接口，主要时硬件中断单独的屏蔽和使能



`int set_handle_irq(void (*handle_irq)(void *))`

设置注册中断控制器的中断处理入口



在中断控制器中完成相关的进入和退出处理后，调用统一的中断处理函数 `    int generic_handle_irq(unsigned int irq)`

该函数进一步调用该硬件中断对应的处理函数



### console 控制台

为了满足编译不同的平台实现无需修改代码并自动使用合适的串口驱动，这里抽象了一个控制台驱动程序。

由串口驱动注册一个串口输入和串口输出接口，如无合适串口驱动未注册相关的接口，则控制台无输出，但是编译以及其他过程不会保错





## 其他

本项目在微信公众号平台同步更新全部的开发教程，欢迎扫码关注：

![a](document/image/a.png)
