---
layout: post
title: "What does OpenSSL's EVP mean?"
date: 2022-08-11 09:12:34
categories: program
tags: openssl
---

* content
{:toc}

20

This explanation from doc/ssleat.txt should give you enough of an answer:

EVP The Digital **EnVeloPe** library is quite large. At it's core are function to perform encryption and decryption by parts while using an initial parameter to specify which of the 17 different ciphers or 4 different message digests to use. On top of these are implmented the digital signature functions, sign, verify, seal and open. Base64 encoding of binary data is also done in this library.