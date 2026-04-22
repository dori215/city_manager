#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

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

  int index=1;

  for(int i=1;i<argc;i++)
    {
      if(command==NULL)
	{
	  command=argv[i];
	  index=i;
	  break;
	}
      else if((strcmp(argv[i], "--role")==0) & (i+1<argc))
	{
	  role=argv[i+1];
	  i++;
	}
      else if((strcmp(argv[i], "--user")==0) & (i+1<argc))
	{
	  user=argv[i+1];
	  i++;
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

  if(strcmp(command, "--add")==0)
    if(index+1>=argc)
      {
	fprintf(stderr,"ERROR-lipseste district_id\n");
	return 1;
      }
 return 0;
}
