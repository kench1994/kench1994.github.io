---
layout: post
title:  Static library linking issues
date:   2022-11-23 10:42:58
categories: linux
tags: linux
---

* content
{:toc}

- You won't get dependencies issues when packing compiled object files to static library because the the linker is not involved yet.
- You do have to link to all the libraries depended by other libraries directly used by your program in the linking phase
- When linking to static libraries, you should order them according to dependency. If A rely on B, then A goes before B. (TIP: if A rely on B while B also rely on A, use -lA -lB -lA)

Links to other related questions:

- GCC Static library linking order: [Why does the order in which libraries are linked sometimes cause errors in GCC?](https://stackoverflow.com/questions/45135/linker-order-gcc?lq=1) (see the 150+ answer, not the answer the questioner marked correct)
- Recursive dependency: [How to deal with recursive dependencies between static libraries using the binutils linker?](https://stackoverflow.com/questions/2738292/how-to-deal-with-recursive-dependencies-between-static-libraries-using-the-binut) (Both answers are helpful)