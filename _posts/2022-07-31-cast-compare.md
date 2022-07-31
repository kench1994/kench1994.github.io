---
layout: post
title:  "cast compare"
date:   2020-10-16 19:14:08
categories: c/c++
tags: program
---

* content
{:toc}

**static_cast**

- C++ compiler already knows how to convert between scaler types such as float to int. Use static_cast for them.
- When you ask compiler to convert from type A to B, static_cast calls B's constructor passing A as param. Alternatively, A could have a conversion operator (i.e. A::operator B()). If B doesn't have such constructor, or A doesn't have a conversion operator, then you get compile time error.
- Cast from A* to B* always succeeds if A and B are in inheritance hierarchy (or void) otherwise you get compile error.
- Gotcha: If you cast base pointer to derived pointer but if actual object is not really derived type then you don't get error. You get bad pointer and very likely a segfault at runtime. Same goes for A& to B&.
- Gotcha: Cast from Derived to Base or viceversa creates new copy! For people coming from C#/Java, this can be a huge surprise because the result is basically a chopped off object created from Derived.

**dynamic_cast**

- dynamic_cast uses runtime type information to figure out if cast is valid. For example, (Base*) to (Derived*) may fail if pointer is not actually of derived type.
- This means, dynamic_cast is very expensive compared to static_cast!
- For A* to B*, if cast is invalid then dynamic_cast will return nullptr.
- For A& to B& if cast is invalid then dynamic_cast will throw bad_cast exception.
- Unlike other casts, there is runtime overhead.


**const_cast**

- While static_cast can do non-const to const it can't go other way around. The const_cast can do both ways.
- One example where this comes handy is iterating through some container like set<T> which only returns its elements as const to make sure you don't change its key. However if your intent is to modify object's non-key members then it should be ok. You can use const_cast to remove constness.
- Another example is when you want to implement T& SomeClass::foo() as well as const T& SomeClass::foo() const. To avoid code duplication, you can apply const_cast to return value of one function from another.


**reinterpret_cast**

- This basically says that take these bytes at this memory location and think of it as given object.
- For example, you can load 4 bytes of float to 4 bytes of int to see how bits in float looks like.
- Obviously, if data is not correct for the type, you may get segfault.
- There is no runtime overhead for this cast.