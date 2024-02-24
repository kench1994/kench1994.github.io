---
layout: post
title:  windows clone(from ssd to NVMe)
date:   2024-02-24 07:54:02
categories: windows
tags: windows
---

* content
{:toc}

先正常启动进入系统开启NVMe驱动加载

``sc config stornvme start=boot``

然后进入 ``PE`` 进行用 ``Ghost`` 或者傲梅进行系统克隆