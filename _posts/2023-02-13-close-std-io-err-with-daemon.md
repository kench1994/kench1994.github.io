---
layout: post
title:  Why is it necessary to close standard input/output/error when writing a daemon?
date:   2023-02-13 09:12:32
categories: program
tags: program
---

* content
{:toc}

#### ANSWER1:
Not only stdin, stdout and stderr should be closed, but all open files.

From "Advanced Programming in the UNIX Environment", W. Richard Stevens, Addison-Weseley, 18th Printing, 1999, page 417.
>Unneeded file descriptors should be closed. This prevents the daemon from holding open any descriptors that is may have inherited from its parent (which could be a shell or some other process).

Mr. Stevens proposal is to get the maximum file descriptor and close all files up to that value.

The cited chapter is about 'Daemon Processes'. Please note that closing file descriptors is only one point from five when writing daemons.


#### ANSWER2:

You don't have to definitely close standard output/error, but you have to be sure where your daemon (and child) are going to writing to. Redirecting the output to a log file is a common practice.

Closing standard input is required so that the daemon (and/or child) doesn't get stuck trying to read some input from the user.

But if you're going to detach you daemon from a controlling TTY using ``setsid()``, the standard input/output/error are going to be invalid.