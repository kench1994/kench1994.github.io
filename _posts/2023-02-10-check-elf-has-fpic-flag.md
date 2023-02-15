---
layout: post
title:  Linux 查看elf是否启用了fPIC标志
date:   2023-02-10 09:26:10
categories: linux
tags: linux
---

* content
{:toc}
``` shell
[user@host lib]$ readelf -d libMyLib1.so | grep TEXT   # Bad, not -fPIC
 0x00000016 (TEXTREL)
 
[user@host lib]$ readelf -d libMyLib2.so | grep TEXT   # Good, -fPIC
```