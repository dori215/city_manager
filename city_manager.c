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

int check_access(const char *file_path, const char *role, const char *action_type)
{
        struct stat st;
        if (stat(file_path, &st)==-1)
            return 0;
        if(strcmp(role,"manager")==0)
           {
               if(strcmp(action_type,"read")==0)
               {
                 if(!(st.st_mode & S_IRUSR))
                       {
                            fprintf(stderr, "ERROR-manager access denied\n");
                            return 1;
                       }
               }
               if(strcmp(action_type,"write")==0)
               {
                 if(!(st.st_mode & S_IWUSR))
                       {
                            fprintf(stderr, "ERROR-manager access denied\n");
                            return 1;
                       }
               }
           }
       else if(strcmp(role,"inspector")==0)
          {
                if(strcmp(action_type,"read")==0)
                 {
                  if(!(st.st_mode & S_IRGRP))
                    {
                        fprintf(stderr, "ERROR-inspector access denied\n");
                        return 1;
                    }
                 }
                if(strcmp(action_type,"write")==0)
                 {
                  if(!(st.st_mode & S_IWGRP))
                    {
                         fprintf(stderr, "ERROR-inspector access denied\n");
                         return 1;
                    }
                 }
      }
    return 0;
}

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
  int view_id=-1;
  int remove_id=-1;
  int threshold_value=-1;

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
    if(strcmp(command,"--view")==0 && i+1<argc)
    {
        view_id=atoi(argv[i+1]);
        i++;
    }
    else if(strcmp(command,"--remove_report")==0 && i+1<argc)
    {
       remove_id=atoi(argv[i+1]);
       i++;
    }
    else if(strcmp(command,"--update_threshold")==0 && i+1<argc)
    {
        threshold_value=atoi(argv[i+1]);
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
  printf("Raportul a fost creat cu ID-ul: %d\n", rep.report_id);

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

  if(check_access(path, role, "write")==1)
       return 1;

  int fd=open(path, O_WRONLY | O_CREAT | O_APPEND, 0664);

  if(fd==-1)
  {
    perror("ERROR-nu s-a putut deschide fisierul");
    return 1;
  }

  if(write(fd, &rep, sizeof(Report))==-1)
  {
     perror("ERROR-nu s-a putut scrie in fisier\n");
     close(fd);
     return 1;
  }
   // creare fisier district.cfg
   struct stat st_cfg;
    char cfg_path[512];
    strcpy(cfg_path, district);
    strcat(cfg_path, "/district.cfg");

    if(check_access(cfg_path, role, "write")==1)
       return 1;

    int fd_cfg=open(cfg_path, O_WRONLY | O_CREAT, 0640);
    if(fd_cfg!=-1)
      {
          write(fd_cfg,"2",1);
          close(fd_cfg);
          chmod(cfg_path,0640);
      }
    else perror("ERROR-nu s-a putut crea district.cfg");
    close(fd);
    chmod(path, 0664);

    // creare fisier logged_district
    char log_path[512];
    strcpy(log_path, district);
    strcat(log_path, "/logged_district");

    if(check_access(log_path, role, "write")==1)
        return 1;

    if(strcmp(role, "manager")==0)
    {
        int fd_log=open(log_path, O_WRONLY | O_CREAT | O_APPEND, 0644);
        if(fd_log!=-1)
        {
            char log_entry[512];
            sprintf(log_entry, "%ld | %s | %s\n ", time(NULL), nume_utilizator, command);
            write(fd_log, log_entry, strlen(log_entry));
            close(fd_log);
            chmod(log_path,0644);
         }
      else perror("ERROR-nu s-a putut deschide logged_district");
    }
}

  if(strcmp(command, "--list")==0)
  {
      char path[512];
      strcpy(path, district);
      strcat(path, "/reports.dat");

      if(check_access(path, role, "read")==1)
            return 1;

      struct stat st;
   if(stat(path, &st)==-1)
   {
       perror("ERROR-nu exista date in district");
       return 1;
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

  if(strcmp(command, "--view")==0)
  {
      if(view_id==-1)
        {
            fprintf(stderr, "ERROR-lipseste id-ul\n");
            return 1;
        }
      char path[512];
      struct stat st;
      strcpy(path, district);
      strcat(path, "/reports.dat");

      if(check_access(path, role, "read")==1)
         return 1;

      if(stat(path, &st)==-1)
      {
          perror("ERROR-nu exista date in district");
          return 1;
      }

  int fd=open(path, O_RDONLY);
  if(fd==-1)
     {
         perror("ERROR-nu s-a putut deschide fisierul\n");
         return 1;
     }

  Report r;
  int gasit=0;
  while(read(fd, &r, sizeof(Report))>0)
    {
        if(r.report_id==view_id)
          {
               printf("Detalii raport ID:%d\nInspector:%s | Categorie:%s | Severitate:%d | Locatie: Lat %.4f; Long %.4f | Descriere:%s | Data:%s ", r.report_id, r.inspector_name, r.issue_category, r.severity_level, r.latitude, r.longitude, r.description_text, ctime(&r.timestamp));
               gasit=1;
               break;
          }
    }
   if(!gasit) printf("Raportul nu a fost gasit in district\n");
   close(fd);
  }

  if(strcmp(command, "--remove_report")==0)
    {
         if(strcmp(role, "manager")!=0)
            {
                  fprintf(stderr, "ERROR-doar managerul poate sterge\n");
                  return 1;
            }

        char path[512];
        strcpy(path, district);
        strcat(path, "/reports.dat");
        if(check_access(path, role, "write")==1)
            return 1;

        int fd=open(path, O_RDWR);
        if(fd==-1)
           {
                perror("ERROR-nu s-a putut deschide fisierul");
                return 1;
           }

        Report r;
        off_t pos=0;
        int gasit=0;
        while(read(fd, &r, sizeof(Report))>0)
            {
               if(r.report_id==remove_id)
                 {
                    gasit=1;
                    break;
                 }
            pos=pos+sizeof(Report);
            }

    if(gasit)
    {
        Report next;
        while(read(fd, &next, sizeof(Report))>0)
            {
                lseek(fd, pos, SEEK_SET);
                write(fd, &next, sizeof(Report));
                pos=pos+sizeof(Report);
                lseek(fd, pos+sizeof(Report), SEEK_SET);
            }
        struct stat st;
        fstat(fd, &st);
        ftruncate(fd, st.st_size-sizeof(Report));
        printf("Raportul a fost sters\n");
    }
    else printf("Raportul nu a fost gasit\n");
    close(fd);

    char log_p[512];
    sprintf(log_p, "%s/logged_district", district);
    int fl=open(log_p, O_WRONLY | O_APPEND);
    if(fl!=-1)
     {
        char l[256];
        sprintf(l, "%ld | %s | remove %d\n", time(NULL), nume_utilizator, remove_id);
        write(fl, l, strlen(l));
        close(fl);
     }
    }

  if(strcmp(command, "--update_threshold")==0)
    {
        if(strcmp(role, "manager")!=0)
           {
                fprintf(stderr, "ERROR-doar managerul poate modifica pragul\n");
                return 1;
           }

        char path[512];
        strcpy(path, district);
        strcat(path, "/district.cfg");
        struct stat st;

        if(stat(path, &st)==-1)
        {
             perror("ERROR-nu s-a putut accesa district.cfg");
             return 1;
        }
        if((st.st_mode & 0777)!=0640)
         {
             fprintf(stderr, "ERROR-permisiunile nu sunt 640\n");
             return 1;
         }

         int fd=open(path, O_WRONLY | O_TRUNC);
         if(fd==-1)
         {
              perror("ERROR-nu s-a putut deschide fisierul");
              return 1;
         }

        char buf[16];
        int len=sprintf(buf, "%d", threshold_value);
        write(fd, buf, len);
        close(fd);

        printf("Pragul de severitate actualizat\n");

        char log_p[512];
        sprintf(log_p, "%s/logged_district", district);
        int fd_log=open(log_p, O_WRONLY | O_APPEND);
        if(fd_log!=-1)
           {
                char entry[256];
                sprintf(entry, "%ld | %s | %s | update_threshold %d\n", time(NULL), role, nume_utilizator, threshold_value);
                write(fd_log, entry, strlen(entry));
                close(fd_log);
           }
    }
  return 0;
}