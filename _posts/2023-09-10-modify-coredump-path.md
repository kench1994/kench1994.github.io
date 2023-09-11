---
layout: post
title:  linux 修改 coreump 生成路径
date:   2023-09-10 16:55:14
categories: linux
tags: coredump
---

* content
{:toc}

``echo "%e-%p-%t.core" | sudo tee /proc/sys/kernel/core_pattern``

kernel.core_pattern
``` shell
%%  a single % character
%p  PID of dumped process
%u  (numeric) real UID of dumped process
%g  (numeric) real GID of dumped process
%s  number of signal causing dump
%t  time of dump, expressed as seconds since the Epoch (00:00h, 1 Jan 1970, UTC)
%h  hostname (same as nodename returned by uname(2))
%e  executable filename (without path prefix)
%c  core file size soft resource limit of crashing process (since Linux 2.6.24)
```



取消限制进程可开启资源数目
``ulimit -n unlimited``

取消限制生成的core大小
``ulimit -c unlimited``
`` export LD_LIBRARY_PATH=`pwd` ``

测试dump
``sleep 10 & killall -SIGSEGV sleep``