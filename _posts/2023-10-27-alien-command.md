---
layout: post
title:  使用 alien 转换或安装一个外来的二进制软件包
date:   2023-10-27 22:33:34
categories: linux
tags: linux
---

* content
{:toc}



**``alien``(1)** 可以将 Red Hat 的 rpm、Stampede 的 slp、Slackware 的 tgz 和 Solaris 的 pkg 二进制软件包文件格式转换为 Debian 的 deb 软件包。如果你想使用来自另一个发行版的软件包，你可以使用 alien 转换并安装它。alien 也支持 LSB 软件包。

**[警告]**
>不应该用 alien(1) 来替代必要的系统软件包，例如 sysvinit、libc6、libpam-modules 等等。实际上，alien(1) 应该只用于 non-free 中仅提供二进制文件的软件包，并且它们应该兼容 LSB 或被静态链接。对于自由软件，你应该使用它们的源软件包来制作一个真正的 Debian 软件包。


使用示例：``sudo alien --scripts -i ob-deploy-x.x.x.el7.x86_64.rpm``