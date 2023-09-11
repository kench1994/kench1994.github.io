#include <unistd.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <dlfcn.h>
#include <syslog.h>
#include <thread>
#include <future>
#include <functional>

#include "simpleini/SimpleIni.h"

static const char *service = nullptr;
const char *user = "root";
const char *config = "services.ini";

char pid_filepath[PATH_MAX];

static int sigterm_signaled = 0;
static int sigintr_signaled = 0;
static int sigalrm_signaled = 0;

struct EntryNames
{
    EntryNames() : module(nullptr),
                   init(nullptr),
                   loop(nullptr),
                   release(nullptr)
    { }
    ~EntryNames()
    {
        free_name(module);
        free_name(init);
        free_name(loop);
        free_name(release);
    }
    void free_name(char *&name)
    {
        if (name)
        {
            free(name);
            name = nullptr;
        }
    }

    char *module;
    char *init;
    char *loop;
    char *release;
};

typedef int 	(*InitFunc)();
typedef void 	(*ReleaseFunc)();
typedef void	(*LoopFunc)();

struct ServiceEntry
{
    ServiceEntry() : init(nullptr),
                     loop(nullptr),
                     release(nullptr),
                     handle(nullptr)
    { }

    ~ServiceEntry()
    {
        if (handle != nullptr)
        {
            if (dlclose(handle) != 0)
            {
                syslog(LOG_ERR, "dlclose() failed: %s",dlerror());
            }
            handle = nullptr;
        }
    }

    InitFunc	init;
    LoopFunc	loop;
    ReleaseFunc release;
    void*		handle;
};

void show_help(const char *name)
{
    printf("%s -u\trun as user\n"
            "\t-c\tconfig_file"
            "\t-s\tservice name {engine|detector|collector}", name);
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
    int handle_signals[] = {SIGINT, SIGTERM, SIGQUIT, SIGALRM, SIGIO};
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

    if (service == nullptr || *service == '\0')
        return -1;

    snprintf(pid_filepath, sizeof(pid_filepath), "./%s.pid", service);

    // PID文件已存在
    if (access(pid_filepath, F_OK) == 0)
    { // do some logging
        pid_t pid = read_pid_from_file(pid_filepath);
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

    if ((fp = fopen(pid_filepath, "w+")) == NULL)
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
    if (access(pid_filepath, F_OK) != 0)
    {
        syslog(LOG_ERR, "PID file %s not found", pid_filepath);
        return -1;
    }
    if (unlink(pid_filepath) != 0)
    {
        syslog(LOG_ERR, "unlink failed: %s", strerror(errno));
        return -1;
    }
    return 0;
}

int read_config(const char *service_name, EntryNames &name)
{
    if (config == nullptr || *config == '\0')
    {
        fprintf(stderr, "empty config\n");
        return -1;
    }
    CSimpleIniA ini(true);
    ini.LoadFile(config);

    CSimpleIniA::TNamesDepend sections;
    ini.GetAllSections(sections);

    CSimpleIniA::TNamesDepend::iterator it;
    for (it = sections.begin(); it != sections.end(); ++it)
    {
        if (strcmp(it->pItem, service_name) == 0)
        {
            break;
        }
    }
    if (it == sections.end())
    {
        return -1;
    }

    CSimpleIniA::TNamesDepend keys;
    ini.GetAllKeys(service_name, keys);
    for (it = keys.begin(); it != keys.end(); ++it)
    {
        const char *key = it->pItem;
        if (strcmp(key, "init_func") == 0)
        {
            name.init = strdup(ini.GetValue(service_name, key));
        } else if (strcmp(key, "release_func") == 0)
        {
            name.release = strdup(ini.GetValue(service_name, key));
        } else if (strcmp(key, "main_loop_func") == 0)
        {
            name.loop = strdup(ini.GetValue(service_name, key));
        } else if (strcmp(key, "library") == 0)
        {
            name.module = strdup(ini.GetValue(service_name, key));
        }
    }

    if (name.init == nullptr || name.release == nullptr)
        return -1;

    return 0;
}

int LoadModule(EntryNames &name, ServiceEntry &entry)
{
    if (name.init == nullptr || name.release == nullptr)
    {
        fprintf(stderr, "init or release function name empty\n");
        return -1;
    }

    char module_name[PATH_MAX];
    snprintf(module_name, sizeof(module_name), "lib%s.so", name.module);

    void *handle = dlopen(module_name, RTLD_LAZY);
    if (handle == nullptr)
    {
        fprintf(stderr, "dlopen failed: %s\n", dlerror());
        return -1;
    }

    entry.init = (InitFunc)dlsym(handle, name.init);
    if (entry.init == nullptr)
    {
        fprintf(stderr, "dlsym failed: %s\n", dlerror());
        dlclose(handle);
        return -1;
    }
    entry.release = (ReleaseFunc)dlsym(handle, name.release);
    if (entry.release == nullptr)
    {
        fprintf(stderr, "dlsym failed: %s\n", dlerror());
        dlclose(handle);
        return -1;
    }

    if (name.loop != nullptr)
    {
        entry.loop = (LoopFunc)dlsym(handle, name.loop);
        if (entry.loop == nullptr)
        {
            fprintf(stderr, "dlsym failed: %s\n", dlerror());
            return -1;
        }
    }

    entry.handle = handle;
    return 0;
}

int main(int argc, char **argv)
{
    int opt;
    while ((opt = getopt(argc, argv, "c:s:u:h")) != -1)
    {
        switch (opt)
        {
        case 'c':
        {
            config = optarg;
            break;
        }
        case 's':
        {
            service = optarg;
            break;
        }
        case 'u':
        {
            user = optarg;
            break;
        }
        case 'h':
        {
            show_help(argv[0]);
            exit(EXIT_SUCCESS);
        }
        default:
            break;
        }
    }

    if (service == nullptr || *service == '\0')
    {
        service = strrchr(argv[0], '/')+1;
        if (service == nullptr || *service == '\0')
        {
            return -1;
        }
    }

    // read config
    EntryNames name;
    if (read_config(service, name) != 0)
    {
        fprintf(stderr, "reading config failed\n");
        exit(EXIT_FAILURE);
    }

    ServiceEntry entry;
    if (LoadModule(name, entry) != 0)
    {
        fprintf(stderr, "LoadModule failed\n");
        exit(EXIT_FAILURE);
    }

    // 后台服务化
    if (daemonlize() != 0)
    {
        fprintf(stderr, "daemonlize failed\n");
        exit(EXIT_FAILURE);
    }

    openlog(service, LOG_PID | LOG_NDELAY, LOG_LOCAL0);

    // 注册信号
    if (register_signals() != 0)
    {
        syslog(LOG_ERR, "register_signals failed");
        exit(EXIT_FAILURE);
    }

    // TODO run as another user

    // 创建PID文件
    if (create_pidfile() != 0)
    {
        syslog(LOG_ERR, "create_pidfile failed");
        exit(EXIT_FAILURE);
    }

    if (entry.init)
        entry.init();

    std::shared_ptr<std::thread> spthr;
    if (entry.loop)
    {
        spthr = std::shared_ptr<std::thread>(new std::thread(entry.loop));
    }

    while (1)
    {
        if (sigterm_signaled || sigintr_signaled || sigalrm_signaled)
            break;
        sleep(1);
    }

    if (entry.release)
        entry.release();

    if (spthr)
    {
        spthr->join();
    }

    delete_pidfile();

    return 0;
}