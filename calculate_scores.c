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

#define nameSize 64
#define categorySize 32
#define descriptionSize 256

//structura pt rapoarte
typedef struct
{
    int report_id;
    char inspector_name[nameSize];
    float latitude;
    float longitude;
    char issue_category[categorySize];
    int severity_level;//1-minor,2-moderat,3-critic
    time_t timestamp;
    char description_text[descriptionSize];
}Report;

//structura pt a tine evidenta scorurilor
typedef struct InspectorScore
{
    char name[64];
    int score;
}InspectorScore;

int main(int argc, char *argv[])
{
    if(argc!=2)
    {
        printf("ERROR-Numar incorect de argumente\n");
        return 1;
    }

    char path[512];
    sprintf(path,"%s/reports.dat",argv[1]);

    int fd=open(path, O_RDONLY);
    if(fd==-1)
    {
           printf("Districtul nu are rapoarte\n");
            return 1;
    }

    Report r;
    InspectorScore inspector_score[100];
    int number_inspectors=0;

    while(read(fd, &r, sizeof(Report))>0)
    {
        int gasit=0;

        for(int i=0;i<number_inspectors;i++)
        {
            if(strcmp(inspector_score[i].name, r.inspector_name)==0)
            {
                  inspector_score[i].score+=r.severity_level;
                  gasit++;
                  break;
            }
        }
        if(!gasit)
        {
            strcpy(inspector_score[number_inspectors].name, r.inspector_name);
            inspector_score[number_inspectors].score=r.severity_level;
            number_inspectors++;
        }
    }
    close(fd);

    printf("Workload- District %s\n", argv[1]);
    for(int i=0;i<number_inspectors;i++)
            printf("Nume- %s | Workload score: %d\n", inspector_score[i].name, inspector_score[i].score);
    printf("\n");
    return 0;
}