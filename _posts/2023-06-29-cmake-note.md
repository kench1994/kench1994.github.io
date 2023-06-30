---
layout: post
title:  cmake 备忘
date:   2023-06-29 13:36:42
categories: cmake 
tags: cmake
---

* content
{:toc}

### 1. 生成构建系统(Unix Makefile、Ninja、Visual Studio等)

``` shell
cmake -H. -Bbuild
```
- -H表示当前目录中搜索根CMakeLists.txt文件。
- -Bbuild告诉CMake在一个名为build的目录中生成所有的文件。

##### 相当于

``` shell
$ mkdir -p build
$ cd build
$ cmake ..
```

### 2. 开始编译
``` shell
$ cmake --build ./build/ [--target <target-name>]
```