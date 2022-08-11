---
layout: post
title: "编译安装gcc"
date: 2022-02-09 05:57:00
categories: program
tags: gcc
---

* content
{:toc}

```
# 下载gcc源码包
wget https://ftp.gnu.org/gnu/gcc/gcc-7.5.0/gcc-7.5.0.tar.gz

# 解压文件
tar -xzvf gcc-7.5.0.tar.gz

# 进入解压目录
cd gcc-7.5.0

# 下载编译gcc所需的依赖文件和库
./contrib/download_prerequisites

# 如果这里没反应可以手动下载依赖存放到gcc源码目录(建议使用迅雷等工具)
wget ftp://gcc.gnu.org/pub/gcc/infrastructure/gmp-6.1.0.tar.bz2
wget ftp://gcc.gnu.org/pub/gcc/infrastructure/mpfr-3.1.4.tar.bz2
wget ftp://gcc.gnu.org/pub/gcc/infrastructure/mpc-1.0.3.tar.gz
wget ftp://gcc.gnu.org/pub/gcc/infrastructure/isl-0.16.1.tar.bz2
# 放置完成再次执行 ./contrib/download_prerequisites

# 或者更换下载源
# 将文件 ./contrib/download_prerequisites 中的字符串：
ftp://gcc.gnu.org/pub/gcc/infrastructure/
# 替换为
http://www.mirrorservice.org/sites/sourceware.org/pub/gcc/infrastructure/

# 建立输出目录，用于存放编译时所有产生的中间文件
mkdir build

# 进入输出目录
cd build

# 执行configure配置
../configure --enable-checking=release --enable-languages=c,c++ --disable-multilib -prefix=/usr/local/gcc-7.5.0

# --disable-multilib: 用于关闭多个多个arch的库，比如只需要64位的，不需要32位的库，那就添加这个配置


# 编译gcc，指定编译使用的线程数为8，编译耗时较长，可能需要比较久
make -j8

# 安装
sudo make install

# 添加到环境变量 ~/.zshrc 或 ~/.bashrc
PATH=/usr/local/gcc-7.5.0/bin:$PATH:$HOME/.local/bin:$HOME/bin
export CC=/usr/local/gcc-7.5.0/bin/gcc
export CXX=/usr/local/gcc-7.5.0/bin/g++
export LD_LIBRARY_PATH=/usr/local/gcc-7.5.0/lib64:/usr/local/lib:$LD_LIBRARY_PATH
export PATH

# 使环境变量生效
source ~/.zshrc
```