---
layout: post
title:  vscode 中使用 clangd 替代 Microsoft C/C++ intelliSense
date:   2023-09-10 16:31:03
categories: vscode
tags: clangd
---

* content
{:toc}

不少人可能会觉得微软官方vscode c/c++ 插件的 intellisense 速度较慢，因而更喜欢使用 clangd 的 intellisense 功能。【使用clangd的intellisense就不需要cpptools的配套c_cpp_propertites.json文件】

- a. 关闭 c/c++ 插件的 intellisense 引擎（插件扩展中找下）
- b. 安装 clangd 插件，安装完成一般会提示你安装 clangd 语言服务器，选择 install

或者 F1 后 clangd: Download language serve 进行下载

- c. 正常会帮你配置好了，关闭的话 按F1 > Open Remote Settings(Json) 把 clangd.path 条目关闭

**注：** centos7 下 GLIBC 版本过低，会出现 clangd server 启动失败，忽略步骤b，可以使用我编译出来的版本。放在 /usr/local/clangd 目录下(我也忘了我怎么编译的，应该就是切换到gcc8，发布时带上libstdc++)