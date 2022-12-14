---
layout: post
title: "rpm拉取package方法"
date: 2021-12-30 22:19:13
categories: linux
tags: linux
---

* content
{:toc}

1.Install the yum-utils package:
``` shell
yum install yum-utils
```

2.Run the command followed by the desired package:
``` shell
yumdownloader <package>
```