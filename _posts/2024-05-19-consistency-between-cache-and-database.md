---
layout: post
title:  缓存和数据库数据一致性问题
date:   2024-05-19 13:51:13
categories: 架构
tags: 高并发
---

* content
{:toc}

> 现在使用得比较多的是 **Cache Aside Pattern（旁路缓存模式）**。
首先说明这个保证的是在**最终一致性**的基础上尽可能减少不一致的时间，并不是保证**强一致性**。

