---
layout: post
title:  STL容器迭代过程中删除元素
date:   2024-12-12 13:20:35
categories: c/c++
tags: stl
---

* content
{:toc}

#### 1.连续内存序列容器(vector,string,deque)

序列容器的erase方法返回值是指向紧接在被删除元素之后的元素的有效迭代器，可以根据这个返回值来安全删除元素。

``` c++
vector<int> c;
for(vector<int>::iterator it = c.begin(); it != c.end();)
{
    if(*it)
        it = c.erase(it);
    else
        ++it; 
}
```

#### 2.关联容器(set,multiset,map,multimap)

关联容器的erase方法没有返回值，被删除的迭代器失效，所以删除前必须确保能得到下一个迭代器，可以用“后置递增迭代器”技术。

``` c++
map<int,int> m;
for(map<int,int>::iterator it = m.begin(); it != m.end();)
{
    if(条件) 
        m.erase(it++);
    else
        ++it;
}
```
m.erase得到it的一个副本，在erase真正开始之前it已经递增了。
所以erase得到了当前的迭代器，在erase内部工作开始之前it已经++了，正好满足我们的需要。


#### 3.非连续内存序列容器(list)

只所以单独列出来是因为以上两种方法对list都适用，可依自己喜好选择。

- 1.vector数据结构

vector和数组类似，拥有一段连续的内存空间，并且起始地址不变。
因此能高效的进行随机存取，时间复杂度为o(1);
但因为内存空间是连续的，所以在进行插入和删除操作时，会造成内存块的拷贝，时间复杂度为o(n)。
另外，当数组中内存空间不够时，会重新申请一块内存空间并进行内存拷贝。

- 2.list数据结构

list是由双向链表实现的，因此内存空间是不连续的。
只能通过指针访问数据，所以list的随机存取非常没有效率，时间复杂度为o(n);
但由于链表的特点，能高效地进行插入和删除。