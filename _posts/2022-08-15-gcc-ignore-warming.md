---
layout: post
title: "gcc如何忽略第三方库引发的告警"
date: 2022-08-15 09:24:49
categories: program
tags: gcc
---

* content
{:toc}

利用代码可以精准实现忽略处理，思路：
参考[《6.61.12 Diagnostic Pragmas》](https://link.jianshu.com/?t=https%3A%2F%2Fgcc.gnu.org%2Fonlinedocs%2Fgcc%2FDiagnostic-Pragmas.html%23Diagnostic-Pragmas)可以解决。示例代码：
``` c++
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wall"
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wsign-conversion"
#pragma GCC diagnostic ignored "-Wshadow"
#endif
#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif
```
> 特殊情况说明

``diagnostic ignored``不识别多个告警写一行情况，也不报错。
``` c++
#pragma GCC diagnostic ignored "-Wall -Wformat=2 -Wconversion -Wsign-conversion -Wshadow"
```

如果需要忽略 ``-Wformat=2``，应该使用忽略 ``-Wformat``。
``` c++
//编译错误
#pragma GCC diagnostic ignored "-Wformat=2"
//正确
#pragma GCC diagnostic ignored "-Wformat"
```