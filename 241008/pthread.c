#include <stdio.h>
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

int gNum;

void *mythread(void *params) {
    while (1) {
        printf("child thread : getpid(%4d), getppid(%4d), gNum(%d)\n",
               (int) getpid(), (int) getppid(), gNum);
        sleep(5);
        gNum++;
    }
}

int main() {
    pthread_t tid;
    if (pthread_create(&tid, NULL, mythread, NULL)) {
        printf("thread create fail\n");
    }

    //if (pthread_join(tid, NULL)) {}

    while (1) {
        printf("mainthread : getpid(%4d), getppid(%4d), gNum(%d)\n",
               (int) getpid(), (int) getppid(), gNum);
        sleep(5);
        gNum++;
    }
}
