---
layout: post
title:  openssl 生成证书
date:   2023-10-27 11:17:41
categories: 加密学
tags: 证书
---

* content
{:toc}

``` shell
openssl genrsa 2048 >ca-key.pem
openssl req -new -x509 -nodes -days 3600 -key ca-key.pem -out ca-cert.pem
openssl req -newkey rsa:2048 -days 3600 -nodes -keyout server-key.pem -out server-req.pem
openssl rsa -in server-key.pem -out server-key.pem
openssl x509 -req -in server-req.pem -days 3600 -CA ca-cert.pem -CAkey ca-key.pem -set_serial 01 -out server-cert.p
```