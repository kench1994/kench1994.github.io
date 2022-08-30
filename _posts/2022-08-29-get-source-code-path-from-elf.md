---
layout: post
title:  "get source code path/filename from elf file"
date:   2020-08-29 09:14:19
categories: program
tags: linux
---

* content
{:toc}

I suggest to use this command and tools like sed or grep.
``` shell
$ readelf --string-dump=.debug_str YOUR_PROGRAM
```

this show path and source file name:
``` shell
$ readelf --string-dump=.debug_str YOUR_PROGRAM | sed -n '/\/\|\.c/{s/.*\]  //p}'
```

However, make sure your_program is not stripped by using command
``` shell
file YOUR_PROGRAM
```