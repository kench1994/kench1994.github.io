---
layout: post
title:  什么是weak symbol?
date:   2022-11-28 13:34:20
categories: linux
tags: linux
---

* content
{:toc}

我們用nm看動態庫時，會發現有些符號類型是"V"，手冊里解釋如下：
>"V" The symbol is a weak object.  When a weak defined symbol is linked with a normal  defined  symbol, the normal defined symbol is used with no error. When a weak undefined symbol is linked and the symbol is not defined, the value of the weak symbol becomes zero with no error.

說的是動態庫中的weak symbol，缺省會被normal symbol替代，如果沒有定義，則該symbol的值為0。
很抽象，是不是，我一直想找一個簡單的例子。
             
最近看過一篇文章：
http://www.cs.virginia.edu/~wh5a/blog/2006/07/20/the-weak-attribute-of-gcc/
終於對所謂的weak symbol有了一點了解。
http://gcc.gnu.org/onlinedocs/gcc-4.1.1/gcc/Function-Attributes.html
講了__attribute__的語法。
``` c
//weak.c
extern void foo() __attribute__((weak));
int main() {
  if (foo) foo();
}
 ```
程序居然能夠編譯通過，甚至成功執行！讓我們來看看是為什麼？
首先聲明了一個符號foo()，屬性為weak，但並不定義它，這樣，鏈接器會將此未定義的weak symbol賦值為0，也就是說foo()並沒有真正被調用，試試看，去掉if條件，肯定core dump！
 ``` c
//strong.c
extern void foo() ;
int main() {
  if (foo) foo();
}
```
這個是一般程序，編譯過不了：
strong.c: undefined reference to `foo'
再添上一個定義文件：
``` c
//foo.c
void foo() {
  printf("in foo.\n");
}
```
OK！
 
用nm檢查一下：
``` shell
linux:~/test/weak # nm weak.o
         w foo
00000000 T main
linux:~/test/weak # nm foo.o
00000000 T foo
         U printf
``` 
鏈接時，前面那個weak symbol會被后面這個代替，如果沒有鏈接foo.o,也沒問題，對應符號為0。
 
這就是weak symbol的意義。