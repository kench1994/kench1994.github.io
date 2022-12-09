---
layout: post
title:  GDB Tutorial
date:   2022-12-08 16:08:52
categories: gdb
tags: gdb
---

* content
{:toc}

#### 启动GDB的方法有以下几种：
1.bin也就是你的执行文件，一般在当前目录下。
``gdb --tui --args bin args``

2.用gdb同时调试一个运行程序和core文件，core是程序非法执行后core dump后产生的文件。
``gdb --tui bin coredump``

3.调试已经运行的进程
``gdb bin PID``
（也可以用-p或者--pid指定进程ID，例如：``gdb program -p=10210``）
如果你的程序是一个服务程序，那么你可以指定这个服务程序运行时的进程ID。gdb会自动attach上去，并调试它。program应该在PATH环境变量中搜索到。
``-q``
启动时不显示提示信息
``-tui``			
也可以按 Ctrl + x 然后按 a 进入 TUI（文本用户界面）模式
``Ctrl+l``			
清理程序打印,您可能已经注意到，程序打印到屏幕可能会扰乱调试会话的显示，因此要清理打印并重新绘制 TUI，


#### 调试相关
``bt	backtrace``	查看堆栈
``` shell
(gdb) backtrace
#0  Node<int>::next (this=0x0) at main.cc:28
#1  0x2a16c in LinkedList<int>::remove (this=0x40160, 
    item_to_remove=@0xffbef014) at main.cc:77
#2  0x1ad10 in main (argc=1, argv=0xffbef0a4) at main.cc:111
(gdb)
```

``l	list``		查看源码
``` shell
gdb) l
1	int main(int argc, char **argv)
2	{
3	    int a = 10;
4	    int b = 20;
5	
6	    int result = 0;
7	
8	    result = a + b / a;
9	
10	    return 0;
```

``r	run``			执行到断点
``` shell
(gdb) r
Starting program: /home/imaginemiracle/Miracle/Source_Code/abstract_syntax_tree/test/a.out 

Breakpoint 1, main (argc=1, argv=0x7fffffffe058) at test.c:10
10	    return 0;
```


``p``				打印出源码中定义的变量值，以及其的地址
``` shell
(gdb) p a
$1 = 10
(gdb) pb
Undefined command: "pb".  Try "help".
(gdb) p b
$2 = 20
(gdb) p &a
$3 = (int *) 0x7fffffffdf54
(gdb) p &b
$4 = (int *) 0x7fffffffdf58
```

也可以反过来使用变量的地址和变量的类型来打印出变量的值，这种同样适用于结构体等其它类型。
``` shell
(gdb) p *(int *) 0x7fffffffdf54
$5 = 10
(gdb) p *(int *) 0x7fffffffdf58
$6 = 20
```

``info break`` 				查看断点信息
``b	break``				设置断点
``b 	zx_audio.c: 353``		在zx_audio.c文件的353行加一个断点
``b	func``				在函数func()入口处。
``clear <line_number>``		清除断点
``clear <function_namer>``	清除断点
``d``						删除所有断点
``delete 12`` 				根据GDB分配给断点的标识符，@1、@2...等等来删除；
``whatis a`` 				a的类型
``ptype a`` 				a类型的结构

``info threads`` 
调试多线程
如要查看所有线程的回溯，请使用 thread apply all 命令，后跟

``info register``　 列出寄存器
``info frame``　   列出栈帧
``info files``　　列出当前文件
``info share``　 列出当前共享库
``info source``   查看当前文件的程序语言

#### 找不到源代码的解决方案
https://blog.csdn.net/nicholas_duan/article/details/117515155
``info source``		查看代码路径
``show dir``
``dir 目录``
``set dir 目录1:目录2:目录3`` 设置源码目录，会补上默认目录
``dir``
``pwd``
``cd 目录``
``set substitute-path from-path to-path``

调试的时候找不到源码有多种解决方法，需要根据实际情况选择最合适的解决方案。
- 编译时使用绝对路径时，推荐使用 set substitute-path from-path to-path 的方式。
- 编译时使用相对路径时，使用 set substitute from-path to-path 或者 dir new-path 都可以。
- 对于临时查找一个问题，单独调试某一个文件时使用 cd 命令就可以搞定了。
- 直接在 gdb 环境输入 dir 命令回车确认，可以重置 dir 目录 或者 set dir 目录 命令修改过的源代码搜索目录集合。

#### 移动堆栈
bt (or backtrace) will give you a call stack.
frame <args> will select a frame on the call stack for inspection
info frame <args> will give you information about a specific frame from the stack. When called without arguments it will display the currently selected frame
info locals can give you information about any local variables on the stack.
up 1	往外层的堆栈帧移动一层
down 2  向内层堆栈帧移动二层。如果不指定n，则n默认为1.


#### 调试多进程
set follow-fork-mode child


#### 信号

>gdb通常可以捕捉到发送给它的大多数信号，通过捕捉信号，它就可决定对于正在运行的进程要做些什么工作。例如，按CTRL-C将中断信号发送给gdb，通常就会终止gdb。但是你或许不想中断gdb，真正的目的是要中断gdb正在运行的程序，因此，gdb要抓住该信号并停止它正在运行的程序，这样就可以执行某些调试操作。

Handle命令可控制信号的处理，他有两个参数，一个是信号名，另一个是接受到信号时该作什么。几种可能的参数是：
``nostop`` 接收到信号时，不要将它发送给程序，也不要停止程序。
``stop`` 接受到信号时停止程序的执行，从而允许程序调试；显示一条表示已接受到信号的消息（禁止使用消息除外）
``print`` 接受到信号时显示一条消息
``noprint`` 接受到信号时不要显示消息（而且隐含着不停止程序运行）
``pass`` 将信号发送给程序，从而允许你的程序去处理它、停止运行或采取别的动作。
``nopass`` 停止程序运行，但不要将信号发送给程序。
例如，假定你截获SIGPIPE信号，以防止正在调试的程序接受到该信号，而且只要该信号一到达，就要求该程序停止，并通知你。要完成这一任务，可利用如下命令：
``` shell
(gdb) handle SIGPIPE stop print
```
请注意，UNIX的信号名总是采用大写字母！你可以用信号编号替代信号名
如果你的程序要执行任何信号处理操作，就需要能够测试其信号处理程序，为此，就需要一种能将信号发送给程序的简便方法，这就是signal命令的任务。该 命令的参数是一个数字或者一个名字，如SIGINT。假定你的程序已将一个专用的SIGINT（键盘输入，或CTRL-C；信号2）信号处理程序设置成采 取某个清理动作，要想测试该信号处理程序，你可以设置一个断点并使用如下命令：
``` shell
（gdb） signal 2
continuing with signal SIGINT(2)
```
该程序继续执行，但是立即传输该信号，而且处理程序开始运行.

#### 参考
https://www.gitbook.com/book/wizardforcel/100-gdb-tips