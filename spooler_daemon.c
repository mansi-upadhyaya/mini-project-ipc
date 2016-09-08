#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <signal.h>
#include "structure_def.h"
#include "structure_def1.h"

void * que_manage(void *);
void * job_print(void *);
void   print_update(int );

int job_count = 0;

int job_id1 = 1;

job job1;
job_summary  * job_list;

job_summary * shmaddr = NULL;
print_job * shm_print_job = NULL;

void main()
{  
    key_t key = 4567;
    int shmid = 0, retval = 0, shmid1 = 0;
    pthread_t print_manager, print;
    pid_t pid, sid;
    key_t key1 = 1111;

    pid = fork();
   if (pid < 0)
   {
      perror("fork:");
      exit(EXIT_FAILURE);
   }
   if (pid > 0)
   {
      exit(EXIT_SUCCESS);
   }

   umask(0);

   sid = setsid();

   if (sid < 0)
   {
      perror("sid:");
      exit(EXIT_FAILURE);
   }

   if ((chdir("/")) < 0)
   {
      exit(EXIT_FAILURE);
   }
  
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

 
    if((shmid = shmget(key, sizeof(job_summary), 0766|IPC_CREAT	)) == -1)
    {
       perror("shmget");
       exit(1);
    }
   
    if((shmaddr =  shmat(shmid, (void *)0, 0)) == (void *)0)
    {
       perror("shmat");
       exit(1);
    }

    
    if((shmid1 = shmget(key1, sizeof(print_job), 0766|IPC_CREAT )) == -1)
    {
       perror("shmget");
       exit(1);
    }

    if((shm_print_job =  shmat(shmid1, (void *)0, 0)) == (void *)0)
    {
       perror("shmat");
       exit(1);
    }

    printf("%d", shm_print_job->print_status);


	if((retval=pthread_create(&print_manager, NULL, que_manage, NULL)) != 0 ) {
		printf("Thread not created\n");	}

    if((retval=pthread_create(&print, NULL, job_print, NULL)) != 0 ) {
        printf("Thread not created\n"); } 

   
   pthread_join(print);

   pthread_join(print_manager);
   
}


void * que_manage(void * arg)
{
    int msqid;
    key_t key;
    msg  * rbuf;

    key = 9999;
  

    if ((msqid = msgget(key, 0777)) < 0) 
    {


        perror("msgget");
        exit(1);
    }

    rbuf = (msg *)malloc(sizeof(msg));
 


	while(1)
	{
	   msgrcv(msqid, rbuf, sizeof(msg), 0, 0); 

       shmaddr->job_ent[job_count].job_id = job_id1++;
       strcpy(shmaddr->job_ent[job_count].file_name, rbuf->file_name);
       shmaddr->job_ent[job_count].pages = rbuf->job_attr;
       shmaddr->job_ent[job_count].status = pending;
       shmaddr->job_ent[++job_count].job_id = 0;
    }
}

void * job_print(void * arg)
{
	while(1)
    {
       int count = 0;
       while (shmaddr->job_ent[count].status == completed || shmaddr->job_ent[count].status == canceled)
       {
       	   count++;
       }
    	if (shmaddr->job_ent[count].status == pending)
        {
        	if(shm_print_job->status == ready)
            {
            	shm_print_job->print_status = shmaddr->job_ent[0].pages;
                signal(SIGUSR2, print_update);     
               // kill( , SIGUSR2);           
            } 
 
        }
        
    }
}

void  print_update(int arg)
{
	  shmaddr->job_ent[0].status = in_process;
      while (shm_print_job->print_status != 0)
      {
           shmaddr->job_ent[0].pages =shm_print_job->print_status;

           sleep(1);
      }

      shmaddr->job_ent[0].status = completed;	
}

