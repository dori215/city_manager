#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

//handler pt sigint
void handle_sigint(int sig)
{
    printf("Semnal SIGINT primit\n");
    if(unlink(".monitor_pid")==-1)
        perror("Eroare la stergerea fisierului");

    exit(0);
}

//handler pt sigusr1
void handle_sigusr1(int sig)
{
    printf("A fost adaugat un nou raport\n");
}

int main()
{
    //verificare daca exista alt monitor activ
    int pid_file_fd=open(".monitor_pid", O_RDONLY);
    if(pid_file_fd!=-1)
    {
        char old_pid[16]={0};
        read(pid_file_fd, old_pid, 15);
        printf("ERROR-monitorul este deja activ cu pid-ul: %s\n", old_pid);
        close(pid_file_fd);
        return 1;
    }

    //setam actiunea pt sigint
    struct sigaction sa_int;
    sa_int.sa_handler = handle_sigint;
    sigemptyset(&sa_int.sa_mask);
    sa_int.sa_flags = 0;
    if(sigaction(SIGINT, &sa_int, NULL)==-1)
    {
        perror("Eroare la sigint");
        return 1;
    }

    //setam actiunea pt sigusr1
    struct sigaction sa_usr1;
    sa_usr1.sa_handler = handle_sigusr1;
    sigemptyset(&sa_usr1.sa_mask);
    sa_usr1.sa_flags = 0;

    if(sigaction(SIGUSR1, &sa_usr1, NULL)==-1)
    {
        perror("Eroare la sigusr1");
        return 1;
    }

    //scriem pid ul curent in fisier
    pid_t pid=getpid();
    int fd=open(".monitor_pid", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if(fd==-1)
    {
        perror("ERROR- nu s-a putut crea monitor_pid");
        return 1;
    }

    char pid_str[32];
    sprintf(pid_str, "%d", pid);
    write(fd, pid_str, strlen(pid_str));
    close(fd);

    printf("Monitor pornit cu pid-ul: %d\n", pid);

    while(1)
       pause();

 return 0;
}