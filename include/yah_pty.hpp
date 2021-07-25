/**
 * basic functions for pseudo terminal
 * 
 * reference: Advanced Programming in the Unix Environment, Chapter 19
 */

#ifndef YAH_PTY_H
#define YAH_PTY_H

#define MAX_PTYNAME 20

#include <stdlib.h>
#include <termios.h>
#include <sys/ioctl.h>

#define YAH_PTY_COLS 1024
#define YAH_PTY_ROWS 4096

namespace yah {

  // create and open next usable pty device
  // pts_name is the name of the pty device
  // pts_namesz is the max size of pts_name
  // return pty's fd if success, return -1 if failed
  int yah_ptym_open(char* pts_name, int pts_namesz);

  // based on the pts_name provided, openthe pty device
  // return ptr's fd if success, return -1 if failed
  int yah_ptys_open(char* pts_name);

  // fork a new subprocess to run the pty
  // param ptrfdm: the pty device file descriptor
  // param slave_name: just like pts_name, the name of the pty device
  // param slave_namesz: just like pts_namesz, the size of slave_name
  // param slave_termios, param slave_winsize: settings of pty device
  // return:
  //   the pid of subprocess for the parent process
  //   0 for the child process
  pid_t yah_pty_fork(int* ptrfdm, char* slave_name, int slave_namesz,
                      const struct termios* slave_termios,
                      const struct winsize* slave_winsize);

}
#endif
