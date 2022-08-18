---
layout: post
title: "Dockerfile设置默认时区"
date: 2022-08-11 09:12:34
categories: docker
tags: docker
---

* content
{:toc}


我们打包镜像时使用的基础镜像基本上都是采用UTC（格林时间），与我们常用的北京(上海)时间（CST）相差8个小时。我们可以在创建一个容器的时候，将宿主机时区相关的目录文件挂在到容器上：

``` shell
docker run --rm -it -v /etc/localtime:/etc/localtime ubuntu
# 进入容器，date测试
root@3e6c1d137e02:/# date
Mon Aug  5 15:26:20 CST 2017
```

这种方法相对麻烦，每次新建容器都需要挂载宿主机器时区相关的配置目录/文件。为了简化这一步，可以通过Dockerfile打包新的镜像，修改容器的默认时区。 下面分三个基础系统镜像分别实现：

**Ubuntu**

使用ubuntu作为基础镜像，需要先下载安装tzdata包，默认时区是UTC时区，修改配置文件，并通过dpkg-reconfigure重置时区配置生效。 安装完成之后，为了尽量减少镜像体力，删除安装过程中产生的各种非必要文件。
``` dockerfile
FROM ubuntu
 
MAINTAINER fastjrun
 
ENV TIME_ZONE Asia/Shanghai
 
RUN sed -i s@/archive.ubuntu.com/@/mirrors.aliyun.com/@g /etc/apt/sources.list \
    && apt-get update \
    && apt-get install -y tzdata \
    && ln -snf /usr/share/zoneinfo/$TIME_ZONE /etc/localtime && echo $TIME_ZONE > /etc/timezone \
    && dpkg-reconfigure -f noninteractive tzdata \
    && apt-get clean \
    && rm -rf /tmp/* /var/cache/* /usr/share/doc/* /usr/share/man/* /var/lib/apt/lists/*
```

**Alphine**

Alphine号称最小的Linux系统镜像（才5M), 先采用apk包管理器来安装tzdata包，设置相关配置文件。
``` dockerfile
FROM alpine
MAINTAINER igitlib
 
RUN apk add --no-cache tzdata \
    && cp /usr/share/zoneinfo/Asia/Shanghai /etc/localtime \
    && echo "Asia/Shanghai" > /etc/timezone \
    && apk del tzdata
```

**Centos**

centos就相对很简单，只需要添加配置文件即可。
``` dockerfile
FROM centos
 
ENV TIME_ZONE Asia/Shanghai
 
RUN ln -snf /usr/share/zoneinfo/$TIME_ZONE /etc/localtime
```