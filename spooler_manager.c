#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h> 
#include <sys/msg.h>
#include "structure_def.h"
#include "structure_def1.h"

void view(void );


int msqid;

void main(int argc, char  * argv[])
{
    key_t key;
    msg  * rbuf[10];
    int  atoi_check;
    int retsend;
   
    if(argv[1]!=0)
    {  
  
    	if(strcmp(argv[1], "-v")== 0)
    	{
        	view();
    	}

        key = 9999;

        if ((msqid = msgget(key, 0777|IPC_CREAT)) < 0)
        {
            perror("msgget");
            exit(1);
        }


    	if(strcmp(argv[1], "add") == 0)
    	{
            rbuf[0] = (msg *)malloc(sizeof(msg));
    		if(strcmp(argv[2], "-f") == 0)
        	{
        		strcpy(rbuf[0]->file_name, argv[3]);
        	}
        	else
        	{
        		printf("enter into correct format \n");
            	exit(1);
        	}
            if(argv[4] != 0)
            {
        	if(strcmp(argv[4], "-p") == 0)
        	{
        		atoi_check = atoi(argv[5]);
                if (atoi_check > 0)
                {
                	rbuf[0]->job_attr = atoi_check;
                }
                else
                {
                	printf("enter valid number of pages i.e it should be an integer and should be greater than zero \n");
                    exit(1);
                }
            }
            }
        	else
            {
            	rbuf[0]->job_attr = (rand() % (100 + 1 -10)) + 10;
            } 

            rbuf[0]->type = ADD_JOB;

    		if (retsend = msgsnd(msqid, rbuf[0], sizeof(msg), 0) < 0)
            {
            	perror("msgsend");
                exit(1);
                
            }
          
            exit(0);
          
		} 
        if (strcmp(argv[1], "-d") == 0)
        {
            int i = 2;
        	while(argv[i] != 0)
            {   
                rbuf[i]= (msg *)malloc(sizeof(msg));
            	rbuf[i]->job_attr = atoi(argv[i]);
            	rbuf[i]->type = DELETE_JOB;
            	msgsnd(msqid, rbuf[i], sizeof(msg), 0);
                i++;
            }
            

        }  
	}
    else
    {
    	printf("enter into correct format \n");
    }
}


void view()
{
 
   key_t key = 4567;
   int shmid = 0;
   int job_print=0;
  
   job_summary * shmaddr;

   char * enum_stn[] = {"completed", "in-progress" , "pending", "cancled"};


    if((shmid = shmget(key, sizeof(job_summary), 0766)) == -1)
    {
       perror("shmget");
       exit(1);
    }

    if((shmaddr = shmat(shmid, (void *)0, 0)) == (void *)0)
    {
       perror("shmat");
       exit(1);
    }

    printf("job_id    file_name     pages    status \n");
   
    while(shmaddr->job_ent[job_print].job_id!=0)
    { 
    	printf("%d           %s         %d     %s\n", shmaddr->job_ent[job_print].job_id, shmaddr->job_ent[job_print].file_name, shmaddr->job_ent[job_print].pages, enum_stn[(int )shmaddr->job_ent[job_print].status] );
    job_print++;
	}
   shmdt(shmaddr);
   exit(0);
}
