---
layout: post
title:  哈希函数常用构造方法
date:   2024-12-12 11:46:51
categories: 编程
tags: 哈希
---

* content
{:toc}

### 哈希函数构造准则 

hash函数的构造准则：简单、均匀。

- （1）散列函数的计算简单，快速；

- （2）使哈希地址均匀地分布在地址集{0,1，…，m-1}上，并且冲突最小。 

### 哈希函数常用构造方法

介绍七种常用的哈希函数的构造方法：直接定址法、相乘取整法、平方取中法、除留余数法、伪随机数法、数字分析法和分段折叠法。

#### （1）直接定址法

取关键字或关键字的某个线性函数值为哈希地址：

``H(key) = a*key + b``
其中a和b为常数，这种哈希函数叫做自身函数。当``a=1，b=0``时，``H(key)=key``。 

注意：由于直接定址所得地址集合和关键字集合的大小相同，因此，对于不同的关键字不会发生冲突。但是，因为需要提前确定关键字的取值范围，且取值范围不能太大，所以，实际中能使用直接定址法的场景少之又少。

举例：有一个从1岁到100岁的人口统计表，其中，年龄作为关键字，哈希函数取关键字自身，即哈希函数为``H(key)= key``。这样，若要询问25岁的人有多少，则只要查表中地址为25的桶即可。

#### （2）相乘取整法

首先用关键字key乘上某个常数``A(0 < A < 1)``，并抽取出``key*A``的小数部分；然后用m乘以该小数后取整。

注意：该方法最大的优点是m的选取比除留余数法要求更低。比如，完全可选择它是2的整数次幂。虽然该方法对任何A的值都适用，但对某些值效果会更好。Knuth建议选取 0.61803……。

#### （3）平方取中法

当无法确定关键字中哪几位分布较均匀时，先求出关键字的平方值，然后按需要取平方值的中间几位作为哈希地址。

通过平方扩大差别，另外，中间几位与关键字中的每一位都相关，故不同关键字会以较高的概率产生不同的、均匀的哈希地址。这是一种较常用的构造哈希函数的方法。

举例：将一组关键字(0100，0110，1010，1001，0111)

平方后得(0010000，0012100，1020100，1002001，0012321)

若取表长为1000，则可取中间的三位数作为散列地址集：(100，121，201，020，123)。 

#### （4）除留余数法

假设散列表长为m，其散列函数公式定义为：

``H(key) = key MOD p (p ≤ m)``

MOD表示求余数。这是一种最简单，也最常用的构造哈希函数的方法。它不仅可以对关键字直接取模，也可在对关键字进行折迭、平方取中等运算之后取模。温馨提示，在使用除留余数法时，对p的选择很重要，一般情况下可以选p为质数或不包含小于20的质因素的合数。

#### （5）伪随机数法

选择一个伪随机函数，取关键字的随机函数值为它的哈希地址，即 ``H(key) = random (key)``，其中random为伪随机函数。通常，当关键字长度不等时采用此法构造哈希函数较恰当。 

#### （6）数字分析法 

假设已经知道哈希表中所有的关键字值，而且关键字值都是数字，则可以取关键字值的若干位数字组成哈希地址，这种方法叫做数字分析法。

举例：有1000个记录，关键字为10位十进制整数x1x2…x10，如哈希表长度为2000。假设经过分析，各关键字中 x3、x5和x7的取值分布近似随机，则可去哈希函数为：h(key)=h(x1x2…x10)=x3x5x7。例如，h(3778597189)=757，h(9166372560)=632。 

#### （7）分段叠加法

将关键字拆分成位数相等的几部分，其中最后一部分的位数可以不同；然后，将这几部分相加，舍弃最高进位后的结果就是该关键字的哈希地址。分段叠加法又可以分成边界叠加法和移位叠加法两种，移位叠加是将分割后的每部分低位对齐相加，边界叠加是将奇数段正序偶数段逆序然后相加。

关键字位数很多，而且关键字中每一位上数字分布大致均匀时，可以采用折叠法得到哈希地址。

举例：根据国际标准图书编号（ISBN）建立一个哈希表。如一个国际标准图书编号 0-442-20586-4的哈希地址为：

使用移位叠加 5864 +4220+04 =1 0088，故H（0-442-20586-4）= 0088（将分割后的每一部分的最低位对齐）。

使用边界叠加法叠加 5864 +0224+04 =6092，故H（0-442-20586-4）= 6092（从一端向另一端沿分割界来回叠加）。

 

### 小 结 

有许多种不同的哈希函数设计方法，这里主要讨论了七种常用的、不同类型关键字的希函数设计方法。在应用的时候，要因地制宜，不同的场景采用不同的方法。如：关键字是ISBN时可以使用分段叠加法构造哈希函数；是整数类型时可以用除留余数法、直接定址法和数字分析法等构造哈希函数；是小数类型说常用伪随机数法来构造哈希函数等。