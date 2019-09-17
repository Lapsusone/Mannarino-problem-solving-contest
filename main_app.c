/* ----------------------------------------------------------------------- */
/*
  Easy embeddable cross-platform high resolution timer function. For each
  platform we select the high resolution timer. You can call the 'ns()'
  function in your file after embedding this.
*/
#include <stdint.h>
#if defined(__linux)
#  define HAVE_POSIX_TIMER
#  include <time.h>
#  ifdef CLOCK_MONOTONIC
#     define CLOCKID CLOCK_MONOTONIC
#  else
#     define CLOCKID CLOCK_REALTIME
#  endif
#elif defined(__APPLE__)
#  define HAVE_MACH_TIMER
#  include <mach/mach_time.h>
#elif defined(_WIN32)
#  define WIN32_LEAN_AND_MEAN
#  include <windows.h>
#endif
static uint64_t ns() {
	static uint64_t is_init = 0;
#if defined(__APPLE__)
	static mach_timebase_info_data_t info;
	if (0 == is_init) {
		mach_timebase_info(&info);
		is_init = 1;
	}
	uint64_t now;
	now = mach_absolute_time();
	now *= info.numer;
	now /= info.denom;
	return now;
#elif defined(__linux)
	static struct timespec linux_rate;
	if (0 == is_init) {
		clock_getres(CLOCKID, &linux_rate);
		is_init = 1;
	}
	uint64_t now;
	struct timespec spec;
	clock_gettime(CLOCKID, &spec);
	now = spec.tv_sec * 1.0e9 + spec.tv_nsec;
	return now;
#elif defined(_WIN32)
	static LARGE_INTEGER win_frequency;
	if (0 == is_init) {
		QueryPerformanceFrequency(&win_frequency);
		is_init = 1;
	}
	LARGE_INTEGER now;
	QueryPerformanceCounter(&now);
	return (uint64_t)((1e9 * now.QuadPart) / win_frequency.QuadPart);
#endif
}
/* ----------------------------------------------------------------------- */

#include <windows.h>
#include <time.h>
#include <math.h>
#include <inttypes.h>
#include <stdlib.h>

/* FreeRTOS kernel includes. */
#include "FreeRTOS.h"
#include "task.h"

/* Supporting functions include file */
#include "supporting_functions/supporting_functions.h"

/* Used as a loop counter to create a very crude delay. */
#define mainDELAY_LOOP_COUNT		( 0xffffff )

/* The task functions. */
void vTask1(void *pvParameters);
void vTask2(void *pvParameters);

/* Define the strings that will be passed in as the task parameters. These are
defined const and off the stack to ensure they remain valid when the tasks are
executing. */
const char *pcTextForTask1 = "Task 1 is running\r\n";
const char *pcTextForTask2 = "\r\nTask 2 is running\r\n";

/*-----------------------------------------------------------*/

int main_app(void)
{
	/* Create two tasks and pass string parameters to the task functions. */
	xTaskCreate(vTask1, "Task 1", 100, (void*)pcTextForTask1, 1, NULL);	
	xTaskCreate(vTask2, "Task 2", 100, (void*)pcTextForTask2, 1, NULL);

	/* Start the scheduler to start the tasks executing. */
	vTaskStartScheduler();
	return 0;
}
/*-----------------------------------------------------------*/

void vTask1(void *pvParameters)
{
	clock_t start, end;
	double cpu_time_used;
	char buffer[50] = { 0 };

	volatile uint32_t ul;

	/* The string to print out is passed via the parameter.  Cast this to a
	character pointer. */
	char *pcTaskString;
	pcTaskString = (char *)pvParameters;
	end = clock();
	/* As per most tasks, this task is implemented in an infinite loop. */
	for ( ;; )
	{
		start = clock();
		cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
		snprintf(buffer, 50, "%lf\n", cpu_time_used);

		/* Print out the string parameter of this task. */
		vPrintString(pcTaskString);
		vPrintString(buffer);
		//printf("%" PRIu64 "\n", ns());
		end = clock();

		/* Delay for a period of time. */
		for (ul = 0; ul < mainDELAY_LOOP_COUNT; ul++)
		{
			
		}
	}

}
/*-----------------------------------------------------------*/

void vTask2(void *pvParameters)
{
	volatile uint32_t ul;
	char *pcTaskString;
	pcTaskString = (char *)pvParameters;

	/* As per most tasks, this task is implemented in an infinite loop. */
	for ( ;; )
	{
		/* Print out the string parameter of this task. */
		vPrintString(pcTaskString);

		/* Delay for a period of time. */
		for (ul = 0; ul < mainDELAY_LOOP_COUNT; ul++)
		{

		}
	}
}
