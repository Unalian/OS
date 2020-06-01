# include <stdio.h>
# include <unistd.h>
# include <sys/msg.h>
# include <string.h>
# include <errno.h>
# define MSG_KEY 114514
# define MAX_BUF 2550

struct msg_st {
    long msg_type;
    char msg[MAX_BUF];
};

void server_process(int msg_queue_id) {
    struct msg_st data;
    data.msg_type = 1; // server type
    pid_t my_pid = getpid();//进程号
    sprintf(data.msg, "hello client, my pid is %d", (int) my_pid);
    int res = msgsnd(msg_queue_id, (void *)&data, sizeof(struct msg_st), IPC_NOWAIT);
    //写入消息队列
    //打开消息队列 其中
    //msg_queue_id是消息队列号。data为struct
    //flag为IPC_NOWAIT，意为非阻塞形式发送消息，消息队列满或者队列中消息总数等于系统限制，则出错返回EAGAIN
    if (res < 0) {//失败建立消息队列
        puts("sever send message error!");
        printf("errno=%d [%s]\n",errno,strerror(errno));
    }
    
    
    data.msg_type = 2; // client type 等待来自client的消息
    res = msgrcv(msg_queue_id, &data, sizeof(struct msg_st), data.msg_type, IPC_NOWAIT); // read the msg from queue //成功返回数据部分长度，失败返回-1
    if (res < 0) {//如果读消息的时候，另一个process没有发消息，就会出现没有读到任何信息的情况，这个时候要不停读
        if (errno != 42) {
            puts("sever receive message error!");
            printf("errno=%d [%s]\n",errno,strerror(errno));
        }
        for(;res <0 && (int) errno == 42;) {//errno == 42 -> 没有读到任何信息
            res = msgrcv(msg_queue_id, &data, sizeof(struct msg_st), data.msg_type, IPC_NOWAIT); // read the msg from queue
            if (res < 0 && errno != 42) {
                puts("sever receive message error!");
                printf("errno=%d [%s]\n",errno,strerror(errno));
                break;
            }
        }
    }
    printf("server read the message:(%s) from the client\n", data.msg);
}

void client_process(int msg_queue_id) {
    struct msg_st data;
    data.msg_type = 2; // client type
    pid_t my_pid = getpid();
    sprintf(data.msg, "hello server, my pid is %d", (int) my_pid);
    int res = msgsnd(msg_queue_id, &data, sizeof(struct msg_st), 0); // write the msg into queue
    if (res < 0) {
        puts("sever send message error!");
        printf("errno=%d [%s]\n",errno,strerror(errno));
    }
    
    data.msg_type = 1; // server type 等待来自server的消息
    res = msgrcv(msg_queue_id, &data, sizeof(struct msg_st), data.msg_type, 0); // read the msg from queue
    if (res < 0) {
        if (errno != 42) {
            puts("client receive message error!");
            printf("errno=%d [%s]\n",errno,strerror(errno));
        }
        for(;res < 0 && (int) errno == 42;) {
            res = msgrcv(msg_queue_id, &data, sizeof(struct msg_st), data.msg_type, IPC_NOWAIT); // read the msg from queue
            if (res < 0 && errno != 42) {
                puts("client receive message error!");
                printf("errno=%d [%s]\n",errno,strerror(errno));
                break;
            }
        }
    }
    printf("client read the message:(%s) from the server\n", data.msg);
}

int main() {
    pid_t pid = fork();
   
    int msg_queue_id = msgget(MSG_KEY, IPC_EXCL);//IPC_EXCL，若该消息队列已存在，则出错返回EEXIST
    if (msg_queue_id < 0) {
        msg_queue_id = msgget(MSG_KEY, IPC_CREAT | 0666);//IPC_CREAT，若消息队列不存在，则创建//返回队列号

        if (msg_queue_id < 0) {
            puts("create msg queue error!");
            return 1;
        }
    }
    if (pid > 0) server_process(msg_queue_id);// pid1 父进程 pid2 的子进程
    if (pid == 0) client_process(msg_queue_id);// pid2 父进程 pid1 的子进程
    return 0;
}
