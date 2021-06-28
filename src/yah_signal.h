/**
 * Functions that deal with signal.
 * 
 * See also: signal(3)
 * 
 * Like: signal handler
 * or send signal to other process
 */

#ifndef YAH_SIGNAL_H
#define YAH_SIGNAL_H

#include <signal.h>

/**
 * register all signal handlers
 * return the number(>=0) of handlers that have been registerd.
 * return -1 if error.
 */
int signal_handler_register(void);

/**
 * handle SIGHUP
 */
void sighup_handler(int signo, siginfo_t* info, void* context);

/**
 * handle SIGTERM
 */
void sigterm_handler(int signo, siginfo_t* info, void* context);

/**
 * handle SIGCHLD
 */
void sigchld_handler(int signo, siginfo_t* info, void* context);

void sigabrt_handler(int signo, siginfo_t* info, void* context);

/**
 * send signal to pid
 */
#define send_signal(pid, signo) kill(pid, signo)

/**
 * send signal to self
 */
#define send_signal_to_current(signo) raise(signo)

#endif
