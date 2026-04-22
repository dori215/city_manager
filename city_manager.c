#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#define nameSize 64
#define categorySize 32
#define descriptionSize 256

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

int main(int argc, char *argv[])
{
  char *role=NULL;
  char *user=NULL;
  char *command=NULL;
  char *district=NULL;

  int index=1;

  for(int i=1;i<argc;i++)
    {
   if(strcmp(argv[i], "--role")==0 && i+1<argc)
	{
	  role=argv[i+1];
	  i++;
	}
      else if((strcmp(argv[i], "--user")==0) && i+1<argc)
	{
	  user=argv[i+1];
	  i++;
	}
      else if(command==NULL )
   {
     command=argv[i];
      if(i+1<argc)
    {
       district=argv[i+1];
       i++;
    }
  }
 }

  if(!role)
    {
      fprintf(stderr, "ERROR-lipseste rolul\n");
      return 1;
    }

  if(!command)
    {
      fprintf(stderr, "ERROR-lipseste comanda\n");
      return 1;
    }

  if(!district)
    {
      fprintf(stderr, "ERROR-lipseste district\n");
      return 1;
    }

    char *nume_utilizator;
    if(user!=NULL)
         nume_utilizator=user;
    else
         nume_utilizator="Necunoscut";

    printf("Rol: %s\nUser: %s\nCommand: %s\nDistrict: %s\n",role,nume_utilizator,command,district);

  if(strcmp(command, "--add")==0)
  {
      struct stat st={0};
    if(stat(district, &st)==-1)
    {
       if(mkdir(district, 0750)==-1)
      {
          perror("ERROR-nu s-a putut crea");
          return 1;
      }
    }
    chmod(district,0750);
    printf("Directorul '%s' e gata cu permisiunile corecte\n", district);

  Report rep;

  srand(time(NULL));
  rep.report_id=rand()%1000;
  rep.timestamp=time(NULL);
  if(user!=NULL)
        strncpy(rep.inspector_name, user, nameSize-1);
  else strcpy(rep.inspector_name, "Anonim");

   rep.latitude=(float)(rand()%90);
   rep.longitude=(float)(rand()%180);

  strcpy(rep.issue_category, "road");
  rep.severity_level=(rand()%3)+1;

  strcpy(rep.description_text, "pod picat");

  char path[512];
  strcpy(path, district);
  strcat(path, "/reports.dat");

  int fd=open(path, O_WRONLY | O_CREAT | O_APPEND, 0664);

  if(fd==-1)
  {
    perror("ERROR-nu s-a putut deschide fisierul");
    return 1;
  }

  if(write(fd, &rep, sizeof(Report))==-1)
  {
     perror("ERROR-nu s-a putut scrie in fisier");
     close(fd);
     return 1;
  }
    close(fd);
}

  if(strcmp(command, "--list")==0)
  {
      char path[512];
      strcpy(path, district);
      strcat(path, "/reports.dat");

      int fd=open(path,O_RDONLY);

      if(fd==-1)
      {
         perror("ERROR-nu s-a putut deschide fisierul");
         return 1;
      }
      printf("Lista de repoarte: %s\n", district);

      Report r;
      while(read(fd, &r, sizeof(Report))>0)
         printf("ID:%d | Inspector:%s | Categorie:%s | Severitate:%d\nDescriere:%s\n",r.report_id,r.inspector_name,r.issue_category,r.severity_level,r.description_text);
   close(fd);
  }

 return 0;
}
