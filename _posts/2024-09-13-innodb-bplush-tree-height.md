---
layout: post
title:  MySQL之InnoDB主键索引的B+树的高度计算
date:   2024-09-13 22:11:47
categories: 数据库
tags: mysql
---

* content
{:toc}

- 高度为2和3、主键bigint类型、一行记录数据大小1k
- InnoDB存储引擎最小储存单元是页，一页大小就是16k。
- B+树叶子存的是数据，内部节点存的是键值+指针。索引组织表通过非叶子节点的二分查找法以及指针确定数据在哪个页中，进而再去数据页中找到需要的数据；

假设B+树的高度为2的话，即有一个根结点和若干个叶子结点。``这棵B+树的存放总记录数为 = 根结点指针数 * 单个叶子节点记录行数``

假设一行记录的数据大小为1k，那么``单个叶子节点可以存的记录数 = 16k/1k = 16``

**非叶子节点内存放多少指针呢？**

我们假设主键ID为``bigint类``型，长度为8字节(int类型的话，一个int就是32位，4字节)，而指针大小是固定的在InnoDB源码中设置为6字节，假设n指主键个数即key的个数，``n*8 + (n + 1) * 6 = 16K=16*1024B`` , 算出n约为 1170,意味着根节点会有1170个key与1171个指针

因此，一棵**高度为2的B+树，能存放1171* 16 = 18736条**这样的数据记录。

同理一棵**高度为3的B+树，能存放1171 * 1171 * 16 = 21939856**，也就是说，可以存放两千万左右的记录。B+树高度一般为1-3层，已经满足千万级别的数据存储。