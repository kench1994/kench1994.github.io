---
layout: post
title:  Linux环境中动态库文件(.so文件)的realname,soname和linkname
date:   2022-11-09 15:16:59
categories: linux
tags: linux
---

* content
{:toc}

### realname：
实际等同于库文件的filename，是在库文件生成时就被指定的，如：gcc -shared -o $(realname) dependence flags
realname的一般格式为 lib$(name).so.$(major).$(minor).$(revision)，$(name)是动态库的名字，$(major).$(minor).$(revision)分别表示主版本号，子版本号和修正版本号
soname：
在库文件生成时被指定，如：gcc -shared -o $(realname) dependence flags -Wl,-soname,$(soname)
其一般格式为lib$(name).so.$(major).$(minor)，soname会被写入库文件中
可以使用readelf -d $(realname)查看库文件的

### soname
不同realname的库文件可以有相同的soname，有利于库文件的升级和兼容，例如当版本从1.0.0升级到1.0.1，库文件的接口没有变化（或者接口增加，但原有接口不变）的情况下，可以指定相同的soname，这样使用1.0.0生成的程序仍然可以运行。
soname在链接和加载库文件时使用，当时使用-l$(name)链接库文件时，链接器会读取lib$(name).so文件中的soname值，并将其记录在生成的程序中，当运行程序时，会从相应的目录加载名为$(soname)的文件，所以，在运行程序之前，$(soname)的库文件必须已生成，
可以采用软连接的方式生成该文件，如：ln -s $(realname) $(soname)
也可以使用ldconfig命令自动生成，如 ldconfig -n $(dir) 会生成$(dir)目录下所有库文件对应的$(soname)文件

### linkname：
在链接时使用，一般格式为lib$(name).so，通常是$(realname)文件或者$(soname)文件的软链接
当使用-l$(name)选项时，链接器会从相应目录链接lib$(name).so的文件，如果该文件不存在，则会链接lib$(name).a文件，如果该文件也不存在，会发生链接错误
如果链接的是lib$(name).a文件，在运行程序时也就没$(realname)文件和$(soname)文件什么事了


