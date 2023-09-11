---
layout: post
title:  cmake 备忘
date:   2023-06-29 13:36:42
categories: cmake 
tags: cmake
---

* content
{:toc}

### 1. 生成构建系统(Unix Makefile、Ninja、Visual Studio等)

``` shell
cmake -H. -Bbuild
```
- -H表示当前目录中搜索根CMakeLists.txt文件。
- -Bbuild告诉CMake在一个名为build的目录中生成所有的文件。

##### 相当于

``` shell
$ mkdir -p build
$ cd build
$ cmake ..
```

### 2. 开始编译
``` shell
$ cmake --build ./build/ [--target <target-name>]
```

============================================================

``CMakeLists.txt sample``

``` cmake
cmake_minimum_required (VERSION 3.15)
set(PROJECT_NAME trcsrvcore)
project(${PROJECT_NAME} VERSION w.x.y.z)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 11)

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_C_FLAGES "${CMAKE_C_FLAGES} -O3 -Wall")
set(CMAKE_CXX_FLAGES "${CMAKE_CXX_FLAGES} -O3 -Wall")

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/version.h.in
    ${CMAKE_CURRENT_SOURCE_DIR}/version.h)

string(FIND "${CMAKE_TOOLCHAIN_FILE}" "aarch64" res)
if(NOT ${res} EQUAL -1)
    set(PUBLIC_LIBS xx/aarch64/libs_x64)
    set(OUTPUT_PATH ${PROJECT_BINARY_DIR}/../xx/aarch64)
else()
    set(PUBLIC_LIBS xx/x86_64/libs_x64)
    set(OUTPUT_PATH ${PROJECT_BINARY_DIR}/../xx/x86_64)
endif()

message(STATUS "PUBLIC_LIBS : ${PUBLIC_LIBS}")

#设置gcc、g++编译参数
add_compile_options(-fPIC -fvisibility=hidden)

#添加宏定义
set(EXPORTS_DEF "${PROJECT_NAME}_EXPORTS")
string(TOUPPER "${EXPORTS_DEF}" EXPORTS_DEF)
add_definitions("-D${EXPORTS_DEF}")

#当前项目文件
file(GLOB SRC_CURRENT
    ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}.rc
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    )

#添加生成动态库
add_library(${PROJECT_NAME} SHARED ${SRC_CURRENT} ${SRC_PUBLIC})
#设置库文件的输出目录
SET(LIBRARY_OUTPUT_PATH ${OUTPUT_PATH})		#设置库文件的输出目录

#添加包含目录
target_include_directories(${PROJECT_NAME} PRIVATE xx/inc .)

#设置链接参数
target_link_options(${PROJECT_NAME} PRIVATE -Wl,-rpath,. -Wl,-rpath,./lib3rds_x64/ -Wl,-rpath,./libs_x64/ 
    -Wl,-rpath,../lib3rds_x64/ -Wl,-rpath,../libs_x64/
    -Wl,-Bsymbolic -Wl,--exclude-libs,ALL -Wl,--no-undefined
    -Wl,--version-script=${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}.version)

#添加链接库目录
target_link_directories(${PROJECT_NAME} PRIVATE ${PUBLIC_LIBS})
#添加链接库
target_link_libraries(${PROJECT_NAME} PRIVATE xx)
#以下为linux编译需要的依赖
target_link_libraries(${PROJECT_NAME} PRIVATE dl pthread)
```