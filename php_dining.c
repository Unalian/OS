# include <stdio.h>
# include <unistd.h>
# include <pthread.h>
# include <semaphore.h>
# define N 5
# define left(x) x
# define right(x) (x + 1) % N
# define THINKING 1
# define HUNGRY 2
# define EATING 3

pthread_t thread[N];
sem_t sem[N];
int id[N];

void* solve(void* param) {
    int number = *((int*) param);
    int STATE = THINKING;
    for (;;) {
        if (STATE == EATING) {
            printf("philosopher: %d end eating\n", number); // end eat
            sem_post(&sem[left(number)]); // release left fork
            sem_post(&sem[right(number)]); // release right fork
            break;
        } else if (STATE == THINKING) {
            printf("philosopher: %d is thinking\n", number);
            STATE = HUNGRY;
        } else {
            printf("philosopher: %d is trying to eat\n", number);
            if (number % 2 == 0) {
                if (sem_wait(&sem[left(number)]) == 0) { // get the left fork
                    if (sem_trywait(&sem[right(number)]) == 0) { // try to get the right fork
                        printf("philosopher: %d begin to eat.\n", number); // begin to eat
                        STATE = EATING;
                    } else {
                        sem_post(&sem[left(number)]);
                        printf("philosopher: %d can not eat. Do not have the right fork\n", number); // no right fork
                        STATE = THINKING;
                    }
                } else {
                    printf("philosopher: %d can not eat. Do not have the left fork\n", number); // no left fork
                    STATE = THINKING;
                }
            } else {
                if (sem_wait(&sem[right(number)]) == 0) { // get the right fork
                    if (sem_trywait(&sem[left(number)]) == 0) { // try to get the left fork
                        printf("philosopher: %d begin to eat.\n", number); // begin to eat
                        STATE = EATING;
                    } else {
                        sem_post(&sem[right(number)]);
                        printf("philosopher: %d can not eat. Do not have the left fork\n", number); // no right fork
                        STATE = THINKING;
                    }
                } else {
                    printf("philosopher: %d can not eat. Do not have the right fork\n", number); // no left fork
                    STATE = THINKING;
                }
            }
        }
        sleep(1);
    }
    pthread_exit(NULL);
}

void create_thread() {
    for (int i = 0; i < N; i++) {
        int check = pthread_create(&thread[i],NULL,solve,&id[i]);
        if (check != 0) {
            puts("create thread error!");
        }
    }
}

void wait_thread() {
    for (int i = 0; i < N; i++) {
        pthread_join(thread[i], NULL);
        printf("thread %d ends\n", i);
    }
}

int ph_lunch() {
    // init thread function argument
    for (int i = 0; i < N; i++) {
        id[i] = i;
    }

    // init semaphore
    for (int i = 0; i < N; i++) {
        int check = sem_init(&sem[i], 0, 1);
        if (check != 0) {
            puts("Init semaphore error!");
        }
    }
    create_thread();
    wait_thread();
    return 1;
}

int main() {
    ph_lunch();
}

