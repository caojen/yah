#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <sys/wait.h>

#include "yah_exec.hpp"


namespace yah {
    int yah_exec_shell(const char* command, char* output, int outputsz) {
        // init a pipe for connection between child and parent
        int fd[2];
        if(pipe(fd) != 0) {}

        // fork a child to exec the command
        pid_t pid;
        if((pid = fork()) < 0) {} 
        else if(pid == 0) {
            /* child process */
            // child process send data, so just close fd[0]
            close(fd[0]);
            // make child.stdout redirect to fd[1]
            if(dup2(fd[1], STDOUT_FILENO) != STDOUT_FILENO) {}
            // close STDIN to stop unexpected block
            close(STDIN_FILENO);
            // use system call execl, generate "" around command
            // char __command[YAH_MAXLINE + 3];
            // snprintf(__command, YAH_MAXLINE + 2, "\"%s\"", command);
            // yah_log("generate command: %s", __command);
            // execl(YAH_SEHLL_PATH, YAH_SHELL, "-c", __command);
            execl("/bin/sh", "sh", "-c", command, NULL);
        } else {
            /* parent process */
            close(fd[1]);
            // wait for child stop
            int status;
            waitpid(pid, &status, 0);
            // child.fd[1] has been closed.
            // read until read return 0
            int n;
            int size = 0;
            char buf[64];
            memset(output, 0, outputsz * sizeof(char));
            while(size < outputsz - 1 && (n = read(fd[0], buf, 64)) != 0) {
                // copy buf append to output
                int copysz = n;
                if(size + copysz >= outputsz) {
                    copysz = outputsz - size - 1;
                }
                memcpy(output + size, buf, copysz);

                size += copysz;
            }
            return !WIFEXITED(status);
        }
        return 1;
    }
}
