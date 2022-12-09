---
layout: post
title:  What does "%.*s" mean as a format specifier in printf?
date:   2022-12-08 16:31:50
categories: program
tags: c/c++
---

* content
{:toc}

``%.*s`` means print the first X number of characters from the following buffer. In this case, print the first sizeof(outv->deliveryAddressCity) - 1 characters from mi->deliveryAddressCity, preventing writing beyond the bounds of outv->deliveryAddressCity.

**A shorter example:**
``` c++
printf("%.*s", 4, "hello world");
```

would print ``hell``.

https://cplusplus.com/reference/cstdio/printf/