#include <errno.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <sys/unistd.h>
#include <stm32f10x.h>
#include "defs/defs.h"

extern uint32_t __get_MSP(void);

// errno
#undef errno
extern int errno;

/*
 Переменные среды - пустой список.
 */
char *__env[1] = { 0 };
char **environ = __env;


int _read(int file, char *ptr, int len) WEAK;
int _write(int file, char *ptr, int len) WEAK;

// exit - экстренный выход. В качестве выхода - зацикливаемся.
void WEAK _exit(int status)
{
    while (1);
}

// close - закрытие файла.
int WEAK _close(int file)
{
    return -1;
}
/*
 execve - запуск бинарника в адресном пространстве текущего процесса.
 */
int WEAK _execve(char *name, char **argv, char **env)
{
    errno = ENOMEM;
    return -1;
}

/*
 fork = создание нового процесса.
 */
int WEAK _fork()
{
    errno = EAGAIN;
    return -1;
}

/*
 fstat - состояние открытого файла.
 */
int WEAK _fstat(int file, struct stat *st)
{
    st->st_mode = S_IFCHR;
    return 0;
}

/*
 getpid - получить ID текущего процесса.
 */
int WEAK _getpid()
{
    return 1;
}

/*
 isatty - является ли файл терминалом.
 */
int WEAK _isatty(int file)
{
    switch (file)
    {
    case STDOUT_FILENO:
    case STDERR_FILENO:
    case STDIN_FILENO:
        return 1;
    default:
        //errno = ENOTTY;
        errno = EBADF;
        return 0;
    }
}

/*
 kill - послать сигнал процессу.
 */
int WEAK _kill(int pid, int sig)
{
    errno = EINVAL;
    return (-1);
}

/*
 link - устанвить новое имя для существующего файла.
 */
int WEAK _link(char *old, char *new)
{
    errno = EMLINK;
    return -1;
}

/*
 lseek - установить позицию в файле.
 */
int WEAK _lseek(int file, int ptr, int dir)
{
    return 0;
}

/*
 sbrk - увеличить размер области данных, использутся для malloc.
 */
caddr_t WEAK _sbrk(int incr)
{
    extern char _ebss; 
    static char *heap_end;
    char *prev_heap_end;

    if (heap_end == 0)
    {
        heap_end = &_ebss;
    }
    prev_heap_end = heap_end;

    char * stack = (char*) __get_MSP();
    if (heap_end + incr > stack)
    {
        _write(STDERR_FILENO, "Heap and stack collision\n", 25);
        errno = ENOMEM;
        return (caddr_t) -1;
        //abort ();
    }

    heap_end += incr;
    return (caddr_t) prev_heap_end;

}

/*
 read - чтение из файла, у нас пока для чтения есть только stdin.
 */
int WEAK _read(int file, char *ptr, int len)
{
    int num = 0;
    switch (file)
    {
    case STDIN_FILENO:
        //break;
    default:
        errno = EBADF;
        return -1;
    }
    return num;
}

/*
 stat - состояние открытого файла.
 */
int WEAK _stat(const char *filepath, struct stat *st)
{
    st->st_mode = S_IFCHR;
    return 0;
}

/*
 times - временная информация о процессе (сколько тиков: системных, процессорных и т.д.).
 */
clock_t WEAK _times(struct tms *buf)
{
    return -1;
}

/*
 unlink - удалить имя файла.
 */
int WEAK _unlink(char *name)
{
    errno = ENOENT;
    return -1;
}

/*
 wait - ожидания дочерних процессов.
 */
int WEAK _wait(int *status)
{
    errno = ECHILD;
    return -1;
}

/*
 write - запись в файл - у нас есть только stderr/stdout.
 */
int WEAK _write(int file, char *ptr, int len)
{
    switch (file)
    {
    case STDOUT_FILENO: /*stdout*/
    case STDERR_FILENO: /* stderr */
        //break;
    default:
        errno = EBADF;
        return -1;
    }
    return len;
}
