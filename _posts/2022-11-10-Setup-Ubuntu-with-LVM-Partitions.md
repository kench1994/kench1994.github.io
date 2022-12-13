---
layout: post
title:  Set up Ubuntu Desktop with LVM Partitions
date:   2022-11-10 14:28:23
categories: linux
tags: linux
---

* content
{:toc}

- **PV 物理卷（physical volume）**：LVM 的基本存储逻辑块。创建物理卷它可以用硬盘分区，也可以用硬盘本身
- **VG 卷组（Volume Group）**：一个 LVM 卷组由一个或多个物理卷组成。
- **LV 逻辑卷（logical volume）**：LV 建立在VG之上，可以在 LV 之上建立文件系统 

![lvm](/assets/lvm.png)

https://help.ubuntu.com/community/UbuntuDesktopLVM

http://blog.chinaunix.net/uid-25266990-id-3088089.html