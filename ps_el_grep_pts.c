#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait/sys.h>

int main()
{
    int fd1[2], fd2[2];
    pid_t pid1, pid2;

    if (pipe(fd1) == -1)
    {
        perror("pipe");
        exit(1);
    }

    if ((pid1 = fork()) == 0)
    {
        // Child 1: ps -el
        dup2(fd1[1], STDOUT_FILENO); // Send output of ps to fd1 write end
        close(fd1[0]); // Close unused read end
        close(fd1[1]);
        execlp("ps", "ps", "-el", NULL);
        perror("execlp ps");
        exit(1);
    }

    if (pipe(fd2) == -1)
    {
        perror("pipe");
        exit(1);
    }

    if ((pid2 = fork()) == 0)
    {
        // Child 2: grep pts
        dup2(fd1[0], STDIN_FILENO);  // Read input from fd1 read end
        // You can also redirect STDOUT to fd2[1] if you want
        // dup2(fd2[1], STDOUT_FILENO);
        close(fd1[1]); // Close unused write end
        close(fd1[0]);
        close(fd2[0]);
        close(fd2[1]);
        execlp("grep", "grep", "pts", NULL);
        perror("execlp grep");
        exit(1);
    }

    // Parent closes all FDs
    close(fd1[0]);
    close(fd1[1]);
    close(fd2[0]);
    close(fd2[1]);

    // Wait for children to finish
    wait(NULL);
    wait(NULL);

    return 0;
}
