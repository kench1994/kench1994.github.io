---
layout: post
title: "vscode xmake工程头文件跳转"
date: 2022-08-11 09:12:34
categories: program
tags: openssl
---

* content
{:toc}


生成 compile_commands.json
``` shell
xmake project -k compile_commands .vscode/
```

``Ctrl+Shift+P`` -> **C/C++: Edit Configurations (UI)** 进入``c_cpp_properties.json``

``compileCommands``字段中指定``xmake``生成的json文件
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