#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <error.h>
#include <sys/wait.h>
int main()
{
        int fd1[2],fd2[2];
        pid_t pid1,pid2;

        if(pipe(fd1)==-1)
        {
                perror("Perror");
                exit(0);
        }

        if((pid1=fork())==0)
        {
                dup2(fd1[1],STDOUT_FILENO);
                close(fd1[0]);
                close(fd1[1]);

                execlp("cat","cat","file.txt",NULL);
                perror("execlp");
                exit(1);
        }

        else if((pid2=fork())==0)
        {
                dup2(fd1[0],STDIN_FILENO);
                close(fd1[1]);
                close(fd1[0]);

                execlp("wc","wc","-l",NULL);
                perror("execlp");
                exit(1);
        }

        close(fd1[1]);
        close(fd1[0]);

        wait(NULL);
        wait(NULL);
        return 0;
}