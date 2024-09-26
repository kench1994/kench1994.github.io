---
layout: post
title:  信号量中死锁产生的条件
date:   2024-09-25 14:24:56
categories: 并发编程
tags: 信号量
---

* content
{:toc}

#### 什么是信号量？

信号量是一种同步原语，用于控制并发编程中对公共资源的访问。它维护一个表示可用资源数量的计数。

信号量本身并不会导致死锁，但信号量使用不当会导致并发编程中出现死锁情况：

``` c
sem_wait(left);
sem_wait(right);
eat();
sem_post(right);
sem_post(left);
```