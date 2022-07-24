---
layout: post
title:  "0MQ not support IPC on windows"
date:   2020-10-16 19:14:08
categories: c/c++
tags: zmq
---

* content
{:toc}

0MQ不支持UDP。另外，仅在具有POSIX兼容命名管道实现的OS上才支持IPC。因此，在Windows上，您实际上只能使用'inproc'，TCP或PGM。但是，除此以外，0MQ的主要功能之一就是您的协议只是地址的一部分。您可以举任何例子，更改套接字地址，一切仍然可以正常工作（当然要遵守上述限制）。另外[zguide](https://zguide.zeromq.org/)，有很多示例（[python](https://github.com/imatix/zguide/tree/master/examples/Python)中提供了很多示例）