---
layout: post
title:  Centos 不启动 X
date:   2023-02-14 16:58:33
categories: linux
tags: linux
---

* content
{:toc}

#### 启用X
``systemctl set-default multi-user.target``

#### 不启用X
``systemctl set-default graphical.target``