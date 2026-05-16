#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>

int main(void)
{
    char input[1024];
    printf("Bun venit in City Hub!\n");

    while(1)
    {
        printf("Hub: ");
        if(fgets(input, 1024, stdin)==NULL)
            break;

        input[strcspn(input, "\n")]=0;

        if (strlen(input)==0)
            continue;
        if(strcmp(input, "quit")==0)
        {
             int pid_fd=open(".monitor_pid", O_RDONLY);//deschidem fisierul ca sa aflam pid ul monitorului care ruleaza in spate
            if(pid_fd!=-1)
            {
                char old_pid[16]={0};
                if(read(pid_fd, old_pid, 15)>0)
                {
                    pid_t pid=(pid_t)atoi(old_pid);
                    kill(pid, SIGINT);// trimitem sigint pt ca monitorul sa stearga .monitor_pid
                }
             close(pid_fd);
            }
            break;//oprim hub ul principal
        }

        char copy_input[1024];
        strcpy(copy_input, input);

        char *p=strtok(copy_input, " ");

        //start_monitor
        if(strcmp(p,"start_monitor")==0)
        {
            pid_t hub_mon=fork();//cream un proces in fundal ca hub ul principal sa ramana liber pentru alte comenzi
            if(hub_mon==0)
            {
                int fd[2];// fd[0]-citire, fd[1]-scriere
                pipe(fd);//pipe ul prin care monitorul trimite textul si managerul il citeste

                pid_t monitor_pid=fork();//creare monitor
                if(monitor_pid==0)
                {
                        close(fd[0]);
                        dup2(fd[1], STDOUT_FILENO);//inlocuieste iesirea standard cu capatul de scriere al pipe ului
                        close(fd[1]);
                        execlp("./monitor_reports", "monitor_reports", NULL);
                        perror("Eroare la pornire monitor_reports");
                        exit(1);
                }
                else
                {
                    close(fd[1]);

                    char buffer[256];
                    int duplicate=0;//variabila pt a sti ce mesaj afisam la inchiderea monitorului

                    while(1)
                    {
                            int bytes_read=read(fd[0], buffer, 255);//citim din fd[0]
                            if(bytes_read<=0)
                                break;

                            buffer[bytes_read]='\0';
                            if(strstr(buffer, "ERROR")!=NULL)//cautam sirul ERROR de la "ERROR-monitorul este deja activ" si punem var pe 1
                                duplicate=1;

                            printf("\nMonitor: %s", buffer);
                            printf("Hub: ");
                            fflush(stdout);//asiguram afisarea datelor in timp real
                    }
                if(duplicate==0)
                    printf("\nHub alert: Monitorul principal a fost oprit\n");
                else
                    printf("\nHub alert: Monitorul duplicat a fost respins\n");
                printf("Hub: ");
                close(fd[0]);
                exit(0);
                }
            }
        continue;
        }
    //calculate_scores
        else if(strcmp(p,"calculate_scores")==0)
        {
            char *p1=strtok(NULL, " ");
            while(p1!=NULL)
            {
                int fd[2];
                pipe(fd);
                pid_t scorer_pid=fork();
                if(scorer_pid==0)
                {
                    close(fd[0]);
                    dup2(fd[1], STDOUT_FILENO);
                    close(fd[1]);
                    execlp("./calculate_scores", "calculate_scores", p1, NULL);
                    perror("Nu s-a gasit calculate_scores\n");
                    exit(1);
                }
                else
                {
                    close(fd[1]);
                    char buffer[512];

                    while(1)
                    {
                           int bytes_read=read(fd[0], buffer, 511);
                           if(bytes_read<=0)
                                break;
                            buffer[bytes_read]='\0';
                            printf("\nScorer: %s", buffer);
                    }
                    close(fd[0]);
                    wait(NULL);//asteptam sa termine de calculat inainte de a trece la urm district
                }
            p1=strtok(NULL, " ");
            }
            printf("\n");
        }
        else
            printf("ERROR-Comanda necunoscuta\n");
    }
    return 0;
}