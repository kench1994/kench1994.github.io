---
layout: post
title: "shell echo with/without quotes"
date: 2022-08-29 09:14:19
categories: shell
tags: linux
---

* content
{:toc}

Expression	|	Result	|	Comments
:-------- | :--------| :--
"$a"	|	apple	|	variables are expanded inside ""
'$a'	|	$a	|	variables are not expanded inside ''
"'$a'"	|	'apple'	|	'' has no special meaning inside ""
'"$a"'	|	"$a"	|	"" is treated literally inside ''
'\''	| invalid	| can not escape a ' within ''; use "'" or $'\'' (ANSI-C quoting)
"red$arocks"	|	red	|	$arocks does not expand $a; use ${a}rocks to preserve $a
"redapple$"	|	redapple$	|	$ followed by no variable name evaluates to $
'\"'	|	\"	|	\ has no special meaning inside ''
"\'"	|	\'	|	\' is interpreted inside "" but has no significance for '
"\""	|	"	|	\" is interpreted inside ""
"*"	|	*	|	glob does not work inside "" or ''
"\t\n"	|	\t\n	|	\t and \n have no special meaning inside "" or ''; use ANSI-C quoting
"`echo hi`"	|	hi	|	`` and $() are evaluated inside "" (backquotes are retained in actual output)
'`echo hi`'	|	`echo hi`	|	`` and $() are not evaluated inside '' (backquotes are retained in actual output)
'${arr[0]}'	|	${arr[0]}	|	array access not possible inside ''
"${arr[0]}"	|	apple	|	array access works inside ""
$'$a\''	|	$a'	|	single quotes can be escaped inside ANSI-C quoting
"$'\t'"	|	$'\t'	|	ANSI-C quoting is not interpreted inside ""
'!cmd'	|	!cmd	|	history expansion character '!' is ignored inside ''
"!cmd"	|	cmd args	|	expands to the most recent command matching "cmd"
$'!cmd'	|	!cmd	|	history expansion character '!' is ignored inside ANSI-C quotes

**double-quote in expansion prevents unwanted word splitting and globbing**