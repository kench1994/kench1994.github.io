---
layout: post
title:  MySQL 三大日志
date:   2024-11-26 13:40:09
categories: MySQL
tags: MySQL
---

* content
{:toc}

- undo log（回滚日志）：是 Innodb 存储引擎层生成的日志，实现了事务中的原子性，主要用于事务回滚和 MVCC。

- redo log（重做日志）：是 Innodb 存储引擎层生成的日志，实现了事务中的持久性，主要用于掉电等故障恢复；

- binlog （归档日志）：是 Server 层生成的日志，主要用于数据备份和主从复制；

除了上面三种日志之外，常见MySQL日志还有：

- 慢查询日志(slow log)：记录所有执行时间超过long_query_time的所有查询或不使用索引的查询。

- 中继日志(relay log)：MySQL主从复制时从服务器使用的日志。


之前我们了解了一条查询语句的执行流程，并介绍了执行过程中涉及的处理模块。一条查询语句的执行过程一般是经过连接器、分析器、优化器、执行器等功能模块，最后到达存储引擎。

那么，一条 SQL 更新语句的执行流程又是怎样的呢？

首先我们创建一个表 user_info，主键为 id，创建语句如下：

``` sql
CREATE TABLE `T` (
  `ID` int(11) NOT NULL,
  `c` int(11) DEFAULT NULL,
  PRIMARY KEY (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;
```

 插入一条数据：

``INSERT INTO T VALUES ('2', '1');``

如果要将 ID=2 这一行的 c 的值加 1，SQL 语句为：

``UPDATE T SET c = c + 1 WHERE ID = 2;``

前面介绍过 SQL 语句基本的执行链路，这里把那张图拿过来。因为，更新语句同样会走一遍查询语句走的流程。

通过连接器，客户端与 MySQL 建立连接
update 语句会把 T 表上的所有查询缓存结果清空
分析器会通过词法分析和语法分析识别这是一条更新语句
优化器会决定使用 ID 这个索引（聚簇索引）
执行器负责具体执行，找到匹配的一行，然后更新
更新过程中还会涉及 redo log（重做日志）和 binlog（归档日志）的操作
其中，这两种日志默认在 数据库的 data 目录下，redo log 是 ``ib_logfile0 ``格式的，binlog 是 ``xxx-bin.000001`` 格式的。

接下来让我们分别去研究下日志模块中的 redo log 和 binlog。


### redo log

在 MySQL 中，如果每一次的更新操作都需要写进磁盘，然后磁盘也要找到对应的那条记录，然后再更新，整个过程 IO 成本、查找成本都很高。为了解决这个问题，MySQL 的设计者就采用了日志（redo log）来提升更新效率。

而日志和磁盘配合的整个过程，其实就是 MySQL 里的 WAL 技术，WAL 的全称是 Write-Ahead Logging，它的关键点就是先写日志，再写磁盘。

具体来说，当有一条记录需要更新的时候，InnoDB 引擎就会先把记录写到 redo log（redolog buffer）里面，并更新内存（buffer pool），这个时候更新就算完成了。同时，InnoDB 引擎会在适当的时候（如系统空闲时），将这个操作记录更新到磁盘里面（刷脏页）。

redo log 是 InnoDB 存储引擎层的日志，又称重做日志文件，redo log 是循环写的，redo log 不是记录数据页更新之后的状态，而是记录这个页做了什么改动。

redo log 是固定大小的，比如可以配置为一组 4 个文件，每个文件的大小是 1GB，那么日志总共就可以记录 4GB 的操作。从头开始写，写到末尾就又回到开头循环写，如下图所示。

图中展示了一组 4 个文件的 redo log 日志，checkpoint 是当前要擦除的位置，擦除记录前需要先把对应的数据落盘（更新内存页，等待刷脏页）。write pos 到 checkpoint 之间的部分可以用来记录新的操作，如果 write pos 和 checkpoint 相遇，说明 redolog 已满，这个时候数据库停止进行数据库更新语句的执行，转而进行 redo log 日志同步到磁盘中。checkpoint 到 write pos 之间的部分等待落盘（先更新内存页，然后等待刷脏页）。

有了 redo log 日志，那么在数据库进行异常重启的时候，可以根据 redo log 日志进行恢复，也就达到了 crash-safe。

redo log 用于保证 crash-safe 能力。innodb_flush_log_at_trx_commit 这个参数设置成 1 的时候，表示每次事务的 redo log 都直接持久化到磁盘。这个参数建议设置成 1，这样可以保证 MySQL 异常重启之后数据不丢失。

### binlog

MySQL 整体来看，其实就有两块：一块是 Server 层，它主要做的是 MySQL 功能层面的事情；

还有一块是引擎层，负责存储相关的具体事宜。redo log 是 InnoDB 引擎特有的日志，而 Server 层也有自己的日志，称为 binlog（归档日志）。

binlog 属于逻辑日志，是以二进制的形式记录的是这个语句的原始逻辑，依靠 binlog 是没有 crash-safe 能力的。

binlog 有两种模式，statement 格式的话是记 sql 语句，row 格式会记录行的内容，记两条，更新前和更新后都有。

sync_binlog 这个参数设置成 1 的时候，表示每次事务的 binlog 都持久化到磁盘。这个参数也建议设置成 1，这样可以保证 MySQL 异常重启之后 binlog 不丢失。

为什么会有两份日志呢？

因为最开始 MySQL 里并没有 InnoDB 引擎。MySQL 自带的引擎是 MyISAM，但是 MyISAM 没有 crash-safe 的能力，binlog 日志只能用于归档。而 InnoDB 是另一个公司以插件形式引入 MySQL 的，既然只依靠 binlog 是没有 crash-safe 能力的，所以 InnoDB 使用另外一套日志系统——也就是 redo log 来实现 crash-safe 能力。