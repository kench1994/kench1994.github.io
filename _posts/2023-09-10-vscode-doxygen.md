---
layout: post
title:  vscode中使用doxygen插件生成注释模板
date:   2023-09-10 16:20:16
categories: vscode
tags: doxygen
---

* content
{:toc}

``CTRL`` + ``SHITFT`` + ``p`` => ``Preference:Open User Settings(JSON)``

``` json
{
	// generic 通用信息
	"doxdocgen.generic.authorName": "huangc",
	"doxdocgen.generic.authorTag": "@author {author}",
	// 日期格式与模板
	"doxdocgen.generic.dateFormat": "YYYY-MM-DD",
	"doxdocgen.generic.dateTemplate": "@date {date}",

	// 文件注释的格式
	"doxdocgen.file.fileTemplate": "@File Name: {name}",
	"doxdocgen.file.versionTag": "@version 1.0",
	"doxdocgen.file.copyrightTag": [
		"@copyright Copyright (c) {year} 厦门天锐科技股份有限公司"
	],
	// 文件注释的组成及其排序
	"doxdocgen.file.fileOrder": [
		"copyright", //版权
		"brief",     // 简介
		"version",   // 版本
		"author",    // 作者
		"date",      // 日期
		"empty",     // 空行
		"custom"     // 自定义
	],
	// 根据自动生成的注释模板
	"doxdocgen.generic.order": [
		"brief",
		"date",
		"author",
		"tparam",
		"param",
		"return"
	],
	"doxdocgen.generic.paramTemplate": "@param{indent:8}{param}{indent:25}Param",
	"doxdocgen.generic.returnTemplate": "@return {type} ",
	"doxdocgen.generic.splitCasingSmartText": true,
}
```