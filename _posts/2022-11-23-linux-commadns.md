---
layout: post
title:  Linux 常用指令
date:   2022-11-23 13:11:11
categories: linux
tags: linux
---

* content
{:toc}

### **top** 
可以实时动态地查看系统的整体运行情况，是一个综合了多方信息监测系统性能和运行信息的实用工具。通过top命令所提供的互动式界面，用热键可以管理。

- 表头含义：

PID | USER | PR | VIRT | RES | SHR |
 - | - | - | - | - | - | - | 
进程id | 该进程对应的用户 | 优先级 | 虚拟内存 | 常驻内存 | 共享内存

计算一个进程实际使用的内存 = 常驻内存（RES）- 共享内存（SHR）

S | %CPU | %MEM | TIME+ | COMMAND | 
 - | - | - | - | - | - | - | 
表示进程的状态status（sleeping，其中S表示睡眠，R表示运行） | 表示CPU的占用百分比 | 表示内存的占用百分比 | 执行的时间 | 进程的名称


- 选项
```
-b：以批处理模式操作；
-c：显示完整的治命令；
-d：屏幕刷新间隔时间；
-I：忽略失效过程；
-s：保密模式；
-S：累积模式；
-i<时间>：设置间隔时间；
-u<用户名>：指定用户名；
-p<进程号>：指定进程；
-n<次数>：循环显示的次数。
 ```

- 交互命令
``` 
h：显示帮助画面，给出一些简短的命令总结说明；
k：终止一个进程；
i：忽略闲置和僵死进程，这是一个开关式命令；
q：退出程序；
r：重新安排一个进程的优先级别；
S：切换到累计模式；
s：改变两次刷新之间的延迟时间（单位为s），如果有小数，就换算成ms。输入0值则系统将不断刷新，默认值是5s；
f或者F：从当前显示中添加或者删除项目；
o或者O：改变显示项目的顺序；
l：切换显示平均负载和启动时间信息；
m：切换显示内存信息；
t：切换显示进程和CPU状态信息；
c：切换显示命令名称和完整命令行；
M：根据驻留内存大小进行排序；
P：根据CPU使用百分比大小进行排序；
T：根据时间/累计时间进行排序；
w：将当前设置写入~/.toprc文件中。
 ```

### **netstat**
用来打印Linux中网络系统的状态信息，可让你得知整个Linux系统的网络情况。

- 语法

```
-n或--numeric：直接使用ip地址，而不通过域名服务器；
-p或--programs：显示发起连接的进程pid和进程名称；
-l或--listening：显示状态为 LISTEN（监听）的连接；
-t或--tcp：显示TCP传输协议的连线状况；
-u或--udp：显示UDP传输协议的连线状况；

-a或--all：显示所有连线中的Socket；
-A<网络类型>或--<网络类型>：列出该网络类型连线中的相关地址；
-c或--continuous：持续列出网络状态；
-C或--cache：显示路由器配置的快取信息；
-e或--extend：显示网络其他相关信息；
-F或--fib：显示FIB；
-g或--groups：显示多重广播功能群组组员名单；
-h或--help：在线帮助；
-i或--interfaces：显示网络界面信息表单；
-M或--masquerade：显示伪装的网络连线；
-N或--netlink或--symbolic：显示网络硬件外围设备的符号连接名称；
-o或--timers：显示计时器；
-r或--route：显示Routing Table；
-s或--statistice：显示网络工作信息统计表；
-v或--verbose：显示指令执行过程；
-V或--version：显示版本信息；
-w或--raw：显示RAW传输协议的连线状况；
-x或--unix：此参数的效果和指定"-A unix"参数相同；
--ip或--inet：此参数的效果和指定"-A inet"参数相同。
 ```

- 实例

列出所有端口 (包括监听和未监听的)
```
netstat -a     #列出所有端口
netstat -at    #列出所有tcp端口
netstat -au    #列出所有udp端口
 ```

在netstat输出中不显示主机，端口和用户名(host, port or user)
当你不想让主机，端口和用户名显示，使用netstat -n。将会使用数字代替那些名称。同样可以加速输出，因为不用进行比对查询。

```netstat -an```

如果只是不想让这三个名称中的一个被显示，使用以下命令:
 ```
netsat -a --numeric-ports
netsat -a --numeric-hosts
netsat -a --numeric-users
 ```

找出程序运行的端口

并不是所有的进程都能找到，没有权限的会不显示，使用 root 权限查看所有的信息。
```netstat -ap | grep ssh```

找出运行在指定端口的进程：
```netstat -an | grep ':80'```

通过端口找进程ID
```netstat -anp|grep 8081 | grep LISTEN|awk '{printf $7}'|cut -d/ -f1```
