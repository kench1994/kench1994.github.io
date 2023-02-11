---
layout: post
title: --host,--target，--build
date:  2023-02-10 11:46:22
categories: linux
tags: linux
---

* content
{:toc}

>在交叉编译中比较 常见 的一些参数就是build、host和target了，正确的理解这三者的含义对于交叉编译是非常重要的，下面就此进行解释
``` shell
--build=编译该软件所使用的平台
--host=该软件将运行的平台
--target=该软件所处理的目标平台
 ```

build：自动测试在用平台名称，若无法检测出来则需要指定。
host：若无指定，自动使用build的结果。

build和host相同时表示本地编译，若不相同则表示交叉编译。

target：表示需要处理的目标平台名称，若无指定使用host相同名称，gcc、binutils等于平台指令相关软件有此参数，多数软件此参数无用处。