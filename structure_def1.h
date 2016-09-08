#pragma once

typedef enum{busy = 0,ready = 1, printer_stalled = 2,  printer_ink_glow = 3}printer_status;

typedef struct print_job_status
{
	printer_status status;
    int print_status;
}print_job;

