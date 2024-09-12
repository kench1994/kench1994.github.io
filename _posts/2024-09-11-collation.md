---
layout: post
title:  字符序
date:   2024-09-11 09:12:40
categories: 编码
tags: 字符序
---

* content
{:toc}

什么是字符集和字符序?简单的说:

- 字符集(character set):定义了字符以及字符的编码.
- 字符序(collation):定义了字符的比较规则.

关于字符集,我们在Mysql中最容易遇到的问题就是插入的字符中含有``emoji``表情,如果我们设置的字符集不是``utf8mb4``,而是``utf8``我们就会遇见1366的错误码.其中的原因就是Mysql的utf8是被阉割后只支持3个字节,而``emoji``表情需要4个字节.
关于字符序,例如我们在查询中的sql语句是:``select * from t_content where content = 'abc'``,很可能会出现``content=ABC``的记录.而这种情况发生主要就是字符序导致的.

字符集中的``Charset``表示该字符集在Mysql中的名称,``Default collation``表示该字符集默认的字符序,``Maxlen``表示该字符集支持的最大长度.

字符序中的``Collation``表示字符序在``Mysql``中的名称,``Charset``表示其对应的字符集,``Default``表示是不是默认的的字符序.对于字节序的命名规则,它的组成一般都是字符集_语言_后缀.而其中的后缀代表它的比较规则,如下表所示:

|   后缀   |   描述   |
| ---- | ---- |
|   ai   |  不区分重音    |
|  as    |   区分重音   |
|  ci    |   不区分大小写   |
|  cs    |   区分大小写   |
|  bin	 |  以二进制方式比较    |



> 对于任何 Unicode 字符集，使用 xxx_general_ci 排序规则执行的操作比使用 xxx_unicode_ci 排序规则执行的操作快。c