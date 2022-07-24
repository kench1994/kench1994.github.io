---
layout: post
title:  "ldd command"
date:   2021-10-27 19:38:13
categories: linux
tags: linux
---

* content
{:toc}

用ldd命令查看可执行文件依赖于哪些共享库：

``` shell
$ ldd main
    linux-gate.so.1=>(0xb7f5c000)
    libstack.so =>not found
    libc.so.6=>/lib/tls/i686/cmov/libc.so.6(0xb7dcf000)
    /lib/ld-linux.so.2(0xb7f42000)
```
ldd模拟运行一遍main，在运行过程中做动态链接，从而得知这个可执行文件依赖于哪些共享库，每个共享库都在什么路径下，加载到进程地址空间的什么地址。`/lib/ld-linux.so.2`是动态链接器，它的路径是在编译链接时指定的，我们在第 2 节 “main函数和启动例程”讲过gcc在做链接时用`-dynamic-linker`指定动态链接器的路径，它也像其它共享库一样加载到进程的地址空间中。`libc.so.6`的路径`/lib/tls/i686/cmov/libc.so.6`是由动态链接器ld-linux.so.2在做动态链接时搜索到的，而`libstack.so`的路径没有找到。`linux-gate.so.1`这个共享库其实并不存在于文件系统中，它是由内核虚拟出来的共享库，所以它没有对应的路径，它负责处理系统调用。总之，共享库的搜索路径由动态链接器决定，从ld.so(8)的Man Page可以查到共享库路径的搜索顺序：


首先在环境变量`LD_LIBRARY_PATH`所记录的路径中查找。


然后从缓存文件`/etc/ld.so.cache`中查找。这个缓存文件由`ldconfig`命令读取配置文件`/etc/ld.so.conf`之后生成，稍后详细解释。

如果上述步骤都找不到，则到默认的系统路径中查找，先是`/usr/lib`然后是`/lib`。