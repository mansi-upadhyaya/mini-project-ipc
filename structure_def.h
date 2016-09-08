#pragma once

#define TOTAL_JOB 70
#define ADD_JOB 1
#define DELETE_JOB 2

typedef enum {completed = 0,in_process = 1, pending = 2, canceled = 3}job_status;

typedef struct job_item{
   int job_id ;
   char file_name[256];
   int pages;
   job_status status;

}job;

typedef struct total_jobs
{
   job  job_ent[TOTAL_JOB];
}job_summary;


typedef struct message_buff
{
   long type;
   char file_name[256];
   int job_attr;
}msg;



