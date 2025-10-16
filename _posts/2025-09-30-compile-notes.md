---
layout: post
title:  "compileNotes"
date:   2025-09-30 11:59:56
categories: c/c++
tags: 编译
---

* content
{:toc}

### openssl
#### linux64 with zlib
``` shell
mkdir build
./config -fPIC no-shared zlib --with-zlib-include=`pwd`/../zlib-1.2.11/build/include --with-zlib-lib=`pwd`/../zlib-1.2.11/build/lib --prefix=`pwd`/build --libdir=`pwd`/build --static
make -j4
```

#### 交叉编译
``` shell
export CC=/usr/gcc-linaro-5.4.1-2017.05-x86_64_aarch64-linux-gnu/bin/aarch64-linux-gnu-gcc
export LD=/usr/gcc-linaro-5.4.1-2017.05-x86_64_aarch64-linux-gnu/bin/aarch64-linux-gnu-ld
export AR=/usr/gcc-linaro-5.4.1-2017.05-x86_64_aarch64-linux-gnu/bin/aarch64-linux-gnu-ar

CFLAGS="-O2 -fPIC" ./Configure linux-aarch64 no-asm no-shared --prefix=`pwd`/build
# makefile 里手动加下-fPIC
make -j4
make install
```


### boost
``` shell
git clone https://github.com/boostorg/boost.git --branch boost-1.68.0
cd boost
mkdir -p build/release
mkdir -p build/debug
./b2 --clean
```

#### 可选需要加入编译的组件(--with-xxxx 加入要编译的boost组件)

``` shell
./bootstrap.sh --with-toolset=gcc --with-libraries=chrono,timer,system,thread,filesystem,date_time,program_options
```
#### release版

``` shell
./b2 -j8 toolset=gcc variant=release threading=multi address-model=64 cxxflags=-fPIC cflags=-fPIC stage --stagedir=`pwd`/build/release/
```
#### debug版

``` shell
./b2 -j8 toolset=gcc variant=debug threading=multi address-model=64 cxxflags=-fPIC cflags=-fPIC stage --stagedir=`pwd`/build/debug/
```
> 会一起编译出动态库和静态库

#### windows
``` shell
./b2 install toolset=msvc-14.2 address-model=64 variant=release link=shared threading=multi runtime-link=shared --with-chrono --with-timer --with-system --with-thread --with-filesystem --with-date_time --build-dir=build --prefix=stage
```

#### 交叉编译
参考 https://github.com/kench1994/boost-system.1.58.0/blob/f92b9a1cae85a69520eb0683b882e79aee276476/xmake.lua
