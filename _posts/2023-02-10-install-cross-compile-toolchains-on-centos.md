---
layout: post
title:  CentOS安装aarch64交叉编译器
date:   2023-02-10 11:49:22
categories: 
tags: 
---

* content
{:toc}

#### 1. 先用yum命令搜索一下可安装的交叉编译器安装包：
``` shell
root@mycentos:/root # yum search aarch64
……
binutils-aarch64-linux-gnu.x86_64 : Cross-build binary utilities for aarch64-linux-gnu
gcc-aarch64-linux-gnu.x86_64 : Cross-build binary utilities for aarch64-linux-gnu
gcc-c++-aarch64-linux-gnu.x86_64 : Cross-build binary utilities for aarch64-linux-gnu
uboot-images-armv8.noarch : u-boot bootloader images for aarch64 boards
```
选一个执行安装命令：
``` shell
root@mycentos:/root # yum install gcc-c++-aarch64-linux-gnu.x86_64
```
这步安装完成后，仅仅是只安装编译链工具：
``` shell
root@mycentos:/root # ll /usr/bin/aarch64-linux-gnu-*
-rwxr-xr-x 1 root root  975K Jul 14  2017 /usr/bin/aarch64-linux-gnu-addr2line
-rwxr-xr-x 2 root root 1004K Jul 14  2017 /usr/bin/aarch64-linux-gnu-ar
-rwxr-xr-x 2 root root  1.5M Jul 14  2017 /usr/bin/aarch64-linux-gnu-as
-rwxr-xr-x 2 root root  651K Jun 28  2017 /usr/bin/aarch64-linux-gnu-c++
-rwxr-xr-x 1 root root  971K Jul 14  2017 /usr/bin/aarch64-linux-gnu-c++filt
-rwxr-xr-x 1 root root  647K Jun 28  2017 /usr/bin/aarch64-linux-gnu-cpp
-rwxr-xr-x 1 root root   33K Jul 14  2017 /usr/bin/aarch64-linux-gnu-elfedit
-rwxr-xr-x 2 root root  651K Jun 28  2017 /usr/bin/aarch64-linux-gnu-g++
-rwxr-xr-x 1 root root  647K Jun 28  2017 /usr/bin/aarch64-linux-gnu-gcc
-rwxr-xr-x 1 root root  301K Jun 28  2017 /usr/bin/aarch64-linux-gnu-gcov
-rwxr-xr-x 1 root root  1.1M Jul 14  2017 /usr/bin/aarch64-linux-gnu-gprof
-rwxr-xr-x 4 root root  2.0M Jul 14  2017 /usr/bin/aarch64-linux-gnu-ld
-rwxr-xr-x 4 root root  2.0M Jul 14  2017 /usr/bin/aarch64-linux-gnu-ld.bfd
-rwxr-xr-x 2 root root  988K Jul 14  2017 /usr/bin/aarch64-linux-gnu-nm
-rwxr-xr-x 2 root root  1.2M Jul 14  2017 /usr/bin/aarch64-linux-gnu-objcopy
-rwxr-xr-x 2 root root  1.7M Jul 14  2017 /usr/bin/aarch64-linux-gnu-objdump
-rwxr-xr-x 2 root root 1004K Jul 14  2017 /usr/bin/aarch64-linux-gnu-ranlib
-rwxr-xr-x 2 root root  506K Jul 14  2017 /usr/bin/aarch64-linux-gnu-readelf
-rwxr-xr-x 1 root root  975K Jul 14  2017 /usr/bin/aarch64-linux-gnu-size
-rwxr-xr-x 1 root root  975K Jul 14  2017 /usr/bin/aarch64-linux-gnu-strings
-rwxr-xr-x 2 root root  1.2M Jul 14  2017 /usr/bin/aarch64-linux-gnu-strip
```

此时如果用 ``/usr/bin/aarch64-linux-gnu-gcc`` 去编译c源码时会发现所有头文件都找不着，因为 ``gcc-c++-aarch64-linux-gnu.x86_64`` 包不负责头文件的安装。

#### 2. 安装aarch64的glibc开发包
`` sysroot-glibc-linaro-{版本}-{日期}-aarch64-linux-gnu`` 开发包由linaro提供，到 http://releases.linaro.org/components/toolchain/binaries/ 去选择版本下载。
这里我选择下载 ``sysroot-glibc-linaro-2.25-2019.12-aarch64-linux-gnu.tar.xz``。
下载下来是个压缩包，先用xz命令进行解压，再用tar解包得到开发包目录。
``` shell
root@mycentos:/root # xz -d sysroot-glibc-linaro-2.25-2019.12-aarch64-linux-gnu.tar.xz
root@mycentos:/root # tar -xvf sysroot-glibc-linaro-2.25-2019.12-aarch64-linux-gnu.tar
etc  lib  sbin  usr  var
``` 
运行 ``aarch64-linux-gnu-gcc`` 命令查看它的sysroot目录，：
``` shell
root@mycentos:/root # aarch64-linux-gnu-gcc -v
Using built-in specs.
COLLECT_GCC=aarch64-linux-gnu-gcc
COLLECT_LTO_WRAPPER=/usr/libexec/gcc/aarch64-linux-gnu/4.8.5/lto-wrapper
Target: aarch64-linux-gnu
Configured with: ../gcc-4.8.5-20150702/configure --bindir=/usr/bin --build=x86_64-redhat-linux-gnu --datadir=/usr/share --disable-decimal-float --disable-dependency-tracking --disable-gold --disable-libgomp --disable-libmudflap --disable-libquadmath --disable-libssp --disable-nls --disable-plugin --disable-shared --disable-silent-rules --disable-sjlj-exceptions --disable-threads --enable-checking= --enable-gnu-unique-object --enable-initfini-array --enable-languages=c,c++ --enable-linker-build-id --enable-nls --enable-obsolete --enable-targets=all --exec-prefix=/usr --host=x86_64-redhat-linux-gnu --includedir=/usr/include --infodir=/usr/share/info --libexecdir=/usr/libexec --localstatedir=/var --mandir=/usr/share/man --prefix=/usr --program-prefix=aarch64-linux-gnu- --sbindir=/usr/sbin --sharedstatedir=/var/lib --sysconfdir=/etc --target=aarch64-linux-gnu --with-bugurl=http://bugzilla.redhat.com/bugzilla/ --with-linker-hash-style=gnu --with-newlib --with-sysroot=/usr/aarch64-linux-gnu/sys-root --with-system-libunwind --with-system-zlib --without-headers --with-isl=/builddir/build/BUILD/gcc-4.8.5-20150702/isl-install --with-cloog=/builddir/build/BUILD/gcc-4.8.5-20150702/cloog-install
Thread model: single
gcc version 4.8.5 20150623 (Red Hat 4.8.5-16) (GCC) 
``` 
找到 sysroot 目录 ``--with-sysroot=/usr/aarch64-linux-gnu/sys-root``
将解压出来的 ``etc lib sbin usr var`` 这些目录全部移动到 ``/usr/aarch64-linux-gnu/sys-root`` 中，就可以用 ``aarch64-linux-gnu-gcc`` 编译源码了。


### ADDIT

> ``arm`` 相关的交叉编译工具链由很多，例如``arm-linux-eabi，arm-none-eabi``。``STM32`` 处理器使用的是 ``arm-none-eabi``。

``arm-none-eabi`` 和 ``arm-linux-eabi`` 等的区别：

1. ``arm-linux-eabi`` 用于编译 ``arm linux`` 内核代码。
2. ``arm-none-eabi`` 不指名操作系统，可以是``linux`` , 也可以是 ``vxworks`` 等
3. ``arm-none-eabi`` 不包含 ``__linux__`` 等特定宏，所以往往编译 ``linux`` 内核的时候通不过，不支持那些跟操作系统关系密切的函数，比如 ``fork(2)``，使用的是 ``newlib`` 这个专用于嵌入式系统的C库。
4. ``arm-none-linux-eabi`` 用于 ``Linux`` 的，使用 ``Glibc``>。