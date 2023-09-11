---
layout: post
title:  linux 修改 coreump 生成路径
date:   2023-09-10 16:55:14
categories: linux
tags: coredump
---

* content
{:toc}

``echo "%e-%p-%t.core" | sudo tee /proc/sys/kernel/core_pattern``