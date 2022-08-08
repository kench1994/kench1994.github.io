---
layout: post
title:  "Pass SIGINT to debugging terminal in vscode"
date:   2022-08-08 09:17:30
categories: linux
tags: gdb vscode linux signals
---

* content
{:toc}

1. Requires "stopAtEntry": true in lauch.json
2. In the Debug Console of vs code write:
``` shell
-exec handle SIGINT pass
-exec handle SIGINT nostop
```

[future documents](https://sourceware.org/gdb/current/onlinedocs/gdb/Signals.html)
