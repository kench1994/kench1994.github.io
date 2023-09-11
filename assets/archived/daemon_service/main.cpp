#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <syslog.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/wait.h>
#include <limits.h>
#include <assert.h>
#include <libgen.h>

#include "proto.h"

#define NMAXPROCESS  	16
#define PROC_NAME	"bgserv"

// 单线程
static int reload_signaled = 0;
static int sigchld_signaled = 0;
static int sigterm_signaled = 0;
static int sigintr_signaled = 0;
static int sigalrm_signaled = 0;

static fd_set event_rmask;
static int event_max_fd = 0;

static const char *service = NULL;

static int nworkers = NMAXPROCESS;

static time_t cache_time;

static const char *pid_file = "";

struct process_info {
    pid_t 		pid;
    proc_status	status;
    int			retries;
    int			abnormal;
    int			start_times;
    int			exit_signal;
    int			channel[2];
} processes[NMAXPROCESS];

int service_idx = -1;
// TODO make these configable
struct _service_map {
    const char *service;
    const char *relpath;
    const char *pidfile;
    bool		accept_args;
    char file_path[PATH_MAX];
} service_map[] = {
    {"control",
     "ctrlserver/dlpctrserver",
     "/var/run/policy_engine.pid",
     true,
     ""
    },
    {"collector",
     "trbusserver/trbsrvcoretester",
     "/var/run/collector.pid",
     false,
     ""
    },
    {"fileservice",
     "/usr/local/tiprayftp/sbin/proftpd",
     "/var/run/file_service.pid",
     false,
     ""
    },
};

const char *user = "root";

void show_help(const char *name)
{
    printf("%s -n number\n"
            "\t-n\tnumber of workers\n"
            "\t-u\trun as user\n"
            "\t-s\tservice name {control|cp|file_service|detector|collector}", name);
}

int my_strtol(char *buf)
{
    char *endptr = NULL;
    long n = strtol(buf, &endptr, 10);
    if (buf == endptr || *endptr != '\0' ||
            (errno == ERANGE && (n == LONG_MAX || n == LONG_MIN)))
    {
        return -1;
    }
    return n;
}

FILE *sig_filehandler = NULL;

void init()
{
    FD_ZERO(&event_rmask);
    int i = 0;
    for (; i < nworkers; i++)
    {
        processes[i].channel[0] = -1;
        processes[i].channel[1] = -1;
        processes[i].pid = -1;
        processes[i].retries = 0;
        processes[i].abnormal = 0;
        processes[i].status = IDLE;
        processes[i].exit_signal = 0;
    }

    sig_filehandler = fopen("sig.log", "a+");
}

long read_pid_from_file(const char *pidfile)
{
    char buf[20] = {0};
    FILE *fp = fopen(pidfile, "r");
    if (fp == NULL)
    {
        return 0;
    }
    int nread = fread(buf, 1, sizeof(buf), fp);
    if (nread == 0)
    {
        fclose(fp);
       return -1;
    }
    fclose(fp);

    if (buf[nread-1] == '\n')
    {
        buf[nread-1] = '\0';
        --nread;
    }

    long pid = my_strtol(buf);
    if (pid == -1)
    {
        syslog(LOG_ERR, "stale pid file");
        return -1;
    }
    return pid;
}

int create_pidfile()
{
    char pid_s[20];
    FILE *fp = NULL;

    // PID文件已存在
    if (access(pid_file, F_OK) == 0)
    { // do some logging
        pid_t pid = read_pid_from_file(pid_file);
        if (pid > 0 && kill(pid, 0) == 0)
        {
            syslog(LOG_ERR, "another instance is already running");
            return -1;
        } else
        {
            // 旧进程产生的PID文件没有删除
            syslog(LOG_ALERT, "old pidfile exists, reuse it");
        }
    }

    if ((fp = fopen(pid_file, "w+")) == NULL)
    {
        syslog(LOG_ERR, "fopen failed: %s", strerror(errno));
        return -1;
    }

    pid_t pid = getpid();
    int n = snprintf(pid_s, sizeof(pid_s), "%d\n", pid);
    if (fwrite(pid_s, 1, n, fp) != (size_t)n)
    {
        syslog(LOG_ERR, "fwrite failed: %s", strerror(errno));
        return -1;
    }
    fclose(fp);

    return 0;
}

int delete_pidfile()
{
    if (access(pid_file, F_OK) != 0)
    {
        syslog(LOG_ERR, "PID file %s not found", pid_file);
        return -1;
    }
    if (unlink(pid_file) != 0)
    {
        syslog(LOG_ERR, "unlink failed: %s", strerror(errno));
        return -1;
    }
    return 0;
}

int daemonlize()
{
    int fd = 0;

    switch (fork())
    {
    case -1:
        fprintf(stderr, "fork failed: %s", strerror(errno));
        return -1;
    case 0:
        break;
    default:
        exit(EXIT_SUCCESS);
    }

    if (setsid() == -1)
    {
        fprintf(stderr, "setsid() failed: %s", strerror(errno));
        return -1;
    }

    if ((fd = open("/dev/null", O_RDWR)) == -1)
    {
        fprintf(stderr, "open /dev/null: %s", strerror(errno));
        return -1;
    }

    if (dup2(fd, STDIN_FILENO) == -1)
    {
        fprintf(stderr, "dup2() failed: %s", strerror(errno));
        return -1;
    }

    if (dup2(fd, STDOUT_FILENO) == -1)
    {
        fprintf(stderr, "dup2() failed: %s", strerror(errno));
        return -1;
    }

    if (dup2(fd, STDERR_FILENO) == -1)
    {
        fprintf(stderr, "dup2() failed: %s", strerror(errno));
        return -1;
    }

    return 0;
}

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
        break;
    case SIGCHLD:
    {
        sigchld_signaled = 1;

        int status;
        pid_t pid;
        while ((pid = waitpid(-1, &status, WNOHANG)) > 0)
        {
            char buf[1024];
            snprintf(buf, sizeof buf, "SIGCHLD from child pid: %d\n", pid);
            fwrite(buf, strlen(buf), 1, sig_filehandler);
            fflush(sig_filehandler);

            int i;
            for (i = 0; i < nworkers; i++)
            {
                if (processes[i].pid == pid)
                {
                    if (WIFSIGNALED(status))
                        processes[i].exit_signal = WTERMSIG(status);
                    if (processes[i].status == EXITING)
                    {
                        processes[i].status = EXITED;
                    } else
                    {
                        if (processes[i].status == STARTING)
                            processes[i].abnormal++;
                        processes[i].status = EXITING;
                    }
                } else
                {
                    snprintf(buf, sizeof buf, "processes[%d].pid[%d] != pid[%d]\n", i, processes[i].pid, pid);
                }
            }
        }
        break;
    }
    case SIGALRM:
        sigalrm_signaled = 1;
        break;
    default:
        break;
    }
    return ;
}

int register_signals()
{
    int handle_signals[] = {SIGINT, SIGTERM, SIGQUIT, SIGCHLD, SIGALRM, SIGIO};
    int ignore_signals[] = {SIGHUP, SIGUSR1, SIGUSR2, SIGPIPE};

    struct sigaction sa;
    for (size_t i = 0; i < sizeof(handle_signals)/sizeof(int); i++)
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

    for (size_t i = 0; i < sizeof(ignore_signals)/sizeof(int); i++)
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

void Syslog(int pri, const char* fmt, ...)
{
    int log = 1;
    if (log)
    {
        va_list args;
        va_start(args, fmt);
        vsyslog(pri, fmt, args);
        va_end(args);
    }
}

int spawn_workers()
{
    int i = 0;
    pid_t pid;
    int has_exiting = 0;

    for (i = 0; i < nworkers; i++)
    {
        switch (processes[i].status)
        {
        case EXITING:
            has_exiting = 1;
            continue;

        case EXITED:
            // 正常退出不重试
            if (processes[i].exit_signal == 0)
            {
                Syslog(LOG_INFO, "process[%d] exit normally, do nothing", i);
                continue;
            }
            // TODO 一定时间内的重试次数
            if (processes[i].retries >= 3)
            {
                Syslog(LOG_ERR, "process[%d] startup retried %d times, giving up",
                       i, processes[i].retries);
                continue;
            }
            Syslog(LOG_INFO, "child got sig: %d", processes[i].exit_signal);

            /* FALL THROUGH */
        case IDLE:
        {
            if (socketpair(AF_UNIX, SOCK_STREAM, 0, processes[i].channel) == -1)
            {
                Syslog(LOG_ERR, "socketpair error: %s", strerror(errno));
                return -1;
            }
            // nonblock
            int on = 1;
            if(ioctl(processes[i].channel[0], FIONBIO, &on) == -1)
            {
                Syslog(LOG_ERR, "ioctl nonblock: %s", strerror(errno));
                return -1;
            }
            // async
            if (ioctl(processes[i].channel[0], FIOASYNC, &on) == -1)
            {
                Syslog(LOG_ERR, "ioctl async: %s", strerror(errno));
                return -1;
            }
            // setown
            if (fcntl(processes[i].channel[0], F_SETOWN, getpid()) == -1)
            {
                Syslog(LOG_ERR, "fcntl setown: %s", strerror(errno));
                return -1;
            }
            // no FD_CLOEXEC since we want the execved child process to inherit socket
            if (fcntl(processes[i].channel[0], F_SETFD, FD_CLOEXEC) == -1)
            {
                Syslog(LOG_ERR, "fcntl cloexec error: %s", strerror(errno));
                return -1;
            }

            //Syslog(LOG_INFO, "start to fork");
            switch (pid = fork())
            {
            case 0:
            {
                if (dup2(processes[i].channel[1], CHANNEL_ID) == -1)
                {
                    Syslog(LOG_ERR, "dup2 error: %s", strerror(errno));
                    return -1;
                }
                close(processes[i].channel[1]);
                char *exe_path = strdup(service_map[service_idx].file_path);
                const char *dir = dirname(exe_path);
                if (chdir(dir) != 0)
                {
                    free(exe_path);
                    Syslog(LOG_ERR, "change dir to %s failed", dir);
                    return -1;
                }
                free(exe_path);

                if (service_map[service_idx].accept_args)
                    execlp(service_map[service_idx].file_path, service_map[service_idx].file_path, "-u", user, NULL);
                else
                    execlp(service_map[service_idx].file_path, service_map[service_idx].file_path, NULL);
                Syslog(LOG_ERR, "execlp[%s] failed : %s", service_map[service_idx].file_path, strerror(errno));
                break;
            }
            case -1:
                Syslog(LOG_ERR, "fork error: %s", strerror(errno));
                return -1;

            default:
                processes[i].pid = pid;
                close(processes[i].channel[1]);
                // add to event loop
                Syslog(LOG_INFO, "add fd %d[%d] to event list", processes[i].channel[0], pid);
                FD_SET(processes[i].channel[0], &event_rmask);
                if (event_max_fd < processes[i].channel[0])
                    event_max_fd = processes[i].channel[0];

                if (processes[i].status == EXITED)
                {
                    processes[i].retries++;
                } else
                {
                    Syslog(LOG_INFO, "process[%d] start 1 time", i);
                }

                processes[i].status = STARTING;
                processes[i].start_times++;
                break;
            }
        }
            break;

        default:
            break;
        }
    }
    return has_exiting;
}

int main(int argc, char *argv[])
{
    int opt;
    while ((opt = getopt(argc, argv, "n:s:u:h")) != -1)
    {
        switch (opt)
        {
        case 'n':
        {
            nworkers = my_strtol(optarg);
            if (nworkers == -1)
            {
                fprintf(stderr, "no number found in %s\n", optarg);
                exit(EXIT_FAILURE);
            }
            if (nworkers > NMAXPROCESS)
            {
                fprintf(stderr, "worker number[%d] is too large(>%d)\n", nworkers, NMAXPROCESS);
                exit(EXIT_FAILURE);
            }
            if (nworkers <= 0)
            {
                fprintf(stderr, "worker number[%d] is too small(<= 0)\n", nworkers);
                exit(EXIT_FAILURE);
            }
        }
            break;
        case 's':
        {
            service = optarg;
            for (size_t i = 0; i < sizeof(service_map)/sizeof(service_map[0]); ++i)
            {
                if (strcmp(service, service_map[i].service) == 0)
                {
                    service_idx = i;
                    break;
                }
            }
            if (service_idx == -1)
            {
                fprintf(stderr, "service %s not found\n", service);
                exit(EXIT_FAILURE);
            }
            if (service_map[service_idx].relpath[0] == '/')
            {
                strncpy(service_map[service_idx].file_path, service_map[service_idx].relpath, PATH_MAX);
            } else
            {
                getcwd(service_map[service_idx].file_path, PATH_MAX);

                int len = strlen(service_map[service_idx].file_path);
                snprintf(service_map[service_idx].file_path+len,
                         PATH_MAX-len,
                         "/%s",
                         service_map[service_idx].relpath);
            }

            pid_file = service_map[service_idx].pidfile;
            break;
        }
        case 'u':
            user = optarg;
            break;
        case 'h':
        default:
            show_help(argv[0]);
            exit(EXIT_SUCCESS);
        }
    }

    // 后台服务化
    if (daemonlize() != 0)
    {
        fprintf(stderr, "daemonlize failed\n");
        exit(EXIT_FAILURE);
    }

    init();

    openlog(PROC_NAME, LOG_PID | LOG_NDELAY, LOG_LOCAL0);
    // 注册信号
    if (register_signals() != 0)
    {
        syslog(LOG_ERR, "register_signals failed");
        exit(EXIT_FAILURE);
    }
    // 创建PID文件
    if (create_pidfile() != 0)
    {
        syslog(LOG_ERR, "create_pidfile failed");
        exit(EXIT_FAILURE);
    }
    // 创建工作进程
    spawn_workers();

    // 进入循环等待
    for (;;)
    {
        struct timeval tv;
        tv.tv_sec = 1;
        tv.tv_usec = 0;

        char ids[1024] = {0};
        for (int i = 0; i < event_max_fd+1; i++)
        {
            if (FD_ISSET(i, &event_rmask))
            {
                if (*ids != '\0')
                    snprintf(ids + strlen(ids), sizeof(ids), ",%d", i);
                else
                   snprintf(ids, sizeof(ids), "%d", i);
            }
        }
        if (*ids != '\0')
            Syslog(LOG_INFO, "event listen on: [%s]", ids);

        fd_set rmask = event_rmask;
        int count = select(event_max_fd+1, &rmask, NULL, NULL, &tv);
        if (count > 0)
        {
            int i = 0;
            char ids[1024] = {0};
            for (i = 0; i < event_max_fd+1; i++)
            {
                if (FD_ISSET(i, &rmask))
                {
                    if (*ids != '\0')
                        snprintf(ids + strlen(ids), sizeof(ids), ",%d", i);
                    else
                       snprintf(ids, sizeof(ids), "%d", i);
                }
            }
            Syslog(LOG_INFO, "triggered ids: [%s]", ids);

            for (i = 0; i < nworkers; i++)
            {
                if (processes[i].channel[0] > 0 && FD_ISSET(processes[i].channel[0], &rmask))
                {
                    Syslog(LOG_INFO, "handle fd %d", processes[i].channel[0]);
                    count--;

                    // read from child
                    struct message msg;
                    int nread = read(processes[i].channel[0], &msg, sizeof(msg));
                    if (nread > 0)
                    {
                        if (nread != sizeof(msg))
                        {
                            Syslog(LOG_ERR, "garbage reading from client");
                            continue;
                        }
                        if (msg.pid != processes[i].pid)
                        {
                            Syslog(LOG_ERR, "message not from origin child, something is happening");
                            continue;
                        }
                        switch (msg.status)
                        {
                        case RUNNING:
                        {
                            Syslog(LOG_INFO, "child %d is at running status", msg.pid);
                            processes[i].status = RUNNING;

                            msg.worker_id = i;
                            int nwrite = write(processes[i].channel[0], &msg, sizeof(msg));
                            if (nwrite != sizeof(msg))
                            {	// XXX more processing
                                Syslog(LOG_ERR, "write worker id to child failed: ", strerror(errno));
                                continue;
                            }

                            break;
                        }
                        default:
                            break;
                        }
                    } else if (nread == 0)	// worker has exited
                    {
                        Syslog(LOG_INFO, "client connection closed");

                        FD_CLR(processes[i].channel[0], &event_rmask);
                        Syslog(LOG_INFO, "remove fd %d from event list", processes[i].channel[0]);
                        close(processes[i].channel[0]);

                        processes[i].channel[1] = -1;
                        processes[i].channel[0] = -1;

                        if (processes[i].status == EXITING)
                        {
                            processes[i].status = EXITED;
                            syslog(LOG_INFO, "[%s]process %d EXITING -> EXITED", __FUNCTION__, processes[i].pid);
                        }
                        else
                        {
                            processes[i].status = EXITING;
                            syslog(LOG_INFO, "[%s]process %d RUNNING -> EXITING", __FUNCTION__, processes[i].pid);
                        }
                    } else
                    {
                        Syslog(LOG_ERR, "read error: %s", strerror(errno));
                    }
                }
            }
            assert(count == 0);
        }

        // TODO 如果子进程都退出，或者在一定时间内重试一定次数后仍然处于退出状态，则退出管理进程

        if (sigterm_signaled || sigintr_signaled)
        {
            syslog(LOG_INFO, "sigterm: %d, sigintr: %d", sigterm_signaled, sigintr_signaled);
            int i = 0;
            bool all_exit = true;
            for (; i < nworkers; ++i)
            {
                if (processes[i].status != EXITED && processes[i].status != EXITING)
                {
                    all_exit = false;
                    kill(processes[i].pid, SIGTERM);
                }
            }
            if (all_exit)
            {
                goto end;
            }
        }
        if (sigalrm_signaled)
        {
            sigalrm_signaled = 0;
        }

        if (sigchld_signaled)
        {
            if (sigterm_signaled || sigintr_signaled)
            {
                syslog(LOG_INFO, "waiting for children");
                for (int i = 0; i < nworkers; i++)
                {
                    if (processes[i].status != EXITED)
                    {
                        syslog(LOG_INFO, "process[%d] still alive", i);
                    }
                }
            } else if (spawn_workers() == 0)
                sigchld_signaled = 0;
        }
    }

end:
    // 清理
    if (delete_pidfile() != 0)
    {
        syslog(LOG_ERR, "delete pidfile error");
    }
    syslog(LOG_INFO, "exiting...");

    closelog();
}
