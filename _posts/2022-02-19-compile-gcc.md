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

# 建立输出目录，用于存放编译时所有产生的中间文件
mkdir build

# 进入输出目录
cd build

# 执行configure配置
../configure --enable-checking=release --enable-languages=c,c++ --disable-multilib -prefix=/usr/local/gcc-7.5.0

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