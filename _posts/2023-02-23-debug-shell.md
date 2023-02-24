---
layout: post
title:  shell 调试
date:   2023-02-23 10:30:36
categories: linux
tags: linux
---

* content
{:toc}

没错, ``shell`` 也可以调试.

看这样一个脚本:

``` shell
$ cat filesize.sh
#!/bin/bash
for filesize in $(ls -l . | grep "^-" | awk '{print $5}')
do
let totalsize=$totalsize+$filesize
done
echo "Total file size in current directory: $totalsize"
```

运行结果如下:

```
$ ./filesize.sh
Total file size in current directory: 652
```

添加调试信息:

``` shell
$ cat filesize.sh
#!/bin/bash
set -xv   ## 注意这里!
for filesize in $(ls -l . | grep "^-" | awk '{print $5}')
do
let totalsize=$totalsize+$filesize
done
echo "Total file size in current directory: $totalsize"
```

输出结果如下:

``` shell
$ ./fs.sh
++ ls -l .
++ grep '^-'
++ awk '{print $5}'
+ for filesize in '$(ls -l . | grep "^-" | awk '\''{print
$5}'\'')'
+ let totalsize=+178
+ for filesize in '$(ls -l . | grep "^-" | awk '\''{print
$5}'\'')'
+ let totalsize=178+285
+ for filesize in '$(ls -l . | grep "^-" | awk '\''{print
$5}'\'')'
+ let totalsize=463+189
+ echo 'Total file size in current directory: 652'
Total file size in current directory: 652
```
每次执行一条命令都会输出对应的命令和结果.

除了上面的方法, 还可以这样调试:

``$ bash -xv filesize.sh``

直接在运行的时候调试.

很有用的我会乱说~