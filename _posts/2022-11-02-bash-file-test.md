---
layout: post
title:  Bash File Testing
date:   2022-11-01 13:41:34
categories: linux
tags: linux
---

* content
{:toc}

``-b filename`` - Block special file

``-c filename`` - Special character file

``-d directoryname`` - Check for directory Existence

``-e filename`` - Check for file existence, regardless of type (node, directory, socket, etc.)

``-f filename`` - Check for regular file existence not a directory

``-G filename`` - Check if file exists and is owned by effective group ID

``-G filename`` set-group-id - True if file exists and is set-group-id

``-k filename`` - Sticky bit

``-L filename`` - Symbolic link

``-O filename`` - True if file exists and is owned by the effective user id

``-r filename`` - Check if file is a readable

``-S filename`` - Check if file is socket

``-s filename`` - Check if file is nonzero size

``-u filename`` - Check if file set-user-id bit is set

``-w filename`` - Check if file is writable

``-x filename`` - Check if file is executable


**How to use:**

``` shell
#!/bin/bash
file=./file
if [ -e "$file" ]; then
    echo "File exists"
else 
    echo "File does not exist"
fi 
```

A test expression can be negated by using the ! operator
``` shell
#!/bin/bash
file=./file
if [ ! -e "$file" ]; then
    echo "File does not exist"
else 
    echo "File exists"
fi 
```