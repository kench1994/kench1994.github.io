---
layout: post
title:  "为什么UUID是唯一的"
date:   2020-10-16 19:30:13
categories: program
tags: uuid
---

* content
{:toc}

UUID是通用唯一识别码（Universally Unique Identifier）的缩写，是一种软件建构的标准。其目的，是让分布式系统中的所有元素都能有唯一的辨识信息，而不需要通过中央控制端来做辨识信息的指定。如此一来，每个人都可以创建不与其他人冲突的UUID。在这样的情况下，就不需考虑数据库创建时的名称重复问题。

简单地说，UUID是指在一台机器上生成的数字，它保证对在同一时空中的所有机器都是唯一的。在UUID的算法中，可能会用到诸如网卡MAC地址，IP，主机名，进程ID等信息以保证其独立性。

#

组成
1. 当前日期和时间。UUID的第一个部分与时间有关，如果在生成一个UUID后，过几秒又生成一个UUID，则第一个部分不同，其余相同。
2. 时钟序列。
3. 全局唯一的IEEE机器识别号。如果有网卡，从网卡MAC地址获得，没有网卡以其他方式获得。

#

编码规则
UUID全局唯一标识符，定义为一个字符串主键，采用32位数字组成，编码采用16进制，定义了在时间和空间都完全唯一的系统信息。
UUID的编码规则：
1. 1~8位采用系统时间，在系统时间上精确到毫秒级保证时间上的唯一性；
2. 9~16位采用底层的IP地址，在服务器集群中的唯一性；
3. 17~24位采用当前对象的HashCode值，在一个内部对象上的唯一性；
4. 25~32位采用调用方法的一个随机数，在一个对象内的毫秒级的唯一性。

优点
- 能够保证独立性，程序可以在不同的数据库间迁移，效果不受影响。
- 保证生成的ID不仅是表独立的，而且是库独立的，在切分数据库时尤为重要。

缺点
- 相对比较占内存，和 INT 相比，存储一个UUID要花费更多的空间。
- 使用UUID后，URL显得冗长，不够友好。