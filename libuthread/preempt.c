#include <signal.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "private.h"
#include "uthread.h"

/*
 * Frequency of preemption
 * 100Hz is 100 times per second
 */
#define HZ 100

/* keep track of stuff with this*/
static bool preemptEnabled = false;
static struct sigaction prevAction;
static struct itimerval prevTimer;
static sigset_t signalMask;

/* helper func */
static void timer_handler(int signum)
{
        /* TODO Phase 4 */
	(void)signum;
	uthread_yield();
}

void preempt_disable(void)
{
        /* TODO Phase 4 */
	if (!preemptEnabled)
		return;
	
	sigprocmask(SIG_BLOCK, &signalMask, NULL);
}

void preempt_enable(void)
{
        /* TODO Phase 4 */
	if (!preemptEnabled)
		return;
	
	sigprocmask(SIG_UNBLOCK, &signalMask, NULL);
}

void preempt_start(bool preempt)
{
        /* TODO Phase 4 */
	sigemptyset(&signalMask);
	sigaddset(&signalMask, SIGVTALRM);
	
	if (!preempt)
		return;
	
	preemptEnabled = true;
	struct sigaction sa;
	sa.sa_handler = timer_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	
	if (sigaction(SIGVTALRM, &sa, &prevAction) == -1) {
		perror("sigaction");
		exit(1);
	}
	
	struct itimerval timer;
	
	timer.it_interval.tv_sec = 0;
	timer.it_interval.tv_usec = 1000000 / HZ;
	
	timer.it_value = timer.it_interval;
	
	if (setitimer(ITIMER_VIRTUAL, &timer, &prevTimer) == -1) {
		perror("setitimer");
		exit(1);
	}
	
	preempt_enable();
}

void preempt_stop(void)
{
        /* TODO Phase 4 */
	if (!preemptEnabled)
		return;
	
	preempt_disable();
	
	if (setitimer(ITIMER_VIRTUAL, &prevTimer, NULL) == -1) {
		perror("setitimer");
		exit(1);
	}
	
	if (sigaction(SIGVTALRM, &prevAction, NULL) == -1) {
		perror("sigaction");
		exit(1);
	}
	
	preemptEnabled = false;
}
