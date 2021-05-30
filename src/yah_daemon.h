/**
 * Daemonize current process.
 * 
 * return 0 if success
 * return other if failed
 * 
 * Sometimes, this function may not return but call exit
 * because it may call log_error or LOG_ERROR
 */
int yah_daemonize();
