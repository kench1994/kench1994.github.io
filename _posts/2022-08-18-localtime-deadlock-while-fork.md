---
layout: post
title: "localtime函数的死锁风险"
date: 2022-08-18 17:38:27
categories: program
tags: c/c++
---

* content
{:toc}

## **01** localtime函数说明
(1) 函数定义
``` struct tm *localtime(const time_t *t); ``` 

(2) 函数说明：将1970.01.01 00:00:00 到现在经过的秒数转换为换成真实世界所使用的时间日期。

(3) 返回值：返回结构tm的指针，代表目前的当地时间。

## **02** localtime函数使用
``` c++
int main(int argc, char *argv[]) {    
    time_t t0 = time(NULL);    
    time_t t1 = t0 + 1800;   
    struct tm* tm1 = localtime(&t0);    
    struct tm* tm2 = localtime(&t1);     
    char str1[50] = {0};    
    char str2[50] = {0};   
     
    strftime(str1, 50, "%H:%M:%S", tm1);    
    strftime(str2, 50, "%H:%M:%S", tm2);  

    printf("%s\n", str1);    
    printf("%s\n", str2);  

    return 0; 
}
```

假设当前运行时间为22:00:26，那么上述代码运行结果会是什么呢？

``` shell 
[xxxx@xx-xxx-xxxx-xx00 test]$ gcc localtime_test.c -o test 
[xxxx@xx-xxx-xxxx-xx00 test]$ ./test
22:30:26 
22:30:26
```

输出的两个时间其实是一样的，两者并不是相差半个小时。猜想第二次调用``localtime``的返回值把tm1的值给覆盖了，内部可能使用了``tm``类型全局变量。查看``localtime``函数代码如下：

``` c++
/* The C Standard says that localtime and gmtime return the same pointer. */
struct tm _tmbuf;

.....

/* Return the `struct tm' representation of *T in local time. */

struct tm * localtime (const time_t *t) {  
return __tz_convert (t, 1, &_tmbuf); 
}
```

可以看到，``localtime``调用``__tz_conver``t传入的第三个参数``_tmbuf``是一个全局变量，并且``__tz_convert``返回值就是``_tmbuf``的指针。返回指针所指向的全局变量可能被其他线程调用``localtime``给覆盖掉。因此，``localtime``并不是线程安全的。应使用线程安全的``localtime_r``函数代替它。``localtime_r``代码如下：

``` c++
static struct tm * 
localtime_r (const time_t *t, struct tm *tp) 
{  
  struct tm *l = localtime (t);  
  if (! l)    
   return 0;   
  *tp = *l;  
  return tp; 
}
```

由此可见，``localtime_r``内部还是调用了``localtime``，但是，每次调用完成后，马上将返回结果填充到传入的第二个参数``tp``指向的内存里，再返回``tp``，因此，该函数线程是可重入的.

尽管在多线程下，调用``localtime_r``是线程安全的，但是性能可能会受到影响。例如：在输出日志输出时，我们需要通过``localtime_r``获取当前时间，多线程并发调用时，容易发生锁等待，导致性能下降。锁的产生来源于``__tz_convert``的调用，该函数的部分实现如下：

``` c++
struct tm * 
__tz_convert (const time_t *timer, int use_localtime, struct tm *tp) 
{  
  long int leap_correction;  
  int leap_extra_secs;    
   ......   
    
  __libc_lock_lock (tzset_lock);   //加锁   
   ...... //时间转换逻辑   
  __libc_lock_unlock (tzset_lock); //释放锁     
   ...... 
  return tp; 
}
```

## **03** 死锁问题
``localtime_r`` 是线程安全的，但是，对如下两种情况并不安全，甚至会引发死锁。

（1）信号处理函数调用``localtime``：假如进程调用``localtime``，已经获取全局锁，且并没有释放。此时，如果进程接收到信号，在信号处理函数也调用了``localtime``，就会造成死锁。

（2）多线程下``fork``：在多线程下，若线程A调用``localtime``，已经获取全局锁，尚未释放锁。此时，假如线程B调用了``fork``，并且在子进程也调用``localtime``，也会造成死锁，导致子进程一直被``hang``住。因为``fork``出来的子进程只会复制调用它的线程，而其他线程不会被复制到子进程执行，也就是说当前子进程中只有线程B在运行。子进程会复制父进程的用户空间数据，包括了锁的信息。

Redis的日志输出函数``redisLogRaw``中也是调用``localtime``来获取时间的。同时，Redis也是会存在多线程fork的情况。那么，Redis会不会有前文提到的性能问题和死锁问题呢？

在5.0.0之前的版本，Redis确实是使用``localtime``来进行时间转换。但是，由于Redis是单线程的架构，不存在竞争的情况，因此，一般情况下，不存在多个线程调用竞争而导致性能下降或者线程不安全的问题。

我们说Redis是单线的，指它在处理所有的请求都是在一个线程完成的。其实，Redis还是有后台线程，异步去完成某些任务的。截至目前版本，Redis共有三个后台线程，作用如下：

- 异步关闭文件
- AOF的刷盘
- 异步删除大Key


在一定条件下，Redis会启用这些线程后台完成一些任务。因此，还是存在前文提到死锁的风险。既然阻塞的方式获取时间转换会导致死锁，那么，就需要一个无锁、无阻塞的函数替换掉``localtime``。在5.0.0版本中就实现了这样一个函数——``nolocks_localtime``，如下代码实现：

``` c++
void nolocks_localtime(struct tm *tmp, time_t t, time_t tz, int dst) {
     const time_t secs_min = 60;     
     const time_t secs_hour = 3600;     
     const time_t secs_day = 3600*24;      
     
     t -= tz;                           /* Adjust for timezone. */     
     t += 3600*dst;                     /* Adjust for daylight time. */     
     time_t days = t / secs_day;        /* Days passed since epoch. */     
     time_t seconds = t % secs_day;     /* Remaining seconds. */      
     
     tmp->tm_isdst = dst;    
     tmp->tm_hour = seconds / secs_hour;    
     tmp->tm_min = (seconds % secs_hour) / secs_min;    
     tmp->tm_sec = (seconds % secs_hour) % secs_min;    
     
     /* 1/1/1970 was a Thursday, that is, day 4 from the POV of the tm structure * where sunday = 0, so to calculate the day of the week we have to add 4 * and take the modulo by 7. */     
     tmp->tm_wday = (days+4)%7;    
     /* Calculate the current year. */     
     tmp->tm_year = 1970;    
     while(1) {        
          /* Leap years have one day more. */         
          time_t days_this_year = 365 + is_leap_year(tmp->tm_year);        
          if (days_this_year > days) break;         
          days -= days_this_year;        
          tmp->tm_year++;    
   }    
   tmp->tm_yday = days;  /* Number of day of the current year. */
  
    /* We need to calculate in which month and day of the month we are. To do * so we need to skip days according to how many days there are in each * month, and adjust for the leap year that has one more day in February. */     
    int mdays[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};    
    mdays[1] += is_leap_year(tmp->tm_year);    
    
    tmp->tm_mon = 0;   
    while(days >= mdays[tmp->tm_mon]) {        
    days -= mdays[tmp->tm_mon];        
    tmp->tm_mon++;     
    }    
    
    tmp->tm_mday = days+1;  /* Add 1 since our 'days' is zero-based. */     
    tmp->tm_year -= 1900;   /* Surprisingly tm_year is year-1900. */

}
```

``nolocks_localtime``在功能上与``localtime``一样，都是将``time_t``类型的时间戳转换成包含年月日的``tm``类型，但是，它是非阻塞的、无锁的，且线程安全的，多线程下``fork``也是安全的。缺点就是需要自己实现时间的转换逻辑。

## **总结**

（1）在实现日志输出函数或者接口时，时间转换应当尽量避免使用``localtime``或者``localtime_r``函数，尤其是在多线程的调用环境下，可能会影响程序的性能。可以考虑使用一个全局的时间变量，让某个线程定期去更新该时间变量，其他线程直接读取该时间变量。Redis也有类似的用法，在``serverCron``函数(默认每秒调用10次)中调用``updateCacheTime``函数来更新全局的``unix time``。 

（2）信号处理函数或多线程``fork()``中，尽量避免使用这种会持有全局锁的函数，以免造成死锁的情况。