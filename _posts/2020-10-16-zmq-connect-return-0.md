---
layout: post
title:  "ZeroMQ connect return 0 even when no server listen)"
date:   2020-10-16 19:14:08
categories: c/c++
tags: zmq
---

* content
{:toc}

For most transports and socket types the connection is not performed immediately but as needed by ØMQ. Thus a successful call to zmq_connect() does not mean that the connection was or could actually be established. Because of this, for most transports and socket types the order in which a server socket is bound and a client socket is connected to it does not matter. The first exception is when using the inproc:// transport: you must call zmq_bind() before calling zmq_connect(). The second exception are ZMQ_PAIR sockets, which do not automatically reconnect to endpoints.