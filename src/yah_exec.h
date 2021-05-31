/**
 * helper functions that running shell commands
 */

#ifndef YAH_EXEC_H
#define YAH_EXEC_H

// exec a command just like in shell
// run that command like `/bin/sh -c "${command}"`
// auto generate "" around command, avoid using " in command
// store the stdout of the command, max size is outputsz, ending with 0
// return the command status
// warn that, this function will block until the command ended(exit or error).
int yah_exec_shell(const char* command, char* output, int outputsz);

#endif
