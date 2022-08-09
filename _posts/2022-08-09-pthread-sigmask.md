---
layout: post
title: "pthread_sigmask"
date: 2022-08-08 09:17:30
categories: linux
tags: c/c++
---

* content
{:toc}

From posix ```pthread_sigmask```:

> The pthread_sigmask() function shall examine or change (or both) the calling thread's signal mask, regardless of the number of threads in the process. The function shall be equivalent to sigprocmask(), without the restriction that the call be made in a single-threaded process.
In a single-threaded process, the sigprocmask() function shall examine or change (or both) the signal mask of the calling thread.

From ```sigprocmask```:

> In a single-threaded process, the sigprocmask() function allows the calling process to examine or change (or both) the signal mask of the calling thread.

.....

The use of the sigprocmask() function is unspecified in a multi-threaded process.

At the start, each process has one thread, the one that runs main(). The sigprocmask() can be run only in single-threaded process. pthread_sigmask() can be run in any-number-of-threaded process.

> Can I use pthread_sigmask without thread?

Yes.

> Is it ok to call pthread_sigmask without thread or should I use sigprocmask in the second case?

It's ok.

You tagged linux. Most linux use glibc, and in glibc pthread_sigmask() calls the same syscall rt_sigprocmask or just the function sigprocmask(). So on glibc they do exactly the same thing.