#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/file.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <syslog.h>
#include <time.h>
#include <unistd.h>

// cat /var/log/syslog

#define LOCKFILE "/var/run/lab1_daemon.pid"

#define LOCKMODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

int lockfile(int fd)
{
    struct flock fl;
    fl.l_type = F_WRLCK;
    fl.l_start = 0;
    fl.l_whence = SEEK_SET;
    fl.l_len = 0;
    return fcntl(fd, F_SETLK, &fl);
}

void daemonize(const char *cmd) 
{
    int i, fd0, fd1, fd2;
    pid_t pid;
    struct rlimit rl;
    struct sigaction sa;
    
    umask(0); 
    if (getrlimit(RLIMIT_NOFILE, &rl) < 0) 
    {
        printf("Невозможно получить максимальный номер дескриптора\n");
        exit(1);
    }

    if ((pid = fork()) < 0)  // 2
    {
        printf("Ошибка вызова fork\n");
        exit(1);
    } 
    else if (pid != 0) 
    {
        exit(0);
    }

    if (setsid() == -1)
    {
        printf("Проблема с setsid\n");
        exit(1);
    }

    sa.sa_handler = SIG_IGN;

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    if (sigaction(SIGHUP, &sa, NULL) < 0) 
    {
        printf("Невозможно игнорировтаь сигнал sighup\n");
        exit(1);
    }

    if (chdir("/") < 0) 
    {
        printf("Невозможно сделать корневой каталог текущим рабочим каталогом \n");
        exit(1);
    }

    if (rl.rlim_max == RLIM_INFINITY) 
    {
        rl.rlim_max = 1024;
    }

    for (i = 0; i < rl.rlim_max; i++)
    {
        close(i);
    }

    fd0 = open("dev/null", O_RDWR);
    fd1 = dup(0);
    fd2 = dup(0);

    openlog(cmd, LOG_CONS, LOG_DAEMON);
    if (fd0 != -0 || fd1 != 1 || fd2 != 2) 
    {
        syslog(LOG_ERR, "ошибочные файловые дескрипторы %d %d %d", fd0, fd1,
               fd2);
        exit(1);
    }
    syslog(LOG_WARNING, "Демон запущен!");
}

int already_running(void) 
{
    syslog(LOG_ERR, "Проверка на многократный запуск");

    int fd;
    char buf[16];

    fd = open(LOCKFILE, O_RDWR | O_CREAT, LOCKMODE);

    if (fd < 0) 
    {
        syslog(LOG_ERR, "Невозможно открыть файл %s: %s", LOCKFILE,
               strerror(errno));
        exit(1);
    }

    syslog(LOG_WARNING, "Файл открыт");

    if (lockfile(fd) < 0)
    {
        if (errno == EACCES || errno == EAGAIN)
        {
            close(fd);
            syslog(LOG_INFO, "can't lock %s: %s (already locked)", LOCKFILE, strerror(errno));
            return 1;
        }

        syslog(LOG_ERR, "can't lock %s: %s", LOCKFILE, strerror(errno));
        exit(1);
    }

    ftruncate(fd, 0);
    sprintf(buf, "%ld", (long)getpid());
    write(fd, buf, strlen(buf) + 1);

    syslog(LOG_WARNING, "PID записано в файл");

    return 0;
}

void log_time() 
{
    time_t raw_time;
    struct tm *time_info;
    char buf[70];

    time(&raw_time);
    time_info = localtime(&raw_time);
    syslog(LOG_INFO, (asctime(time_info)));
}

void *thr_fn(void *arg) 
{
    int signo;
    int code_error = 0;
    while (!code_error) 
    {
        code_error = sigwait(&mask, &signo);
        switch (signo) 
        {
            case SIGHUP:
                syslog(LOG_INFO, "ПРИНЯТ СИГНАЛ SIGHUP\n");
                break;
            case SIGTERM:
                syslog(LOG_INFO, "ПРИНЯТ СИГНАЛ SIGTERM - ЗАВЕРШИТЬ ПРОЦЕСС\n");
                return 0;
                break;
            default:
                syslog(LOG_INFO, "ПРИНЯТ НЕПРЕДВИДЕННЫЙ СИГНАЛ - %d\n", signo);
                break;
        }
    }
}

int main(int argc, char *argv[]) 
{
    int err;
    pthread_t tid;
    char *cmd;
    struct sigaction sa;

    if ((cmd = strrchr(argv[0], '/')) == NULL)
    {
        cmd = argv[0];
    } 
    else
        cmd++;

    daemonize(cmd);

    if (already_running() != 0) 
    {
        syslog(LOG_ERR, "Демон уже запущен\n");
        exit(1);
    }

    sa.sa_handler = SIG_DFL;  
    sigemptyset(&sa.sa_mask);

    sa.sa_flags = 0;

    if (sigaction(SIGHUP, &sa, NULL) < 0) 
    {
        syslog(LOG_ERR, "Не удалось установить SIG_DFL на SIGHUP\n");
        exit(1);
    }

    sigfillset(&mask);
    if ((err = pthread_sigmask(SIG_BLOCK, &mask, NULL)) != 0) 
    {
        syslog(LOG_ERR, "Не удалось установить SIG_BLOCK\n");
        exit(1);
    }

    err = pthread_create(&tid, NULL, thr_fn, 0);
    if (err != 0) 
    {
        syslog(LOG_ERR, "Ошибка создания потока\n");
        exit(1);
    }

    for (int i = 0; i < 100; ++i) 
    {
        log_time();
        sleep(3);
    }
    
    pthread_join(tid, NULL);

    exit(0);
}
