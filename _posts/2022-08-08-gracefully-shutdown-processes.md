---
layout: post
title: "In what order should I send signals to gracefully shutdown processes?"
date: 2022-08-08 09:42:41
categories: linux
tags: linux signals
---

* content
{:toc}

#### highest score answer
SIGTERM tells an application to terminate. The other signals tell the application other things which are unrelated to shutdown but may sometimes have the same result. Don't use those. If you want an application to shut down, tell it to. Don't give it misleading signals.

Some people believe the smart standard way of terminating a process is by sending it a slew of signals, such as HUP, INT, TERM and finally KILL. This is ridiculous. The right signal for termination is SIGTERM and if SIGTERM doesn't terminate the process instantly, as you might prefer, it's because the application has chosen to handle the signal. Which means it has a very good reason to not terminate immediately: It's got cleanup work to do. If you interrupt that cleanup work with other signals, there's no telling what data from memory it hasn't yet saved to disk, what client applications are left hanging or whether you're interrupting it "mid-sentence" which is effectively data corruption.

For more information on what the real meaning of the signals is, see sigaction(2). Don't confuse "Default Action" with "Description", they are not the same thing.

SIGINT is used to signal an interactive "keyboard interrupt" of the process. Some programs may handle the situation in a special way for the purpose of terminal users.

SIGHUP is used to signal that the terminal has disappeared and is no longer looking at the process. That is all. Some processes choose to shut down in response, generally because their operation makes no sense without a terminal, some choose to do other things such as recheck configuration files.

SIGKILL is used to forcefully remove the process from the kernel. It is special in the sense that it's not actually a signal to the process but rather gets interpreted by the kernel directly.

Don't send SIGKILL. - should certainly never be sent by scripts. If the application handles the , it can take it a second to cleanup, it can take a minute, it can take an hour. Depending on what the application has to get done before it's ready to end. Any logic that "assumes" an application's cleanup sequence has taken long enough and needs to be shortcut or SIGKILLed after X seconds is just plain wrong.SIGKILLSIGTERM

The only reason why an application would need a to terminate, is if something bugged out during its cleanup sequence. In which case you can open a terminal and it manually. Aside from that, the only one other reason why you'd something is because you WANT to prevent it from cleaning itself up.SIGKILLSIGKILLSIGKILL

Even though half the world blindly sends after 5 seconds it's still horribly wrong thing to do.SIGKILL

#### seond high score answer
Short Answer: Send , 30 seconds later, . That is, send , wait a bit (it may vary from program to program, you may know your system better, but 5 to 30 seconds is enough. When shutting down a machine, you may see it automatically waiting up to 1'30s. Why the hurry, after all?), then send .SIGTERMSIGKILLSIGTERMSIGKILL

Reasonable Answer: , , This is more than enough. The process will very probably terminate before .SIGTERMSIGINTSIGKILLSIGKILL

Long Answer: , , , , SIGTERMSIGINTSIGQUITSIGABRTSIGKILL

This is unnecessary, but at least you are not misleading the process regarding your message. All these signals do mean you want the process to stop what it is doing and exit.

No matter what answer you choose from this explanation, keep that in mind!

If you send a signal that means something else, the process may handle it in very different ways (on one hand). On the other hand, if the process doesn't handle the signal, it doesn't matter what you send after all, the process will quit anyway (when the default action is to terminate, of course).

So, you must think as yourself as a programmer. Would you code a function handler for, lets say, to quit a program that connects with something, or would you loop it to try to connect again? That is the main question here! That is why it is important to just send signals that mean what you intend.SIGHUP

Almost Stupid Long Answer:

The table bellow contains the relevant signals, and the default actions in case the program does not handle them.

I ordered them in the order I suggest to use (BTW, I suggest you to use the reasonable answer, not this one here), if you really need to try them all (it would be fun to say the table is ordered in terms of the destruction they may cause, but that is not completely true).

The signals with an asterisk (*) are NOT recommended. The important thing about these is that you may never know what it is programmed to do. Specially ! It may start the apocalipse (it is a free signal for a programmer do whatever he/she wants!). But, if not handled OR in the unlikely case it is handled to terminate, the program will terminate.SIGUSR

In the table, the signals with default options to terminate and generate a core dump are left in the end, just before .SIGKILL

Signal     Value     Action   Comment
----------------------------------------------------------------------
SIGTERM      15       Term    Termination signal
SIGINT        2       Term    Famous CONTROL+C interrupt from keyboard
SIGHUP        1       Term    Disconnected terminal or parent died
SIGPIPE      13       Term    Broken pipe
SIGALRM(*)   14       Term    Timer signal from alarm
SIGUSR2(*)   12       Term    User-defined signal 2
SIGUSR1(*)   10       Term    User-defined signal 1
SIGQUIT       3       Core    CONTRL+\ or quit from keyboard
SIGABRT       6       Core    Abort signal from abort(3)
SIGSEGV      11       Core    Invalid memory reference
SIGILL        4       Core    Illegal Instruction
SIGFPE        8       Core    Floating point exception
SIGKILL       9       Term    Kill signal
Then I would suggest for this almost stupid long answer: , , , , , , SIGTERMSIGINTSIGHUPSIGPIPESIGQUITSIGABRTSIGKILL

And finally, the

Definitely Stupid Long Long Answer:

Don't try this at home.

SIGTERM, , , , , , , , , , , and if nothing worked, .SIGINTSIGHUPSIGPIPESIGALRMSIGUSR2SIGUSR1SIGQUITSIGABRTSIGSEGVSIGILLSIGFPESIGKILL

SIGUSR2 should be tried before because we are better off if the program doesn't handle the signal. And it is much more likely for it to handle if it handles just one of them.SIGUSR1SIGUSR1

BTW, the KILL: it is not wrong to send to a process, as other answer stated. Well, think what happens when you send a command? It will try and only. Why do you think that is the case? And why do you need any other signals, if the very command uses only these two?SIGKILLshutdownSIGTERMSIGKILLshutdown

Now, back to the long answer, this is a nice oneliner:

for SIG in 15 2 3 6 9 ; do echo $SIG ; echo kill -$SIG $PID || break ; sleep 30 ; done
It sleeps for 30 seconds between signals. Why else would you need a oneliner? ;)

Also, recommended: try it with only signals from the reasonable answer. 15 2 9

safety: remove the second when you are ready to go. I call it my for onliners. Always use it to test.echodry-run

Script killgracefully

Actually I was so intrigued by this question that I decided to create a small script to do just that. Please, feel free to download (clone) it here:

GitHub link to Killgracefully repository