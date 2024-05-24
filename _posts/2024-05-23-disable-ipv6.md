---
layout: post
title:  如何关闭IPv6
date:   2024-05-23 11:06:05
categories: linux
tags: IPv6
---

* content
{:toc}

#### 暂时关闭

- 关闭所有网络接口。
```shell
sudo sysctl -w net.ipv6.conf.all.disable_ipv6=1
sudo sysctl -w net.ipv6.conf.default.disable_ipv6=1
```

- 关闭指定网络接口示例。
``` shell
sudo sysctl -w net.ipv6.conf.eth0.disable_ipv6=1
``` 

#### 持久关闭

- 通过修改/etc/sysctl.conf文件持久化关闭IPv6。

a. 修改/etc/sysctl.conf配置文件。
```shell
net.ipv6.conf.all.disable_ipv6=1
net.ipv6.conf.default.disable_ipv6=1
```

b. 如果需要关闭指定网络接口，修改示例如下。
```shell
net.ipv6.conf.eth0.disable_ipv6=1
``` 

重启实例。
```shell
sudo reboot
``` 