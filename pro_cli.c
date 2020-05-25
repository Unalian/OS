# include <stdio.h>
# include <string.h>
# include <unistd.h>
# include <pthread.h>
# include <semaphore.h>
# define N 5
# define CUSTOM_N 4
# define PRODUCER_N 3
# define MAX_LEN 255000
pthread_mutex_t mutex;
pthread_t customer[CUSTOM_N], producer[PRODUCER_N];
int id_cus[CUSTOM_N], id_pro[PRODUCER_N];
sem_t full, empty;
int input_point = 0;
int output_point = 0;
char input_data[MAX_LEN];
char output_data[MAX_LEN];
char buf[N];
int head = 0;
int tail = 0;

void *producer_func(void *param) {
    int number = *((int *) param);
    for (;;) {
        printf("Producer: %d is producing data.\n", number);
        sem_wait(&empty);
        pthread_mutex_lock(&mutex);
        if (input_point >= strlen(input_data)) {
            printf("producer: %d produce nothing and ready to exit.\n", number);
            pthread_mutex_unlock(&mutex);
            sem_post(&empty);
            break;
        }
        buf[tail] = input_data[input_point];
        printf("producer: %d produce the character: %c\n", number, buf[tail]);
        input_point = input_point + 1;
        tail = (tail + 1) % N;
        sem_post(&full);
        if (input_point >= strlen(input_data)) {
            printf("producer: %d produce nothing and ready to exit.\n", number);
            pthread_mutex_unlock(&mutex);
            sem_post(&empty);
            break;
        }
        pthread_mutex_unlock(&mutex);
        sleep(1);
    }
    pthread_exit(NULL);
}

void *customer_func(void *param) {
    int number = *((int *) param);
    for (;;) {
        printf("Customer: %d is getting data.\n", number);
        sem_wait(&full);
        pthread_mutex_lock(&mutex);
        if (output_point >= strlen(input_data)) {
            printf("customer: %d get nothing and ready to exit.\n", number);
            pthread_mutex_unlock(&mutex);
            sem_post(&full);
            break;
        }
        printf("customer: %d get the character: %c\n", number, buf[head]);
        output_data[output_point] = buf[head];
        head = (head + 1) % N;
        output_point = output_point + 1;
        output_data[output_point] = '\0';
        sem_post(&empty);
        if (output_point >= strlen(input_data)) {
            printf("customer: %d get nothing and ready to exit.\n", number);
            pthread_mutex_unlock(&mutex);
            sem_post(&full);
            break;
        }
        pthread_mutex_unlock(&mutex);
        sleep(1);
    }
    pthread_exit(NULL);
}

void init() {
    pthread_mutex_init(&mutex, NULL);
    sem_init(&full, 0, 0);
    sem_init(&empty, 0, N);
    for (int i = 0; i < CUSTOM_N; i++) id_cus[i] = i;
    for (int i = 0; i < PRODUCER_N; i++) id_pro[i] = i;
}

void creat_thread() {
    for (int i = 0; i < PRODUCER_N; i++) {
        pthread_create(&producer[i], NULL, producer_func, &id_pro[i]);
    }
    for (int i = 0; i < CUSTOM_N; i++) {
        pthread_create(&customer[i], NULL, customer_func, &id_cus[i]);
    }
}

void wait_thread() {//等待指令线程运行结束
    pthread_join(customer[0], NULL);
    pthread_join(customer[1], NULL);
    pthread_join(customer[2], NULL);
    pthread_join(customer[3], NULL);
    pthread_join(producer[0], NULL);
    pthread_join(producer[1], NULL);
    pthread_join(producer[2], NULL);
}

int customer_producer() {
    init();
    creat_thread();
    wait_thread();
    return 1;
}

int main() {
    scanf("%s", input_data);
    customer_producer();
    printf("%s", output_data);
    return 0;
}
