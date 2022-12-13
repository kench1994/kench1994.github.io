---
layout: post
title:  apt on ubuntu
date:   2022-12-12 17:25:08
categories: linux
tags: linux
---

* content
{:toc}

apt-get 是一个下载安装软件包的简单命令行接口。

```
命令： 
    update - 重新获取软件包列表 
    upgrade - 进行更新 
    install - 安装新的软件包 
    remove - 移除软件包 
    autoremove - 自动移除全部不使用的软件包 
    purge - 移除软件包和配置文件 
    source - 下载源码档案 
    build-dep - 为源码包配置编译依赖 
    dist-upgrade - 发行版升级
    dselect-upgrade - 依照 dselect 的选择更新 
    clean - 清除下载的归档文件 
    autoclean - 清除旧的的已下载的归档文件 
    check - 检验是否有损坏的依赖 
选项： 
    -h 本帮助文件。 
    -q 输出到日志 - 无进展指示 
    -qq 不输出信息，错误除外 
    -d 仅下载 - 不安装或解压归档文件 
    -s 不实际安装。模拟执行命令
    -y 假定对所有的询问选是，不提示 
    -f 尝试修正系统依赖损坏处 
    -m 如果归档无法定位，尝试继续
    -u 同时显示更新软件包的列表 
    -b 获取源码包后编译 -V 显示详细的版本号
    -c=? 阅读此配置文件
    -o=? 设置自定的配置选项，如 -o dir::cache=/tmp
```

更新系统之前都需要执行 ``sudo apt-get update`` 来更新本地的软件列表

- ``sudo apt-get upgrade``
系统将现有的 Package 升级，如果有相依性的问题，而此相依性需要安装其它新的 Package 或影响到其它 Package 的相依性时，此 Package 就不会被升级
### 
- ``sudo apt-get dist-upgrade``
可以聪明的解决相依性的问题，如果有相依性问题，需要安装/移除新的 Package，就会试着去安装/移除它
(所以通常这个会被认为是有点风险的升级)
可以作为小版本升级来使用，比如从 Ubuntu 14.04.2 升级到 Ubuntu 14.04.5
### 
- ``sudo do-release-upgrade -d``
release 更新，比如从 Ubuntu 14.04 升级到 16.04