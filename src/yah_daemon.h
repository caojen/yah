#ifndef YAH_DAEMON_H
#define YAH_DAEMON_H

/**
 * Daemonize current process.
 * 
 * return 0 if success
 * return other if failed
 * 
 * Sometimes, this function may not return but call exit
 * because it may call log_error or LOG_ERROR
 */
int yah_daemonize(void);

/**
 * check if there is a daemon running
 * if so, return YAH_DAEMON_RUNNING
 * else, return YAH_DAEMON_NOTRUNNING
 * 
 * If there is no any daemon is running,
 * the current process' pid will be written
 * into YAH_LOCKFILE.
 */
int check_daemon_running(void);

/**
 * try locking the file with file descriptor fd
 * 
 * @{param} fd file descriptor. The file should open
 *  with rw, and with YAH_LOCKMODE mode
 */
int lockfile(int);


/**
 * check if there is a airodump program
 * the tested path will be YAH_AIRODUMP
 * 
 * return YAH_AIRODUMP_EXISTS if YAH_AIRODUMP exists
 */
int check_airodump_exists(void);

#endif
