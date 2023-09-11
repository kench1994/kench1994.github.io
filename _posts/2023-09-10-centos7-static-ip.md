---
layout: post
title:  centos7 静态IP设置
date:   2023-09-10 16:53:02
categories: linux
tags: centos7
---

* content
{:toc}

``/etc/sysconfig/network-scripts/ifcfg-ens33``

```ini
TYPE=Ethernet
PROXY_METHOD=none
BROWSER_ONLY=no
BOOTPROTO=none
DEFROUTE=yes
IPV4_FAILURE_FATAL=no
IPV6INIT=yes
IPV6_AUTOCONF=yes
IPV6_DEFROUTE=yes
IPV6_FAILURE_FATAL=no
IPV6_ADDR_GEN_MODE=stable-privacy
NAME=ens33
UUID=6c4c59e2-8521-4692-8de1-afea2f4fa590
DEVICE=ens33
ONBOOT=yes
IPADDR=192.168.152.187
PREFIX=24
GATEWAY=192.168.152.2
DNS1=8.8.8.8
PEERROUTES=no
PEERDNS=no
```