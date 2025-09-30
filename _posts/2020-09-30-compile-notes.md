---
layout: post
title:  "compileNotes"
date:   2020-08-08 11:59:56
categories: c/c++
tags: 编译
---

* content
{:toc}

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
