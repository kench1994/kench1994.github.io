---
layout: post
title:  worksteal线程池
date:   2023-06-27 14:50:24
categories: 并发编程
tags: worksteal 线程池
---

* content
{:toc}

work steal线程池解决了多队列线程池中“饥饿等待”的问题，即有的线程队列中的任务很多处理不过来，有的线程队列是空的，处于“饥饿等待”状态，work steal可以将其它线程队列中的任务steal过来避免“饥饿等待”，提高了线程池执行效率。



### 实现思路

之前的线程池都是通过阻塞方式拿到队列中的任务:

``` c++
    bool pop(T &item) {
        std::unique_lock lock(_mutex);
        _cond.wait(lock, [&]() { return !_queue.empty() || _stop; });
        if (_queue.empty())
            return false;
        item = std::move(_queue.front());
        _queue.pop();
        return true;
    }
```

条件变量会一直阻塞等待直到拿到一个task为止，在线程竞争激烈的情况下这个阻塞会比较耗时，是不是可以在这里做这样一个改进：能不能先通过非阻塞方式从其它队列中尝试取一下任务呢？如果取不到，则再以阻塞方式去取任务。

这个改进有两个好处：

- 减少了阻塞等待，因为它是以非阻塞方式尝试从其它队列中取任务的，无需阻塞；

- 实现了work steal，因为从别的队列拿任务相当于做了work steal。

c++标准库提供了std::try_to_lock_t以非阻塞方式获取锁，这正适合用work steal场景里通过非阻塞方式从其它队列拿到任务。

当然这只是work steal实现的一种思路，还有很多实现work steal的思路，读者可以根据需要去实现其它方式的work steal。

### 接口设计
为了实现work steal需要对之前的Queue增加增加非阻塞方式拿到任务的接口:

``bool try_pop(T& item);``
为了保持足够的灵活性，可以在不需要work steal的时候，让线程池关闭work steal功能，因此还需要增加一个try_pop_if来控制是否需要做work steal的接口：

``bool try_pop_if(T& item, bool (*predict)(T&) = nullptr);``
thread pool需要入队和出队的逻辑，入队的时候尝试以非阻塞方式将任务放到其它队列中，尝试失败则随机选择某个线程的队列；出队的时候先尝试从其它线程队列中steal一下，如果steal成功就执行steal来的任务，如果失败就通过阻塞等待方式拿任务。

### 实现
``` c++
work steal实现的核心代码：

  inline ThreadPool::ThreadPool(int32_t threadNum, bool enableWorkSteal)
    : _threadNum(threadNum ? threadNum : std::thread::hardware_concurrency()),
    _queues(_threadNum),
    _enableWorkSteal(enableWorkSteal),
    _stop(false) {
    auto worker = [this](int32_t id) {
      auto current = getCurrent();
      current->first = id;
      current->second = this;
      while (true) {
        WorkItem workerItem = {};
        if (_enableWorkSteal) {
          // Try to do work steal firstly.
          for (auto n = 0; n < _threadNum * 2; ++n) {
            if (_queues[(id + n) % _threadNum].try_pop_if(
              workerItem,
              [](auto& item) { return item.canSteal; }))
              break;
          }
        }

        // If _enableWorkSteal false or work steal failed, wait for a pop
        // task.
        if (!workerItem.fn && !_queues[id].pop(workerItem)) {
          break;
        }

        if (workerItem.fn) {
          workerItem.fn();
        }
      }
    };

    _threads.reserve(_threadNum);
    for (auto i = 0; i < _threadNum; ++i) {
      _threads.emplace_back(worker, i);
    }
  }

  inline ThreadPool::ERROR_TYPE ThreadPool::scheduleById(std::function<void()> fn,
    int32_t id) {
    if (nullptr == fn) {
      return ERROR_POOL_ITEM_IS_NULL;
    }

    if (_stop) {
      return ERROR_POOL_HAS_STOP;
    }

    if (id == -1) {
      if (_enableWorkSteal) {
        // Try to push to a non-block queue firstly.
        WorkItem workerItem{/*canSteal = */ true, fn };
        for (auto n = 0; n < _threadNum * 2; ++n) {
          if (_queues.at(n % _threadNum).try_push(workerItem))
            return ERROR_NONE;
        }
      }

      id = rand() % _threadNum;
      _queues[id].push(
        WorkItem{/*canSteal = */ _enableWorkSteal, std::move(fn) });
    }
    else {
      assert(id < _threadNum);
      _queues[id].push(WorkItem{/*canSteal = */ false, std::move(fn) });
    }

    return ERROR_NONE;
  }
```

work steal线程池的完整代码在``github.com/alibaba/async_simple``里面，它被async_simple的Executor所使用。如果觉得不错请点赞关注async_simple开源库，赠人玫瑰，手留余香！

后续还会带来更多async_simple的使用示例和源码分析。

### 性能比较
async_simple里面有一个benchmark的测试用例，测试了启用work steal和不启用时的执行任务的耗时情况。

测试代码：
``` c++
#include "ThreadPool.bench.h"
#include <atomic>
#include <cassert>

using namespace async_simple::util;

const int COUNT = 500'000;
const int REPS = 10;

void autoScheduleTasks(bool enableWorkSteal) {
    std::atomic<int> count = 0;
    {
        ThreadPool tp(std::thread::hardware_concurrency(), enableWorkSteal);

        for (int i = 0; i < COUNT; ++i) {
            [[maybe_unused]] auto ret = tp.scheduleById([i, &count] {
                count++;
                int x;
                auto reps = REPS + (REPS * (rand() % 5));
                for (int n = 0; n < reps; ++n)
                    x = i + rand();
                (void)x;
            });
            assert(ret == ThreadPool::ERROR_TYPE::ERROR_NONE);
        }
    }
    assert(count == COUNT);
}

void ThreadPool_noWorkSteal(benchmark::State& state) {
    for ([[maybe_unused]] const auto& _ : state)
        autoScheduleTasks(/*enableWorkSteal = */ false);
}

void ThreadPool_withWorkSteal(benchmark::State& state) {
    for ([[maybe_unused]] const auto& _ : state)
        autoScheduleTasks(/*enableWorkSteal = */ true);
}
```
测试结果：
``` shell
ThreadPool_noWorkSteal    3123980502 ns
ThreadPool_withWorkSteal   831945141 ns
```
worksteal线程池性能提升了3x以上，性能提升还是挺明显的