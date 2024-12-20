---
layout: post
title:  c++动态库符号隐藏
date:   2024-12-19 10:32:41
categories: c/c++
tags: 动态库
---

* content
{:toc}

**linux**:symbols.ver

``` shell
libxxx.so {

global: 
	init;
	stop;
local: 
    *;
};
```

**windows**:symbols.def
``` shell
LIBRARY    "xxx"

EXPORTS
    init    @1
    stop    @2
```