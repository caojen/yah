#include "yah_signal.h"
#include "yah_log.h"
#include "yah_const.h"
#include "yah_config.h"
#include "yah_daemon.h"

int
signal_handler_register(void) {
    struct sigaction sa;

    int signals[] = { SIGHUP, SIGTERM };
    void (*handlers[])(int) = {
        sighup_handler, sigterm_handler
    };
    int ret = sizeof(signals) / sizeof(signals[0]);

    for(int i = 0; i < ret; i++) {
        sa.sa_handler = handlers[i];
        sa.sa_flags = 0;
        sigemptyset(&sa.sa_mask);
        sigaddset(&sa.sa_mask, signals[i]);
        if(sigaction(signals[i], &sa, NULL) < 0) {
            return -1;
        }
    }
    return ret;
}

void
sighup_handler(int signo) {
    /**
     * SIGHUP received. reload config.
     */

    if(signo == SIGHUP) {
        yah_log("SIGHUP received. Reloading config file");
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
sigterm_handler(int signo) {
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
