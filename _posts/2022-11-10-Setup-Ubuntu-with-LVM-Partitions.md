---
layout: post
title:  Set up Ubuntu Desktop with LVM Partitions
date:   2022-11-10 14:28:23
categories: linux
tags: linux
---

* content
{:toc}

---
layout: post
title:  lvm 逻辑卷管理
date:   2024-05-20 15:26:38
categories: linux
tags: lvm
---

* content
{:toc}

>LVM 逻辑卷管理（Logical Volume Manager）通过在硬盘和文件系统之间添加一个逻辑层，来为文件系统屏蔽下层硬盘分区布局，提高硬盘分区管理的灵活性，

使用LVM管理硬盘的基本过程如下：

1. 将硬盘创建为物理卷
2. 将多个物理卷组合成卷组
3. 在卷组中创建逻辑卷
4. 在逻辑卷之上创建文件系统


**基本概念**
- **PV 物理卷（physical volume）**：LVM 的基本存储逻辑块。创建物理卷它可以用硬盘分区，也可以用硬盘本身
- **VG 卷组（Volume Group）**：一个 LVM 卷组由一个或多个物理卷组成。
- **LV 逻辑卷（logical volume）**：LV 建立在VG之上，可以在 LV 之上建立文件系统 

![lvm](/assets/lvm.png)

https://help.ubuntu.com/community/UbuntuDesktopLVM

http://blog.chinaunix.net/uid-25266990-id-3088089.html

[实操参考](https://blog.csdn.net/FallenLeaves_/article/details/129972256)