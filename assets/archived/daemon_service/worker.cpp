#include <stdio.h>
#include <syslog.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <pwd.h>
#include <grp.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "proto.h"

extern char **environ;

static int sigterm_signaled = 0;
static int sigintr_signaled = 0;

void sig_handler(int signo, siginfo_t *siginfo, void *ucontext)
{
    switch (signo)
    {
    case SIGINT:
        sigintr_signaled = 1;
        break;
    case SIGTERM:
        sigterm_signaled = 1;
        break;
    case SIGQUIT:
        sigterm_signaled = 1;
        break;
    case SIGIO:
    default:
        break;
    }
    return ;
}

/* 注册信号处理函数 */
int register_signals()
{
    int handle_signals[] = {SIGINT, SIGTERM, SIGQUIT, SIGIO};
    int ignore_signals[] = {SIGHUP, SIGUSR1, SIGUSR2, SIGPIPE};

    struct sigaction sa;
    for (size_t i = 0; i < sizeof(handle_signals) / sizeof(int); i++)
    {
        memset(&sa, 0, sizeof(sa));
        sa.sa_sigaction = sig_handler;
        sa.sa_flags = SA_SIGINFO;
        sigemptyset(&sa.sa_mask);
        if (sigaction(handle_signals[i], &sa, NULL) == -1)
        {
            syslog(LOG_ERR, "sigaction(%d) failed: %s", handle_signals[i], strerror(errno));
            return -1;
        }
    }

    for (size_t i = 0; i < sizeof(ignore_signals) / sizeof(int); i++)
    {
        memset(&sa, 0, sizeof(sa));
        sa.sa_handler = SIG_IGN;
        sigemptyset(&sa.sa_mask);
        if (sigaction(ignore_signals[i], &sa, NULL) == -1)
        {
            syslog(LOG_ERR, "sigaction(%d) failed: %s", ignore_signals[i], strerror(errno));
            return -1;
        }
    }

    return 0;
}

/* 保存环境变量 */
int save_env()
{
    size_t size = 0;
    int i;

    for (i = 0; environ[i] != NULL; ++i)
    {
        size += strlen(environ[i]) + 1;
    }

    char *p = (char *)calloc(1, size);
    if (!p)
        return -1;

    for (i = 0; environ[i] != NULL; ++i)
    {
        size_t len = strlen(environ[i]) + 1;
        strncpy(p, environ[i], len);
        environ[i] = p;
        p += len;
    }
    return 0;
}

/* 设置进程标题  */
int set_proc_title(const char *name, int argc, char **argv)
{
    char *end = argv[argc-1] + strlen(argv[argc-1]);
    strcpy(argv[0], name);
    char *p = argv[0]+strlen(name);
    if (end > p)
        memset(p, 0, end-p);
    return 0;
}

/* 初始化完成后的工作 */
int init_done(int argc, char **argv)
{
    struct message msg;
    msg.pid = getpid();
    msg.status = RUNNING;

#if 0
    // 通知master子进程初始化完成
    int nwrite = write(CHANNEL_ID, &msg, sizeof(msg));
    if (nwrite != sizeof(msg))
    {
        syslog(LOG_ERR, "write: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }

    // 获取进程编号
    // 由于使用exec的方式，无法通过与父进程共享变量的方式来得到进程编号
    int nread = read(CHANNEL_ID, &msg, sizeof(msg));
    if (nread != sizeof(msg))
    {
        syslog(LOG_ERR, "write: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }
#endif

    // 修改进程名
    char buf[12];
    snprintf(buf, sizeof(buf), "worker[%d]", msg.worker_id);
    save_env();
    set_proc_title(buf, argc, argv);
    return 0;
}

int release()
{
    close(CHANNEL_ID);
    syslog(LOG_INFO, "child %d self destruction", getpid());
    return 0;
}

/* 改变进程当前运行用户/组 */
int change_identity(const char *user)
{
    if (user == NULL)
    {
        syslog(LOG_ERR, "empty user");
        return -1;
    }

    struct group *grp = getgrnam(user);
    if (grp != NULL)
    {
        gid_t gid = grp->gr_gid;
        if (setgid(gid) == -1)
        {
            syslog(LOG_ERR, "failed to setgid for group: %s", user);
            return -1;
        }
        if (initgroups(user, gid) == -1)
        {
            syslog(LOG_ERR, "initgroups failed for user: %s", user);
            return -1;
        }
    }

    struct passwd *pwd = getpwnam(user);
    if (pwd == NULL)
    {
        syslog(LOG_ERR, "failed to getpwnam for user: %s", user);
        return -1;
    }

    uid_t uid = pwd->pw_uid;
    if (setuid(uid) == -1)
    {
        syslog(LOG_ERR, "failed to setuid for user: %s", user);
        return -1;
    }

    return 0;
}

int main(int argc, char **argv)
{
    openlog("worker", LOG_PID | LOG_NDELAY, LOG_LOCAL0);
    const char *user = NULL;
    int opt;
    while ((opt = getopt(argc, argv, "u:")) != -1)
    {
        switch (opt)
        {
        case 'u':
            user = optarg;
            break;
        default:
            break;
        }
    }

    // setuid && setgid
    if (user != NULL && geteuid() == 0)
    {
        if (change_identity(user) == -1)
        {
            syslog(LOG_ERR, "change identity[%s] failed", user);
            exit(EXIT_FAILURE);
        }
    }

    if (register_signals() == -1)
    {
        syslog(LOG_ERR, "register signal failed");
        exit(EXIT_FAILURE);
    }

    init_done(argc, argv);

    syslog(LOG_INFO, "child %d is alive", getpid());

    // Do work
    sleep(1);

    if (sigterm_signaled)
    {
        syslog(LOG_INFO, "child %d terminate at user request", getpid());
        goto end;
    }

    if (sigintr_signaled)
    {
        syslog(LOG_INFO, "child %d received sigintr", getpid());
        goto end;
    }
end:
    release();
    exit(0);
}
