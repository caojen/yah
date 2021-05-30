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
 * handle SIGHUP
 */
void sighup_handler(int signo);

/**
 * handle SIGTERM
 */
void sigterm_handler(int signo);

/**
 * send signal to pid
 */
#define send_signal(pid, signo) kill(pid, signo)

#endif
