---
layout: post
title:  hash冲突的解决方案
date:   2024-11-05 16:40:29
categories: 
tags: 
---

* content
{:toc}


首先回顾一下hash冲突的解决方案有哪些。

### Open addressing

open addressing是通过探测或者搜索数组的方法找到未使用的bucket来解决hash冲突的。

- 优点：

  *  当hash冲突很小的时候，只需要访问对应的bucket就能获得对应的pair<key, value>，不需要再查找额外的数据结构，性能较好。

- 缺点：

  *  对hash函数的要求比较高，否则当hash冲突很大的时，查找速度会很慢。

### Separate chaining

或者叫closed addressing，当发生hash冲突时需要通过额外的数据结构来处理，比如链表或者红黑树。

- 优点：

  * hash冲突处理简单，比如采用链表来解决hash冲突的话，添加节点的时候直接在链表后面添加即可。
  * 对hash函数要求会低一点，即便冲突稍微大一点，也能把查找速度控制得比较好。

- 缺点：

  * 由于需要额外的数据结构处理，性能在一般情况下不如Open addressing。

> STL采用的是Separate chaining的方案。使用链表挂在bucket上解决冲突，当链表超过一定长度时转换为红黑树。


### 使用建议

在hash冲突上，大部分的开源实现都选择了Open addressing这种方式，毕竟理论性能会更好，而flat和node则是两种实现都会提供。结合上面说的各种优缺点，我们可以简单得出一套通用的方案。

首先考虑下面几个点：

1. 是否可以一开始就可以确定好容量。
2. key的copy开销是否很大。
3. value的copy开销是否很大。
4. value的地址不稳定是否会影响代码逻辑。

可以简单归纳为以下四种情况：

- 情况1:value可以是不稳定的，而且容量是已知的，可以一开始确定。

  推荐：使用flat实现，通过一开始reserve两倍的size来减少rehash带来的开销。

- 情况2:value可以是不稳定的，容量未知，key和value的copy开销很小。

  推荐：使用flat实现。

- 情况3:value可以是不稳定的，容量未知，key的copy开销很小但value的copy开销很大。

  推荐：使用flat实现，value使用unique_ptr包裹起来。

- 其他情况均使用node结构。