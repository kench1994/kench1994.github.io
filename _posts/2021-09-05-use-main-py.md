---
layout: post
title:  "Use __main__.py"
date:   2020-12-10 19:11:13
categories: python
tags: python
---

* content
{:toc}

We have all seen the __init__.py file and know its role, but what is __main__.py? I have seen many Python projects either on Github or at work that do not take advantage of this magic file. In my opinion, including a __main__.py is a better way to interface with multi-file python modules.

But first some background: How do most people run their Python programs? Occasionally you will write a program that is organized enough so that it can both be imported as a module to another program or run by itself through a command line interface. In this case, you are probably familiar with the following lines commonly placed at the bottom of the file:

if __name__ == __main__:
  main(sys.argv)
When you run a program by calling the Python interpreter on it, the magic global variable __name__ gets set to __main__. We can use that to know that we are not being imported as a module, but ran. For example:

python myapp.py This works just fine for single file projects.

The problem If you are like me, you do not want your entire application in a single python file. Separating related logic out into their own files makes for easier editing and maintainability. For example:

``` shell
.
├── README.me
├── requirements.txt
├── setup.py
└── src
    ├── __init__.py
    ├── client.py
    ├── logic.py
    ├── models.py
    └── run.py
```

But then this raises confusion to a user who just downloaded your program. Which one is the main file? Is it run.py? Or maybe client.py? What file has my precious if __name__ == __main__ statement? This is where we can get a lot of value from __main__.py.

main.py The magic file main.py is called when you run your project with the -m module flag. If you code is intended to be used a module first, and command line interface second, this makes perfect sense to use. Think of it as a place we can put whatever would be in our body of our if __name__ == __main__ statement. Refactoring the project above to conform:

``` shell
.
├── README.me
├── requirements.txt
├── setup.py
└── myapp
    ├── __init__.py
    ├── __main__.py
    ├── client.py
    ├── logic.py
    ├── models.py
```
    
And now we just run it as a module not a singleton program.

python -m myapp
__main__.py will automatically be executed. This is a perfect place to put a command line interface and handle program arguments!

Thanks for reading!