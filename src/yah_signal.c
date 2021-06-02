#include "yah_signal.h"
#include "yah_log.h"
#include "yah_const.h"
#include "yah_config.h"
#include "yah_daemon.h"
#include "yah_core.h"

int
signal_handler_register(void) {
    struct sigaction sa;

    int signals[] = { SIGHUP, SIGTERM, SIGCHLD };
    void (*handlers[])(int, siginfo_t*, void*) = {
        sighup_handler, sigterm_handler, sigchld_handler
    };
    int ret = sizeof(signals) / sizeof(signals[0]);

    for(int i = 0; i < ret; i++) {
        sa.sa_sigaction = handlers[i];
        sa.sa_flags = SA_SIGINFO;
        sigemptyset(&sa.sa_mask);
        sigaddset(&sa.sa_mask, signals[i]);
        if(sigaction(signals[i], &sa, NULL) < 0) {
            return -1;
        }
    }
    return ret;
}

void
sighup_handler(int signo, siginfo_t* info, void* context) {
    /**
     * SIGHUP received. reload config.
     */

    if(signo == SIGHUP) {
        yah_log("SIGHUP received from pid = %ld. Reloading config file", info->si_pid);
        yah_log("config file:, %s", YAH_CONFFILE);
        int res = yah_reload_config();
        if(res != 0) {
            yah_warn("config file reload failed.");
            yah_warn("process continue...");
        }
        yah_log("reload file done");
    } else {
        yah_warn("SIGHUP handler received error signo");
    }
}

void
sigterm_handler(int signo, siginfo_t* info, void* context) {
    /**
     * SIGTERM received; exiting
     */

    if(signo == SIGTERM) {
        yah_log("%ld: SIGTERM received. exiting...", getpid());
        daemon_exit();
    } else {
        yah_warn("SIGTERM handler received error signo");
    }
}

void
sigchld_handler(int signo, siginfo_t* info, void* context) {
    /**
     * SIGCHLD received;
     * Test the child is the airodump or not.
     * If so, just call reboot
     */
    if(signo == SIGCHLD) {
        pid_t child = info->si_pid;
        yah_log("%ld: SIGTERM received from child's pid = %ld", getpid(), child);
        yah_log("airodump-ng pid = %ld", airodump_pid);
    } else {
        yah_warn("SIGCHLD handler received error signo");
    }
}
