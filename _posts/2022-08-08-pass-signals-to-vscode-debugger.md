---
layout: post
title: "Pass SIGINT to debugging terminal in vscode"
date: 2022-08-08 09:17:30
categories: linux
tags: gdb
---

* content
{:toc}

``` shell
{
    "version": "0.2.0",
    "configurations": [    
        {
            "name": "debugging of local server",
            "type": "cppdbg",
            "request": "attach",
            "program": "/usr/local/pgsql/bin/postgres",
            "processId": "${command:pickProcess}",
            "MIMode": "gdb",
            "setupCommands": [
                {
                    "description": "Enable pretty-printing for gdb",
                    "text": "-enable-pretty-printing",
                    "ignoreFailures": true
                }
                ,
                {   "description": "The new process is debugged after a fork. The parent process runs unimpeded.",
                    "text": "-gdb-set follow-fork-mode child",
                    "ignoreFailures": true
                }
                ,
                {
                    "description": "ignore SIGINT signal",
                    "text": "handle SIGINT nostop noprint pass"
                }
            ]            
        }
    ]
}
```