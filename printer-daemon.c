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

int job_signal=0;


print_job * shm_print_job = NULL;
void * process_signal(void *);
void check_printer(int);
void restart_printer(int);
void *job_print(void *);
void * process_signal(void * arg)
{

    while(1)
    {
        signal(SIGUSR2,check_printer);
        signal(SIGUSR1,restart_printer);

    }

}
void check_printer(int arg)
{
    if(shm_print_job->status==ready)
    {
        job_signal=1;
        shm_print_job->status=busy;
    }
    else if(shm_print_job->status==busy)
    {
        printf("The Printer is busy,Please wait!");
    }
    else if(shm_print_job->status==printer_stalled)
    {
        printf("The Printer is stalled ,Please wait for the printer to restart!");
    }

}

void *job_print(void *arg)
{
    while(1)
        {
        if(job_signal==1)
            {

            while (shm_print_job->print_status!=0)
                {
                    sleep(1);
                --shm_print_job->print_status;

                }
            shm_print_job->status=ready;
            job_signal=0;

            }

        }
}
void restart_printer(int arg)
{
    if(shm_print_job->status==printer_stalled)
    {
        shm_print_job->status=ready;
    }

}

void main()
{

    int shmid = 0, retval = 0;
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




    if((shmid = shmget(key1, sizeof(print_job), 0766 )) == -1)
    {
       perror("shmget");
       exit(1);
    }

    if((shm_print_job =  shmat(shmid, (void *)0, 0)) == (void *)0)
    {
       perror("shmat");
       exit(1);
    }

    printf("%d", shm_print_job->print_status);


    if((retval=pthread_create(&process_signal, NULL,process_signal, NULL)) != 0 ) {
        printf("Thread not created\n"); }

    if((retval=pthread_create(&job_print, NULL,job_print, NULL)) != 0 ) {
        printf("Thread not created\n"); }


   pthread_join(process_signal);

   pthread_join(job_print);

}


