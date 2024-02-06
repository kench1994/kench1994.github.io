---
layout: post
title:  utf8 编码规则
date:   2023-10-27 15:52:54
categories: 编码
tags: 编码
---

* content
{:toc}

UTF-8的编码规则.
```
#1-byte characters have the following format: 0xxxxxxx  			: U+0000 -> U+007F
#2-byte characters have the following format: 110xxxxx 10xxxxxx		        : U+0080 -> U+07FF
#3-byte characters have the following format: 1110xxxx 10xxxxxx 10xxxxxx	  : U+0800 -> U+FFFF
#4-byte characters have the following format: 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx : U+10000 -> U+10FFFF
```

**注意**
一字节的编码可以表示的范围是0x00 -> 0x7F 的字符. 刚好是一般的ascii码的区间. 这样如果编码中有ascii字符.那正好用一个字节就能表示.
二字节编码: 高字节的前三位固定是: 110 , 低字节的前两位固定是: 10, 这样有两个作用:
可以方便的区分一个字节的内容是一个编码的开头,还是一个编码的中间或者结束字节.
如果是0开头,一定是单字节的编码.
如果是10开头,那是非第一字节.
相对的, 110 开头的是一个双编码单元的的字符. (也就是这里的双字节UTF-8字符)
1110开头的是三字节编码的字符.
一个code point的编码被映射到 高字节的5位,和低字节的6位.总共是11位编码.
注意这个编码的起始编码是表示的是: U+0080 ,
展示说: 0x0080 这个code point被编码到: 1100 0010, 1000 0000, 结束编码的空间为: U+07FF

https://zhuanlan.zhihu.com/p/520526723


代码示例:
``` c++
// include/cppjieba/Unicode.hpp
inline RuneStrLite DecodeRuneInString(const char* str, size_t len) {
  RuneStrLite rp(0, 0);
  if (str == NULL || len == 0) {
    return rp;
  }
  if (!(str[0] & 0x80)) { // 0xxxxxxx
    // 7bit, total 7bit
    rp.rune = (uint8_t)(str[0]) & 0x7f;
    rp.len = 1;
  } else if ((uint8_t)str[0] <= 0xdf &&  1 < len) { 
    // 110xxxxxx
    // 5bit, total 5bit
    rp.rune = (uint8_t)(str[0]) & 0x1f;

    // 6bit, total 11bit
    rp.rune <<= 6;
    rp.rune |= (uint8_t)(str[1]) & 0x3f;
    rp.len = 2;
  } else if((uint8_t)str[0] <= 0xef && 2 < len) { // 1110xxxxxx
    // 4bit, total 4bit
    rp.rune = (uint8_t)(str[0]) & 0x0f;

    // 6bit, total 10bit
    rp.rune <<= 6;
    rp.rune |= (uint8_t)(str[1]) & 0x3f;

    // 6bit, total 16bit
    rp.rune <<= 6;
    rp.rune |= (uint8_t)(str[2]) & 0x3f;

    rp.len = 3;
  } else if((uint8_t)str[0] <= 0xf7 && 3 < len) { // 11110xxxx
    // 3bit, total 3bit
    rp.rune = (uint8_t)(str[0]) & 0x07;

    // 6bit, total 9bit
    rp.rune <<= 6;
    rp.rune |= (uint8_t)(str[1]) & 0x3f;

    // 6bit, total 15bit
    rp.rune <<= 6;
    rp.rune |= (uint8_t)(str[2]) & 0x3f;

    // 6bit, total 21bit
    rp.rune <<= 6;
    rp.rune |= (uint8_t)(str[3]) & 0x3f;

    rp.len = 4;
  } else {
    rp.rune = 0;
    rp.len = 0;
  }
  return rp;
}
```


> 扩展

微软很多组件把 UTF-16 LE （或 UTF-16 ）叫做 Unicode ，应该是个历史非常久远的错误。
Unicode 不是单个编码，只有提 UTF-8/16/32 编码的字节数才有意义。

BMP 中的汉字（**常用**汉字都在这里面）
``` shell
UTF-8 3 字节
UTF-16 2 字节
UTF-32 4 字节
```

BMP 外的汉字
``` shell
UTF-8 4 字节
UTF-16 4 字节
UTF-32 4 字节
```