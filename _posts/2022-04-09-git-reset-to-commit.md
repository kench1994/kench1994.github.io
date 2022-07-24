---
layout: post
title: "git reset to commit"
date: 2022-04-09 22:03:13
categories: program
tags: git
---

* content
{:toc}

根据时间范围查看commit log;空格翻页,b返回上一页
``` shell
$ git log --since 2018-08-02 --before 2018-11-15
```

git reset,回到指定commit
``` shell
$ git reset --hard f1eb3aa4b95540a8fb4a8e8d341e4c08136bd756
Updating files: 100% (10043/10043), done.
HEAD is now at f1eb3aa4b Vulkan port (#4664)
```