---
layout: post
title:  多线程与fork()
date:   2022-12-12 19:04:38
categories: c/c++
tags: c/c++
---

* content
{:toc}

## 多线程与fork()
- 多线程与fork()的协作性很差。这是POSIX系列操作系统的历史包袱。因为长期以来程序都是单线程的，fork()运转正常。当20世纪90年代初期引入多线程之后，fork()的适用范围大为缩减
- 在现代Linux glibc中，fork(3)不是直接使用fork(2)系统调用，而是使用clone(2) syscall，不过不影响这里的讨论
### fork()一般不能在多线程中调用
- fork()一般不能在多线程程序中调用。可参阅的文章有：
 http://www.linuxprogrammingblog.com/threads-and-fork-think-twice-before-using-them
 http://www.cppblog.com/lymons/archive/2008/06/01/51836.html
- 因为Linux的fork()只克隆当前线程的thread of control，不克隆其他线程。fork()之后，除了当前线程之外，其他线程都消失了。也就是说不能一下子fork()出一个和父进程一样的多线程子进程
- Linux没有forkall()这样的系统调用，forkall()其实也是很难办的（从语意上），因为其他线程可能等在condition variable上，可能阻塞在系统调用上，可能等着mutex以跨入临界区，还可能在密集的计算中，这些都不好全盘搬到子进程里

### 多线程中fork()可能产生的错误，以及调用函数的相关注意事项
- fork()之后子进程中只有一个线程，其他线程都消失了，这就造成一个危险的局面：
其他线程可能正好位于临界区之内，持有了某个锁， 而它突然死亡，再也没有机会去解锁了
如果子进程试图再对同一个mutex加锁，就会立刻死锁
- 在fork()之后，子进程就相当于处于signal handler之中，你不能调用线程安全的函数（除非它是可重入的），而只能调用异步信号安全（async-signal-safe）的函数
- 比方说，fork()之后，子进程不能调用：
    - malloc。因为malloc()在访问全局状态时几乎肯定会加锁
    - 任何可能分配或释放内存的函数，包括new、map::insert()、 snprintf（在浮点数转换为字符串时有可能需要动态分配内存）……等等
    - 任何Pthreads函数。你不能用pthread_cond_signal()去通知父进程， 只能通过读写pipe来同步（见 http://github.com/chenshuo/muduo-protorpc中Zurg slave示例的Process::start()）
    - printf()系列函数，因为其他线程可能恰好持有stdout/stderr的锁
    - 除了man 7 signal中明确列出的“signal安全”函数之外的任何函数

- 照此看来，唯一安全的做法是：在fork()之后立即调用exec()执行另一个程序，彻底隔断子进程与父进程的联系
- 不得不说，同样是创建进程，Windows的CreateProcess()函数的顾虑要少得多，因为它创建的进程跟当前进程关联较少

-----------------------------------
©著作权归作者所有：来自51CTO博客作者董哥的黑板报的原创作品，请联系作者获取转载授权，否则将追究法律责任
muduo网络库：15---C++多线程编程精要之（RAII与fork()、多线程与fork()、多线程与signal）
https://blog.51cto.com/u_15346415/3674259