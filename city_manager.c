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

void get_permissions_symbolic(mode_t mode, char *str)
{
    strcpy(str, "---------");

  if (mode & S_IRUSR) str[0] = 'r';
  if (mode & S_IWUSR) str[1] = 'w';
  if (mode & S_IXUSR) str[2] = 'x';

  if (mode & S_IRGRP) str[3] = 'r';
  if (mode & S_IWGRP) str[4] = 'w';
  if (mode & S_IXGRP) str[5] = 'x';

  if (mode & S_IROTH) str[6] = 'r';
  if (mode & S_IWOTH) str[7] = 'w';
  if (mode & S_IXOTH) str[8] = 'x';

}

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
    chmod(path, 0664);
}

  if(strcmp(command, "--list")==0)
  {
      char path[512];
      strcpy(path, district);
      strcat(path, "/reports.dat");

      struct stat st;
   if(stat(path, &st)==-1)
   {
       perror("ERROR-nu exista date in district");
       return 1;
   }

  if(strcmp(role, "manager")==0)
  {
      if(!(st.st_mode & S_IRUSR))
       {
         printf("ERROR-Acces nepermis pentru manager\n");
         return 1;
       }
   }
  else if(strcmp(role, "inspector")==0)
      {
         if(!(st.st_mode & S_IRGRP))
          {
             printf("ERROR-Acces nepermis pentru inspector\n");
             return 1;
          }
      }

  char perm_str[11];
  get_permissions_symbolic(st.st_mode, perm_str);

  char *mod_time=ctime(&st.st_mtime);
  mod_time[strlen(mod_time)-1]='\0';

  printf("File: %s | Permissions: %s | Size: %lld bytes | Last modification: %s\n", path, perm_str, (long long)st.st_size, mod_time);

  int fd=open(path, O_RDONLY);
  if(fd==-1)
  {
    perror("ERROR-nu s-a putut deschide fisierul");
    return 1;
  }

  Report r;
  while(read(fd, &r, sizeof(Report))>0)
  {
     printf("ID:%d | Inspector:%s | Category:%s | Severity:%d\nDescription:%s\n", r.report_id, r.inspector_name, r.issue_category, r.severity_level, r.description_text);
  }
  close(fd);
}

 return 0;
}
