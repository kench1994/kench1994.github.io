---
layout: post
title:  openssl 生成证书
date:   2023-10-27 11:17:41
categories: 加密学
tags: 证书
---

* content
{:toc}

- 构成部分:
密码算法库
密钥和证书封装管理功能
SSL通信API接⼝

- ⽤途
建⽴ RSA、DH、DSA key 参数
建⽴ X.509 证书、证书签名请求(CSR)和CRLs(证书回收列表)
计算消息摘要
使⽤各种 Cipher加密/解密
SSL/TLS 客户端以及服务器的测试
处理S/MIME 或者加密邮件

``` shell
# 生成 RSA 私钥(无加密),默认输出格式为 PKCS#1-PEM
openssl genrsa 2048 >ca-key.pem
openssl req -new -x509 -nodes -days 3600 -key ca-key.pem -out ca-cert.pem
openssl req -newkey rsa:2048 -days 3600 -nodes -keyout server-key.pem -out server-req.pem
# 生成 RSA 公钥
openssl rsa -in server-key.pem -out server-key.pem
openssl x509 -req -in server-req.pem -days 3600 -CA ca-cert.pem -CAkey ca-key.pem -set_serial 01 -out server-cert.pem
```

https://blog.csdn.net/qq_45662588/article/details/117534628