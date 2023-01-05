---
layout: post
title:  debug programs with "sudo" in vscode
date:   2022-12-12 16:02:55
categories: vscode
tags: vscode
---

* content
{:toc}

- create a script called "gdbasroot" in e.g. my home directory, containing: ``sudo /usr/bin/gdb "$@"``
``` shell
➜  ls -l /home/rhel/sbin/gdbasroot             
-rwxrwxr-x. 1 rhel rhel 23 Jul 14 10:00 /home/rhel/sbin/gdbasroot
➜  cat /home/rhel/sbin/gdbasroot       
sudo /usr/bin/gdb "$@"
```
- make it executable
- 修改 sudoer 配置，未用户设置gdb免密码
``` json
    ## Allows people in group wheel to run all commands
    %wheel  ALL=(ALL)       ALL
    rhel ALL=(ALL) NOPASSWD:/usr/bin/gdb
```
- modify the launch.json in VSCode to call the script (obviously change username accordingly) by adding "miDebuggerPath":
``` json
...
    "externalConsole": false,
    "miDebuggerPath": "/home/rhel/sbin/gdbasroot",
    "MIMode": "gdb",
...
```

