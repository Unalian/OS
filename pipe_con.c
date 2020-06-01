# include <stdio.h>
# include <unistd.h>
# include <string.h>
# include <stdlib.h>
int communicate() {
    int fd[2];
    int check = pipe(fd);
    if (check != 0) {
        puts("create pipe error!");
        return 0;
    }
    pid_t pid_1, pid_2;
    char msg_1[100], msg_2[100], msg_read[100];
    if ((pid_1 = fork()) == 0) { // into the child process 1
        close(fd[0]); // close read pipe
        strcpy(msg_1, "Child process 1 is sending a message!\n");
        write(fd[1], msg_1, strlen(msg_1)); // write
        close(fd[1]); // close write pipe
        exit(0); // exit
    } else if (pid_1 > 0) {
        waitpid(pid_1, NULL, 0); // wait process child 1 do first
        if ((pid_2 = fork()) == 0) { // into the child process 2
            close(fd[0]); // close read pipe
            strcpy(msg_2, "Child process 2 is sending a message!\n");
            write(fd[1], msg_2, strlen(msg_2)); //write
            close(fd[1]); // close write pipe
            exit(0);
        } else if (pid_2 > 0) { // into the father process
            waitpid(pid_2, NULL, 0); // wait process 2 do first
            close(fd[1]); // close write pip
            read(fd[0], msg_read, sizeof(msg_read));
            printf("%s\n", msg_read);
        }
    }
    return 1;
}

int main() {
    communicate();
}
