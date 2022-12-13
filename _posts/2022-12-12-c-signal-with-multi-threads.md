---
layout: post
title:  Signal handling with multiple threads in Linux
date:   2022-12-12 20:12:39
categories: c/c++
tags: c/c++
---

* content
{:toc}

- Signal handlers are per-process state - that is, all the threads in a process share the same set of installed signal handler functions.
- Signal masks are per-thread state. Signals can be blocked or unblocked on a per-thread basis.
- Signals can be process- or thread-directed. If a signal is process-directed, then an arbitrary thread which does not currently have the signal type blocked is chosen to handle it.

A simple way to handle signals in a multi-threaded application is to create one thread as a dedicated signal-handling thread. All signals of interest are blocked in every thread; no signal handlers are established; and the signal-handling thread calls sigwaitinfo() in a loop, acting on the signals as they're received.

This means that you don't need to worry about whether the functions you want to call are async-signal-safe or not, because signals aren't handled in signal handlers - they're handled synchronously by your dedicated signal-handling thread, which can call any function it likes (for example, it can use the ordinary pthreads synchronisation functions to wake up another thread).