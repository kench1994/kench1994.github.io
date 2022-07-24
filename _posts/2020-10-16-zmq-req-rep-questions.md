---
layout: post
title:  "ZMQ Reliable Request-Reply Patterns(trouble 4 me)"
date:   2020-10-16 19:18:13
categories: c/c++
tags: zmq
---

* content
{:toc}

What is “Reliability”?

Most people who speak of “reliability” don’t really know what they mean. We can only define reliability in terms of failure. That is, if we can handle a certain set of well-defined and understood failures, then we are reliable with respect to those failures. No more, no less. So let’s look at the possible causes of failure in a distributed ZeroMQ application, in roughly descending order of probability:

Application code is the worst offender. It can crash and exit, freeze and stop responding to input, run too slowly for its input, exhaust all memory, and so on.

System code–such as brokers we write using ZeroMQ–can die for the same reasons as application code. System code should be more reliable than application code, but it can still crash and burn, and especially run out of memory if it tries to queue messages for slow clients.

Message queues can overflow, typically in system code that has learned to deal brutally with slow clients. When a queue overflows, it starts to discard messages. So we get “lost” messages.

Networks can fail (e.g., WiFi gets switched off or goes out of range). ZeroMQ will automatically reconnect in such cases, but in the meantime, messages may get lost.

Hardware can fail and take with it all the processes running on that box.

Networks can fail in exotic ways, e.g., some ports on a switch may die and those parts of the network become inaccessible.

Entire data centers can be struck by lightning, earthquakes, fire, or more mundane power or cooling failures.

form 