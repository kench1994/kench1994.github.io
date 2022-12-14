---
layout: post
title: "vscode xmake工程头文件跳转"
date: 2022-08-11 09:12:34
categories: vsscode
tags: vsscode
---

* content
{:toc}


使用``xmake project -k compile_commands .vscode/`` 指令生成 ``compile_commands.json``

**vscode** 界面依次按下 ``Ctrl+Shift+P`` 快捷键,然后选择 **C/C++: Edit Configurations (JSON)** 进入``c_cpp_properties.json``

编辑**compileCommands** 字段值为刚刚 **xmake** 生成的``json``文件
``` json
{
    "configurations": [
        {
            "name": "Linux",
            "includePath": [
                "${workspaceFolder}/**"
            ],
            "defines": [],
            "compilerPath": "/usr/bin/gcc",
            "cStandard": "c99",
            "cppStandard": "c++11",
            "intelliSenseMode": "linux-gcc-x64",
            "compileCommands": "${workspaceFolder}/.vscode/compile_commands.json"
        }
    ],
    "version": 4
}
```