---
layout: post
title: "守护进程实现步骤"
date: 2022-08-18 17:39:24
categories: program
tags: demaon
---

* content
{:toc}


本文通过将一个演示服务改造成守护守护进程，以此介绍 实现守护进程的必要步骤 。演示服务每隔 10 秒打印一条 syslog 日志：
``` c++
#include <syslog.h>
#include <unistd.h>
void echo_forever(char *ident, char *msg)
{
    openlog(ident, (LOG_CONS | LOG_PID), LOG_USER);
    setlogmask(LOG_UPTO(LOG_INFO));
    for(;;) {
        syslog(LOG_ERR, "%s\n", msg);
        sleep(10);
    }
}
int main(int argc, const char *argv[])
{
    echo_forever("echo-forever", "Hello, world!");
}
```
为了将服务改造成守护进程，需要实现一个函数 daemonize ：
``` c++
int daemonize();
```
该函数负责初始化守护进程，通过返回值可以判断执行是否成功：

返回值 | 状态
| :-------- | --------:| 
0	| 成功，且处于守护进程(子进程)上下文
-1	| 失败，错误信息见error
这样一来，只需在服务入口先调用 daemonize 函数即可完成守护进程初始化：
``` c++
int main(int argc, const char *argv[])
{
    if (daemonize() == 0) {
        echo_forever("echo-forever", "Hello, world!");
    }
}
```
编译并启动守护服务：
``` shell
$ make all
$ ./echod
```
注意到，运行 echod 后，程序立马返回了。通过 ps 命令，可以确定 echod 已经作为守护进程在后台运行了：
``` shell
$ ps aux | grep echod
fasion   30807  1.0  0.0   4352    80 ?        S    20:17   0:00 ./echod
fasion   30838  0.0  0.0  14224   932 pts/11   S+   20:17   0:00 grep --color=auto echod
```
syslog 日志也可以看到服务不断打印出来的信息：
``` shell
$ tail -f /var/log/syslog
Jan  9 20:19:10 ant echo-forever[30807]: Hello, world!
Jan  9 20:19:40 ant echo-forever[30807]: message repeated 3 times: [ Hello, world! ]
Jan  9 20:19:50 ant echo-forever[30807]: Hello, world!
```
实现步骤
回头来看看 daemonize 函数的实现，这是本文的重点——初始化守护进程的关键步骤：
``` c++ {.numberLines}
#include <fcntl.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <unistd.h>
int
daemonize()
{
    // fork to run in background
    int pid = fork();
    if (pid == -1) {
        return -1;
    }
    else if (pid != 0) {
        exit(0);
    }
    // become session leader to lose controlling TTY
    setsid();
    // fork again in order to forbid reallocating new controlling TTY
    pid = fork();
    if (pid == -1) {
        return -1;
    }
    else if (pid != 0) {
        exit(0);
    }
    // get maximun number of file descriptors
    struct rlimit rl;
    if (getrlimit(RLIMIT_NOFILE, &rl) < 0) {
        return -1;
    }
    if (rl.rlim_max == RLIM_INFINITY) {
        rl.rlim_max = 1024;
    }
    // close all open file descriptors
    for (int i=0; i<rl.rlim_max; i++) {
        close(i);
    }
    // reopen stdin, stdout and stderr
    int stdin = open("/dev/null", O_RDWR);
    int stdout = dup(stdin);
    int stderr = dup(stdin);
    if (stdin != 0 || stdout != 1 || stderr != 2) {
        return -1;
    }
    // change the current working directory to root
    // in order not to prevent file system from being umounted
    if (chdir("/") < 0) {
        return -1;
    }
    // clear file creation mask
    umask(0);
    // set up signal handler
    struct sigaction sa;
    sa.sa_handler = SIG_IGN;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGHUP, &sa, NULL) == -1) {
        return -1;
    }
    return 0;
}
```
- 第 12-18 行，调用 fork 系统调用派生子进程，以便实现：①如果守护进程由 shell 启动，父进程退出会让 shell 认为命令执行完毕；②子进程虽继承了父进程的进程组 ID ，但获得新的进程 ID ，保证子进程不是组长进程，为调用 setsid 做准备。
- 第 21 行，调用 setsid 创建新会话，使调用进程：①成为新会话首进程；②新进程组组长；③没有控制终端(脱离控制终端)。
- 再次调用 fork 并终止父进程，保证守护进程(子进程)不是会话首进程，防止它再次取得控制终端。
- 第 42-44 行，关闭所有文件描述符，请注意进程文件描述符范围是如何确定的。
- 第 47-52* 行，重新打开 标准输入 ( 0 )、 标准输出 ( 1 )以及 标准错误 ( 2 )，并将其重定向到 /dev/null 上。这样一来，任何试图访问标准输入输出的程序代码均不会产生任何效果。
- 第 56-58 行，将当前工作目录更改为根目录，避免有文件系统因守护进程而无法卸载。
- 第 61 行，调用 umask 将文件模式屏蔽位设置为一个已知值，一般为 0 。因为继承而来的屏蔽位，可能设置为屏蔽某些权限。
- 第 64-70 行，设置信号处理函数，忽略信号 SIGHUP 。