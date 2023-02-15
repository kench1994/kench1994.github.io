---
layout: post
title:  vscode 自动附加调试子进程
date:   2023-02-14 15:35:43
categories: vscode
tags: vscode
---

* content
{:toc}

``` shell
{
    // 使用 IntelliSense 了解相关属性。 
    // 悬停以查看现有属性的描述。
    // 欲了解更多信息，请访问: https://go.microsoft.com/fwlink/?linkid=830387
    "version": "0.2.0",
    "configurations": [
        {
            "name": "(gdb) 启动",
            "type": "cppdbg",
            "request": "launch",
            "program": "${workspaceFolder}/build/x86_64/pack/8f6e0c12/DlpFileEngine",
            "args": [],
            "stopAtEntry": true,
            "cwd": "${workspaceFolder}/build/x86_64/pack/8f6e0c12/",
            // "environment": [
            //     {
            //         "name": "LD_LIBRARY_PATH",
            //         "value": "${LD_LIBRARY_PATH}:${workspaceFolder}/build/pack/8f6e0c12/lib3rds_x64"
            //     }
            // ],
            "externalConsole": false,
            "MIMode": "gdb",
            "miDebuggerPath": "/home/rhel/sbin/gdbasroot",
            "setupCommands": [
                {
                    "description": "Enable funcy printing to gdb",
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
            ],
        },
        {
            "name": "(gdb) 附加",
            "type": "cppdbg",
            "request": "attach",
            "program": "${workspaceFolder}/build/x86_64/pack/8f6e0c12/DlpFileEngine",
            "processId": "${input:readPID}",
            "MIMode": "gdb",
            "miDebuggerPath": "/home/rhel/sbin/gdbasroot",
            "setupCommands": [
                {
                    "description": "Enable funcy printing to gdb",
                    "text": "-enable-pretty-printing",
                    "ignoreFailures": true
                }
                ,
                {   "description":"In this mode GDB will be attached to both processes after a call to fork() or vfork().",
                    "text": "-gdb-set detach-on-fork off",
                    "ignoreFailures": true
                }
                ,
                {   "description": "The new process is debugged after a fork. The parent process runs unimpeded.",
                    "text": "-gdb-set follow-fork-mode child",
                    "ignoreFailures": true
                }
            ]
        },
        {
            "name": "(gdb) attach",
            "type": "cppdbg",
            "request": "attach",
            "program": "${workspaceFolder}/build/x86_64/pack/8f6e0c12/DlpFileEngine",
            "processId": "${command:pickProcess}",
            "MIMode": "gdb",
            "miDebuggerPath": "/home/rhel/sbin/gdbasroot",
            "setupCommands": [
                {
                    "description": "Enable funcy printing to gdb",
                    "text": "-enable-pretty-printing",
                    "ignoreFailures": true
                }
                ,
                {   "description":"In this mode GDB will be attached to both processes after a call to fork() or vfork().",
                    "text": "-gdb-set detach-on-fork off",
                    "ignoreFailures": true
                }
                ,
                {   "description": "The new process is debugged after a fork. The parent process runs unimpeded.",
                    "text": "-gdb-set follow-fork-mode child",
                    "ignoreFailures": true
                }
            ]
        }
    ]  
    ,
    //https://stackoverflow.com/questions/66164972/vs-code-how-to-get-process-id-via-shell-command-in-launch-json
    "inputs": [
        {
          "id": "readPID",
          "type": "command",
          "command": "extension.commandvariable.file.content",
          "args": {
            "fileName": "${workspaceFolder}/build/x86_64/pack/8f6e0c12/var/proftpd.pid"
          }
        }
      ]
}
 ```